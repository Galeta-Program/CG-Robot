#pragma once 
#include "Emitter.h"
#include "../Graphic/ShaderProgram/ComputeShader.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"
#include "../Graphic/SSBO.h"
#include "../Graphic/UBO.h"
#include "../Graphic/Material/Texture.h"

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

	bool haveTexture;

	UBO ubo;
	Texture texture;

public:
	ParticleSystem();
	~ParticleSystem();

	void init(std::vector<int> particlesInEmitter);
	void emit(); // All emitter emits
	void render(float timeNow, float deltaTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	
	void setupEmitter(
		unsigned int idx, 
		glm::vec3 _location,
		glm::vec3 vDir, 
		glm::vec3 aDir, 
		float v, 
		float a, 
		float s
	);

	void setTexture(std::string path);
	void setParticleAmount(unsigned int amount);
};