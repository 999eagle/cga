#pragma once

#include "ILight.h"
#include "..\Shader.h"
#include "..\Common\ShaderStructures.h"
#include "..\QuadRenderer.h"

class DirectionalLight : public ILight
{
public:
	DirectionalLight(const glm::vec3 & color, const glm::vec3 & direction) :
		color(color), direction(direction)
	{
		this->shader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\directionalLight.fs.glsl");
	}
	~DirectionalLight() { }
	virtual void DirectionalLight::Draw(const glm::mat4 & invViewProj, const glm::vec3 & cameraPosition)
	{
		this->SetCommonParameters(invViewProj, cameraPosition);
		glUniform3fv(this->shader->GetUniformLocation("lightColor"), 1, glm::value_ptr(color));
		glUniform3fv(this->shader->GetUniformLocation("lightDirection"), 1, glm::value_ptr(direction));
		QuadRenderer::GetInstance().DrawFullscreenQuad();
	}
private:
	glm::vec3 color, direction;
};
