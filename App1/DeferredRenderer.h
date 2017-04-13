#pragma once

#include "Shader.h"

#define GBUFFER_COUNT 3

class DeferredRenderer
{
public:
	DeferredRenderer(GLsizei width, GLsizei height);
	~DeferredRenderer();
	GLint GetDiffuseTextureLocation();
	GLint GetWorldMatrixLocation();
	void StartGeometryPass(glm::mat4 viewProj);
	void EndGeometryPass();
	void StartLightPass();
	void EndLightPass();

private:
	GLuint geometryFrameBufferId;
	GLuint geometryTextureIds[GBUFFER_COUNT];
	std::unique_ptr<Shader> geometryShader;
};
