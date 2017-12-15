#pragma once

#include "..\System.h"
#include "..\World.h"
#include "..\..\DeferredRenderer.h"
#include "..\..\PostProcessing\PostProcessing.h"
#include "..\..\Common\ShaderStructures.h"

#include "..\Components\ModelComponents.h"
#include "..\Components\TransformComponent.h"
#include "..\Components\LightComponent.h"
#include "..\Components\CameraComponent.h"

#include "..\..\PostProcessing\GammaPostProcessing.h"
#include "..\..\PostProcessing\Hdr.h"
#include "..\..\PostProcessing\Bloom.h"

namespace ECS { namespace Systems
{
	class RenderSystem : public System
	{
	public:
		typedef VertexPositionTextureNTB VertexType;

		RenderSystem(GLsizei windowWidth, GLsizei windowHeight, vr::IVRSystem * vr);
		~RenderSystem();

		void Update(World & world, const AppTime & time);
		void FixedUpdate(World & world, const AppTime & time) { }
		void ApplyLightShader(Shader * shader, const glm::vec3 & cameraPosition, const glm::mat4 & invViewProj);
	private:
		void RenderWithCamera(const AppTime & time, ECS::Components::CameraComponent * camera, GLuint targetFB,
			std::vector<std::tuple<ECS::Components::MeshComponent<VertexType>*, ECS::Components::MaterialComponent*, glm::mat4>> meshes,
			std::vector<ECS::Components::LightComponent*> ambientLights,
			std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> pointLights,
			std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> directionalLights,
			std::vector<std::pair<ECS::Components::LightComponent*, glm::mat4>> shadowMapLights);
		void EnsureFramebuffers(size_t requiredNum);
		void CreateFramebuffer(size_t index);

		vr::IVRSystem * vr;

		GLsizei windowWidth, windowHeight, renderWidth, renderHeight;
		GLuint * frameBufferIds, * textureIds;
		size_t numFrameBuffers;
		DeferredRenderer * renderer;
		PostProcessing * postProcessing;
		GammaPostProcessing * gammaPost;
		HdrPostProcessing * hdrPost;
		BloomPostProcessing * bloomPost;
		Shader * lightShaderAmbient;
		Shader * lightShaderDirectional;
		Shader * lightShaderPoint;
		Shader * shadowMapShader;
	};
}}
