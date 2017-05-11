#include "pch.h"
#include "Shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	this->programId = 0;

	std::string vsCodeStr, fsCodeStr;
	if (!this->ReadFile(vertexShaderPath, vsCodeStr)) return;
	if (!this->ReadFile(fragmentShaderPath, fsCodeStr)) return;
	const GLchar* vsCode = vsCodeStr.c_str();
	const GLchar* fsCode = fsCodeStr.c_str();

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

#ifdef _DEBUG
	glObjectLabel(GL_PROGRAM, this->programId, -1, ("Shader program. VS: " + std::string(vertexShaderPath) + " FS: " + std::string(fragmentShaderPath)).c_str());
#endif

	glDeleteShader(vsId);
	glDeleteShader(fsId);
}

void Shader::Apply()
{
	glUseProgram(this->programId);
}

GLint Shader::GetUniformLocation(const GLchar * uniformName) const
{
	return glGetUniformLocation(this->programId, uniformName);
}

bool Shader::ReadFile(const char * filePath, std::string & data)
{
	std::stringstream dataStream;
	std::ifstream file;
	std::string line;
	int lineNum = 0;
	std::string directory = std::string(filePath);
	directory = directory.substr(0, directory.find_last_of("\\/") + 1);
	try
	{
		file.exceptions(std::ifstream::failbit);
		file.open(filePath);
		file.exceptions(std::ifstream::badbit);
		while (std::getline(file, line))
		{
			lineNum++;
			if (line.find("#include ", 0) == 0)
			{
				if (line[9] != '"')
				{
					std::cerr << "Error in shader file " << filePath << std::endl << "Line " << lineNum << ": \" expected after #include " << std::endl;
					return false;
				}
				auto closeQuote = line.find('\"', 10);
				if (closeQuote == std::string::npos)
				{
					std::cerr << "Error in shader file " << filePath << std::endl << "Line " << lineNum << ": \" expected before end of line" << std::endl;
					return false;
				}
				if (!this->ReadFile((directory + line.substr(10, closeQuote - 10) + ".inc.glsl").c_str(), line))
				{
					return false;
				}
			}
			dataStream << line << std::endl;
		}
		file.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "Couldn't read shader file " << filePath << std::endl;
		return false;
	}
	data = dataStream.str();
	return true;
}
