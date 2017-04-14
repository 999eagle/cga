#pragma once

#include "ModelData.h"

class QuadRenderer
{
public:
	~QuadRenderer();
	static QuadRenderer & GetInstance()
	{
		static QuadRenderer instance;
		return instance;
	}
	void DrawFullscreenQuad() const;
private:
	QuadRenderer();
	std::shared_ptr<Mesh<VertexPositionTexture>> fullscreenQuad;
};

