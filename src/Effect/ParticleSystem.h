#pragma once 
#include "Emitter.h"
#include "../Graphic/SSBO.h"

#include <map>

class ParticleSystem
{
private:
	std::vector<Particle> particles;
	SSBO<Particle> ssbo;

	std::vector<Emitter> emitters;
	std::map<unsigned int, unsigned int> particlesPerEmmitter; // how many particles an emmitter have, [0] = 0

	unsigned int particleAmount;

public:
	ParticleSystem();
	~ParticleSystem();

	void init(unsigned int _particleAmount);
	void emit(); // All emitter emits

	void setParticleAmount(unsigned int amount);
	
};