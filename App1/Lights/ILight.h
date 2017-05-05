#pragma once

#include "..\Shader.h"

class ILight
{
public:
	virtual void Draw(const glm::mat4 & invViewProj, const glm::vec3 & cameraPosition) = 0;
protected:
	std::shared_ptr<Shader> shader;
	void SetCommonParameters(const glm::mat4 & invViewProj, const glm::vec3 & cameraPosition)
	{
		this->shader->Apply();
		glUniform1i(this->shader->GetUniformLocation("colorTexture"), 0);
		glUniform1i(this->shader->GetUniformLocation("normalTexture"), 1);
		glUniform1i(this->shader->GetUniformLocation("depthTexture"), 2);
		glUniformMatrix4fv(this->shader->GetUniformLocation("invViewProj"), 1, GL_FALSE, glm::value_ptr(invViewProj));
		glUniform3fv(this->shader->GetUniformLocation("cameraPosition"), 1, glm::value_ptr(cameraPosition));
	}
};
