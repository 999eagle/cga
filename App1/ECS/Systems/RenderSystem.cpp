#include "pch.h"
#include "RenderSystem.h"

#include "..\Components\VRTrackedDeviceComponent.h"

using namespace ECS::Systems;

RenderSystem::RenderSystem(GLsizei windowWidth, GLsizei windowHeight, vr::IVRSystem * vr)
	: windowWidth(windowWidth), windowHeight(windowHeight), renderWidth(windowWidth), renderHeight(windowHeight), vr(vr)
{
	if (this->vr != NULL)
	{
		uint32_t width, height;
		this->vr->GetRecommendedRenderTargetSize(&width, &height);
		this->renderWidth = width;
		this->renderHeight = height;
	}
	this->frameBufferIds = NULL;
	this->textureIds = NULL;
	this->numFrameBuffers = 0;
	this->renderer = new DeferredRenderer(this->renderWidth, this->renderHeight);
	this->postProcessing = new PostProcessing(this->renderWidth, this->renderHeight);
	this->gammaPost = new GammaPostProcessing();
	this->hdrPost = new HdrPostProcessing();
	this->bloomPost = new BloomPostProcessing(this->renderWidth, this->renderHeight);
	this->lightShaderAmbient = new Shader("Shader\\passthrough.vs.glsl", "Shader\\ambientLight.fs.glsl");
	this->lightShaderPoint = new Shader("Shader\\passthrough.vs.glsl", "Shader\\pointLight.fs.glsl");
	this->lightShaderDirectional = new Shader("Shader\\passthrough.vs.glsl", "Shader\\directionalLight.fs.glsl");
	this->shadowMapShader = new Shader("Shader\\simple.vs.glsl", "Shader\\empty.fs.glsl");
}

RenderSystem::~RenderSystem()
{
	delete this->renderer;
	delete this->postProcessing;
	delete this->gammaPost;
	delete this->hdrPost;
	delete this->bloomPost;
	delete this->lightShaderAmbient;
	delete this->lightShaderPoint;
	delete this->lightShaderDirectional;
	delete this->shadowMapShader;

	if (this->numFrameBuffers >= 1)
	{
		glDeleteFramebuffers((GLsizei)this->numFrameBuffers, this->frameBufferIds);
		glDeleteTextures((GLsizei)this->numFrameBuffers, this->textureIds);
		free(this->frameBufferIds);
		free(this->textureIds);
	}
}

void RenderSystem::Update(ECS::World & world, const AppTime & time)
{
	std::vector<ECS::Components::LightComponent*> ambientLights;
	std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> pointLights;
	std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> directionalLights;
	std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> shadowMapLights;
	std::vector<std::tuple<ECS::Components::MeshComponent<VertexType>*, ECS::Components::MaterialComponent*, glm::mat4>> meshes;

	std::vector<std::pair<ECS::Components::CameraComponent*, ECS::Components::VRTrackedDeviceComponent*>> cameras;

	for (auto * it : world.GetEntities())
	{
		auto camera = it->GetComponent<ECS::Components::CameraComponent>();
		auto mesh = it->GetComponent<ECS::Components::MeshComponent<VertexType>>();
		auto material = it->GetComponent<ECS::Components::MaterialComponent>();
		auto light = it->GetComponent<ECS::Components::LightComponent>();
		auto transform = it->GetComponent<ECS::Components::TransformComponent>()->GetWorldTransform();
		auto vr = it->GetComponent<ECS::Components::VRTrackedDeviceComponent>();
		if (camera != NULL)
		{
			cameras.push_back(std::make_pair(camera, vr));
		}
		if (mesh != NULL && material != NULL)
		{
			meshes.push_back(std::make_tuple(mesh, material, transform));
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

	if (cameras.size() == 0)
	{
		return;
	}
	if (cameras.size() == 1)
	{
		RenderWithCamera(time, cameras[0].first, 0, meshes, ambientLights, pointLights, directionalLights, shadowMapLights);
	}
	else
	{
		this->EnsureFramebuffers(cameras.size());
		size_t i = 0;
		int leftEyeTex = -1, rightEyeTex = -1;
		for (auto it : cameras)
		{
			this->RenderWithCamera(time, it.first, this->frameBufferIds[i], meshes, ambientLights, pointLights, directionalLights, shadowMapLights);
			if (it.second != NULL && this->vr != NULL)
			{
				if (it.second->GetDevice() == VRTrackedDevice_EyeLeft)
				{
					leftEyeTex = i;
				}
				else if (it.second->GetDevice() == VRTrackedDevice_EyeRight)
				{
					rightEyeTex = i;
				}
			}
			i++;
		}
		glViewport(0, 0, this->windowWidth, this->windowHeight);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->frameBufferIds[0]);
		glBlitFramebuffer(0, 0, this->renderWidth, this->renderHeight, 0, 0, this->windowWidth, this->windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		if (leftEyeTex != -1)
		{
			vr::Texture_t tex = { (void*)(uintptr_t)(this->textureIds[leftEyeTex]), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
			vr::EVRCompositorError err = vr::VRCompositor()->Submit(vr::Eye_Left, &tex);
			if (err != vr::VRCompositorError_None)
			{
				std::cerr << "Error submitting left eye" << std::endl;
			}
		}
		if (rightEyeTex != -1)
		{
			vr::Texture_t tex = { (void*)(uintptr_t)(this->textureIds[leftEyeTex]), vr::TextureType_OpenGL, vr::ColorSpace_Auto };
			vr::EVRCompositorError err = vr::VRCompositor()->Submit(vr::Eye_Right, &tex);
			if (err != vr::VRCompositorError_None)
			{
				std::cerr << "Error submitting right eye" << std::endl;
			}
		}

		glFlush();
		glFinish();
	}
}

void RenderSystem::RenderWithCamera(const AppTime & time, ECS::Components::CameraComponent * camera, GLuint targetFB,
	std::vector<std::tuple<ECS::Components::MeshComponent<VertexType>*, ECS::Components::MaterialComponent*, glm::mat4>> meshes,
	std::vector<ECS::Components::LightComponent*> ambientLights,
	std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> pointLights,
	std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> directionalLights,
	std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> shadowMapLights)
{
	auto cameraTransform = camera->GetEntity()->GetComponent<ECS::Components::TransformComponent>()->GetWorldTransform();
	glm::mat4 view, proj, viewProj, invViewProj;
	glm::vec3 camPos;
	view = glm::inverse(cameraTransform);
	proj = camera->GetProj();
	camPos = cameraTransform * glm::vec4(0, 0, 0, 1);
	viewProj = proj * view;
	invViewProj = glm::inverse(viewProj);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	GLint worldMatrixLocation = this->renderer->GetWorldMatrixLocation();
	this->renderer->StartGeometryPass(viewProj);
	glViewport(0, 0, this->renderWidth, this->renderHeight);
	auto geometryShader = this->renderer->GetGeometryShader();

	for (auto it : meshes)
	{
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, glm::value_ptr(std::get<2>(it)));
		std::get<1>(it)->material->Use(geometryShader);
		std::get<0>(it)->mesh->Draw();
	}

	this->renderer->EndGeometryPass();

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
				glUniformMatrix4fv(worldLocation, 1, GL_FALSE, glm::value_ptr(std::get<2>(mesh)));
				std::get<0>(mesh)->mesh->Draw();
			}
		}
		glViewport(0, 0, this->renderWidth, this->renderHeight);
		glDisable(GL_DEPTH_TEST);
	}

	this->postProcessing->BindFramebuffer();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	this->postProcessing->Swap();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	this->renderer->StartLightPass();

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

	this->renderer->EndLightPass();
	this->bloomPost->Draw(this->postProcessing);
	this->hdrPost->Draw(this->postProcessing, (float)time.GetElapsedSeconds());
	this->postProcessing->Swap(false);
	this->postProcessing->BindReadTexture();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFB);
	this->gammaPost->Draw();
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

void RenderSystem::EnsureFramebuffers(size_t requiredNum)
{
	if (requiredNum <= this->numFrameBuffers) { return; }
	this->frameBufferIds = (GLuint*)realloc(this->frameBufferIds, sizeof(GLuint) * requiredNum);
	this->textureIds = (GLuint*)realloc(this->textureIds, sizeof(GLuint) * requiredNum);
	for (size_t i = this->numFrameBuffers; i < requiredNum; i++)
	{
		this->CreateFramebuffer(i);
	}
	this->numFrameBuffers = requiredNum;
}

void RenderSystem::CreateFramebuffer(size_t index)
{
	// generate buffers and textures
	glGenFramebuffers(1, this->frameBufferIds + index);
	glGenTextures(1, this->textureIds + index);

	// set up geometry frame buffer and geometry textures
	glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferIds[index]);
	glBindTexture(GL_TEXTURE_2D, this->textureIds[index]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->renderWidth, this->renderHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->textureIds[index], 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Error while creating Renderbuffer" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#ifdef _DEBUG
	glObjectLabel(GL_FRAMEBUFFER, this->frameBufferIds[index], -1, "Renderbuffer");
	glObjectLabel(GL_TEXTURE, this->textureIds[index], -1, "Renderbuffer Texture");
#endif
}
