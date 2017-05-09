#pragma once

#include "..\Component.h"
#include "..\..\ModelData.h"

namespace ECS { namespace Components {
	class MaterialComponent : public Component
	{
	public:
		MaterialComponent(std::shared_ptr<Material> material) : material(material) { }
		std::shared_ptr<Material> material;
	};

	template<typename VertexType>
	class MeshComponent : public Component
	{
	public:
		MeshComponent(std::shared_ptr<Mesh<VertexType>> mesh) : mesh(mesh) { }
		std::shared_ptr<Mesh<VertexType>> mesh;
	};
}}
