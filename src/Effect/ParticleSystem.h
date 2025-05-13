#pragma once 
#include "Emitter.h"
#include "../Graphic/ShaderProgram/ComputeShader.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"
#include "../Graphic/SSBO.h"
#include "../Graphic/UBO.h"

#include <map>

class ParticleSystem
{
private:
	SSBO<Particle> ssbo;
	unsigned int ssboBindingPoint;

	GraphicShader graphicShader;
	ComputeShader computeShader;
	
	std::vector<Emitter> emitters;
	std::vector<int> particlesPerEmmitter; // how many particles an emmitter have, [0] = 0
	unsigned int particleAmount;

	UBO ubo;

public:
	ParticleSystem();
	~ParticleSystem();

	void init(std::vector<int> particlesInEmitter);
	void emit(); // All emitter emits
	void render(float deltaTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	
	void setupEmitter(
		unsigned int idx,
		glm::vec3 _location,
		glm::vec3 vDirA,
		glm::vec3 vDirB,
		glm::vec3 vDirC,
		glm::vec3 aDirA,
		glm::vec3 aDirB,
		glm::vec3 aDirC,
		glm::vec3 _color,
		float vMin,
		float vMax,
		float aMin,
		float aMax,
		float sMin,
		float sMax,
		float lMin,
		float lMax
	);


	void setParticleAmount(unsigned int amount);
};