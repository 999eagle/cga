#pragma once

#include "..\System.h"
#include "..\World.h"
#include "..\..\DeferredRenderer.h"
#include "..\..\PostProcessing\PostProcessing.h"
#include "..\..\Common\ShaderStructures.h"

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
		void UpdateVRTracking();

		vr::IVRSystem * vr;
		GLsizei windowWidth, windowHeight;
		DeferredRenderer * leftRenderer;
		DeferredRenderer * rightRenderer;
		PostProcessing * postProcessing;
		GammaPostProcessing * gammaPost;
		HdrPostProcessing * hdrPost;
		BloomPostProcessing * bloomPost;
		Shader * lightShaderAmbient;
		Shader * lightShaderDirectional;
		Shader * lightShaderPoint;
		Shader * shadowMapShader;

		vr::TrackedDevicePose_t trackedDevicePoses[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 trackedDeviceMatrices[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 hmdPose;
		glm::mat4 eyePoseLeft;
		glm::mat4 eyePoseRight;
		glm::mat4 projLeft;
		glm::mat4 projRight;
	};
}}
