#pragma once

#include "../src/Utilty/LoadShaders.h"

#include <string>
#include <GL/glew.h>

class ShaderProgram
{
private:
	unsigned int program;

public:
	ShaderProgram() : program(0) {}
	~ShaderProgram();

	void load(ShaderInfo* info);

	void use() const;
	void unUse() const;

	inline unsigned int getId() const { return program; }
};