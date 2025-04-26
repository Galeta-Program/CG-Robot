#include "ShaderProgram.h"
#include "../../Utilty/Error.h"

#include <fstream>
#include <sstream>
#include <iostream>

ShaderProgram::~ShaderProgram()
{
	GLCall(glDeleteProgram(program));
}

void ShaderProgram::use() const
{
	GLCall(glUseProgram(program));
}

void ShaderProgram::unUse() const
{
	GLCall(glUseProgram(0));

}

const GLchar* ShaderProgram::ReadShader(const char* filename)
{
	FILE* in = fopen(filename, "rb");

	if (!in) {
		std::cout << "Shader file: " << filename << " cannot be opened." << std::endl;
		return NULL;
	}

	fseek(in, 0, SEEK_END);
	int length = ftell(in);
	rewind(in);

	GLchar* shaderSource = new GLchar[length + 1];

	fread(shaderSource, 1, length, in);
	fclose(in);

	shaderSource[length] = '\0';

	return shaderSource;
}

GLuint ShaderProgram::load(ShaderInfo* shaders)
{
	if (shaders == NULL) 
	{ 
		return 0;
	}

	GLCall(program = glCreateProgram());

	ShaderInfo* shaderInfo = shaders;
	while (shaderInfo->type != GL_NONE) 
	{		
		const GLchar* source = ReadShader(shaderInfo->filename);

		if (source == NULL)
		{
			return 0;
		}

		GLCall(GLuint shader = glCreateShader(shaderInfo->type));
		GLCall(glShaderSource(shader, 1, &source, NULL));
		GLCall(glCompileShader(shader));

		int compileResult;
		GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult));
		if (compileResult == GL_FALSE)
		{
			std::cout << "Shader: \"" << shaders->filename << "\" compile failed." << std::endl;
		}

		GLCall(glAttachShader(program, shader));

		delete[] source;
		++shaderInfo;
	}

	GLCall(glLinkProgram(program));

	GLint linkResult;
	GLCall(glGetProgramiv(program, GL_LINK_STATUS, &linkResult));
	if (!linkResult)
	{
		if (linkResult == GL_FALSE)
		{
			std::cout << "Shader: \"" << shaders->filename << "\" link failed." << std::endl;
		}

		return 0;
	}

	return program;
}