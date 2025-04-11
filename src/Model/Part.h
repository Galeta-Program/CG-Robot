#pragma once

#include "../Graphic/VBO.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Part
{
private:
	VBO<glm::vec3> vbo;
	VBO<glm::vec2> uVbo;
	VBO<glm::vec3> nVbo;
	VBO<glm::vec3> mVbo;

	std::vector<unsigned int> faces;
	std::vector<std::string> mtls;

	unsigned int verticesSize;
	unsigned int uvsSize;
	unsigned int normalsSize;

public:
	Part(const char* obj);
	Part(Part&& other) noexcept;
	Part(const Part& other) = delete;
	~Part();

	Part& operator=(Part&& other) noexcept;
	Part& operator=(const Part& other) = delete;

	void Load2Buffer(const char* obj);
	void invalidBuffers();

	inline GLuint vboId() const { return vbo.getId(); }
	inline GLuint uVboId() const { return uVbo.getId(); }
	inline GLuint nVboId() const { return nVbo.getId(); }
	inline GLuint mVboId() const { return mVbo.getId(); }
	
	inline unsigned int getVerticesSize() { return verticesSize; }
	inline unsigned int getUvsSize() { return uvsSize; }
	inline unsigned int getNormalsSize() { return normalsSize; }
	inline unsigned int& getFace(unsigned int index) { return faces[index]; }
	inline std::vector<std::string>& getMtls() { return mtls; }
};

