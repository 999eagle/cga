#pragma once

#include "..\Component.h"

namespace ECS { namespace Components {
	enum LightType
	{
		LightType_Ambient,
		LightType_Point,
		LightType_Directional,
	};

	class LightComponent : public Component
	{
	public:
		LightComponent(LightType type, const glm::vec3 & color)
			: type(type), color(color)
		{
		}

		LightType type;
		glm::vec3 color;
	};
}}
