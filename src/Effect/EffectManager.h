#pragma once

#include "ParticleSystem.h"

#include <map>
#include <string>

class EffectManager
{
private:
	static EffectManager instance;
	std::map<std::string, ParticleSystem> effects;

	EffectManager() {}
	~EffectManager() {}

public:
	static EffectManager& getInstance();

	ParticleSystem& getEffect(std::string name);
	void registerEffect(std::string name,
		unsigned int particleAmount,
		const char* vs,
		const char* fs,
		const char* cs,
		std::vector<EmitterSettings> settings,
		const char* texturePath
	);

	inline unsigned int getSize() const { return effects.size(); }
};