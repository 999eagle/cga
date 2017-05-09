#pragma once

#include "..\Shader.h"
#include "..\Common\ShaderStructures.h"
#include "..\QuadRenderer.h"

class GammaPostProcessing
{
public:
	GammaPostProcessing()
	{
		this->shader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\postGamma.fs.glsl");
	}
	~GammaPostProcessing() { }
	void Draw()
	{
		this->shader->Apply();
		glUniform1i(this->shader->GetUniformLocation("colorTexture"), 0);
		QuadRenderer::GetInstance().DrawFullscreenQuad();
	}
private:
	std::shared_ptr<Shader> shader;
};

