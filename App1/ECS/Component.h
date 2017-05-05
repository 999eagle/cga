#pragma once

namespace ECS
{
	class Component
	{
	public:
		typedef uint8_t id_t;
	protected:
		static id_t nextId;
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
