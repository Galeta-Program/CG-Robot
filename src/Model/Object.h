#pragma once

#include "../Scene/SceneGraph/Node.h"
#include "../Graphic/VAO.h"
#include "../Graphic/Material/Texture.h"
#include "../Scene/Camera.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"

#include <string>
#include <Vector>
#include <map>

class Object
{
protected:
	GraphicShader program;

	VAO vao;
	EBO ebo;
	VBO<glm::mat4> instancingVbo;

	unsigned int instancingCount = 1;
	std::vector<glm::vec3> instancingOffests;

	bool dirty;
	glm::vec3 rotateOffsetInEuler;
	glm::quat rotateOffset;
	glm::vec3 translateOffset;
	glm::mat4 modelMatrix; // Transformation matrix of itself
	

	Object() {}
	~Object() {}

public:	
	virtual void gatherData() = 0;
	virtual void render(CG::Camera* camera, const ShaderProgram* inProgram = nullptr, GLint type = GL_TRIANGLES) = 0;
	void modifyInstance(unsigned int count);
	void setShader(const char* vert, const char* frag);

	void setTranslate(glm::vec3 trans);
	void setRotate(glm::vec3 eular);
	void setRotate(glm::quat quaternion);
	void updateEuler();
	void updateModelMatrix();
	void setInstancingOffests(std::vector<glm::vec3> _instancingOffests);

	inline const ShaderProgram& getShaderProgram() { return program; }
	inline unsigned int getInstancingCount() { return instancingCount; }
	inline glm::mat4 getModelMatrix() const { return modelMatrix; }
	inline glm::vec3 getTranslateOffset() const { return translateOffset; }
	inline glm::quat getRotateAngle() const { return rotateOffset; }
	inline glm::vec3 getEulerRotateAngle() const { return rotateOffsetInEuler; }
	inline const std::vector<glm::vec3>& getInstancingOffests() { return instancingOffests; }
};

