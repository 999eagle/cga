#pragma once
#include "..\Component.h"

namespace ECS { namespace Components {
	class TransformComponent : public Component
	{
	public:
		TransformComponent(const glm::mat4 & matrix) : matrix(matrix) { }
		TransformComponent() : TransformComponent(glm::mat4()) { }

		glm::mat4 & GetLocalTransform() { return this->matrix; }
		void SetLocalTransform(const glm::mat4 & matrix) { this->matrix = matrix; }
		glm::mat4 GetWorldTransform()
		{
			if (this->parent == NULL) return this->matrix;
			return this->parent->GetWorldTransform() * this->matrix;
		}
		void SetWorldTransform(const glm::mat4 & matrix)
		{
			if (this->parent == NULL)
			{
				this->matrix = matrix;
			}
			else
			{
				this->matrix = glm::inverse(this->parent->GetWorldTransform()) * matrix;
			}
		}
		TransformComponent * GetParent() { return this->parent; }
		void SetParent(TransformComponent * newParent, bool keepWorldTransform = false)
		{
			if (this->parent != NULL)
			{
				this->parent->children.erase(this);
			}
			if (keepWorldTransform)
			{
				auto world = GetWorldTransform();
				this->parent = newParent;
				this->SetWorldTransform(world);
			}
			else
			{
				this->parent = newParent;
			}
			this->parent->children.insert(this);
		}
		const std::set<TransformComponent*> & GetChildren() { return this->children; }
	private:
		glm::mat4 matrix;
		TransformComponent * parent;
		std::set<TransformComponent*> children;
	};
}}
