#include "pch.h"
#include "BlurRenderer.h"

#include "..\QuadRenderer.h"

BlurRenderer::BlurRenderer(GLsizei width, GLsizei height)
{
	this->postProcessing = new PostProcessing(width, height);
	this->blurShader = new Shader("Shader\\passthrough.vs.glsl", "Shader\\gaussBlur.fs.glsl");
	this->shaderHorizontalLocation = this->blurShader->GetUniformLocation("horizontal");
	this->shaderTextureLocation = this->blurShader->GetUniformLocation("colorTexture");
}

BlurRenderer::~BlurRenderer()
{
	delete this->postProcessing;
	delete this->blurShader;
}

void BlurRenderer::BlurCurrentlyBoundTexture(int count)
{
	this->postProcessing->BindFramebuffer();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	this->postProcessing->Swap(false);
	this->postProcessing->BindFramebuffer();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	this->blurShader->Apply();
	for (int i = 0; i < count; i++)
	{
		glUniform1i(this->shaderHorizontalLocation, GL_TRUE);
		QuadRenderer::GetInstance().DrawFullscreenQuad();
		this->postProcessing->Swap();
		glUniform1i(this->shaderHorizontalLocation, GL_FALSE);
		QuadRenderer::GetInstance().DrawFullscreenQuad();
		if (i == count - 1)
		{
			this->postProcessing->Swap(false);
			this->postProcessing->BindReadTexture();
		}
		else
		{
			this->postProcessing->Swap();
		}
	}
}

void BlurRenderer::BindBlurredTexture(GLint index)
{
	this->postProcessing->BindReadTexture(index);
}
