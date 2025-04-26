#pragma once
#include "../src/Model/Part.h"
#include "../include/glm/gtc/quaternion.hpp"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtx/transform.hpp"

class Node
{
private:
	Part part;

	std::vector<Node*> children;

	bool dirty;

	glm::vec3 rotateOffsetInEuler;
	glm::quat rotateOffset;
	glm::vec3 translateOffset;

	glm::mat4 modelMatrix; // Transformation matrix of itself
	glm::mat4 parentModelMatrix; // Transformation matrix of itself

public:
	Node(const char* obj, const char* mtl, std::vector<Node*> _childern = {});
	Node(Node&& other) noexcept;
	Node(const Node& other) = delete;
	~Node() {}

	Node& operator=(Node&& other) noexcept;
	Node& operator=(const Node& other) = delete;

	void addChildren(const std::vector<Node*> _childern);
	void updateParentMatrix(const glm::mat4 parent);

	void setTranslate(glm::vec3 trans);
	void setRotate(glm::vec3 eular);
	void setRotate(glm::quat quaternion);

	void updateModelMatrix();

	inline bool isDirty() { return dirty; }
	inline Part& getPart() { return part; }
	inline glm::mat4 getModelMatrix() const { return modelMatrix; }
	inline glm::mat4 getParentModelMatrix() const { return parentModelMatrix; }
	inline glm::vec3 getTranslateOffset() const { return translateOffset; }
	inline glm::quat getRotateAngle() const { return rotateOffset; }
	inline glm::vec3 getEulerRotateAngle() const { return rotateOffsetInEuler; }
};

