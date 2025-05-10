#pragma once
#include "ShaderProgram.h"
#include <string>
#include <GL/glew.h>


class GraphicShader: ShaderProgram
{
private:
	unsigned int program;

public:
	GraphicShader() : program(0) {}
	~GraphicShader();

	GLuint load(ShaderInfo* shaders) override;

	void use() const override;
};