#include "pch.h"
#include "InputSystem.h"

#include "..\..\Common\util.h"
#include "..\Components\TransformComponent.h"
#include "..\Components\VRTrackedDeviceComponent.h"

ECS::Systems::InputSystem::InputSystem(vr::IVRSystem * vr)
	: vr(vr)
{
	if (this->vr != NULL)
	{
		vrMatrixToGlm(this->eyePoseLeft, this->vr->GetEyeToHeadTransform(vr::Eye_Left));
		vrMatrixToGlm(this->eyePoseRight, this->vr->GetEyeToHeadTransform(vr::Eye_Right));
	}
}

void ECS::Systems::InputSystem::Update(ECS::World & world, const AppTime & time)
{
	glfwPollEvents();
	this->VRUpdate(world, time);
}

void ECS::Systems::InputSystem::VRUpdate(ECS::World & world, const AppTime & time)
{
	if (this->vr == NULL) { return; }

	vr::VREvent_t event;
	while (this->vr->PollNextEvent(&event, sizeof(event)))
	{
	}

	vr::VRCompositor()->WaitGetPoses(this->trackedDevicePoses, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		if (this->trackedDevicePoses[i].bPoseIsValid)
		{
			vrMatrixToGlm(this->trackedDeviceMatrices[i], this->trackedDevicePoses[i].mDeviceToAbsoluteTracking);
		}
	}
	if (this->trackedDevicePoses[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
	{
		this->hmdPose = this->trackedDeviceMatrices[vr::k_unTrackedDeviceIndex_Hmd];
	}

	for (auto * it : world.GetEntities())
	{
		auto trackedDev = it->GetComponent<ECS::Components::VRTrackedDeviceComponent>();
		auto transform = it->GetComponent<ECS::Components::TransformComponent>();
		if (trackedDev != NULL)
		{
			switch (trackedDev->GetDevice())
			{
			case VRTrackedDevice_Hmd:
				transform->SetLocalTransform(this->hmdPose);
				break;
			case VRTrackedDevice_EyeLeft:
				transform->SetLocalTransform(this->eyePoseLeft);
				break;
			case VRTrackedDevice_EyeRight:
				transform->SetLocalTransform(this->eyePoseRight);
				break;
			}
		}
	}
}
