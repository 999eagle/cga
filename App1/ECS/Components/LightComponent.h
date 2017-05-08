#pragma once

#include "..\Component.h"
#include "..\..\Lights\ILight.h"

namespace ECS { namespace Components {
	class LightComponent : public Component
	{
	public:
		LightComponent(std::shared_ptr<ILight> light) : light(light) { }
		LightComponent(ILight * light) : LightComponent(std::shared_ptr<ILight>(light)) { }
		std::shared_ptr<ILight> light;
	};
}}
