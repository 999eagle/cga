#pragma once

#include "..\Component.h"
#include "..\..\ModelData.h"

namespace ECS { namespace Components {
	class MaterialComponent : public Component
	{
	public:
		MaterialComponent(Material * material) : material(material) { }
		Material * material;
	};

	template<typename VertexType>
	class MeshComponent : public Component
	{
	public:
		MeshComponent(Mesh<VertexType> * mesh) : mesh(mesh) { }
		Mesh<VertexType> * mesh;
	};
}}
