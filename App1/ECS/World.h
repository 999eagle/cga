#pragma once
#include "..\AppTime.h"
#include "Entity.h"
#include "System.h"

namespace ECS
{
	class World
	{
	public:
		typedef size_t entityId_t;

		World();
		~World();

		template<typename T> bool HasSystem();
		template<typename T> T* GetSystem();
		template<typename T, typename ... U> void AddSystem(U && ... args);
		template<typename T> void RemoveSystem();

		entityId_t AddEntity(Entity* entity);
		Entity* GetEntity(entityId_t const id);
		void RemoveEntity(entityId_t const id);
		void RemoveEntity(std::vector<Entity*>::iterator it);
		std::vector<Entity*> & GetEntities();

		void Update(const AppTime & time);
		void FixedUpdate(const AppTime & time);

	private:
		std::vector<Entity*> entities;
		std::vector<System*> systems;
		std::queue<entityId_t> unusedIds;
	};
}

template<typename T>
bool ECS::World::HasSystem()
{
	auto id = ECS::TemplateSystem<T>::getId();
	return id < this->systems.size() && this->systems[id];
}

template<typename T>
T* ECS::World::GetSystem()
{
	if (!this->HasSystem<T>()) return NULL;
	return (T*)this->systems[ECS::TemplateSystem<T>::getId()];
}

template<typename T, typename ... U>
void ECS::World::AddSystem(U && ... args)
{
	if (this->HasSystem<T>()) return;
	auto id = ECS::TemplateSystem<T>::getId();
	if (id >= this->systems.size())
	{
		this->systems.resize(id + 1);
	}
	this->systems[id] = new T(std::forward<U>(args) ...);
}

template<typename T>
void ECS::World::RemoveSystem()
{
	if (!this->HasSystem<T>()) return;
	auto id = ECS::TemplateSystem<T>::getId();
	delete this->systems[id];
	this->systems[id] = NULL;
}
