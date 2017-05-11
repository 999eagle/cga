#pragma once

#include "..\Shader.h"
#include "..\Common\ShaderStructures.h"
#include "..\QuadRenderer.h"

class HdrPostProcessing
{
public:
	HdrPostProcessing()
	{
		this->shader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\postHdr.fs.glsl");
	}
	~HdrPostProcessing() { }
	void Draw(float exposure)
	{
		this->shader->Apply();
		glUniform1i(this->shader->GetUniformLocation("colorTexture"), 0);
		glUniform1f(this->shader->GetUniformLocation("exposure"), exposure);
		QuadRenderer::GetInstance().DrawFullscreenQuad();
	}
private:
	std::shared_ptr<Shader> shader;
};
