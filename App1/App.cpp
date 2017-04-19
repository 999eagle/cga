#include "pch.h"
#include "App.h"

#include "Importer.h"

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

	this->deferredRenderer = std::make_unique<DeferredRenderer>(width, height);
	this->postProcessing = std::make_unique<PostProcessing>(width, height);
	this->camera = std::unique_ptr<Camera>(new Camera(this->window, width, height, 45.0, 0.1, 100.0));

	App::currentApp = this;
	return true;
}

void App::LoadContent()
{
	this->lights.push_back(std::unique_ptr<ILight>(new AmbientLight(glm::vec3(0.005, 0.005, 0.005))));
	this->lights.push_back(std::unique_ptr<ILight>(new PointLight(glm::vec3(0.3, 0.3, 0.3), glm::vec3(1.0, 1.0, 0.0), 2.5)));
	this->ground = ModelImporter::GetInstance().LoadModel("Content\\Model\\ground.obj");
	this->ground->meshes[0].second = MaterialImporter::GetInstance().LoadMaterial("Content\\Material\\rustediron2\\material.mat");
	this->nanosuit = ModelImporter::GetInstance().LoadModel("Content\\Model\\nanosuit\\nanosuit.obj");
	this->gammaPostProc = std::make_unique<GammaPostProcessing>();
}

void App::UnloadContent()
{
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

	this->camera->Update(time, this->window);
}

void App::Draw(const AppTime & time)
{
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glm::mat4 world;
	GLint worldMatrixLocation = this->deferredRenderer->GetWorldMatrixLocation();
	this->deferredRenderer->StartGeometryPass(this->camera->GetViewProj());

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, glm::value_ptr(world));
	this->ground->Draw(this->deferredRenderer->GetGeometryShader());

	world = glm::scale(world, glm::vec3(0.1, 0.1, 0.1));
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, glm::value_ptr(world));
	this->nanosuit->Draw(this->deferredRenderer->GetGeometryShader());

	this->deferredRenderer->EndGeometryPass();
	this->postProcessing->BindFramebuffer();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	this->deferredRenderer->StartLightPass();

	for (auto it = this->lights.begin(); it != this->lights.end(); it++)
	{
		(*it)->Draw(this->camera->GetInverseViewProj(), this->camera->GetPosition());
	}

	this->deferredRenderer->EndLightPass();
	this->postProcessing->BindFramebuffer();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	this->gammaPostProc->Draw();
}
