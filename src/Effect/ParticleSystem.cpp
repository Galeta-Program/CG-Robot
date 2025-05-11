#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::init(std::vector<int> particlesInEmitter)
{
	for (unsigned int i = 0; i < particlesInEmitter.size(); i++)
	{
		emitters.emplace_back(Emitter());
		particleAmount += particlesInEmitter[i];
		particlesPerEmmitter.emplace_back(particlesInEmitter[i]);
	}

	ssbo.initialize(particleAmount * sizeof(Particle), GL_DYNAMIC_DRAW);
}

void ParticleSystem::emit()
{
	unsigned int currentOffset = 0;
	for (int i = 0; i < emitters.size(); i++)
	{
		emitters[i].emit(ssbo, currentOffset, currentOffset + particlesPerEmmitter[i] - 1);
		currentOffset += particlesPerEmmitter[i];
	}
}

void ParticleSystem::update()
{
	ssbo.bind();
	// Dispatch compute shader
	// TODO: Add compute shader dispatch code here
	ssbo.unbind();
}

void ParticleSystem::render()
{
	// TODO: Implement rendering using the SSBO data
}

void ParticleSystem::setParticleAmount(unsigned int amount)
{
	particleAmount = amount;
}