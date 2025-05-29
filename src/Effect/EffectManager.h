#pragma once
#include "ParticleSystem.h"
#include "Lightning.h"
#include <map>
#include <string>
#include <glm/glm.hpp>

struct EffectParam
{
	// ps
	std::vector<glm::vec3> pos;
	std::vector<glm::vec3> dir;
	std::vector<bool> enable;

	// ln
	glm::vec3 center;
	std::vector<glm::vec3> endpoints;
};

struct Effect
{
	std::string name; // Can only use the effects that is defined beforehand. Like Fire, Firework
	bool isStart; // emit()
	bool isFinished; // cancel rendering
	bool isParticleEffect;
	EffectParam param;
};

struct EffectStorage
{
	ParticleSystem* ps;
	Lightning* ln;
	bool isParticle;
};

class EffectManager
{
private:
	static EffectManager instance;
	std::vector<std::string> effectNameList;
	std::map<std::string, EffectStorage> effects;
	std::map<std::string, std::vector<bool>> effectParamMask;

	EffectStorage* currentEffect;

	EffectManager() : currentEffect(nullptr) {  }
	~EffectManager() {}

public:
	static EffectManager& getInstance();

	EffectStorage& getEffect(std::string name);
	void registerParticleEffect(
		std::string name,
		std::vector<int> particleAmount,
		const char* vs,
		const char* fs,
		const char* cs,
		std::vector<EmitterSettings> settings,
		const char* texturePath = ""
	);
	void registerLightningEffect(std::string name);
	void passParam(std::string name, EffectParam param);
	void setCurrentEffect(std::string name);
	void render(float timeNow, float deltaTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	inline std::vector<std::string>& getNameList() { return effectNameList; }
	inline unsigned int getSize() { return effects.size(); }
};