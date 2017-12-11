#include "pch.h"
#include "PhysicsSystem.h"

#include "..\Components\RigidBodyComponent.h"
#include "..\Components\TransformComponent.h"
#include "..\..\CollisionShapes.h"

ATTRIBUTE_ALIGNED16(struct) ECSMotionState : public btMotionState
{
	btTransform centerOfMassOffset;
	ECS::Components::TransformComponent * transformComp;

	BT_DECLARE_ALIGNED_ALLOCATOR();

	ECSMotionState(ECS::Components::TransformComponent * transformComponent, const btTransform & centerOfMassOffset = btTransform::getIdentity())
		: transformComp(transformComponent), centerOfMassOffset(centerOfMassOffset)
	{
	}

	virtual void getWorldTransform(btTransform & worldTrans) const
	{
		auto trans = btTransform();
		trans.setFromOpenGLMatrix(glm::value_ptr(transformComp->GetWorldTransform()));
		worldTrans = trans * centerOfMassOffset.inverse();
	}

	virtual void setWorldTransform(const btTransform & worldTrans)
	{
		auto trans = worldTrans * centerOfMassOffset;
		glm::mat4 worldMat;
		trans.getOpenGLMatrix(glm::value_ptr(worldMat));
		transformComp->SetWorldTransform(worldMat);
	}
};

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
	for (auto * it : world.GetEntities())
	{
		auto rbComp = it->GetComponent<ECS::Components::RigidBodyComponent>();
		auto transformComp = it->GetComponent<ECS::Components::TransformComponent>();
		if (rbComp != NULL)
		{
			if (rbComp->rigidBody == NULL)
			{
				rbComp->motionState = new ECSMotionState(transformComp, rbComp->centerOfMassOffset);
				rbComp->rigidBody = new btRigidBody(rbComp->mass, rbComp->motionState, rbComp->collisionShape->GetCollisionShape().get());
				if (rbComp->mass == 0 && rbComp->isKinematic)
				{
					rbComp->rigidBody->setCollisionFlags(rbComp->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
					rbComp->rigidBody->setActivationState(DISABLE_DEACTIVATION);
				}
				this->physicsWorld->addRigidBody(rbComp->rigidBody);
			}
		}
	}
}
