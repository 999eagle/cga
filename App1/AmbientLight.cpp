#include "pch.h"
#include "AmbientLight.h"

#include "QuadRenderer.h"

AmbientLight::AmbientLight(const glm::vec3 &lightColor)
{
	this->lightColor = glm::vec3(lightColor);
	this->lightShader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\ambientLight.fs.glsl");
}

AmbientLight::~AmbientLight()
{
}

void AmbientLight::Draw()
{
	this->lightShader->Apply();
	glUniform3f(this->lightShader->GetUniformLocation("lightColor"), this->lightColor.r, this->lightColor.g, this->lightColor.b);
	glUniform1i(this->lightShader->GetUniformLocation("colorTexture"), 0);
	glUniform1i(this->lightShader->GetUniformLocation("normalTexture"), 1);
	glUniform1i(this->lightShader->GetUniformLocation("depthTexture"), 2);
	QuadRenderer::GetInstance().DrawFullscreenQuad();
}
