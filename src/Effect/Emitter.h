#pragma once
#include <glm/glm.hpp>
#include <random>
#include <chrono>
#include <vector>
#include "../Graphic/SSBO.h"

struct Particle
{
	float size;
	float initialSize;      
	float lifetime;
	float initialLifetime;  
	glm::vec4 velocity;          // .w for speed
	glm::vec4 acceleration;      // .w for magnitude
	glm::vec3 pos;
	glm::vec4 color;             
};

class Emitter
{
private:
	glm::vec3 location;

	glm::vec3 vDirection;
	glm::vec3 aDirection;

	float velocity;
	float acceleration;

	float size; // ideal particle size
	float lifetime;

	std::mt19937 randomEngine;

public:
	Emitter();
	~Emitter() {}

	void init(
		glm::vec3 _location, 
		glm::vec3 vDir,
		glm::vec3 aDir,
		float v,
		float a,
		float s
		);

	void emit(SSBO<Particle>& ssbo, unsigned int rangeFrom, unsigned int rangeTo);

	void setLocation(glm::vec3 _location);
	void setVelocity(float v);
	void setAcceleration(float a);
	void setSize(float s);
	void setVelocityDirection(glm::vec3 dir);
	void setAccelerationDirection(glm::vec3 dir);

	inline glm::vec3 getPos() const { return location; }
	inline glm::vec3 getVDir() const { return vDirection; }
	inline glm::vec3 getADir() const { return aDirection; }
	inline float getVelocity() const { return velocity; }
	inline float getAcceleration() const { return acceleration; }
	inline float getSize() const { return size; }
};
