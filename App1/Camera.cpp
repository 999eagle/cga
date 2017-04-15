#include "pch.h"
#include "Camera.h"

void Camera::Update(const AppTime & time, GLFWwindow* window)
{
	float rotationSpeed = (float)(time.GetElapsedSeconds() * 0.05);
	float moveSpeed = (float)(time.GetElapsedSeconds() * 2);

	auto rightAxis = glm::normalize(glm::cross(this->forward, this->up));

	double cursorX, cursorY;
	glfwGetCursorPos(window, &cursorX, &cursorY);
	float yaw = (float)(cursorX - this->lastCursorX) * rotationSpeed;
	float pitch = (float)(cursorY - this->lastCursorY) * rotationSpeed;
	pitch = glm::clamp(this->currentPitch + pitch, -this->maxPitch, this->maxPitch) - this->currentPitch;
	this->forward = glm::vec4(this->forward, 1.0) * glm::mat4_cast(glm::angleAxis(yaw, this->up) * glm::angleAxis(pitch, rightAxis));
	this->lastCursorX = cursorX;
	this->lastCursorY = cursorY;
	this->currentPitch += pitch;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		this->position += this->forward * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		this->position -= this->forward * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		this->position += rightAxis * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		this->position -= rightAxis * moveSpeed;

	CalcView();
	this->viewProj = this->proj * this->view;
	this->invViewProj = glm::inverse(this->viewProj);

}
