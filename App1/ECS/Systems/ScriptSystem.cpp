#include "pch.h"
#include "ScriptSystem.h"
#include "..\Components\ScriptComponent.h"

void ECS::Systems::ScriptSystem::Update(ECS::World & world, const AppTime & time)
{
	for (auto * it : world.GetEntities())
	{
		auto c = it->GetComponent<ECS::Components::ScriptComponent>();
		if (c != NULL)
		{
			for (auto * script : c->GetScripts())
			{
				if (script != NULL)
				{
					script->SetScriptData(this->scriptData);
					script->Update(time);
				}
			}
		}
	}
}

void ECS::Systems::ScriptSystem::FixedUpdate(ECS::World & world, const AppTime & time)
{
	for (auto * it : world.GetEntities())
	{
		auto c = it->GetComponent<ECS::Components::ScriptComponent>();
		if (c != NULL)
		{
			for (auto * script : c->GetScripts())
			{
				if (script != NULL)
				{
					script->SetScriptData(this->scriptData);
					script->FixedUpdate(time);
				}
			}
		}
	}
}
