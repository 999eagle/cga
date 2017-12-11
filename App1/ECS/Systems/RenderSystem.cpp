#include "pch.h"
#include "RenderSystem.h"

#include "..\..\Common\util.h"
#include "..\Components\ModelComponents.h"
#include "..\Components\TransformComponent.h"
#include "..\Components\LightComponent.h"
#include "..\Components\CameraComponent.h"

using namespace ECS::Systems;

RenderSystem::RenderSystem(GLsizei windowWidth, GLsizei windowHeight, vr::IVRSystem * vr)
	: windowWidth(windowWidth), windowHeight(windowHeight), vr(vr)
{
	if (this->vr != NULL)
	{
		uint32_t width, height;
		this->vr->GetRecommendedRenderTargetSize(&width, &height);
		this->windowWidth = width;
		this->windowHeight = height;
		this->rightRenderer = new DeferredRenderer(this->windowWidth, this->windowHeight);

		glm::mat4 eye;
		vrMatrixToGlm(eye, this->vr->GetEyeToHeadTransform(vr::Eye_Left));
		this->eyePoseLeft = glm::inverse(eye);
		vrMatrixToGlm(eye, this->vr->GetEyeToHeadTransform(vr::Eye_Right));
		this->eyePoseRight = glm::inverse(eye);
	}
	this->leftRenderer = new DeferredRenderer(this->windowWidth, this->windowHeight);
	this->postProcessing = new PostProcessing(this->windowWidth, this->windowHeight);
	this->gammaPost = new GammaPostProcessing();
	this->hdrPost = new HdrPostProcessing();
	this->bloomPost = new BloomPostProcessing(this->windowWidth, this->windowHeight);
	this->lightShaderAmbient = new Shader("Shader\\passthrough.vs.glsl", "Shader\\ambientLight.fs.glsl");
	this->lightShaderPoint = new Shader("Shader\\passthrough.vs.glsl", "Shader\\pointLight.fs.glsl");
	this->lightShaderDirectional = new Shader("Shader\\passthrough.vs.glsl", "Shader\\directionalLight.fs.glsl");
	this->shadowMapShader = new Shader("Shader\\simple.vs.glsl", "Shader\\empty.fs.glsl");
}

RenderSystem::~RenderSystem()
{
	delete this->leftRenderer;
	if (this->vr != NULL)
	{
		delete this->rightRenderer;
	}
	delete this->postProcessing;
	delete this->gammaPost;
	delete this->hdrPost;
	delete this->bloomPost;
	delete this->lightShaderAmbient;
	delete this->lightShaderPoint;
	delete this->lightShaderDirectional;
	delete this->shadowMapShader;
}

void RenderSystem::Update(ECS::World & world, const AppTime & time)
{
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	ECS::Components::CameraComponent * camera = NULL;
	for (auto * it : world.GetEntities())
	{
		auto c = it->GetComponent<ECS::Components::CameraComponent>();
		if (c != NULL)
		{
			camera = c;
			break;
		}
	}
	if (camera == NULL) return;

	this->UpdateVRTracking();


	auto cameraTransform = camera->GetEntity()->GetComponent<ECS::Components::TransformComponent>()->GetWorldTransform();
	glm::mat4 projLeft, projRight;
	glm::mat4 view, proj, viewProj, invViewProj;
	glm::vec3 camPos;
	view = glm::inverse(cameraTransform);
	if (this->vr != NULL)
	{
		vrMatrixToGlm(projLeft, this->vr->GetProjectionMatrix(vr::Eye_Left, camera->GetNearPlane(), camera->GetFarPlane()));
		vrMatrixToGlm(projRight, this->vr->GetProjectionMatrix(vr::Eye_Right, camera->GetNearPlane(), camera->GetFarPlane()));
		proj = projLeft;
		view = this->eyePoseLeft * this->hmdPose * view;
		camPos = glm::inverse(view) * glm::vec4(0, 0, 0, 1);
	}
	else
	{
		proj = glm::perspective(camera->GetFOV(), camera->GetAspectRatio(), camera->GetNearPlane(), camera->GetFarPlane());
		camPos = cameraTransform * glm::vec4(0, 0, 0, 1);
	}
	viewProj = proj * view;
	invViewProj = glm::inverse(viewProj);

	GLint worldMatrixLocation = this->leftRenderer->GetWorldMatrixLocation();
	this->leftRenderer->StartGeometryPass(viewProj);
	auto geometryShader = this->leftRenderer->GetGeometryShader();

	std::vector<ECS::Components::LightComponent*> ambientLights;
	std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> pointLights;
	std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> directionalLights;
	std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> shadowMapLights;
	std::vector<std::pair<ECS::Components::MeshComponent<VertexType>*, glm::mat4>> meshes;

	for (auto * it : world.GetEntities())
	{
		auto mesh = it->GetComponent<ECS::Components::MeshComponent<VertexType>>();
		auto material = it->GetComponent<ECS::Components::MaterialComponent>();
		auto light = it->GetComponent<ECS::Components::LightComponent>();
		auto transform = it->GetComponent<ECS::Components::TransformComponent>()->GetWorldTransform();
		if (mesh != NULL && material != NULL)
		{
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, glm::value_ptr(transform));
			material->material->Use(geometryShader);
			mesh->mesh->Draw();
			meshes.push_back(std::make_pair(mesh, transform));
		}
		if (light != NULL)
		{
			switch (light->type)
			{
			case ECS::Components::LightType_Ambient:
				ambientLights.push_back(light);
				break;
			case ECS::Components::LightType_Point:
				pointLights.push_back(std::make_pair(light, transform));
				break;
			case ECS::Components::LightType_Directional:
				directionalLights.push_back(std::make_pair(light, transform));
				break;
			}
			if (light->shadowMapResolution > 0)
			{
				shadowMapLights.push_back(std::make_pair(light, transform));
			}
		}
	}

	this->leftRenderer->EndGeometryPass();

	if (!shadowMapLights.empty())
	{
		this->shadowMapShader->Apply();
		auto worldLocation = this->shadowMapShader->GetUniformLocation("world");
		auto viewProjLocation = this->shadowMapShader->GetUniformLocation("viewProj");
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		for (auto it : shadowMapLights)
		{
			glViewport(0, 0, it.first->shadowMapResolution, it.first->shadowMapResolution);
			glBindFramebuffer(GL_FRAMEBUFFER, it.first->shadowMapFramebufferId);
			glClearDepth(1.f);
			glClear(GL_DEPTH_BUFFER_BIT);
			auto viewProj = glm::ortho(-10.f, 10.f, -10.f, 10.f) * glm::inverse(it.second);
			glUniformMatrix4fv(viewProjLocation, 1, GL_FALSE, glm::value_ptr(viewProj));
			for (auto mesh : meshes)
			{
				glUniformMatrix4fv(worldLocation, 1, GL_FALSE, glm::value_ptr(mesh.second));
				mesh.first->mesh->Draw();
			}
		}
		glViewport(0, 0, this->windowWidth, this->windowHeight);
		glDisable(GL_DEPTH_TEST);
	}

	this->postProcessing->BindFramebuffer();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	this->postProcessing->Swap();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	this->leftRenderer->StartLightPass();

	auto quadRenderer = QuadRenderer::GetInstance();
	if (!ambientLights.empty())
	{
		this->ApplyLightShader(this->lightShaderAmbient, camPos, invViewProj);
		auto colorLocation = this->lightShaderAmbient->GetUniformLocation("lightColor");
		for (auto * it : ambientLights)
		{
			glUniform3fv(colorLocation, 1, glm::value_ptr(it->color));
			quadRenderer.DrawFullscreenQuad();
		}
	}
	if (!pointLights.empty())
	{
		this->ApplyLightShader(this->lightShaderPoint, camPos, invViewProj);
		auto colorLocation = this->lightShaderPoint->GetUniformLocation("lightColor");
		auto positionLocation = this->lightShaderPoint->GetUniformLocation("lightPosition");
		auto radiusLocation = this->lightShaderPoint->GetUniformLocation("lightRadius");
		for (auto it : pointLights)
		{
			auto position = it.second * glm::vec4(0.f, 0.f, 0.f, 1.f);
			auto scale = it.second[0].x;
			glUniform3fv(colorLocation, 1, glm::value_ptr(it.first->color));
			glUniform3fv(positionLocation, 1, glm::value_ptr(position));
			glUniform1f(radiusLocation, scale);
			quadRenderer.DrawFullscreenQuad();
		}
	}
	if (!directionalLights.empty())
	{
		this->ApplyLightShader(this->lightShaderDirectional, camPos, invViewProj);
		auto colorLocation = this->lightShaderDirectional->GetUniformLocation("lightColor");
		auto directionLocation = this->lightShaderDirectional->GetUniformLocation("lightDirection");
		auto shadowMapTextureLocation = this->lightShaderDirectional->GetUniformLocation("shadowMapTexture");
		auto viewProjLocation = this->lightShaderDirectional->GetUniformLocation("lightViewProj");
		auto enableShadowMapLocation = this->lightShaderDirectional->GetUniformLocation("enableShadowMap");
		glActiveTexture(GL_TEXTURE6);
		for (auto it : directionalLights)
		{
			if (it.first->shadowMapResolution > 0)
			{
				glBindTexture(GL_TEXTURE_2D, it.first->shadowMapTextureId);
				glUniform1i(shadowMapTextureLocation, 6);
				glUniform1i(enableShadowMapLocation, GL_TRUE);
				auto viewProj = glm::ortho(-10.f, 10.f, -10.f, 10.f) * glm::inverse(it.second);
				glUniformMatrix4fv(viewProjLocation, 1, GL_FALSE, glm::value_ptr(viewProj));
			}
			else
			{
				glUniform1i(enableShadowMapLocation, GL_FALSE);
			}
			auto direction = it.second * glm::vec4(0.f, 0.f, -1.f, 0.f);
			glUniform3fv(colorLocation, 1, glm::value_ptr(it.first->color));
			glUniform3fv(directionLocation, 1, glm::value_ptr(direction));
			quadRenderer.DrawFullscreenQuad();
		}
	}

	this->leftRenderer->EndLightPass();
	this->bloomPost->Draw(this->postProcessing);
	this->hdrPost->Draw(this->postProcessing, (float)time.GetElapsedSeconds());
	this->postProcessing->Swap(false);
	this->postProcessing->BindReadTexture();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	this->gammaPost->Draw();
}

void ECS::Systems::RenderSystem::UpdateVRTracking()
{
	if (this->vr == NULL) return;

	vr::VRCompositor()->WaitGetPoses(this->trackedDevicePoses, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		if (this->trackedDevicePoses[i].bPoseIsValid)
		{
			vrMatrixToGlm(this->trackedDeviceMatrices[i], this->trackedDevicePoses[i].mDeviceToAbsoluteTracking);
		}
	}
	if (this->trackedDevicePoses[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
	{
		this->hmdPose = glm::inverse(this->trackedDeviceMatrices[vr::k_unTrackedDeviceIndex_Hmd]);
	}
}

void ECS::Systems::RenderSystem::ApplyLightShader(Shader * shader, const glm::vec3 & cameraPosition, const glm::mat4 & invViewProj)
{
	shader->Apply();
	glUniform1i(shader->GetUniformLocation("colorTexture"), 0);
	glUniform1i(shader->GetUniformLocation("normalTexture"), 1);
	glUniform1i(shader->GetUniformLocation("depthTexture"), 2);
	glUniformMatrix4fv(shader->GetUniformLocation("invViewProj"), 1, GL_FALSE, glm::value_ptr(invViewProj));
	glUniform3fv(shader->GetUniformLocation("cameraPosition"), 1, glm::value_ptr(cameraPosition));
}
