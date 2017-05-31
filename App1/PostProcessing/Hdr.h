#pragma once

#include "..\Shader.h"
#include "..\Common\ShaderStructures.h"
#include "..\QuadRenderer.h"
#include "PostProcessing.h"

class HdrPostProcessing
{
public:
	HdrPostProcessing()
	{
		this->shader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\postHdr.fs.glsl");
		this->exposure = .5f;
		this->data = (float*)malloc(sizeof(float) * 4);
	}
	~HdrPostProcessing() { free(this->data); }
	void Draw(PostProcessing * postProcessing, float frameTime)
	{
		postProcessing->Swap();
		this->shader->Apply();
		glUniform1i(this->shader->GetUniformLocation("colorTexture"), 0);
		glUniform1f(this->shader->GetUniformLocation("exposure"), this->exposure);
		QuadRenderer::GetInstance().DrawFullscreenQuad();
		postProcessing->Swap(false);
		postProcessing->BindReadTexture();
		glGenerateMipmap(GL_TEXTURE_2D);
		glGetTexImage(GL_TEXTURE_2D, 10, GL_RGB, GL_FLOAT, this->data);
		const float gamma = 1.f / 2.2f;
		const float targetBrightness = .35f;
		float brightness = (pow(this->data[0], gamma) + pow(this->data[1], gamma) + pow(this->data[2], gamma)) / 3.f;
		if (isnan(brightness))
		{
			brightness = targetBrightness;
		}
		float targetExposure = this->exposure - brightness + targetBrightness;
		this->exposure = glm::clamp(glm::mix(this->exposure, targetExposure, .03f * frameTime * 60.f), .2f, .8f);
		postProcessing->Swap(false);
	}
private:
	std::shared_ptr<Shader> shader;
	float exposure;
	float * data;
};
