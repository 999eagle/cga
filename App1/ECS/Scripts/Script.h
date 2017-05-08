#pragma once
#include "..\..\AppTime.h"

namespace ECS { namespace Scripts {
	class ScriptData
	{
	public:
		ScriptData(GLFWwindow * window) : window(window) { }
		GLFWwindow * GetWindow() { return this->window; }
	private:
		GLFWwindow * window;
	};

	class Script
	{
	public:
		void SetScriptData(ScriptData * data) { this->data = data; }

		virtual void Update(const AppTime & time) = 0;
		virtual void FixedUpdate(const AppTime & time) = 0;
	protected:
		ScriptData * data;
	};
}}
