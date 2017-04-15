#pragma once

class PostProcessing
{
public:
	PostProcessing(GLsizei width, GLsizei height);
	~PostProcessing();
	void BindFramebuffer();
private:
	GLuint pingpongFramebufferIds[2], pingpongTextureIds[2];
	int lastBound = 0;
};

