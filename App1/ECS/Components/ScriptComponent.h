#pragma once

#include "..\Component.h"
#include "..\Systems\ScriptSystem.h"

namespace ECS { namespace Components {
	class ScriptComponent;

	class Script
	{
	public:
		void SetScriptData(ECS::Systems::ScriptData * data) { this->data = data; }
		void SetComponent(ScriptComponent * component) { this->component = component; }

		virtual void Start() = 0;
		virtual void Update(const AppTime & time) = 0;
		virtual void FixedUpdate(const AppTime & time) = 0;
	protected:
		ECS::Systems::ScriptData * data;
		ScriptComponent * component;
	};

	class ScriptComponent : public Component
	{
	public:
		~ScriptComponent()
		{
			for (auto * it : this->scripts)
			{
				delete it;
			}
		}
		std::vector<Script*> & GetScripts() { return this->scripts; }

		template<typename T, typename ... U>
		Script * AddScript(U && ... args)
		{
			Script * script = new T(std::forward<U>(args) ...);
			if (!garbage.empty())
			{
				scripts[garbage.front()] = script;
				garbage.pop();
			}
			else
			{
				scripts.push_back(script);
			}
			script->SetComponent(this);
			script->Start();
			return script;
		}
		void RemoveScript(Script * script)
		{
			for (auto it = this->scripts.begin(); it != this->scripts.end(); it++)
			{
				if (*it == script)
				{
					delete *it;
					*it = NULL;
					garbage.push(std::distance(this->scripts.begin(), it));
					break;
				}
			}
		}
	private:
		std::vector<Script*> scripts;
		std::queue<size_t> garbage;
	};
}}
