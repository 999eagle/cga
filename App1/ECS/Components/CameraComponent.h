#pragma once

#include "..\Component.h"
#include "TransformComponent.h"

namespace ECS { namespace Components {
	class CameraComponent : public Component
	{
	public:
		CameraComponent(float fov, float aspectRatio, float nearPlane, float farPlane)
			: fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane)
		{ }

		const float & GetFOV() { return this->fov; }
		const float & GetAspectRatio() { return this->aspectRatio; }
		const float & GetNearPlane() { return this->nearPlane; }
		const float & GetFarPlane() { return this->farPlane; }
	private:
		float fov, aspectRatio, nearPlane, farPlane;
	};
}}
