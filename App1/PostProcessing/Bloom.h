#pragma once

#include "..\Shader.h"
#include "..\Common\ShaderStructures.h"
#include "..\QuadRenderer.h"
#include "BlurRenderer.h"
#include "PostProcessing.h"

class BloomPostProcessing
{
public:
	BloomPostProcessing(GLint width, GLint height)
	{
		this->brightnessShader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\postBloomBrightness.fs.glsl");
		this->bloomShader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\postBloomRender.fs.glsl");
		this->blurRenderer = new BlurRenderer(width, height);
	}
	~BloomPostProcessing()
	{
		delete this->blurRenderer;
	}
	void Draw(PostProcessing * postProcessing)
	{
		// Pass 1: render brightness into postProcessing FB 1
		postProcessing->Swap();
		this->brightnessShader->Apply();
		glUniform1i(this->brightnessShader->GetUniformLocation("colorTexture"), 0);
		QuadRenderer::GetInstance().DrawFullscreenQuad();
		// Pass 2: blur postProcessing FB 1 into blurRenderer FB 3 times
		postProcessing->Swap(false);
		postProcessing->BindReadTexture();
		this->blurRenderer->BlurCurrentlyBoundTexture(1, 4);
		this->blurRenderer->BlurCurrentlyBoundTexture(2, 3);
		// Pass 3: apply bloom effect
		// pass 2 didn't overwrite postProcessing FB 0, so we still have the completely unprocessed texture available in that buffer
		postProcessing->Swap();
		// bind  blurred texture to texture 1, so original texture and blurred brightness texture are available to bloom shader
		this->blurRenderer->BindBlurredTexture(1);
		this->bloomShader->Apply();
		glUniform1i(this->bloomShader->GetUniformLocation("colorTexture"), 0);
		glUniform1i(this->bloomShader->GetUniformLocation("bloomTexture"), 1);
		QuadRenderer::GetInstance().DrawFullscreenQuad();

	}
private:
	std::shared_ptr<Shader> brightnessShader, bloomShader;
	BlurRenderer * blurRenderer;
};
