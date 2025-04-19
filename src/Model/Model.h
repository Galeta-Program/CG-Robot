#pragma once

#include "../Scene/SceneGraph/Node.h"
#include "../Graphic/VAO.h"
#include "../Graphic/Texture.h"
#include "../Scene/Camera.h"

#include <string>
#include <Vector>
#include <map>

class Model
{
private:
	VAO vao;

	VBO<glm::vec3> vbo;
	VBO<glm::vec2> uVbo;
	VBO<glm::vec3> nVbo;
	VBO<glm::vec3> mVbo;

	Texture texture;

	std::map<std::string, glm::vec3> KDs; //mtl-name&Kd
	std::map<std::string, glm::vec3> KSs; //mtl-name&Ks -> not used now

	int partSum;
	std::vector<Node> parts;

public:
	Model() : partSum(0) {}
	~Model() {}

	void initialize(std::vector<std::string>& mtlPaths, std::vector<std::string>& objPaths);
	void setPartsRelationship(std::vector<std::vector<unsigned int>> relationships);
	void mapMtlNameToKds(std::vector<std::string>& materials, std::vector<glm::vec3>& Kds);
	void loadModel(const char* mtlPaths, const char* objPath);
	void gatherPartsData();
	void render(GLuint program, CG::Camera* camera);

	inline Node& getPart(unsigned int index) { return parts[index]; }
	inline unsigned int getPartsAmount() { return parts.size(); }
};

