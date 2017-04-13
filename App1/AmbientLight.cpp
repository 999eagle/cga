#include "pch.h"
#include "AmbientLight.h"

AmbientLight::AmbientLight(const glm::vec3 &lightColor)
{
	this->lightColor = glm::vec3(lightColor);
	this->lightShader = std::make_unique<Shader>("Shader\\passthrough.vs.glsl", "Shader\\ambientLight.fs.glsl");

	// full screen quad as ambient light affects the whole screen
	this->vertices.push_back(VertexPositionTexture{ glm::vec3(-1.0, 1.0, 0.0), glm::vec2(0.0, 1.0) });
	this->vertices.push_back(VertexPositionTexture{ glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0) });
	this->vertices.push_back(VertexPositionTexture{ glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0) });
	this->vertices.push_back(VertexPositionTexture{ glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0) });
	this->indices.push_back(0);
	this->indices.push_back(1);
	this->indices.push_back(2);
	this->indices.push_back(0);
	this->indices.push_back(2);
	this->indices.push_back(3);

	glGenVertexArrays(1, &this->varrayId);
	glGenBuffers(1, &this->vbufferId);
	glGenBuffers(1, &this->ebufferId);
	glBindVertexArray(this->varrayId);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositionTexture) * this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionTexture), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPositionTexture), (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * this->indices.size(), this->indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
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
	glBindVertexArray(this->varrayId);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}
