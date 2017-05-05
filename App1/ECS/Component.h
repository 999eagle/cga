#pragma once

namespace ECS
{
	class Entity;
	class Component
	{
	public:
		typedef uint8_t id_t;
		Entity * GetEntity() { return this->entity; }
		void SetEntity(Entity * entity) { this->entity = entity; }
	protected:
		static id_t nextId;
		Entity * entity;
	};

	template<typename T>
	class TemplateComponent : public Component
	{
	public:
		static id_t getId()
		{
			static id_t id = ECS::Component::nextId++;
			return id;
		}
	};
}
