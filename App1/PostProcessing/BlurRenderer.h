#pragma once

#include "PostProcessing.h"
#include "..\Shader.h"

class BlurRenderer
{
public:
	BlurRenderer(GLsizei width, GLsizei height);
	~BlurRenderer();
	void BlurCurrentlyBoundTexture(int count = 1);
	void BindBlurredTexture(GLint index = 0);
private:
	PostProcessing * postProcessing;
	Shader * blurShader;
	GLint shaderHorizontalLocation, shaderTextureLocation, shaderViewportSizeLocation;
	GLsizei width, height;
};
