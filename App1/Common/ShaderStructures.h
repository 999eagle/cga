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

struct VertexPositionTexture
{
	glm::vec3 position;
	glm::vec2 texCoord;
};
