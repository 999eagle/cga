#pragma once

#include "..\Component.h"
#include "..\Scripts\Script.h"

namespace ECS { namespace Components {
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
		std::vector<ECS::Scripts::Script*> & GetScripts() { return this->scripts; }
	private:
		std::vector<ECS::Scripts::Script*> scripts;
	};
}}
