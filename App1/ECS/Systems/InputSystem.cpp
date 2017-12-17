#include "pch.h"
#include "InputSystem.h"

#include "..\..\Common\util.h"
#include "..\Components\TransformComponent.h"
#include "..\Components\VRTrackedDeviceComponent.h"

ECS::Systems::InputSystem::InputSystem(vr::IVRSystem * vr)
	: vr(vr), leftControllerIndex(vr::k_unTrackedDeviceIndexInvalid), rightControllerIndex(vr::k_unTrackedDeviceIndexInvalid)
{
	if (this->vr != NULL)
	{
		vrMatrixToGlm(this->eyePoseLeft, this->vr->GetEyeToHeadTransform(vr::Eye_Left));
		vrMatrixToGlm(this->eyePoseRight, this->vr->GetEyeToHeadTransform(vr::Eye_Right));

		for (auto i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
		{
			if (!this->vr->IsTrackedDeviceConnected(i))
				continue;
			this->SetupVRTrackedDevice(i);
		}
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
		switch (event.eventType)
		{
		case vr::VREvent_TrackedDeviceActivated:
			this->SetupVRTrackedDevice(event.trackedDeviceIndex);
			break;
		case vr::VREvent_TrackedDeviceRoleChanged:
			this->UpdateControllerRoles();
			break;
		}
	}

	vr::VRCompositor()->WaitGetPoses(this->trackedDevicePoses, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	for (auto i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
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
			case VRTrackedDevice_ControllerLeft:
				if (this->leftControllerIndex != vr::k_unTrackedDeviceIndexInvalid && this->trackedDevicePoses[this->leftControllerIndex].bPoseIsValid)
				{
					transform->SetLocalTransform(this->trackedDeviceMatrices[this->leftControllerIndex]);
				}
				break;
			case VRTrackedDevice_ControllerRight:
				if (this->rightControllerIndex != vr::k_unTrackedDeviceIndexInvalid && this->trackedDevicePoses[this->rightControllerIndex].bPoseIsValid)
				{
					transform->SetLocalTransform(this->trackedDeviceMatrices[this->rightControllerIndex]);
				}
				break;
			}
		}
	}
}

void ECS::Systems::InputSystem::SetupVRTrackedDevice(vr::TrackedDeviceIndex_t index)
{
	auto devClass = this->vr->GetTrackedDeviceClass(index);
	switch (devClass)
	{
	case vr::TrackedDeviceClass_Controller:
		this->CheckControllerRole(index);
	}
}

void ECS::Systems::InputSystem::UpdateControllerRoles()
{
	this->leftControllerIndex = vr::k_unTrackedDeviceIndexInvalid;
	this->rightControllerIndex = vr::k_unTrackedDeviceIndexInvalid;

	for (auto i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		if (!this->vr->IsTrackedDeviceConnected(i))
			continue;
		auto devClass = this->vr->GetTrackedDeviceClass(i);
		if (devClass == vr::TrackedDeviceClass_Controller)
		{
			this->CheckControllerRole(i);
		}
	}
}

void ECS::Systems::InputSystem::CheckControllerRole(vr::TrackedDeviceIndex_t index)
{
	auto role = this->vr->GetInt32TrackedDeviceProperty(index, vr::Prop_ControllerRoleHint_Int32);
	switch (role)
	{
	case vr::TrackedControllerRole_LeftHand:
		this->leftControllerIndex = index;
		break;
	case vr::TrackedControllerRole_RightHand:
		this->rightControllerIndex = index;
		break;
	case vr::TrackedControllerRole_Invalid:
		if (this->leftControllerIndex == vr::k_unTrackedDeviceIndexInvalid)
		{
			this->leftControllerIndex = index;
		}
		else if (this->rightControllerIndex == vr::k_unTrackedDeviceIndexInvalid)
		{
			this->rightControllerIndex = index;
		}
		break;
	}
}
