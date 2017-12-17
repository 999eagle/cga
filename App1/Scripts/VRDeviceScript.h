#pragma once

#include "..\ECS\Components\ScriptComponent.h"
#include "..\ECS\Systems\RenderSystem.h"
#include "..\Common\util.h"

namespace Scripts
{
	class VRDeviceScript : public ECS::Components::Script
	{
	public:
		typedef ECS::Systems::RenderSystem::VertexType VertexType;

		VRDeviceScript(vr::IVRSystem * vr, vr::TrackedDeviceIndex_t index)
			: vr(vr), index(index), textureId(-1)
		{
		}

		void Start() { }
		void FixedUpdate(const AppTime & time) { }
		void Update(const AppTime & time)
		{
			bool isConnected = this->vr->IsTrackedDeviceConnected(index);
			if (this->hasModel && !isConnected)
			{
				if (this->hasTextures)
				{
					this->component->GetEntity()->RemoveComponent<ECS::Components::MaterialComponent>();
					this->hasTextures = false;
				}
				this->component->GetEntity()->RemoveComponent<ECS::Components::MeshComponent<VertexType>>();
				this->hasModel = false;
			}
			if (!this->hasModel && isConnected)
			{
				if (this->modelName.size() == 0)
				{
					this->modelName = GetTrackedDeviceString(this->vr, this->index, vr::Prop_RenderModelName_String);
				}
				vr::RenderModel_t * model;
				auto error = vr::VRRenderModels()->LoadRenderModel_Async(this->modelName.c_str(), &model);
				if (error == vr::VRRenderModelError_Loading)
				{
					return;
				}
				else if (error == vr::VRRenderModelError_None)
				{
					std::vector<VertexType> vertices;
					std::vector<GLuint> indices;
					for (auto i = 0u; i < model->unVertexCount; i++)
					{
						VertexType vertex;
						vrVectorToGlm(vertex.position, model->rVertexData[i].vPosition);
						vrVectorToGlm(vertex.normal, model->rVertexData[i].vNormal);
						vertex.texCoord = glm::vec2(model->rVertexData[i].rfTextureCoord[0], model->rVertexData[i].rfTextureCoord[1]);
						vertices.push_back(vertex);
					}
					for (auto i = 0u; i < model->unTriangleCount * 3; i += 3)
					{
						indices.push_back(model->rIndexData[i + 0]);
						indices.push_back(model->rIndexData[i + 1]);
						indices.push_back(model->rIndexData[i + 2]);
						VertexType & vtxA = vertices.at(model->rIndexData[i + 0]);
						VertexType & vtxB = vertices.at(model->rIndexData[i + 1]);
						VertexType & vtxC = vertices.at(model->rIndexData[i + 2]);
						glm::vec3 deltaBA = vtxB.position - vtxA.position;
						glm::vec3 deltaCA = vtxC.position - vtxA.position;
						glm::vec2 uvDeltaBA = vtxB.texCoord - vtxA.texCoord;
						glm::vec2 uvDeltaCA = vtxC.texCoord - vtxA.texCoord;
						float area = uvDeltaBA.x * uvDeltaCA.y - uvDeltaBA.y * uvDeltaCA.x;
						glm::vec3 tangent;
						if (area != 0)
						{
							float factor = 1 / area;
							tangent = factor * (deltaBA * uvDeltaCA.y - deltaCA * uvDeltaBA.y);
						}
						vtxA.tangent += tangent;
						vtxA.bitangent += glm::cross(vtxA.normal, tangent);
						vtxB.tangent += tangent;
						vtxB.bitangent += glm::cross(vtxB.normal, tangent);
						vtxC.tangent += tangent;
						vtxC.bitangent += glm::cross(vtxC.normal, tangent);
					}
					for (auto it : vertices)
					{
						it.tangent = glm::normalize(it.tangent);
						it.bitangent = glm::normalize(it.bitangent);
					}
					this->component->GetEntity()->AddComponent<ECS::Components::MeshComponent<VertexType>>(std::make_shared<Mesh<VertexType>>(vertices, indices, "VRRenderModel " + this->modelName));
					this->textureId = model->diffuseTextureId;
					this->hasModel = true;
				}
			}
			if (!this->hasTextures && this->textureId >= 0 && isConnected)
			{
				vr::RenderModel_TextureMap_t * texture;
				auto error = vr::VRRenderModels()->LoadTexture_Async(this->textureId, &texture);
				if (error == vr::VRRenderModelError_Loading)
				{
					return;
				}
				else if (error == vr::VRRenderModelError_None)
				{
					auto newTexture = std::make_shared<Texture>();
					newTexture->type = aiTextureType_DIFFUSE;
					glGenTextures(1, &newTexture->textureId);
					glBindTexture(GL_TEXTURE_2D, newTexture->textureId);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_SRGB, texture->unWidth, texture->unHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->rubTextureMapData);
					glGenerateMipmap(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, 0);

#ifdef _DEBUG
					glObjectLabel(GL_TEXTURE, newTexture->textureId, -1, ("VRRenderModelTexture" + std::to_string(this->textureId)).c_str());
#endif
					std::vector<std::shared_ptr<Texture>> textures;
					textures.push_back(newTexture);
					auto material = std::make_shared<Material>(textures);
					this->component->GetEntity()->AddComponent<ECS::Components::MaterialComponent>(material);
					this->hasTextures = true;
				}
			}
		}
	private:
		bool hasModel, hasTextures;
		std::string modelName;
		vr::TextureID_t textureId;
		vr::IVRSystem * vr;
		vr::TrackedDeviceIndex_t index;
	};
}
