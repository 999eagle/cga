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

void App::LoadContent()
{
	this->simpleShader = std::make_unique<Shader>("Shader\\simple.vs.glsl", "Shader\\simple.fs.glsl");

	this->vertices.push_back(VertexPositionColor{ glm::vec3(-0.5, 0.5, 0.0), glm::vec3(1.0, 0.0, 0.0) });
	this->vertices.push_back(VertexPositionColor{ glm::vec3(-0.5, -0.5, 0.0), glm::vec3(0.0, 1.0, 0.0) });
	this->vertices.push_back(VertexPositionColor{ glm::vec3(0.5, -0.5, 0.0), glm::vec3(0.0, 0.0, 1.0) });
	this->vertices.push_back(VertexPositionColor{ glm::vec3(0.5, 0.5, 0.0), glm::vec3(1.0, 1.0, 1.0) });
	this->indices.push_back(0);
	this->indices.push_back(1);
	this->indices.push_back(2);
	this->indices.push_back(0);
	this->indices.push_back(2);
	this->indices.push_back(3);

	glGenVertexArrays(1, &this->varrayId);
	glGenBuffers(1, &this->vbufferId);
	glGenBuffers(1, &this->ebufferId);
	glBindVertexArray(this->varrayId);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositionColor) * this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColor), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColor), (GLvoid*)(sizeof(VertexPositionColor::color)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * this->indices.size(), this->indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void App::UnloadContent()
{
	glDeleteVertexArrays(1, &this->varrayId);
	glDeleteBuffers(1, &this->vbufferId);
	glDeleteBuffers(1, &this->ebufferId);
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

	this->LoadContent();

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

	this->UnloadContent();
}

void App::StaticKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	App::currentApp->KeyCallback(window, key, scancode, action, mode);
}

void App::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
}

void App::FixedUpdate(const AppTime & time)
{
}

void App::Update(const AppTime & time)
{
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, GL_TRUE);
}

void App::Draw(const AppTime & time)
{
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	this->simpleShader->Apply();
	glBindVertexArray(this->varrayId);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}
