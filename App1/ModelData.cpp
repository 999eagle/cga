#include "pch.h"
#include "ModelData.h"

#include "Importer.h"

std::shared_ptr<Texture>
	Material::nullDiffuse = NULL,
	Material::nullNormal = NULL,
	Material::nullMetallic = NULL,
	Material::nullRoughness = NULL;

Material::Material(const std::vector<std::shared_ptr<Texture>> & textures)
{
	for (auto it = textures.begin(); it != textures.end(); it++)
	{
		switch ((*it)->type)
		{
		case aiTextureType_DIFFUSE:
			this->diffuseTexture = *it;
			break;
		case aiTextureType_NORMALS:
		case aiTextureType_HEIGHT:
			this->normalTexture = *it;
			break;
		case aiTextureType_SPECULAR:
			this->metallicTexture = *it;
			break;
		case aiTextureType_SHININESS:
			this->roughnessTexture = *it;
			break;
		default:
			std::cerr << "Error while loading material: Unknown texture type " << (*it)->type << std::endl;
			break;
		}
	}
	Material::InitNull();
	if (this->diffuseTexture == NULL) this->diffuseTexture = Material::nullDiffuse;
	if (this->normalTexture == NULL) this->normalTexture = Material::nullNormal;
	if (this->metallicTexture == NULL) this->metallicTexture = Material::nullMetallic;
	if (this->roughnessTexture == NULL) this->roughnessTexture = Material::nullRoughness;
}

void Material::InitNull()
{
	if (Material::nullDiffuse != NULL) return;
	auto & importer = TextureImporter::GetInstance();
	Material::nullDiffuse = importer.LoadTexture("Content\\Texture\\null_diffuse.png", aiTextureType_DIFFUSE);
	Material::nullNormal = importer.LoadTexture("Content\\Texture\\null_normal.png", aiTextureType_NORMALS);
	Material::nullMetallic = importer.LoadTexture("Content\\Texture\\null_metallic.png", aiTextureType_SPECULAR);
	Material::nullRoughness = importer.LoadTexture("Content\\Texture\\null_roughness.png", aiTextureType_SHININESS);
}

void Material::Use(const Shader & shader) const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->diffuseTexture->textureId);
	glUniform1i(shader.GetUniformLocation("diffuseTexture0"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->normalTexture->textureId);
	glUniform1i(shader.GetUniformLocation("normalTexture0"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->metallicTexture->textureId);
	glUniform1i(shader.GetUniformLocation("metallicTexture0"), 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, this->roughnessTexture->textureId);
	glUniform1i(shader.GetUniformLocation("roughnessTexture0"), 3);
	glActiveTexture(GL_TEXTURE0);
}
