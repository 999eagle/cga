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
	Material(const std::vector<std::shared_ptr<Texture>> & textures) : textures(textures) { }
	void Use(const Shader & shader) const;
private:
	std::vector<std::shared_ptr<Texture>> textures;
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
};

template<typename VertexType>
class Model
{
public:
	Model(const std::vector<std::pair<std::shared_ptr<Mesh<VertexType>>, std::shared_ptr<Material>>> & meshes) : meshes(meshes) { }
	void Draw(const Shader & shader) const;
	std::vector<std::pair<std::shared_ptr<Mesh<VertexType>>, std::shared_ptr<Material>>> meshes;
private:
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
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexType), (GLvoid*)offsetof(VertexType, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexType), (GLvoid*)offsetof(VertexType, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexType), (GLvoid*)offsetof(VertexType, tangent));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexType), (GLvoid*)offsetof(VertexType, bitangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(VertexType), (GLvoid*)offsetof(VertexType, texCoord));
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
}

template<typename VertexType>
void Model<VertexType>::Draw(const Shader & shader) const
{
	for (auto it = this->meshes.begin(); it != this->meshes.end(); it++)
	{
		it->second->Use(shader);
		it->first->Draw();
	}
}
