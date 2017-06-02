#include "pch.h"
#include "BlurRenderer.h"

#include "..\QuadRenderer.h"

BlurRenderer::BlurRenderer(GLsizei width, GLsizei height)
{
	this->width = width;
	this->height = height;
	this->postProcessing = new PostProcessing(width, height);
	this->blurShader = new Shader("Shader\\passthrough.vs.glsl", "Shader\\gaussBlur.fs.glsl");
	this->shaderHorizontalLocation = this->blurShader->GetUniformLocation("horizontal");
	this->shaderTextureLocation = this->blurShader->GetUniformLocation("colorTexture");
	this->shaderViewportSizeLocation = this->blurShader->GetUniformLocation("viewportSize");
}

BlurRenderer::~BlurRenderer()
{
	delete this->postProcessing;
	delete this->blurShader;
}

void BlurRenderer::BlurCurrentlyBoundTexture(int count)
{
	this->postProcessing->Swap(false);
	this->postProcessing->BindFramebuffer();
	this->blurShader->Apply();
	glViewport(0, 0, this->width, this->height);
	glUniform2f(this->shaderViewportSizeLocation, width, height);
	for (int i = 0; i < count; i++)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		glUniform1i(this->shaderHorizontalLocation, GL_TRUE);
		QuadRenderer::GetInstance().DrawFullscreenQuad();
		this->postProcessing->Swap();
		glGenerateMipmap(GL_TEXTURE_2D);
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
