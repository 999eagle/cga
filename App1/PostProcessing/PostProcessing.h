#pragma once

class PostProcessing
{
public:
	PostProcessing(GLsizei width, GLsizei height);
	~PostProcessing();
	void BindFramebuffer();
	void BindReadTexture();
	void Swap(bool bindBuffers = true);
private:
	GLuint pingpongFramebufferIds[2], pingpongTextureIds[2];
	int currentDrawBuffer = 0;
};

