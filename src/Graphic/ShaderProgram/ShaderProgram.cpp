#include "ShaderProgram.h"
#include "../../Utilty/Error.h"

#include <fstream>
#include <sstream>
#include <iostream>

ShaderProgram::~ShaderProgram()
{
	std::cout << "Program " << program << " has been deleted.\n";
	GLCall(glDeleteProgram(program));
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