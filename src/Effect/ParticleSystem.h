#pragma once 
#include "Emitter.h"
#include "../Graphic/ShaderProgram/ComputeShader.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"
#include "../Graphic/SSBO.h"
#include "../Graphic/UBO.h"
#include "../Graphic/Material/Texture.h"

#include <map>

struct EmitterSettings
{
	unsigned int idx;
	glm::vec3 _location;
	glm::vec3 vDir;
	glm::vec3 aDir;
	float v;
	float a;
	float s;
};

class ParticleSystem
{
private:
	SSBO<Particle> ssbo;
	unsigned int ssboBindingPoint;

	GraphicShader graphicShader;
	ComputeShader computeShader;

	std::vector<Emitter> emitters;
	std::vector<unsigned int> particlesPerEmmitter; // how many particles an emmitter have, [0] = 0
	unsigned int particleAmount;

	bool haveTexture;

	UBO ubo;
	Texture texture;

public:
	ParticleSystem();
	ParticleSystem(ParticleSystem&& other) noexcept;
	ParticleSystem(const ParticleSystem& other) = delete;
	~ParticleSystem();

	ParticleSystem& operator=(ParticleSystem&& other) noexcept;
	ParticleSystem& operator=(const ParticleSystem& other) = delete;


	void init(std::vector<int> particlesInEmitter, const char* vs, const char* fs, const char* cs);
	void emit(); 
	void render(float timeNow,
		float deltaTime,
		const glm::mat4& viewMatrix,
		const glm::mat4& projectionMatrix
	); 

	void addEmitter(unsigned int particleCount, glm::vec3 initialPos = glm::vec3(0.0f, -1000.0f, 0.0f));
	void setupEmitter(std::vector<EmitterSettings> settings);
	void setEmitterPos(unsigned int index, glm::vec3 pos);
	void setEmitterDir(unsigned int index, glm::vec3 dir);
	void setEmitterVelocity(unsigned int index, float val);
	void setEmitterAcceleration(unsigned int index, float val);
	void setEmitterParticleSize(unsigned int index, float val);

	void disableEmitter(unsigned int idx);
	void enableEmitter(unsigned int idx);

	void reset();

	void setTexture(std::string path);
	void setParticleAmount(unsigned int amount);

	glm::vec3 getEmitterPos(unsigned int index);
	glm::vec3 getEmitterDir(unsigned int index);
	bool getEmitterEnableState(unsigned int index);

	inline unsigned int getEmitterCount() { return emitters.size(); }
};