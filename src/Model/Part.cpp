#include "Part.h"
#include "../src/Utilty/OBJLoader.hpp"
#include <glm/glm.hpp>

template class VBO<glm::vec3>;
template class VBO<glm::vec2>;

Part::Part(const char* obj)
{
	Load2Buffer(obj);
}

Part::Part(Part&& other) noexcept :
	vbo(std::move(other.vbo)),
	uVbo(std::move(other.uVbo)),
	nVbo(std::move(other.nVbo)),
	mVbo(std::move(other.mVbo)),
	faces(std::move(other.faces)),
	mtls(std::move(other.mtls))
{}

Part::~Part()
{

}

Part& Part::operator=(Part&& other) noexcept
{
	if (this != &other)
	{
		vbo		= std::move(other.vbo);
		uVbo	= std::move(other.uVbo);
		nVbo	= std::move(other.nVbo);
		mVbo	= std::move(other.mVbo);
		faces	= std::move(other.faces);
		mtls	= std::move(other.mtls);
	}

	return *this;
}


void Part::Load2Buffer(const char* obj)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.

	bool res = LoadOBJ(obj, vertices, uvs, normals, faces, mtls);
	if (!res) printf("load failed\n");

	vbo.initialize(vertices);
	uVbo.initialize(uvs);
	nVbo.initialize(normals);

	verticesSize = vertices.size();
	uvsSize = uvs.size();
	normalsSize = normals.size();
}

void Part::invalidBuffers()
{
	vbo.invalid();
	uVbo.invalid();
	nVbo.invalid();
	mVbo.invalid();
}
