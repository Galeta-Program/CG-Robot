# include "Emitter.h"
# include <glm/gtc/random.hpp>

Emitter::Emitter()
	: location(0.0f), vDirection(0.0f, 1.0f, 0.0f), aDirection(0.0f), velocity(0.0f), acceleration(0.0f), size(1.0f)
{
	unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	randomEngine.seed(seed);
}

void Emitter::init(
	glm::vec3 _location,
	glm::vec3 vDir,
	glm::vec3 aDir,
	float v,
	float a,
	float s
){
	setLocation(_location);
	setVelocityDirection(vDir);
	setAccelerationDirection(aDir);
	setVelocity(v);
	setAcceleration(a);
	setSize(s);
}

// ideal emitter 
// noice in compute shader
void Emitter::emit(SSBO<Particle>& ssbo, unsigned int rangeFrom, unsigned int rangeTo)
{
	std::vector<Particle> particles(rangeTo - rangeFrom + (unsigned int)(1));
	
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		Particle& p = particles[i];

		p.pos = location;
		p.size = size;
		p.initialSize = size;
		p.lifetime = 0;
		p.initialLifetime = 6;

		glm::vec3 velocityDir = vDirection;
		p.velocity.x = velocityDir.x;
		p.velocity.y = velocityDir.y;
		p.velocity.z = velocityDir.z;
		p.velocity.w = velocity;

		glm::vec3 accelerationDir = aDirection;
		p.acceleration.x = accelerationDir.x;
		p.acceleration.y = accelerationDir.y;
		p.acceleration.z = accelerationDir.z;
		p.acceleration.w = acceleration;

		p.color = glm::vec4(1, 1, 1, 1);
	}

	ssbo.writeRange(particles, rangeFrom, particles.size());
}

void Emitter::setLocation(glm::vec3 _location)
{
	location = _location;
}

void Emitter::setVelocity(float v)
{
	velocity = v;
}

void Emitter::setAcceleration(float a)
{
	acceleration = a;
}

void Emitter::setSize(float s)
{
	if (s <= 0)
	{
		return;
	}

	this->size = s;
}

void Emitter::setVelocityDirection(glm::vec3 dir)
{
	if (dir == glm::vec3(0, 0, 0))
	{
		return;
	}

	vDirection = glm::normalize(dir);
}

void Emitter::setAccelerationDirection(glm::vec3 dir)
{
	if (dir == glm::vec3(0, 0, 0))
	{
		return;
	}

	aDirection = glm::normalize(dir);
}
