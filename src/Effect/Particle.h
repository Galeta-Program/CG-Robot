#pragma once
#include <glm/glm.hpp>

struct ParticleAttribute
{
	glm::vec3 pos;
	glm::vec3 direction;
	double velocity;
	double accerleration;
	double size;
	glm::vec4 color;

	double age;
	double life;
};

class Particle
{
private:

	ParticleAttribute attribute;

public:
	Particle();
	~Particle();

};