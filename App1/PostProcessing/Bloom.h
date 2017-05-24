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
		this->width = width;
		this->height = height;
		this->brightnessShader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\postBloomBrightness.fs.glsl");
		this->bloomShader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\postBloomRender.fs.glsl");
		this->blurRenderer[0] = new BlurRenderer(width, height);
		this->blurRenderer[1] = new BlurRenderer(width / 2, height / 2);
		this->blurRenderer[2] = new BlurRenderer(width / 4, height / 4);
		this->blurRenderer[3] = new BlurRenderer(width / 8, height / 8);
	}
	~BloomPostProcessing()
	{
		delete this->blurRenderer[0];
		delete this->blurRenderer[1];
		delete this->blurRenderer[2];
		delete this->blurRenderer[3];
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
		this->blurRenderer[0]->BlurCurrentlyBoundTexture(2);
		postProcessing->BindReadTexture();
		this->blurRenderer[1]->BlurCurrentlyBoundTexture(2);
		postProcessing->BindReadTexture();
		this->blurRenderer[2]->BlurCurrentlyBoundTexture(2);
		postProcessing->BindReadTexture();
		this->blurRenderer[3]->BlurCurrentlyBoundTexture(2);
		// Pass 3: apply bloom effect
		// pass 2 didn't overwrite postProcessing FB 0, so we still have the completely unprocessed texture available in that buffer --> blend bloom additively
		postProcessing->BindFramebuffer();
		glViewport(0, 0, width, height);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		// bind  blurred texture to texture 1, so original texture and blurred brightness texture are available to bloom shader
		this->bloomShader->Apply();
		glUniform1i(this->bloomShader->GetUniformLocation("colorTexture"), 0);
		this->blurRenderer[0]->BindBlurredTexture();
		QuadRenderer::GetInstance().DrawFullscreenQuad();
		this->blurRenderer[1]->BindBlurredTexture();
		QuadRenderer::GetInstance().DrawFullscreenQuad();
		this->blurRenderer[2]->BindBlurredTexture();
		QuadRenderer::GetInstance().DrawFullscreenQuad();
		this->blurRenderer[3]->BindBlurredTexture();
		QuadRenderer::GetInstance().DrawFullscreenQuad();
		glDisable(GL_BLEND);
	}
private:
	std::shared_ptr<Shader> brightnessShader, bloomShader;
	BlurRenderer *blurRenderer[4];
	GLsizei width, height;
};
