#include "ParticleSyetem.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::emit()
{
	for (int i = 0; i < emitters.size(); i++)
	{
		emitters[i].emit();
	}
}

void ParticleSystem::setParticleAmount(unsigned int amount)
{
	particleAmount = amount;
}