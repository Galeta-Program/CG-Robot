#include "Part.h"
#include <glm/glm.hpp>

template class VBO<glm::vec3>;
template class VBO<glm::vec2>;

Part::Part(const char* obj)
{
	LoadToBuffer(obj);
}

Part::Part(Part&& other) noexcept :
	ebo(std::move(other.ebo)),
	vbo(std::move(other.vbo)),
	faces(std::move(other.faces)),
	mtlNames(std::move(other.mtlNames)),
	vertexSize(other.vertexSize),
	elementSize(other.elementSize)
{}

Part::~Part()
{

}

Part& Part::operator=(Part&& other) noexcept
{
	if ( this != &other )
	{
		ebo		= std::move(other.ebo);
		vbo		= std::move(other.vbo);
		faces	= std::move(other.faces);
		mtlNames	= std::move(other.mtlNames);
		vertexSize = other.vertexSize;
		elementSize = other.elementSize;
	}

	return *this;
}


void Part::LoadToBuffer(const char* obj)
{
	bool res = LoadOBJ(obj, faces, mtlNames, arrangedVertex, elementIndex);
	if (!res) printf("load failed\n");

	vbo.initialize(arrangedVertex);
	ebo.initialize(elementIndex);

	vertexSize = arrangedVertex.size();
	elementSize = elementIndex.size();
}
