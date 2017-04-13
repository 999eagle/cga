#pragma once
class Shader
{
public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void Apply();
	GLint GetUniformLocation(const GLchar* uniformName);
private:
	GLuint programId;
};

