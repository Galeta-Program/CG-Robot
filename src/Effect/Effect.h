#pragma once
#include "../Graphic/ShaderProgram/ShaderProgram.h"

class Effect
{
private:
	ShaderProgram program;

public:
	Effect();
	~Effect();

	void use();
};