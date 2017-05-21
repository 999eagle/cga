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

		RenderSystem(GLsizei windowWidth, GLsizei windowHeight);
		~RenderSystem();

		void Update(World & world, const AppTime & time);
		void FixedUpdate(World & world, const AppTime & time) { }
		void ApplyLightShader(Shader * shader, const glm::vec3 & cameraPosition, const glm::mat4 & invViewProj);
	private:
		GLsizei windowWidth, windowHeight;
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
