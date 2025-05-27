#include "EffectManager.h"

EffectManager EffectManager::instance;

EffectManager& EffectManager::getInstance()
{
	return instance;
}

EffectStorage& EffectManager::getEffect(std::string name)
{
	if (effects.find(name) != effects.end()) {
		return effects[name];
	}
}

void EffectManager::registerParticleEffect(
	std::string name,
	std::vector<int> particleAmount,
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

	effectNameList.emplace_back(name);
	effects[name].isParticle = true;

	effects[name].ps = new ParticleSystem();
	effects[name].ps->init(particleAmount, vs, fs, cs);
	effects[name].ps->setupEmitter(settings);
	effects[name].ps->emit();

	if (texturePath != "")
	{
		effects[name].ps->setTexture(texturePath);
	}

	effects[name].ln = nullptr;
	
}

void EffectManager::registerLightningEffect(std::string name)
{
	if (effects.find(name) != effects.end()) {
		return;
	}
	effectNameList.emplace_back(name);

	effects[name].ln = new Lightning();
	effects[name].ps = nullptr;
	effects[name].isParticle = false;
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
		return;
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
		if (currentEffect->isParticle)
		{
			currentEffect->ps->render(timeNow, deltaTime, viewMatrix, projectionMatrix, emitter);
		}
		else
		{
			currentEffect->ln->render(deltaTime, viewMatrix, projectionMatrix);
		}
	}
}
