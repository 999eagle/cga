#pragma once

class CollisionShape
{
public:
	virtual std::shared_ptr<btCollisionShape> GetCollisionShape() = 0;
protected:
	std::shared_ptr<btCollisionShape> shapePtr;
};

class BoxCollisionShape : public CollisionShape
{
public:
	BoxCollisionShape(float x, float y, float z)
		: halfExtents(x / 2, y / 2, z / 2)
	{ }

	std::shared_ptr<btCollisionShape> GetCollisionShape()
	{
		if (shapePtr.get() == NULL)
			shapePtr = std::shared_ptr<btCollisionShape>(new btBoxShape(this->halfExtents));
		return shapePtr;
	}
private:
	btVector3 halfExtents;
};

class CapsuleCollisionShape : public CollisionShape
{
public:
	CapsuleCollisionShape(float radius, float height)
		: radius(radius), height(height)
	{ }

	std::shared_ptr<btCollisionShape> GetCollisionShape()
	{
		if (shapePtr.get() == NULL)
			shapePtr = std::shared_ptr<btCollisionShape>(new btCapsuleShape(this->radius, this->height));
		return shapePtr;
	}
private:
	btScalar radius, height;
};
