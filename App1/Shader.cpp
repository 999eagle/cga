#include "pch.h"
#include "Shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	// read files
	std::stringstream vsStream, fsStream;
	try
	{
		std::ifstream vsFile, fsFile;
		vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		vsFile.open(vertexShaderPath);
		fsFile.open(fragmentShaderPath);
		vsStream << vsFile.rdbuf();
		fsStream << fsFile.rdbuf();
		vsFile.close();
		fsFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "Couldn't read shader file " << vertexShaderPath << " or " << fragmentShaderPath << std::endl;
	}

	const GLchar* vsCode = vsStream.str().c_str();
	const GLchar* fsCode = fsStream.str().c_str();

	// create shaders and compile
	GLuint vsId, fsId;
	GLint success;
	GLchar infoLog[512];
	vsId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsId, 1, &vsCode, NULL);
	glCompileShader(vsId);
	glGetShaderiv(vsId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vsId, sizeof(infoLog), NULL, infoLog);
		std::cerr << "Error while compiling vertex shader " << vertexShaderPath << ":\n" << infoLog << std::endl;
	}
	fsId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsId, 1, &fsCode, NULL);
	glCompileShader(fsId);
	glGetShaderiv(fsId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fsId, sizeof(infoLog), NULL, infoLog);
		std::cerr << "Error while compiling fragment shader " << fragmentShaderPath << ":\n" << infoLog << std::endl;
	}

	// create shader program
	this->programId = glCreateProgram();
	glAttachShader(this->programId, vsId);
	glAttachShader(this->programId, fsId);
	glLinkProgram(this->programId);
	glGetProgramiv(this->programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->programId, sizeof(infoLog), NULL, infoLog);
		std::cerr << "Error while linking shader program:\n" << infoLog << std::endl;
	}

	glDeleteShader(vsId);
	glDeleteShader(fsId);
}

void Shader::Apply()
{
	glUseProgram(this->programId);
}
