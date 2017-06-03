#pragma once

#include "..\System.h"
#include "..\World.h"

namespace ECS { namespace Systems {
	class PhysicsSystem : public System
	{
	public:
		PhysicsSystem();
		~PhysicsSystem();

		void Update(World & world, const AppTime & time);
		void FixedUpdate(World & world, const AppTime & time) { }

	private:
		btDynamicsWorld * physicsWorld;
		btCollisionConfiguration * collisionConfiguration;
		btDispatcher * dispatcher;
		btBroadphaseInterface * pairCache;
		btConstraintSolver * constraintSolver;
	};
} }
