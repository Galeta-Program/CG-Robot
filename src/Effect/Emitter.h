#pragma once
#include <glm/glm.hpp>
#include <random>
#include <chrono>
#include <vector>
#include "../Graphic/SSBO.h"

struct Particle
{
	double size;
	double lifetime;
	glm::vec4 velocity;     // xyz for direction, w for speed
	glm::vec4 acceleration; // xyz for direction, w for magnitude
	glm::vec4 color;
	glm::vec3 pos;
};

class Emitter
{
private:
	glm::vec3 location;

	glm::vec3 vDirectionRangeA; // emitted particle initial direction will be inside the triangular pyramid constructed by directionRangeABC
	glm::vec3 vDirectionRangeB;
	glm::vec3 vDirectionRangeC;

	glm::vec3 aDirectionRangeA; 
	glm::vec3 aDirectionRangeB;
	glm::vec3 aDirectionRangeC;

	float velocityValMin;
	float velocityValMax;

	float accelerationValMin;
	float accelerationValMax;

	double sizeMin;
	double sizeMax;

	glm::vec3 color;

	double lifetimeMin;
	double lifetimeMax;

	std::mt19937 randomEngine;

	template <typename T>
	T rand(T _min, T _max);

	glm::vec3 rand(glm::vec3 first, glm::vec3 second, glm::vec3 third);

public:
	Emitter();
	~Emitter() {}

	void init(
		glm::vec3 _location, 
		glm::vec3 vDirA,
		glm::vec3 vDirB,
		glm::vec3 vDirC,
		glm::vec3 aDirA,
		glm::vec3 aDirB,
		glm::vec3 aDirC,
		glm::vec3 _color,
		double vMin,
		double vMax,
		double aMin,
		double aMax,
		double sMin,
		double sMax,
		double lMin,
		double lMax
		);

	void emit(SSBO<Particle>& ssbo, unsigned int rangeFrom, unsigned int rangeTo);

	void setLocation(glm::vec3 _location);
	void setVelocityRange(double min, double max);
	void setAccelerationRange(double min, double max);
	void setSizeRange(double min, double max);
	void setLifetimeRange(double min, double max);
	void setVelocityDirectionRange(glm::vec3 dirA, glm::vec3 dirB, glm::vec3 dirC);
	void setAccelerationDirectionRange(glm::vec3 dirA, glm::vec3 dirB, glm::vec3 dirC);
	void setColor(glm::vec3 _color);
};
template<typename T>
inline T Emitter::rand(T _min, T _max)
{
	std::uniform_real_distribution<T> dist(_min, _max);
	return dist(randomEngine);
}

