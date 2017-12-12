#pragma once

#include "..\World.h"
#include "..\System.h"

namespace ECS { namespace Systems {
	class InputSystem : public System
	{
	public:
		InputSystem(vr::IVRSystem * vr);

		void Update(World & world, const AppTime & time);
		void FixedUpdate(World & world, const AppTime & time) { }
	private:
		void VRUpdate(ECS::World & world, const AppTime & time);

		vr::IVRSystem * vr;
		vr::TrackedDevicePose_t trackedDevicePoses[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 trackedDeviceMatrices[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 hmdPose;
		glm::mat4 eyePoseLeft;
		glm::mat4 eyePoseRight;
	};
} }
