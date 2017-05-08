#pragma once

#include "..\System.h"
#include "..\World.h"
#include "..\..\AppTime.h"
#include "..\Components\ScriptComponent.h"
#include "..\Scripts\Script.h"

namespace ECS { namespace Systems {
	class ScriptSystem : public System
	{
	public:
		ScriptSystem(GLFWwindow * window)
		{
			this->scriptData = new ECS::Scripts::ScriptData(window);
		}
		ScriptSystem() { delete this->scriptData; }

		void Update(ECS::World & world, const AppTime & time);
		void FixedUpdate(ECS::World & world, const AppTime & time);
	private:
		ECS::Scripts::ScriptData * scriptData;
	};
}}
