#pragma once

#include "../Scene/SceneGraph/Node.h"
#include "../Graphic/VAO.h"
#include "../Graphic/Material/Texture.h"
#include "../Scene/Camera.h"
#include "../Model/Object.h"

#include <string>
#include <Vector>
#include <map>

class LoadedObject : public Object
{
private:
	VBO<Vertex> vbo;
	
	std::vector<Vertex> arrangedVertex;
	std::vector<unsigned int> elementIndex;

	std::vector<unsigned int> faces;
	std::string mtlName;

	unsigned int vertexSize;
	unsigned int elementSize; //v/t/n

public:
	LoadedObject();
	~LoadedObject() {}

	void initialize(const char* mtlPath, const char* objPath, const char* prefix);
	void gatherData() override;
	void render(CG::Camera* camera, GLint type = GL_TRIANGLES) override;
	bool loadOBJ(const char* path);

	inline unsigned int getVertexSize() const { return vertexSize; } // unique vertices (struct Vertex)
	inline unsigned int getElementSize() const { return elementSize; } // indices of vertices
	inline unsigned int getFace(unsigned int index) const { return faces[index]; }
	inline const std::string getMtlName() { return mtlName; }

};

