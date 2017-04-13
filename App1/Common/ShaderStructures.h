#pragma once

#include <glm\glm.hpp>

struct VertexPositionColor
{
	glm::vec3 position;
	glm::vec3 color;

	static GLint GetVertexStride() { return sizeof(position) + sizeof(color); };
};