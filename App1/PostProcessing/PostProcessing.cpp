#include "pch.h"
#include "PostProcessing.h"

PostProcessing::PostProcessing(GLsizei width, GLsizei height)
{
	glGenFramebuffers(2, this->pingpongFramebufferIds);
	glGenTextures(2, this->pingpongTextureIds);
	for (auto i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->pingpongFramebufferIds[i]);
		glBindTexture(GL_TEXTURE_2D, this->pingpongTextureIds[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->pingpongTextureIds[i], 0);
#ifdef _DEBUG
		glObjectLabel(GL_FRAMEBUFFER, this->pingpongFramebufferIds[i], -1, (std::string("Postprocessing Framebuffer ") + (char)(i + 48)).c_str());
		glObjectLabel(GL_TEXTURE, this->pingpongTextureIds[i], -1, (std::string("Postprocessing Texture ") + (char)(i + 48)).c_str());
#endif
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	this->currentDrawBuffer = 0;

}

PostProcessing::~PostProcessing()
{
	glDeleteTextures(2, this->pingpongTextureIds);
	glDeleteFramebuffers(2, this->pingpongFramebufferIds);
}

void PostProcessing::BindFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->pingpongFramebufferIds[this->currentDrawBuffer]);
}

void PostProcessing::BindReadTexture(GLint index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, this->pingpongTextureIds[1 - this->currentDrawBuffer]);
}

void PostProcessing::Swap(bool bindBuffers)
{
	this->currentDrawBuffer = 1 - this->currentDrawBuffer;
	if (bindBuffers)
	{
		this->BindFramebuffer();
		this->BindReadTexture();
	}
}
