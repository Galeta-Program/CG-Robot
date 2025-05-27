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
	void emit(); // All emitter emits
	void render(float timeNow,
		float deltaTime,
		const glm::mat4& viewMatrix,
		const glm::mat4& projectionMatrix,
		unsigned int emitter = -1
	); // used in animation

	void addEmitter();
	void setupEmitter(std::vector<EmitterSettings> settings);
	void setEmitterPos(unsigned int index, glm::vec3 pos);
	void setEmitterDir(unsigned int index, glm::vec3 dir);

	void setTexture(std::string path);
	void setParticleAmount(unsigned int amount);

	glm::vec3 getEmitterPos(unsigned int index);
	glm::vec3 getEmitterDir(unsigned int index);

	inline unsigned int getEmitterCount() { return emitters.size(); }
};