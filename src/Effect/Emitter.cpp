# include "Emitter.h"

Emitter::Emitter()
{
	unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	randomEngine.seed(seed);
}

void Emitter::init(unsigned int amount, glm::vec3 _location, glm::vec3 dirA, glm::vec3 dirB, glm::vec3 dirC, glm::vec3 _color, double vMin, double vMax, double aMin, double aMax, double sMin, double sMax, double lMin, double lMax)
{
	setParticleAmount(amount);
	setLocation(_location);
	setDirectionRange(dirA, dirB, dirC);
	setColor(_color);
	setVelocityRange(vMin, vMax);
	setAccelerationRange(aMin, aMax);
	setSizeRange(sMin, sMax);
	setLifetimeRange(lMin, lMax);
}

void Emitter::emit()
{
}

void Emitter::clear()
{
}

void Emitter::setLocation(glm::vec3 _location)
{
	location = _location;
}

void Emitter::setParticleAmount(unsigned int amount)
{
	particleAmount = amount;
}

void Emitter::setVelocityRange(double min, double max)
{
	velocityMin = min; 
	velocityMax = max;
}

void Emitter::setAccelerationRange(double min, double max)
{
	accelerationMin = min; 
	accelerationMax = max;
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

void Emitter::setDirectionRange(glm::vec3 dirA, glm::vec3 dirB, glm::vec3 dirC)
{
	if (dirA == glm::vec3(0, 0, 0) ||
		dirB == glm::vec3(0, 0, 0) ||
		dirC == glm::vec3(0, 0, 0))
	{
		return;
	}

	directionRangeA = glm::normalize(dirA);
	directionRangeB = glm::normalize(dirB);
	directionRangeC = glm::normalize(dirC);
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
