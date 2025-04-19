#pragma once
#include "../include/glm/glm.hpp"
#include "../include/glm/gtx/transform.hpp"

class Light
{
private:
	glm::vec3 position;

public:
	Light(): position(0, 10, 50) {}
	~Light() {}

	void setPosition(glm::vec3 pos);

	inline glm::vec3 getPos() const { return position; }
};