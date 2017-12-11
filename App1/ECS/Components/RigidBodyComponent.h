#pragma once
#include "..\Component.h"
#include "..\..\CollisionShapes.h"

namespace ECS { namespace Components {
	class RigidBodyComponent : public Component
	{
	public:
		RigidBodyComponent(std::shared_ptr<CollisionShape> collisionShape, const btTransform & centerOfMassOffset = btTransform::getIdentity(), float mass = 0.f, bool isKinematic = true)
			: collisionShape(collisionShape), centerOfMassOffset(centerOfMassOffset), mass(mass), isKinematic(isKinematic)
		{
		}
		~RigidBodyComponent()
		{
			if (this->rigidBody != NULL)
			{
				delete this->rigidBody;
			}
			if (this->motionState != NULL)
			{
				delete this->motionState;
			}
		}

		btRigidBody * rigidBody = NULL;
		btMotionState * motionState = NULL;
		std::shared_ptr<CollisionShape> collisionShape;
		btTransform centerOfMassOffset;
		float mass;
		bool isKinematic;
	};
}}
