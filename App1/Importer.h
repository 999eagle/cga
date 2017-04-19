#pragma once

#include "ModelData.h"

class TextureImporter
{
public:
	~TextureImporter();
	std::shared_ptr<Texture> LoadTexture(const std::string & path, aiTextureType type = aiTextureType_UNKNOWN);
	static TextureImporter & GetInstance()
	{
		static TextureImporter instance;
		return instance;
	}
private:
	std::map<std::string, std::shared_ptr<Texture>> loadedTextures;
	TextureImporter() { }
};

class MaterialImporter
{
public:
	~MaterialImporter() { }
	std::shared_ptr<Material> LoadMaterial(const std::string & albedoMapPath, const std::string & normalMapPath, const std::string & metallicMapPath, const std::string & roughnessMapPath);
	std::shared_ptr<Material> LoadMaterial(const std::string & materialPath);
	static MaterialImporter & GetInstance()
	{
		static MaterialImporter instance;
		return instance;
	}
private:
	std::map<std::string, std::shared_ptr<Material>> loadedMaterials;
	MaterialImporter() { }
};

class ModelImporter
{
public:
	typedef
		VertexPositionTextureNTB
		VertexType;
	typedef
		std::vector<std::pair<std::shared_ptr<Mesh<VertexType>>, std::shared_ptr<Material>>>
		modelDataVector;
	std::shared_ptr<Model<VertexType>> LoadModel(const std::string & path);
	static ModelImporter & GetInstance()
	{
		static ModelImporter instance;
		return instance;
	}
private:
	std::map<std::string, std::shared_ptr<Model<VertexType>>> loadedModels;
	ModelImporter() { }
	std::shared_ptr<Model<VertexType>> LoadModelImplementation(const std::string & path);
	void ProcessNode(modelDataVector & modelData, const aiNode * node, const aiScene * scene, const std::string & directory, const std::string & name);
	void ProcessMesh(modelDataVector & modelData, const aiMesh * mesh, const aiScene * scene, const std::string & directory, const std::string & name);
	void LoadTextures(std::vector<std::shared_ptr<Texture>> & textures, const aiMaterial * material, aiTextureType type, const std::string & directory);
};
