#include "pch.h"
#include "Entity.h"

ECS::Entity::~Entity()
{
	for (auto *it : components)
	{
		delete it;
	}
}
