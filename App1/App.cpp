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
	const uint64_t ticksPerSecond = glfwGetTimerFrequency();
	const uint64_t targetFixedTicks = ticksPerSecond / 20;
	const uint64_t maxTickDelta = ticksPerSecond / 10;
	const int64_t tickEpsilon = ticksPerSecond / 4000;
	uint64_t leftoverTicks = 0;
	uint64_t currentTicks = 0;
	uint64_t lastTicks = glfwGetTimerValue();
	uint64_t tickDelta = 0;
	uint64_t totalTicks = 0;

	auto appTime = AppTime(ticksPerSecond);

	while (!glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();
		currentTicks = glfwGetTimerValue();
		tickDelta = currentTicks - lastTicks;
		lastTicks = currentTicks;
		// cap time delta to protect against very slow moments
		if (tickDelta > maxTickDelta)
			tickDelta = maxTickDelta;
		leftoverTicks += tickDelta;
		// remove errors less than 1/4000th of a second
		if (abs((long long)(leftoverTicks - targetFixedTicks)) < tickEpsilon)
			leftoverTicks = targetFixedTicks;
		while (leftoverTicks >= targetFixedTicks)
		{
			leftoverTicks -= targetFixedTicks;
			totalTicks += targetFixedTicks;
			appTime.SetElapsedTicks(targetFixedTicks);
			appTime.SetTotalTicks(totalTicks);
			this->FixedUpdate(appTime);
		}
		appTime.SetElapsedTicks(tickDelta);
		appTime.SetTotalTicks(totalTicks + leftoverTicks);
		this->Update(appTime);
		this->Draw(appTime);

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

void App::FixedUpdate(const AppTime & time)
{
}

void App::Update(const AppTime & time)
{
}

void App::Draw(const AppTime & time)
{
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}
