#include "pch.h"
#include "App.h"

#include "Importer.h"
#include "ECS\Systems\RenderSystem.h"
#include "ECS\Components\LightComponent.h"
#include "ECS\Components\TransformComponent.h"
#include "ECS\Components\ModelComponents.h"
#include "ECS\Components\CameraComponent.h"
#include "ECS\Systems\ScriptSystem.h"
#include "ECS\Systems\PhysicsSystem.h"
#include "ECS\Components\RigidBodyComponent.h"
#include "CollisionShapes.h"

#include "Scripts\CameraInputScript.h"
#include "Common\util.h"

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
	if (this->vr != NULL)
	{
		vr::VR_Shutdown();
		this->vr = NULL;
	}
	if (this->glfwInitialized)
	{
		glfwTerminate();
		this->glfwInitialized = false;
	}
	App::currentApp = NULL;
}

bool App::Initialize(int width, int height, const char* title, bool useVr)
{
	if (App::currentApp != NULL)
		return false;
	if (!glfwInit())
		return false;
	this->glfwInitialized = true;
	if (useVr)
	{
		vr::EVRInitError error = vr::EVRInitError::VRInitError_None;
		this->vr = vr::VR_Init(&error, vr::EVRApplicationType::VRApplication_Scene);
		if (error != vr::VRInitError_None)
		{
			this->vr = NULL;
			return false;
		}
	}
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
	this->world->AddSystem<ECS::Systems::RenderSystem>(width, height, this->vr);
	this->world->AddSystem<ECS::Systems::PhysicsSystem>();

	App::currentApp = this;
	return true;
}

void App::LoadContent()
{
	ECS::Components::TransformComponent * transformComponent;
	int width, height;
	glfwGetWindowSize(this->window, &width, &height);

	auto e = new ECS::Entity();
	e->AddComponent<ECS::Components::CameraComponent>(45.0f, (float)width / (float)height, 0.1f, 5.0f);
	e->GetComponent<ECS::Components::TransformComponent>()->SetLocalTransform(glm::translate(glm::mat4(), glm::vec3(0.f, 1.f, 3.f)));
	e->AddComponent<ECS::Components::ScriptComponent>();
	e->GetComponent<ECS::Components::ScriptComponent>()->AddScript<Scripts::CameraInputScript>(this->window);
	this->world->AddEntity(e);

	// directional light
	e = new ECS::Entity();
	transformComponent = e->GetComponent<ECS::Components::TransformComponent>();
	transformComponent->SetLocalTransform(glm::mat4_cast(rotateForwardTo(glm::vec3(-1.f, -1.f, 0.f))));
	e->AddComponent<ECS::Components::LightComponent>(ECS::Components::LightType_Directional, glm::vec3(20.f, 20.f, 20.f), 4096);
	this->world->AddEntity(e);

	// point lights
	e = new ECS::Entity();
	auto lightRootTransform = e->GetComponent<ECS::Components::TransformComponent>();
	this->world->AddEntity(e);
	e = new ECS::Entity();
	transformComponent = e->GetComponent<ECS::Components::TransformComponent>();
	transformComponent->SetParent(lightRootTransform);
	transformComponent->SetLocalTransform(glm::scale(glm::translate(glm::mat4(), glm::vec3(1.f, 1.f, 0.f)), glm::vec3(.9f, .9f, .9f)));
	e->AddComponent<ECS::Components::LightComponent>(ECS::Components::LightType_Point, glm::vec3(.0f, .3f, .0f));
	this->world->AddEntity(e);
	e = new ECS::Entity();
	transformComponent = e->GetComponent<ECS::Components::TransformComponent>();
	transformComponent->SetParent(lightRootTransform);
	transformComponent->SetLocalTransform(glm::scale(glm::translate(glm::mat4(), glm::vec3(-1.f, 1.f, 0.f)), glm::vec3(.9f, .9f, .9f)));
	e->AddComponent<ECS::Components::LightComponent>(ECS::Components::LightType_Point, glm::vec3(.3f, .0f, .0f));
	e->GetComponent<ECS::Components::TransformComponent>()->SetParent(lightRootTransform);
	this->world->AddEntity(e);
	e = new ECS::Entity();
	transformComponent = e->GetComponent<ECS::Components::TransformComponent>();
	transformComponent->SetParent(lightRootTransform);
	transformComponent->SetLocalTransform(glm::scale(glm::translate(glm::mat4(), glm::vec3(0.f, 1.f, 1.f)), glm::vec3(.9f, .9f, .9f)));
	e->AddComponent<ECS::Components::LightComponent>(ECS::Components::LightType_Point, glm::vec3(.0f, .0f, .3f));
	e->GetComponent<ECS::Components::TransformComponent>()->SetParent(lightRootTransform);
	this->world->AddEntity(e);
	e = new ECS::Entity();
	transformComponent = e->GetComponent<ECS::Components::TransformComponent>();
	transformComponent->SetParent(lightRootTransform);
	transformComponent->SetLocalTransform(glm::scale(glm::translate(glm::mat4(), glm::vec3(0.f, 1.f, -1.f)), glm::vec3(.9f, .9f, .9f)));
	e->AddComponent<ECS::Components::LightComponent>(ECS::Components::LightType_Point, glm::vec3(.3f, .3f, .0f));
	e->GetComponent<ECS::Components::TransformComponent>()->SetParent(lightRootTransform);
	this->world->AddEntity(e);

	e = ModelImporter::GetInstance().LoadModel(this->world.get(), "Content\\Model\\ground.obj");
	e = (*e->GetComponent<ECS::Components::TransformComponent>()->GetChildren().begin())->GetEntity();
	e->GetComponent<ECS::Components::MaterialComponent>()->material = MaterialImporter::GetInstance().LoadMaterial("Content\\Material\\rustediron2\\material.mat");
	auto offset = btTransform::getIdentity();
	offset.setOrigin(btVector3(.0f, .005f, .1f));
	e->AddComponent<ECS::Components::RigidBodyComponent>(std::shared_ptr<CollisionShape>(new BoxCollisionShape(1.f, 0.01f, 1.f)), offset);

	e = ModelImporter::GetInstance().LoadModel(this->world.get(), "Content\\Model\\girl\\Beautiful Girl.3ds");
	transformComponent = e->GetComponent<ECS::Components::TransformComponent>();
	auto t = transformComponent->GetLocalTransform();
	t = glm::scale(glm::mat4(), glm::vec3(.05f, .05f, .05f)) * t;
	transformComponent->SetLocalTransform(t);
	offset = btTransform::getIdentity();
	offset.setOrigin(btVector3(.0f, -1.f, .0f));
	e->AddComponent<ECS::Components::RigidBodyComponent>(std::shared_ptr<CollisionShape>(new CapsuleCollisionShape(.15f, 2.f)), offset, 1.f);

	e = ModelImporter::GetInstance().LoadModel(this->world.get(), "Content\\Model\\cube\\cube.obj");
	transformComponent = e->GetComponent<ECS::Components::TransformComponent>();
	transformComponent->SetLocalTransform(glm::translate(glm::vec3(0.f, 5.f, 0.f)));
	offset = btTransform::getIdentity();
	offset.setOrigin(btVector3(.0f, .25f, .0f));
	e->AddComponent<ECS::Components::RigidBodyComponent>(std::shared_ptr<CollisionShape>(new BoxCollisionShape(.5f, .5f, .5f)), offset, 1.f, false);
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
