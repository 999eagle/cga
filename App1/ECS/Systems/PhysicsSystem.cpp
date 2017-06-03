#include "pch.h"
#include "PhysicsSystem.h"

ECS::Systems::PhysicsSystem::PhysicsSystem()
{
	this->collisionConfiguration = new btDefaultCollisionConfiguration();
	this->dispatcher = new btCollisionDispatcher(this->collisionConfiguration);
	this->pairCache = new btDbvtBroadphase();
	this->constraintSolver = new btSequentialImpulseConstraintSolver();
	this->physicsWorld = new btDiscreteDynamicsWorld(this->dispatcher, this->pairCache, this->constraintSolver, this->collisionConfiguration);
}

ECS::Systems::PhysicsSystem::~PhysicsSystem()
{
	delete this->physicsWorld;
	delete this->constraintSolver;
	delete this->pairCache;
	delete this->dispatcher;
	delete this->collisionConfiguration;
}

void ECS::Systems::PhysicsSystem::Update(World & world, const AppTime & time)
{
	this->physicsWorld->stepSimulation((btScalar)time.GetElapsedSeconds());
}
