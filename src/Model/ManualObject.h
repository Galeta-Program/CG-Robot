#pragma once

#include "../Scene/SceneGraph/Node.h"
#include "../Graphic/VAO.h"
#include "../Graphic/Material/Texture.h"
#include "../Scene/Camera.h"
#include "../Model/Object.h"

#include <string>
#include <Vector>
#include <map>

class ManualObject : public Object
{
private:
	VBO<glm::vec3> pVbo;
	VBO<glm::vec3> cVbo;
	VBO<glm::vec3> nVbo;

	std::vector<glm::vec3> points;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> normals;

public:
	ManualObject();
	~ManualObject() {}

	void initialize(std::vector<glm::vec3>& objPoints, std::vector<glm::vec3>& objColors);
	void computeNormal(GLint type = GL_TRIANGLES);
	void gatherData() override;
	void render(CG::Camera* camera, GLint type = GL_TRIANGLES) override;
};

