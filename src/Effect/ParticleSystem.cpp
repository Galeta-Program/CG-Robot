#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::init(unsigned int _particleAmount)
{
	for (unsigned int i = 0; i < _particleAmount; i++)
	{
		particles.emplace_back(Particle());
	}

	ssbo.initialize(particles, GL_DYNAMIC_DRAW);
}

void ParticleSystem::emit()
{
	for (int i = 0; i < emitters.size(); i++)
	{
		emitters[i].emit(particles, particlesPerEmmitter[i], particlesPerEmmitter[i + 1]);
	}


}

void ParticleSystem::setParticleAmount(unsigned int amount)
{
	particleAmount = amount;
}