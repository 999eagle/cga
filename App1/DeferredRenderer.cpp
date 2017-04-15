#include "pch.h"
#include "DeferredRenderer.h"

DeferredRenderer::DeferredRenderer(GLsizei width, GLsizei height)
{
	GLuint attachments[GBUFFER_COUNT] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT };

	// generate buffers and textures
	glGenFramebuffers(1, &this->geometryFrameBufferId);
	glGenTextures(GBUFFER_COUNT, this->geometryTextureIds);

	// set up geometry frame buffer and geometry textures
	glBindFramebuffer(GL_FRAMEBUFFER, this->geometryFrameBufferId);
	for (int i = 0; i < GBUFFER_COUNT; i++)
	{
		glBindTexture(GL_TEXTURE_2D, this->geometryTextureIds[i]);
		switch (i)
		{
		case 0:
		case 1:
			// set up buffers 0 and 1 (color and normals) as rgba16f
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			break;
		case 2:
			// set up buffer 2 (depth) as depth32f
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			break;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, this->geometryTextureIds[i], 0);
	}
	// set all but last buffer (depth) as draw buffer
	glDrawBuffers(GBUFFER_COUNT - 1, attachments);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Error while creating G-Buffer" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#ifdef _DEBUG
	glObjectLabel(GL_FRAMEBUFFER, this->geometryFrameBufferId, -1, "G-Buffer");
	glObjectLabel(GL_TEXTURE, this->geometryTextureIds[0], -1, "G-Buffer Color");
	glObjectLabel(GL_TEXTURE, this->geometryTextureIds[1], -1, "G-Buffer Normals");
	glObjectLabel(GL_TEXTURE, this->geometryTextureIds[2], -1, "G-Buffer Depth");
#endif

	this->geometryShader = std::make_unique<Shader>("Shader\\deferredGeometry.vs.glsl", "Shader\\deferredGeometry.fs.glsl");
}

DeferredRenderer::~DeferredRenderer()
{
	glDeleteTextures(GBUFFER_COUNT, this->geometryTextureIds);
	glDeleteFramebuffers(1, &this->geometryFrameBufferId);
}

GLint DeferredRenderer::GetWorldMatrixLocation() const
{
	return this->geometryShader->GetUniformLocation("world");
}

const Shader & DeferredRenderer::GetGeometryShader() const
{
	return *(this->geometryShader);
}

void DeferredRenderer::StartGeometryPass(glm::mat4 viewProj)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->geometryFrameBufferId);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->geometryShader->Apply();
	glUniformMatrix4fv(this->geometryShader->GetUniformLocation("viewProj"), 1, GL_FALSE, glm::value_ptr(viewProj));

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void DeferredRenderer::EndGeometryPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// disable depth testing
	glDisable(GL_DEPTH_TEST);
}

void DeferredRenderer::StartLightPass()
{
	// use additive blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->geometryFrameBufferId);
	for (int i = 0; i < GBUFFER_COUNT; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, this->geometryTextureIds[i]);
	}
}

void DeferredRenderer::EndLightPass()
{
	glDisable(GL_BLEND);
}
