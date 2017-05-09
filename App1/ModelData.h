#pragma once

#include "Common\ShaderStructures.h"
#include "Shader.h"

struct Texture
{
	GLuint textureId;
	aiTextureType type;
};

class Material
{
public:
	Material(const std::vector<std::shared_ptr<Texture>> & textures);
	~Material() { }
	void Use(const Shader & shader) const;
	static inline void InitNull();
private:
	std::shared_ptr<Texture> diffuseTexture, normalTexture, metallicTexture, roughnessTexture;
	static std::shared_ptr<Texture> nullDiffuse, nullNormal, nullMetallic, nullRoughness;
};

template<typename VertexType>
class Mesh
{
public:
	Mesh(const std::vector<VertexType> &vertices, const std::vector<GLuint> &indices, const std::string & debugName = "");
	~Mesh();
	void Draw() const;
private:
	std::vector<VertexType> vertices;
	std::vector<GLuint> indices;
	GLuint vertexArrayId, vertexBufferId, elementBufferId;
	
	template<VertexAttributeType attribute>
	void EnableVertexAttribute(GLint size, GLuint & index)
	{
		if (VertexAttribute<VertexType, attribute>::exists)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index++, size, GL_FLOAT, GL_FALSE, sizeof(VertexType), (void*)VertexAttribute<VertexType, attribute>::offset);
		}
	}
};


/*************************************\
 Implementation
\*************************************/

template<typename VertexType>
Mesh<VertexType>::Mesh(const std::vector<VertexType> &vertices, const std::vector<GLuint> &indices, const std::string & debugName)
{
	this->vertices = vertices;
	this->indices = indices;

	glGenBuffers(1, &this->vertexBufferId);
	glGenBuffers(1, &this->elementBufferId);
	glGenVertexArrays(1, &this->vertexArrayId);

	glBindVertexArray(this->vertexArrayId);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->indices.size(), this->indices.data(), GL_STATIC_DRAW);
	GLuint inputIndex = 0;
	glEnableVertexAttribArray(inputIndex);
	glVertexAttribPointer(inputIndex++, 3, GL_FLOAT, GL_FALSE, sizeof(VertexType), (GLvoid*)offsetof(VertexType, position));
	this->EnableVertexAttribute<VertexAttribute_color>(3, inputIndex);
	this->EnableVertexAttribute<VertexAttribute_texCoord>(2, inputIndex);
	this->EnableVertexAttribute<VertexAttribute_normal>(3, inputIndex);
	this->EnableVertexAttribute<VertexAttribute_tangent>(3, inputIndex);
	this->EnableVertexAttribute<VertexAttribute_bitangent>(3, inputIndex);
	glBindVertexArray(0);

#ifdef _DEBUG
	glObjectLabel(GL_BUFFER, this->vertexBufferId, -1, ("Mesh " + debugName + " vertices").c_str());
	glObjectLabel(GL_BUFFER, this->elementBufferId, -1, ("Mesh " + debugName + " indices").c_str());
	glObjectLabel(GL_VERTEX_ARRAY, this->vertexArrayId, -1, ("Mesh " + debugName + " array").c_str());
#endif
}

template<typename VertexType>
Mesh<VertexType>::~Mesh()
{
	glDeleteBuffers(1, &this->vertexBufferId);
	glDeleteBuffers(1, &this->vertexBufferId);
	glDeleteVertexArrays(1, &this->vertexArrayId);
}

template<typename VertexType>
void Mesh<VertexType>::Draw() const
{
	glBindVertexArray(this->vertexArrayId);
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
