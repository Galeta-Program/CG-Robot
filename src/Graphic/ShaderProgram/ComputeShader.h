#pragma once
#include "ShaderProgram.h"
#include <GL/glew.h>

class ComputeShader: ShaderProgram
{
private:
	unsigned int program;  

	unsigned int numGroupsX;
	unsigned int numGroupsY;
	unsigned int numGroupsZ;

	const GLchar* ReadShader(const char* filename);

public:
	ComputeShader(): program(0) {}
	ComputeShader(const char* shaderFile, unsigned int x, unsigned int y, unsigned int z);
	~ComputeShader() {}

	GLuint load(const char* shaderFile) override;

	void use() const override;
};