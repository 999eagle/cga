#pragma once

#include "..\AppTime.h"

namespace ECS
{
	class World;

	class System
	{
	public:
		typedef uint8_t id_t;
		virtual ~System() { }
		virtual void Update(World & world, const AppTime & time) = 0;
		virtual void FixedUpdate(World & world, const AppTime & time) = 0;
	protected:
		static id_t nextId;
	};

	template<typename T>
	class TemplateSystem : public System
	{
	public:
		static id_t getId()
		{
			static id_t id = ECS::System::nextId++;
			return id;
		}
	};
}
