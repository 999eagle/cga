#include "pch.h"
#include "Importer.h"

#include "ECS\Components\ModelComponents.h"
#include "ECS\Entity.h"
#include "ECS\Components\TransformComponent.h"

std::shared_ptr<Texture> TextureImporter::LoadTexture(const std::string & path, aiTextureType type)
{
	auto it = this->loadedTextures.find(path);
	if (it != this->loadedTextures.end())
	{
		return it->second;
	}

	int width, height, channels;
	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
	if (image == NULL)
	{
		std::cerr << "Error loading texture data from file " << path << std::endl;
		return NULL;
	}

	auto newTexture = std::make_shared<Texture>();
	newTexture->type = type;
	glGenTextures(1, &newTexture->textureId);
	glBindTexture(GL_TEXTURE_2D, newTexture->textureId);
	GLint internalFormat = GL_COMPRESSED_RGB;
	if (type == aiTextureType_DIFFUSE)
		internalFormat = GL_COMPRESSED_SRGB;
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

#ifdef _DEBUG
	glObjectLabel(GL_TEXTURE, newTexture->textureId, -1, ("Texture: " + path).c_str());
#endif

	this->loadedTextures.insert(std::make_pair(path, newTexture));
	return newTexture;
}

TextureImporter::~TextureImporter()
{
	for (auto it = this->loadedTextures.begin(); it != this->loadedTextures.end(); it++)
	{
		glDeleteTextures(1, &(it->second->textureId));
	}
}

std::shared_ptr<Material> MaterialImporter::LoadMaterial(const std::string & albedoMapPath, const std::string & normalMapPath, const std::string & metallicMapPath, const std::string & roughnessMapPath)
{
	std::vector<std::shared_ptr<Texture>> textures;
	if (albedoMapPath != "")
	{
		textures.push_back(TextureImporter::GetInstance().LoadTexture(albedoMapPath, aiTextureType_DIFFUSE));
	}
	if (normalMapPath != "")
	{
		textures.push_back(TextureImporter::GetInstance().LoadTexture(normalMapPath, aiTextureType_NORMALS));
	}
	if (metallicMapPath != "")
	{
		textures.push_back(TextureImporter::GetInstance().LoadTexture(metallicMapPath, aiTextureType_SPECULAR));
	}
	if (roughnessMapPath != "")
	{
		textures.push_back(TextureImporter::GetInstance().LoadTexture(roughnessMapPath, aiTextureType_SHININESS));
	}
	return std::make_shared<Material>(textures);
}

std::shared_ptr<Material> MaterialImporter::LoadMaterial(const std::string & materialPath)
{
	auto it = this->loadedMaterials.find(materialPath);
	if (it != this->loadedMaterials.end())
	{
		return it->second;
	}
	std::string directory = materialPath.substr(0, materialPath.find_last_of("\\/") + 1);
	std::fstream matFile;
	matFile.open(materialPath, std::fstream::in);
	std::string mat;
	std::vector<std::shared_ptr<Texture>> textures;
	while (std::getline(matFile, mat))
	{
		auto spaceIdx = mat.find(" ");
		auto key = mat.substr(0, spaceIdx);
		auto value = mat.substr(spaceIdx + 1);
		if (key == "map_albedo") textures.push_back(TextureImporter::GetInstance().LoadTexture(directory + value, aiTextureType_DIFFUSE));
		else if (key == "map_normal") textures.push_back(TextureImporter::GetInstance().LoadTexture(directory + value, aiTextureType_NORMALS));
		else if (key == "map_metallic") textures.push_back(TextureImporter::GetInstance().LoadTexture(directory + value, aiTextureType_SPECULAR));
		else if (key == "map_roughness") textures.push_back(TextureImporter::GetInstance().LoadTexture(directory + value, aiTextureType_SHININESS));
		else std::cerr << "Unknown key " << key << " in material " << materialPath << "!" << std::endl;
	}
	auto newMat = std::make_shared<Material>(textures);
	this->loadedMaterials.insert(std::make_pair(materialPath, newMat));
	return newMat;
}

ECS::Entity * ModelImporter::LoadModel(ECS::World * world, const std::string & path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_FindInvalidData |
		aiProcess_GenUVCoords |
		aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Import error in file " << path << ": " << importer.GetErrorString() << std::endl;
		return NULL;
	}
	std::string directory = path.substr(0, path.find_last_of("\\/") + 1);
	std::string name = path;
	return this->ProcessNode(world, scene->mRootNode, scene, directory, name);
}

ECS::Entity * ModelImporter::ProcessNode(ECS::World * world, const aiNode * node, const aiScene * scene, const std::string & directory, const std::string & name)
{
	auto entity = new ECS::Entity();
	world->AddEntity(entity);
	auto transform = entity->GetComponent<ECS::Components::TransformComponent>();
	transform->SetLocalTransform(glm::transpose(glm::make_mat4(&(node->mTransformation.a1))));
	if (node->mNumMeshes == 1)
	{
		this->ProcessMesh(entity, scene->mMeshes[node->mMeshes[0]], scene, directory, name);
	}
	else
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			auto childEntity = new ECS::Entity();
			world->AddEntity(childEntity);
			childEntity->GetComponent<ECS::Components::TransformComponent>()->SetParent(transform);
			this->ProcessMesh(childEntity, scene->mMeshes[node->mMeshes[i]], scene, directory, name);
		}
	}
	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		auto child = this->ProcessNode(world, node->mChildren[i], scene, directory, name);
		child->GetComponent<ECS::Components::TransformComponent>()->SetParent(transform);
	}
	return entity;
}

void ModelImporter::ProcessMesh(ECS::Entity * entity, const aiMesh * mesh, const aiScene * scene, const std::string & directory, const std::string & name)
{
	std::vector<VertexType> vertices;
	std::vector<GLuint> indices;
	std::vector<std::shared_ptr<Texture>> textures;
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		VertexType vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
		}
		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;
		vertex.tangent.x = mesh->mTangents[i].x;
		vertex.tangent.y = mesh->mTangents[i].y;
		vertex.tangent.z = mesh->mTangents[i].z;
		vertex.bitangent.x = mesh->mBitangents[i].x;
		vertex.bitangent.y = mesh->mBitangents[i].y;
		vertex.bitangent.z = mesh->mBitangents[i].z;
		if (glm::dot(glm::cross(vertex.normal, vertex.tangent), vertex.bitangent) < 0)
		{
			vertex.tangent *= -1.0;
		}
		vertices.push_back(vertex);
	}
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	aiMaterial * mat = scene->mMaterials[mesh->mMaterialIndex];
	// load referenced textures
	this->LoadTextures(textures, mat, aiTextureType_DIFFUSE, directory);
	this->LoadTextures(textures, mat, aiTextureType_NORMALS, directory);
	this->LoadTextures(textures, mat, aiTextureType_HEIGHT, directory);
	this->LoadTextures(textures, mat, aiTextureType_SPECULAR, directory);
	this->LoadTextures(textures, mat, aiTextureType_SHININESS, directory);
	auto newMesh = std::make_shared<Mesh<VertexType>>(vertices, indices, name + "\\" + mesh->mName.C_Str());
	auto material = std::make_shared<Material>(textures);
	entity->AddComponent<ECS::Components::MeshComponent<VertexType>>(newMesh);
	entity->AddComponent<ECS::Components::MaterialComponent>(material);
}

void ModelImporter::LoadTextures(std::vector<std::shared_ptr<Texture>> & textures, const aiMaterial * material, aiTextureType type, const std::string & directory)
{
	for (uint32_t i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString path;
		material->GetTexture(type, i, &path);
		textures.push_back(TextureImporter::GetInstance().LoadTexture(directory + std::string(path.C_Str()), type));
	}
}
