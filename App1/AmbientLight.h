#pragma once

#include "ILight.h"
#include "Shader.h"
#include "Common\ShaderStructures.h"
#include "QuadRenderer.h"

class AmbientLight : public ILight
{
public:
	AmbientLight(const glm::vec3 & color) :
		color(color)
	{
		this->shader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\ambientLight.fs.glsl");
	}
	~AmbientLight() { }
	virtual void AmbientLight::Draw(const glm::mat4 & invViewProj, const glm::vec3 & cameraPosition)
	{
		this->SetCommonParameters(invViewProj, cameraPosition);
		glUniform3fv(this->shader->GetUniformLocation("lightColor"), 1, glm::value_ptr(color));
		QuadRenderer::GetInstance().DrawFullscreenQuad();
	}
private:
	glm::vec3 color;
};

