#pragma once

#include "..\System.h"
#include "..\World.h"
#include "..\..\AppTime.h"

namespace ECS { namespace Systems {
	class ScriptData
	{
	public:
		ScriptData(GLFWwindow * window) : window(window) { }
		GLFWwindow * GetWindow() { return this->window; }
	private:
		GLFWwindow * window;
	};

	class ScriptSystem : public System
	{
	public:
		ScriptSystem(GLFWwindow * window)
		{
			this->scriptData = new ScriptData(window);
		}
		ScriptSystem() { delete this->scriptData; }

		void Update(ECS::World & world, const AppTime & time);
		void FixedUpdate(ECS::World & world, const AppTime & time);
	private:
		ScriptData * scriptData;
	};
}}
