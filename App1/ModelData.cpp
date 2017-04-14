#include "pch.h"
#include "ModelData.h"

void Material::Use(const Shader & shader) const
{
	GLuint normalIndex = 0, diffuseIndex = 0, specularIndex = 0;
	GLint i = 0;
	for (; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i]->textureId);
		std::stringstream name;
		switch (this->textures[i]->type)
		{
		case aiTextureType_NORMALS:
		case aiTextureType_HEIGHT:
			name << "normalTexture" << normalIndex++;
			break;
		case aiTextureType_DIFFUSE:
			name << "diffuseTexture" << diffuseIndex++;
			break;
		case aiTextureType_SPECULAR:
			name << "specularTexture" << specularIndex++;
			break;
		default:
			std::cerr << "Error while using material: unknown texture type " << this->textures[i]->type << std::endl;
			break;
		}
		std::string s = name.str();
		glUniform1i(shader.GetUniformLocation(s.c_str()), i);
	}
	glActiveTexture(GL_TEXTURE0);
}
