#pragma once
#include "../Graphic/ShaderProgram/GraphicShader.h"
#include "../Graphic/ShaderProgram/ComputeShader.h"
#include "ParticleSystem.h"

// TODO: normal is also a effect
class Effect
{
private:
	GraphicShader* program;
	ComputeShader* computeProgram;
	ParticleSystem particleSystem;
	
public:
	Effect();
	~Effect();

	void use();
};