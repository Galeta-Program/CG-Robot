#pragma once
#include "ParticleSystem.h"

#include <map>
#include <string>
#include <glm/glm.hpp>

struct EffectParam
{
	glm::vec3 pos;
	glm::vec3 dir;
};

struct Effect
{
	std::string name; // Can only use the effects that is defined beforehand. Like Fire, Firework
	bool isStart; // emit()
	bool isFinished; // cancel rendering
	EffectParam param;
};

class EffectManager
{
private:
	static EffectManager instance;
	std::vector<std::string> effectNameList;
	std::map<std::string, ParticleSystem> effects;
	std::map<std::string, std::vector<bool>> effectParamMask;

	ParticleSystem* currentEffect;

	EffectManager() : currentEffect(nullptr) {  }
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
		std::vector<bool> _effectParamMask,
		const char* texturePath
	);
	void passParam(std::string name, EffectParam param);
	void setCurrentEffect(std::string name);
	void render(float timeNow, float deltaTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, unsigned int emitter = -1);

	inline std::vector<std::string>& getNameList() { return effectNameList; }
	inline unsigned int getSize() { return effects.size(); }
};