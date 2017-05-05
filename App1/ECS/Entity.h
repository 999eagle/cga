#pragma once

#include "Component.h"

namespace ECS
{
	class Entity
	{
	private:
		std::vector<ECS::Component *> components;

	public:
		Entity() { }
		~Entity();

		template<typename T> T* GetComponent();
		template<typename T> bool HasComponent();
		template<typename T, typename ... U> void AddComponent(U && ... args);
		template<typename T> void RemoveComponent();
	};
}

template<typename T>
bool ECS::Entity::HasComponent()
{
	auto id = ECS::TemplateComponent<T>::getId();
	return id < this->components.size() && this->components[id];
}

template<typename T>
T* ECS::Entity::GetComponent()
{
	if (!this->HasComponent<T>()) return NULL;
	return (T*)this->components[ECS::TemplateComponent<T>::getId()];
}

template<typename T, typename ... U>
void ECS::Entity::AddComponent(U && ... args)
{
	if (this->HasComponent<T>()) return;
	auto id = ECS::TemplateComponent<T>::getId();
	if (id >= this->components.size())
	{
		this->components.resize(id + 1);
	}
	this->components[id] = new T(std::forward<U>(args) ...);
}

template<typename T>
void ECS::Entity::RemoveComponent()
{
	if (!this->HasComponent<T>()) return;
	auto id = ECS::TemplateComponent<T>::getId();
	delete this->components[id];
	this->components[id] = NULL;
}
