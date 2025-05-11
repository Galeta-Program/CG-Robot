#pragma once 
#include "Emitter.h"
#include "../Graphic/SSBO.h"

#include <map>

class ParticleSystem
{
private:
	SSBO<Particle> ssbo;
	std::vector<Emitter> emitters;
	std::vector<int> particlesPerEmmitter; // how many particles an emmitter have, [0] = 0
	unsigned int particleAmount;

public:
	ParticleSystem();
	~ParticleSystem();

	void init(std::vector<int> particlesInEmitter);
	void emit(); // All emitter emits
	void render();
	void update(); // Update particles using compute shader

	void setParticleAmount(unsigned int amount);
};