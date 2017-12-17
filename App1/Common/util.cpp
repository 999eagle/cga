#include "pch.h"
#include "util.h"

std::string GetTrackedDeviceString(vr::IVRSystem * vr, vr::TrackedDeviceIndex_t index, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError * error)
{
	auto size = vr->GetStringTrackedDeviceProperty(index, prop, NULL, 0, error);
	if (size == 0)
	{
		return "";
	}

	char * buffer = new char[size];
	size = vr->GetStringTrackedDeviceProperty(index, prop, buffer, size, error);
	auto result = std::string(buffer);
	delete[] buffer;
	return result;
}
