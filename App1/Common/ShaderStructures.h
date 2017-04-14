#pragma once

struct VertexPositionColor
{
	glm::vec3 position;
	glm::vec3 color;
};

struct VertexPositionNormalTexture
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct VertexPositionTextureNTB
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct VertexPositionTexture
{
	glm::vec3 position;
	glm::vec2 texCoord;
};
