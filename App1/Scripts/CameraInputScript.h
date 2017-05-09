#pragma once

#include "..\ECS\Components\ScriptComponent.h"
#include "..\ECS\Components\TransformComponent.h"

namespace Scripts
{
	class CameraInputScript : public ECS::Components::Script
	{
	public:
		CameraInputScript(GLFWwindow * window)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwGetCursorPos(window, &this->lastX, &this->lastY);
		}

		void Start()
		{
			auto entity = this->component->GetEntity();
			this->transformComponent = entity->GetComponent<ECS::Components::TransformComponent>();

			this->position = this->transformComponent->GetWorldTransform() * glm::vec4(0.f, 0.f, 0.f, 1.f);
		}
		void FixedUpdate(const AppTime & time) { }
		void Update(const AppTime & time)
		{
			float rotationSpeed = (float)(time.GetElapsedSeconds() * 0.05);
			float moveSpeed = (float)(time.GetElapsedSeconds() * 2);

			auto window = this->data->GetWindow();
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			float yaw = (float)(mouseX - this->lastX) * rotationSpeed;
			float pitch = (float)(mouseY - this->lastY) * rotationSpeed;
			pitch = glm::clamp(this->currentPitch + pitch, -this->maxPitch, this->maxPitch) - this->currentPitch;
			this->lastX = mouseX;
			this->lastY = mouseY;
			this->currentPitch += pitch;
			this->currentYaw += yaw;

			glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
			glm::vec3 forward = glm::rotate(glm::mat4(), -this->currentYaw, up) * glm::vec4(0.f, 0.f, -1.f, 0.f);
			glm::vec3 right = glm::cross(forward, up);

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				this->position += forward * moveSpeed;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				this->position -= forward * moveSpeed;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				this->position += right * moveSpeed;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				this->position -= right * moveSpeed;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				this->position += up * moveSpeed;
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
				this->position -= up * moveSpeed;

			auto newTransform = glm::translate(glm::mat4(), this->position);
			newTransform = glm::rotate(newTransform, -this->currentYaw, glm::vec3(0.f, 1.f, 0.f));
			newTransform = glm::rotate(newTransform, -this->currentPitch, glm::vec3(1.f, 0.f, 0.f));
			this->transformComponent->SetWorldTransform(newTransform);
		}
	private:
		double lastX, lastY;
		float currentPitch = 0.f, currentYaw = 0.f;
		const float maxPitch = 0.99f * glm::half_pi<float>();
		glm::vec3 position;
		ECS::Components::TransformComponent * transformComponent;
	};
}
