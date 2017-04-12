#pragma once

class App
{
public:
	App();
	~App();
	bool Initialize(int width, int height, const char* title);
	void GameLoop();
private:
	static void StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static App* currentApp;

	GLFWwindow* window;
	bool glfwInitialized = false;
};

