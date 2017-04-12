#pragma once
class Shader
{
public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void Apply();
private:
	GLuint programId;
};

