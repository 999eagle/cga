#pragma once

#include "..\System.h"
#include "..\World.h"
#include "..\..\DeferredRenderer.h"
#include "..\..\PostProcessing.h"
#include "..\..\Common\ShaderStructures.h"

#include "..\..\GammaPostProcessing.h"

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
	private:
		DeferredRenderer * renderer;
		PostProcessing * postProcessing;
		GammaPostProcessing * gammaPost;
	};
}}
