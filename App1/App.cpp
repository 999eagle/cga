#include "pch.h"
#include "App.h"

#include "Importer.h"
#include "ECS\Systems\RenderSystem.h"
#include "ECS\Components\LightComponent.h"
#include "ECS\Components\TransformComponent.h"
#include "ECS\Components\ModelComponents.h"
#include "ECS\Components\CameraComponent.h"
#include "Lights\AmbientLight.h"
#include "Lights\PointLight.h"
#include "ECS\Systems\ScriptSystem.h"

#include "Scripts\CameraInputScript.h"

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

	this->world = std::make_unique<ECS::World>();
	this->world->AddSystem<ECS::Systems::ScriptSystem>(this->window);
	this->world->AddSystem<ECS::Systems::RenderSystem>(width, height);

	App::currentApp = this;
	return true;
}

void App::LoadContent()
{
	auto e = new ECS::Entity();
	e->AddComponent<ECS::Components::CameraComponent>(45.0f, 1270.f / 720.f, 0.1f, 100.0f);
	e->GetComponent<ECS::Components::TransformComponent>()->SetLocalTransform(glm::translate(glm::mat4(), glm::vec3(0.f, 1.f, 3.f)));
	e->AddComponent<ECS::Components::ScriptComponent>();
	e->GetComponent<ECS::Components::ScriptComponent>()->AddScript<Scripts::CameraInputScript>(this->window);
	this->world->AddEntity(e);

	e = new ECS::Entity();
	auto lightRootTransform = e->GetComponent<ECS::Components::TransformComponent>();
	e->AddComponent<ECS::Components::LightComponent>(new AmbientLight(glm::vec3(0.005f, 0.005f, 0.005f)));
	this->world->AddEntity(e);
	e = new ECS::Entity();
	e->AddComponent<ECS::Components::LightComponent>(new PointLight(glm::vec3(0.0f, 0.3f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), 0.9f));
	e->GetComponent<ECS::Components::TransformComponent>()->SetParent(lightRootTransform);
	this->world->AddEntity(e);
	e = new ECS::Entity();
	e->AddComponent<ECS::Components::LightComponent>(new PointLight(glm::vec3(0.3f, 0.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), 0.9f));
	e->GetComponent<ECS::Components::TransformComponent>()->SetParent(lightRootTransform);
	this->world->AddEntity(e);
	e = new ECS::Entity();
	e->AddComponent<ECS::Components::LightComponent>(new PointLight(glm::vec3(0.0f, 0.0f, 0.3f), glm::vec3(0.0f, 1.0f, 1.0f), 0.9f));
	e->GetComponent<ECS::Components::TransformComponent>()->SetParent(lightRootTransform);
	this->world->AddEntity(e);
	e = new ECS::Entity();
	e->AddComponent<ECS::Components::LightComponent>(new PointLight(glm::vec3(0.3f, 0.3f, 0.0f), glm::vec3(0.0f, 1.0f, -1.0f), 0.9f));
	e->GetComponent<ECS::Components::TransformComponent>()->SetParent(lightRootTransform);
	this->world->AddEntity(e);

	e = ModelImporter::GetInstance().LoadModel(this->world.get(), "Content\\Model\\ground.obj");
	e = (*e->GetComponent<ECS::Components::TransformComponent>()->GetChildren().begin())->GetEntity();
	e->GetComponent<ECS::Components::MaterialComponent>()->material = MaterialImporter::GetInstance().LoadMaterial("Content\\Material\\rustediron2\\material.mat");

	e = ModelImporter::GetInstance().LoadModel(this->world.get(), "Content\\Model\\nanosuit\\nanosuit.obj");
	e->GetComponent<ECS::Components::TransformComponent>()->SetLocalTransform(glm::scale(glm::mat4(), glm::vec3(0.1f, 0.1f, 0.1f)));
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
	this->world->FixedUpdate(time);
}

void App::Update(const AppTime & time)
{
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, GL_TRUE);

	//this->camera->Update(time, this->window);
	this->world->Update(time);
}
