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
	}
	~HdrPostProcessing() { }
	void Draw(PostProcessing * postProcessing)
	{
		postProcessing->Swap();
		this->shader->Apply();
		glUniform1i(this->shader->GetUniformLocation("colorTexture"), 0);
		glUniform1f(this->shader->GetUniformLocation("exposure"), this->exposure);
		QuadRenderer::GetInstance().DrawFullscreenQuad();
		postProcessing->Swap(false);
		postProcessing->BindReadTexture();
		postProcessing->Swap(false);
	}
private:
	std::shared_ptr<Shader> shader;
	float exposure;
};
