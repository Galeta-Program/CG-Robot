#include "EffectManager.h"

EffectManager EffectManager::instance;

EffectManager& EffectManager::getInstance()
{
	return instance;
}

ParticleSystem& EffectManager::getEffect(std::string name)
{
	if (effects.find(name) != effects.end()) {
		return effects[name];
	}
}

void EffectManager::registerEffect(
	std::string name,
	unsigned int particleAmount, 
	const char* vs,
	const char* fs,
	const char* cs,
	std::vector<EmitterSettings> settings,
	const char* texturePath
	)
{
	if(effects.find(name) != effects.end()) {
		return;
	}
	effects[name] = ParticleSystem();

	std::vector<int> particlesPerEmitter;
	particlesPerEmitter.emplace_back(particleAmount);
	effects[name].init(particlesPerEmitter, vs, fs, cs);
	effects[name].setupEmitter(settings);
	effects[name].emit();

	if (texturePath != "")
	{
		effects[name].setTexture(texturePath);
	}
}
