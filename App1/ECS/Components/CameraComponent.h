#pragma once

#include "..\Component.h"

namespace ECS { namespace Components {
	class CameraComponent : public Component
	{
	public:
		CameraComponent(float fov, float aspect, float nearPlane, float farPlane)
			: proj(glm::perspective(fov, aspect, nearPlane, farPlane))
		{ }
		CameraComponent(glm::mat4 proj)
			: proj(proj)
		{ }

		const glm::mat4 & GetProj() { return this->proj; }
	private:
		glm::mat4 proj;
	};
}}
