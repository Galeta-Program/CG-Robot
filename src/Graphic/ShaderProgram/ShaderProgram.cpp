#include "ShaderProgram.h"

#include <fstream>
#include <sstream>
#include <iostream>

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(program);
}

void ShaderProgram::load(ShaderInfo* info)
{
	program = LoadShaders(info);
}

void ShaderProgram::use() const
{
	glUseProgram(program);
}

void ShaderProgram::unUse() const
{
	glUseProgram(0);

}