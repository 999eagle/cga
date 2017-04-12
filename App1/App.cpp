#include "pch.h"
#include "App.h"

App* App::currentApp = NULL;

App::App()
{
}

App::~App()
{
	if (this->window != NULL)
	{
		glfwDestroyWindow(this->window);
		this->window = NULL;
	}
	if (this->glfwInitialized)
	{
		glfwTerminate();
		this->glfwInitialized = false;
	}
	App::currentApp = NULL;
}

bool App::Initialize(int width, int height, const char* title)
{
	if (App::currentApp != NULL)
		return false;
	if (!glfwInit())
		return false;
	this->glfwInitialized = true;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	this->window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (this->window == NULL)
		return false;
	glfwMakeContextCurrent(this->window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		return false;

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0, 0, fbWidth, fbHeight);

	glfwSetKeyCallback(window, App::StaticKeyCallback);

	App::currentApp = this;
	return true;
}

void App::GameLoop()
{
	while (!glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(this->window);
	}
}

void App::StaticKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	App::currentApp->KeyCallback(window, key, scancode, action, mode);
}

void App::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
