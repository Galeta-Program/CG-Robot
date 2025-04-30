#include "Object.h"

#include "../Utilty/Error.h"
#include "Part.h"
#include "../Graphic/Material/MaterialManager.h"

void Object::modifyInstance(unsigned int count)
{
	vao.bind();
	instancingCount = count;
	instancingVbo.bind();
	instancingVbo.initialize(instancingCount * sizeof(glm::mat4), GL_DYNAMIC_DRAW);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * 4 * i));
		glVertexAttribDivisor(3 + i, 1);
	}

	if (instancingCount > instancingOffests.size())
	{
		int diff = instancingCount - instancingOffests.size();
		for (int i = 0; i < diff; i++)
			instancingOffests.push_back({ 0.0, 0.0, 0.0 });
	}
}

void Object::setInstancingOffests(std::vector<glm::vec3> _instancingOffests)
{
	instancingOffests = std::move(_instancingOffests);
	modifyInstance(instancingOffests.size());
}

void Object::setShader(const char* vert, const char* frag)
{
	ShaderInfo shaders[] =
	{
		{ GL_VERTEX_SHADER,		vert },
		{ GL_FRAGMENT_SHADER,	frag },
		{ GL_NONE, NULL }
	};
	program.load(shaders);
}

void Object::updateModelMatrix()
{
	modelMatrix = glm::translate(glm::mat4(1.0f), translateOffset) * glm::mat4_cast(rotateOffset);
	dirty = false;
}

void Object::setTranslate(glm::vec3 trans)
{
	translateOffset = trans;
	dirty = true;
	updateModelMatrix();
}

void Object::setRotate(glm::vec3 eular)
{
	glm::quat angle = glm::normalize(glm::quat(glm::radians(eular)));

	if (glm::length(angle - getRotateAngle()) < 0.0001f) {
		return;
	}

	rotateOffsetInEuler = eular;

	rotateOffset = angle;

	dirty = true;
	updateModelMatrix();
}

void Object::setRotate(glm::quat quaternion)
{
	if (quaternion == rotateOffset)
	{
		return;
	}

	rotateOffset = glm::normalize(quaternion);
	dirty = true;
	updateModelMatrix();
}

void Object::updateEuler()
{
	rotateOffsetInEuler = glm::degrees(glm::eulerAngles(glm::normalize(rotateOffset)));
}


