#pragma once

#include "AppTime.h"

class Camera
{
public:
	Camera(GLFWwindow* window, int width, int height, float fov, float nearPlane, float farPlane) :
		aspectRatio((float)width / (float)height), fov(fov), nearPlane(nearPlane), farPlane(farPlane),
		position(0.0, 1.0, 3.0), forward(0.0, 0.0, -1.0), up(0.0, 1.0, 0.0), currentPitch(0.0)
	{
		this->CalcProj();
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetCursorPos(window, &this->lastCursorX, &this->lastCursorY);
	}
	~Camera() {}
	void Update(const AppTime & time, GLFWwindow * window);
	const glm::mat4 & GetViewProj() { return viewProj; }
	const glm::mat4 & GetInverseViewProj() { return invViewProj; }
	const glm::vec3 & GetPosition() { return position; }
	const glm::vec3 & GetForward() { return forward; }
	const glm::vec3 & GetUp() { return up; }
private:
	const float maxPitch = 0.99f * glm::half_pi<float>();
	float currentPitch;
	float aspectRatio, fov, nearPlane, farPlane;
	double lastCursorX, lastCursorY;
	glm::vec3 position, forward, up;
	glm::mat4 view, proj, viewProj, invViewProj;
	void CalcProj()
	{
		this->proj = glm::perspective(this->fov, this->aspectRatio, this->nearPlane, this->farPlane);
	}
	void CalcView()
	{
		this->view = glm::lookAt(this->position, this->position + this->forward, this->up);
	}
};

