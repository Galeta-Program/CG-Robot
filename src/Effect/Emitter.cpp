# include "Emitter.h"
# include <glm/gtc/random.hpp>

glm::vec3 Emitter::rand(glm::vec3 first, glm::vec3 second, glm::vec3 third)
{
	float weight1, weight2;

	weight1 = rand(0.0, 1.0);
	weight2 = rand(0.0, 1.0);

	if (weight1 + weight2 > 1)
	{
		weight1 = 1 - weight1;
		weight2 = 1 - weight2;
	}

	return weight1 * first + weight2 * second + (1 - weight1 - weight2) * third;
}

Emitter::Emitter()
{
	unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	randomEngine.seed(seed);
}

void Emitter::init(
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
){
	setLocation(_location);
	setVelocityDirectionRange(vDirA, vDirB, vDirC);
	setAccelerationDirectionRange(aDirA, aDirB, aDirC);
	setColor(_color);
	setVelocityRange(vMin, vMax);
	setAccelerationRange(aMin, aMax);
	setSizeRange(sMin, sMax);
	setLifetimeRange(lMin, lMax);
}

void Emitter::emit(SSBO<Particle>& ssbo, unsigned int rangeFrom, unsigned int rangeTo)
{
	std::vector<Particle> particles(rangeTo - rangeFrom + 1);
	
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		Particle& p = particles[i];

		p.pos = location;
		p.size = rand(sizeMin, sizeMax);
		p.lifetime = rand(lifetimeMin, lifetimeMax);

		glm::vec3 velocityDir = rand(vDirectionRangeA, vDirectionRangeB, vDirectionRangeC);
		velocityDir = glm::normalize(velocityDir);
		p.velocity.x = velocityDir.x;
		p.velocity.y = velocityDir.y;
		p.velocity.z = velocityDir.z;
		p.velocity.w = rand(velocityValMin, velocityValMax);

		glm::vec3 accelerationDir = rand(aDirectionRangeA, aDirectionRangeB, aDirectionRangeC);
		accelerationDir = glm::normalize(accelerationDir);
		p.acceleration.x = accelerationDir.x;
		p.acceleration.z = accelerationDir.y;
		p.acceleration.x = accelerationDir.z;
		p.acceleration.w = rand(accelerationValMin, accelerationValMax);

		p.color = glm::vec4(color, 1);
	}

	ssbo.writeRange(particles, rangeFrom, rangeTo - rangeFrom + 1);
}

void Emitter::setLocation(glm::vec3 _location)
{
	location = _location;
}

void Emitter::setVelocityRange(double min, double max)
{
	velocityValMin = min; 
	velocityValMax = max;
}

void Emitter::setAccelerationRange(double min, double max)
{
	accelerationValMin = min;
	accelerationValMax = max;
}

void Emitter::setSizeRange(double min, double max)
{
	if (min <= 0 || max <= 0)
	{
		return;
	}

	sizeMin = min; 
	sizeMax = max;
}

void Emitter::setLifetimeRange(double min, double max)
{
	if (min <= 0 || max <= 0)
	{
		return;
	}
	lifetimeMin = min; 
	lifetimeMax = max;
}

void Emitter::setVelocityDirectionRange(glm::vec3 dirA, glm::vec3 dirB, glm::vec3 dirC)
{
	if (dirA == glm::vec3(0, 0, 0) ||
		dirB == glm::vec3(0, 0, 0) ||
		dirC == glm::vec3(0, 0, 0))
	{
		return;
	}

	vDirectionRangeA = glm::normalize(dirA);
	vDirectionRangeB = glm::normalize(dirB);
	vDirectionRangeC = glm::normalize(dirC);
}

void Emitter::setAccelerationDirectionRange(glm::vec3 dirA, glm::vec3 dirB, glm::vec3 dirC)
{
	if (dirA == glm::vec3(0, 0, 0) ||
		dirB == glm::vec3(0, 0, 0) ||
		dirC == glm::vec3(0, 0, 0))
	{
		return;
	}

	aDirectionRangeA = glm::normalize(dirA);
	aDirectionRangeB = glm::normalize(dirB);
	aDirectionRangeC = glm::normalize(dirC);
}

void Emitter::setColor(glm::vec3 _color)
{
	for (int i = 0; i < 4; i++)
	{
		if (_color[i] > 1 || _color[i] < 0)
		{
			return;
		}
	}

	color = _color;
}
