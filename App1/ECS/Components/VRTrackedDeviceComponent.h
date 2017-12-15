#pragma once

#include "..\Component.h"

enum VRTrackedDevice
{
	VRTrackedDevice_Hmd,
	VRTrackedDevice_EyeLeft,
	VRTrackedDevice_EyeRight,
};

namespace ECS{ namespace Components {
	class VRTrackedDeviceComponent : public Component
	{
	public:
		VRTrackedDeviceComponent(VRTrackedDevice device)
			: device(device)
		{ }

		const VRTrackedDevice & GetDevice() { return this->device; }
	private:
		VRTrackedDevice device;
	};
} }
