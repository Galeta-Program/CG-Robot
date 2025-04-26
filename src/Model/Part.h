#pragma once

#include "../Graphic/VBO.h"
#include "../Graphic/EBO.h"
#include "../Graphic/ShaderProgram/OBJLoader.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Vertex
{
	glm::vec3 v;
	glm::vec2 t;
	glm::vec3 n;
};

class Part
{
private:
	EBO ebo;
	VBO<Vertex> vbo;

	std::vector<Vertex> arrangedVertex;
	std::vector<unsigned int> elementIndex;

	std::vector<unsigned int> faces;
	std::vector<std::string> mtlNames;

	unsigned int vertexSize;
	unsigned int elementSize; //v/t/n

public:
	Part(const char* obj, const char* mtl);
	Part(Part&& other) noexcept;
	Part(const Part& other) = delete;
	~Part();

	Part& operator=(Part&& other) noexcept;
	Part& operator=(const Part& other) = delete;

	void LoadToBuffer(const char* obj);
	bool LoadOBJ(
		const char* path,
		std::vector<unsigned int>& outMaterialIndices,
		std::vector<std::string>& outMtls,
		std::vector<Vertex>& outArrangedVertex,
		std::vector<unsigned int>& outElementIndex
	);

	inline GLuint vboId() const { return vbo.getId(); }
	inline GLuint eboId() const { return ebo.getId(); }
	
	inline unsigned int getVertexSize() const { return vertexSize; } // unique vertices (struct Vertex)
	inline unsigned int getElementSize() const { return elementSize; } // indices of vertices
	inline unsigned int getFace(unsigned int index) const { return faces[index]; }
	inline const std::vector<std::string>& getMtlNames() { return mtlNames; }
};

