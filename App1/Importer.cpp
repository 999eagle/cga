#include "pch.h"
#include "Importer.h"

std::shared_ptr<Texture> TextureImporter::LoadTexture(const std::string & path, aiTextureType type)
{
	auto it = this->loadedTextures.find(path);
	if (it != this->loadedTextures.end())
	{
		return it->second;
	}

	auto newTexture = std::make_shared<Texture>();
	newTexture->type = type;

	glGenTextures(1, &newTexture->textureId);
	glBindTexture(GL_TEXTURE_2D, newTexture->textureId);
	int width, height;
	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
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
		glDeleteTextures(1, &it->second->textureId);
	}
}

ModelImporter::ModelImporter()
{
	this->nullDiffuse = TextureImporter::GetInstance().LoadTexture("Content\\Texture\\null_diffuse.png", aiTextureType_DIFFUSE);
	this->nullNormal = TextureImporter::GetInstance().LoadTexture("Content\\Texture\\null_normal.png", aiTextureType_NORMALS);
	this->nullSpecular = TextureImporter::GetInstance().LoadTexture("Content\\Texture\\null_specular.png", aiTextureType_SPECULAR);
}

std::shared_ptr<Model<ModelImporter::VertexType>> ModelImporter::LoadModel(const std::string & path)
{
	auto it = this->loadedModels.find(path);
	if (it != this->loadedModels.end())
	{
		return it->second;
	}
	auto model = this->LoadModelImplementation(path);
	this->loadedModels.insert(std::make_pair(path, model));
	return model;
}

std::shared_ptr<Model<ModelImporter::VertexType>> ModelImporter::LoadModelImplementation(const std::string & path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_FindInvalidData |
		aiProcess_GenUVCoords |
		aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Import error in file " << path << ": " << importer.GetErrorString() << std::endl;
		return NULL;
	}
	modelDataVector modelData;
	std::string directory = path.substr(0, path.find_last_of("\\/") + 1);
	this->ProcessNode(modelData, scene->mRootNode, scene, directory);
	return std::make_shared<Model<VertexType>>(modelData);
}

void ModelImporter::ProcessNode(modelDataVector & modelData, const aiNode * node, const aiScene * scene, const std::string & directory)
{
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		this->ProcessMesh(modelData, scene->mMeshes[node->mMeshes[i]], scene, directory);
	}
	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(modelData, node->mChildren[i], scene, directory);
	}
}

void ModelImporter::ProcessMesh(modelDataVector & modelData, const aiMesh * mesh, const aiScene * scene, const std::string & directory)
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
	// load default textures to ensure that diffuse, normals and specular maps are always present
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) == 0)
	{
		textures.push_back(this->nullDiffuse);
	}
	if (mat->GetTextureCount(aiTextureType_NORMALS) + mat->GetTextureCount(aiTextureType_HEIGHT) == 0)
	{
		textures.push_back(this->nullNormal);
	}
	if (mat->GetTextureCount(aiTextureType_SPECULAR) == 0)
	{
		textures.push_back(this->nullSpecular);
	}
	auto newMesh = std::make_shared<Mesh<VertexType>>(vertices, indices);
	auto material = std::make_shared<Material>(textures);
	modelData.push_back(std::make_pair(newMesh, material));
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
