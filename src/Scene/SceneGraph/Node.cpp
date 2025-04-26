#include "Node.h"

Node::Node(const char* obj, const char* mtl, std::vector<Node*> _childern /*  = {} */) :
	part(obj, mtl),
	dirty(false),
	rotateOffsetInEuler(0, 0, 0),
	rotateOffset(1.0f, 0.0f, 0.0f, 0.0f),
	translateOffset(0.0f),
	modelMatrix(1.0f),
	parentModelMatrix(1.0f)
{
	addChildren(_childern);
	updateModelMatrix();
}

Node::Node(Node&& other) noexcept :
	part(std::move(other.part)),
	dirty(other.dirty),
	rotateOffsetInEuler(other.rotateOffsetInEuler),
	rotateOffset(other.rotateOffset),
	translateOffset(other.translateOffset),
	children(std::move(other.children)),
	modelMatrix(other.modelMatrix),
	parentModelMatrix(other.parentModelMatrix)
{}

Node& Node::operator=(Node&& other) noexcept
{
	part = std::move(other.part);
	dirty = other.dirty;
	rotateOffsetInEuler = other.rotateOffsetInEuler;
	rotateOffset = other.rotateOffset;
	translateOffset = other.translateOffset;
	children = std::move(other.children);
	modelMatrix = other.modelMatrix;
	parentModelMatrix = other.parentModelMatrix;

	return *this;
}

void Node::updateModelMatrix()
{
	modelMatrix = glm::translate(glm::mat4(1.0f), translateOffset) * glm::mat4_cast(rotateOffset);

	for (int i = 0; i < children.size(); i++)
	{
		children[i]->updateParentMatrix(parentModelMatrix * modelMatrix);
	}

	dirty = false;
}

void Node::addChildren(const std::vector<Node*> _childern)
{
	children.insert(children.end(), _childern.begin(), _childern.end());
}

void Node::updateParentMatrix(const glm::mat4 parent)
{
	parentModelMatrix = parent;
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->updateParentMatrix(parent * modelMatrix);
	}
}

void Node::setTranslate(glm::vec3 trans)
{
	translateOffset = trans;
	dirty = true;
	updateModelMatrix();
}

void Node::setRotate(glm::vec3 eular)
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

void Node::setRotate(glm::quat quaternion)
{
	if (quaternion == rotateOffset)
	{
		return;
	}

	rotateOffset = glm::normalize(quaternion);
	dirty = true;
	updateModelMatrix();
}
