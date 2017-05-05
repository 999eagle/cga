#include "pch.h"

#include "World.h"

using namespace ECS;

World::World()
{
}

World::~World()
{
	for (auto *it : systems)
	{
		delete it;
	}
	for (auto *it : entities)
	{
		delete it;
	}
}

void World::Update(const AppTime & appTime)
{
	for (auto *it : systems)
	{
		it->Update(*this, appTime);
	}
}

void World::FixedUpdate(const AppTime & appTime)
{
	for (auto *it : systems)
	{
		it->FixedUpdate(*this, appTime);
	}
}

World::entityId_t World::AddEntity(Entity * entity)
{
	if (this->unusedIds.empty())
	{
		this->entities.push_back(entity);
		return this->entities.size() - 1;
	}
	else
	{
		auto id = this->unusedIds.front();
		this->entities[id] = entity;
		this->unusedIds.pop();
		return id;
	}
}

Entity* World::GetEntity(World::entityId_t const id)
{
	return this->entities[id];
}

void World::RemoveEntity(World::entityId_t const id)
{
	delete this->entities[id];
	this->entities[id] = NULL;
	this->unusedIds.push(id);
}

void World::RemoveEntity(std::vector<Entity*>::iterator it)
{
	delete *it;
	*it = NULL;
	this->unusedIds.push(std::distance(this->entities.begin(), it));
}
