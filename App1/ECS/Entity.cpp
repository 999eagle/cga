#include "pch.h"
#include "Entity.h"

#include "Components\TransformComponent.h"

ECS::Entity::Entity()
{
	this->AddComponent<ECS::Components::TransformComponent>();
}

ECS::Entity::~Entity()
{
	for (auto *it : components)
	{
		delete it;
	}
}
