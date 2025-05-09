#pragma once 
#include "Emitter.h"
#include "../Graphic/SSBO.h"

#include <map>

class ParticleSystem
{
private:
	SSBO<Particle> ssbo;

	std::vector<Emitter> emitters;
	std::map<unsigned int, unsigned int> particlesPerEmmitter; // how many particles an emmitter have

	unsigned int particleAmount;

public:
	ParticleSystem();
	~ParticleSystem();

	void emit(); // All emitter emits

	void setParticleAmount(unsigned int amount);
	
};