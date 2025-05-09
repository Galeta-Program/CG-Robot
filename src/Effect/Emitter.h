#pragma once
#include <glm/glm.hpp>
#include <random>
#include <chrono>
#include <vector>

struct Particle
{
	glm::vec3 pos;
	glm::vec3 direction;
	double velocity;
	double acceleration;
	double size;
	glm::vec4 color;

	double age;
	double lifetime;
};

class Emitter
{
private:
	glm::vec3 location;

	glm::vec3 directionRangeA; // emitted particle initial direction will be inside the triangular pyramid constructed by directionRangeABC
	glm::vec3 directionRangeB;
	glm::vec3 directionRangeC;

	double velocityMin;
	double velocityMax;

	double accelerationMin;
	double accelerationMax;

	double sizeMin;
	double sizeMax;

	glm::vec3 color;

	double lifetimeMin;
	double lifetimeMax;

	std::mt19937 randomEngine;

	template <typename T>
	T rand(T _min, T _max);


public:
	Emitter();
	~Emitter() {}

	void init(
		glm::vec3 _location, 
		glm::vec3 dirA,
		glm::vec3 dirB,
		glm::vec3 dirC,
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
	void emit(std::vector<Particle>& particles, unsigned int rangeFrom, unsigned int rangeTo);
	void clear();

	void setLocation(glm::vec3 _location);
	void setVelocityRange(double min, double max);
	void setAccelerationRange(double min, double max);
	void setSizeRange(double min, double max);
	void setLifetimeRange(double min, double max);
	void setDirectionRange(glm::vec3 dirA, glm::vec3 dirB, glm::vec3 dirC);
	void setColor(glm::vec3 _color);
};

template<typename T>
inline T Emitter::rand(T _min, T _max)
{
	std::uniform_int_distribution<T> dist(_min, _max);
	return dist(engine);
}
