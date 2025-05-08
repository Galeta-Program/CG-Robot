#pragma once 
#include "Emitter.h"

class ParticleSystem
{
private:
	std::vector<Emitter> emitters;

public:
	ParticleSystem();
	~ParticleSystem();
};