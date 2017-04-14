#pragma once

enum VertexAttributeType
{
	VertexAttribute_normal,
	VertexAttribute_color,
	VertexAttribute_texCoord,
	VertexAttribute_tangent,
	VertexAttribute_bitangent
};
template<typename VertexType, VertexAttributeType attribute>
struct VertexAttribute
{
	static constexpr bool exists = false;
	static const size_t offset = 0;
};
#define DefineVertexAttribute(type, component) \
template<> \
struct VertexAttribute<type, VertexAttribute_##component >{static constexpr bool exists = true;static const size_t offset = offsetof(type, component);};

struct VertexPositionColor
{
	glm::vec3 position;
	glm::vec3 color;
};
DefineVertexAttribute(VertexPositionColor, color);

struct VertexPositionNormalTexture
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};
DefineVertexAttribute(VertexPositionNormalTexture, normal);
DefineVertexAttribute(VertexPositionNormalTexture, texCoord);


struct VertexPositionTextureNTB
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};
DefineVertexAttribute(VertexPositionTextureNTB, texCoord);
DefineVertexAttribute(VertexPositionTextureNTB, normal);
DefineVertexAttribute(VertexPositionTextureNTB, tangent);
DefineVertexAttribute(VertexPositionTextureNTB, bitangent);

struct VertexPositionTexture
{
	glm::vec3 position;
	glm::vec2 texCoord;
};
DefineVertexAttribute(VertexPositionTexture, texCoord);

#undef DefineVertexAttribute
