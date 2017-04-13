#pragma once

#include "Shader.h"
#include "Common\ShaderStructures.h"

class AmbientLight
{
public:
	AmbientLight(const glm::vec3 &lightColor);
	~AmbientLight();
	void Draw();
private:
	glm::vec3 lightColor;
	std::unique_ptr<Shader> lightShader;
	std::vector<VertexPositionTexture> vertices;
	std::vector<GLushort> indices;
	GLuint vbufferId, ebufferId, varrayId;
};

