#pragma once

#include "../Scene/SceneGraph/Node.h"
#include "../Graphic/VAO.h"
#include "../Graphic/Material/Texture.h"
#include "../Scene/Camera.h"
#include "../Model/Part.h"

#include <string>
#include <Vector>
#include <map>

class Model
{
private:
	VAO vao;
	EBO ebo;
	VBO<Vertex> vbo;
	VBO<glm::mat4> instancingVbo;

	int partSum;
	std::vector<Node> parts;

	unsigned int instancingCount = 1;
	std::vector<glm::mat4> instancingOffests;

	bool isDisplay = true;

public:
	Model() : partSum(0) {}
	~Model() {}

	void initialize(std::vector<std::string>& mtlPaths, std::vector<std::string>& objPaths);
	void setPartsRelationship(std::vector<std::vector<unsigned int>> relationships);
	void mapMtlNameToKds(std::vector<std::string>& materials, 
						 std::map<std::string, glm::vec3>& mapping,
						 std::vector<glm::vec3>& content);
	void loadModel(const char* mtlPath, const char* objPath);
	void gatherPartsData();
	void render(GLuint program, CG::Camera* camera);
	void modifyInstance(unsigned int count);
	void setVisibility(bool _isDisplay);

	inline Node& getPart(unsigned int index) { return parts[index]; }
	inline unsigned int getPartsAmount() { return parts.size(); }
	inline unsigned int getInstancingCount() { return instancingCount; }
};

