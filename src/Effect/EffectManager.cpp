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
	std::vector<bool> _effectParamMask,
	const char* texturePath
	)
{
	if(effects.find(name) != effects.end()) {
		return;
	}
	effects[name] = ParticleSystem();
	effectNameList.emplace_back(name);

	std::vector<int> particlesPerEmitter;
	particlesPerEmitter.emplace_back(particleAmount);
	effects[name].init(particlesPerEmitter, vs, fs, cs);
	effects[name].setupEmitter(settings);
	effects[name].emit();

	effectParamMask[name] = _effectParamMask;

	if (texturePath != "")
	{
		effects[name].setTexture(texturePath);
	}
}

void EffectManager::passParam(std::string name, EffectParam param)
{
	if (instance.effects.find(name) == instance.effects.end())
	{
		std::cout << "Effect not registered.\n";
		return;
	}

}

void EffectManager::setCurrentEffect(std::string name)
{
	if (name == "None")
	{
		currentEffect = nullptr;
	}

	currentEffect = &effects[name];
}

void EffectManager::render(
	float timeNow, 
	float deltaTime, 
	const glm::mat4& viewMatrix, 
	const glm::mat4& projectionMatrix,
	unsigned int emitter /*= -1*/)
{
	if (currentEffect != nullptr)
	{
		currentEffect->render(timeNow, deltaTime, viewMatrix, projectionMatrix, emitter);
	}
}
