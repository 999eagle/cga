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

void BlurRenderer::BlurCurrentlyBoundTexture()
{
	this->postProcessing->BindFramebuffer();
	this->blurShader->Apply();
	glUniform1i(this->shaderHorizontalLocation, GL_TRUE);
	QuadRenderer::GetInstance().DrawFullscreenQuad();
	this->postProcessing->Swap();
	glUniform1i(this->shaderHorizontalLocation, GL_FALSE);
	QuadRenderer::GetInstance().DrawFullscreenQuad();
	this->postProcessing->Swap(false);
	this->postProcessing->BindReadTexture();
}

void BlurRenderer::BindBlurredTexture(GLint index)
{
	this->postProcessing->BindReadTexture(index);
}
