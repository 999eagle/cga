#pragma once

#include "ILight.h"
#include "Shader.h"
#include "Common\ShaderStructures.h"
#include "QuadRenderer.h"

class PointLight : public ILight
{
public:
	PointLight(const glm::vec3 & color, const glm::vec3 & position, GLfloat radius) :
		color(color), position(position), radius(radius)
	{
		this->shader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\pointLight.fs.glsl");
	}
	~PointLight() { }
	virtual void PointLight::Draw(const glm::mat4 & invViewProj, const glm::vec3 & cameraPosition)
	{
		this->SetCommonParameters(invViewProj, cameraPosition);
		glUniform3fv(this->shader->GetUniformLocation("lightColor"), 1, glm::value_ptr(color));
		glUniform3fv(this->shader->GetUniformLocation("lightPosition"), 1, glm::value_ptr(position));
		glUniform1f(this->shader->GetUniformLocation("lightRadius"), radius);
		QuadRenderer::GetInstance().DrawFullscreenQuad();
	}
private:
	glm::vec3 color, position;
	GLfloat radius;
};
