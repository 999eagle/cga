#include "pch.h"
#include "QuadRenderer.h"

QuadRenderer::QuadRenderer()
{
	auto vertices = std::vector<VertexPositionTexture>();
	auto indices = std::vector<GLuint>();

	vertices.push_back(VertexPositionTexture{ glm::vec3(-1.0, 1.0, 0.0), glm::vec2(0.0, 1.0) });
	vertices.push_back(VertexPositionTexture{ glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0) });
	vertices.push_back(VertexPositionTexture{ glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0) });
	vertices.push_back(VertexPositionTexture{ glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0) });
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);
	this->fullscreenQuad = std::make_shared<Mesh<VertexPositionTexture>>(vertices, indices, "QuadRenderer\\FullscreenQuad");
}

void QuadRenderer::DrawFullscreenQuad() const
{
	this->fullscreenQuad->Draw();
}

QuadRenderer::~QuadRenderer()
{
}
