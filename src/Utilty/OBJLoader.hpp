#include <glm/glm.hpp>
#include <string>
#include <vector>

#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H


struct Vertex
{
	glm::vec3 v;
	glm::vec2 t;
	glm::vec3 n;
};

bool LoadOBJ(
	const char* path,
	std::vector<unsigned int>& outMaterialIndices,
	std::vector<std::string>& outMtls,
	std::vector<Vertex>& outArrangedVertex,
	std::vector<unsigned int>& outElementIndex
);

bool LoadMTL(const char * path,
			 std::vector<glm::vec3> &Kd,
			 std::vector<glm::vec3> &Ka,
			 std::vector<glm::vec3> &Ks,
			 std::vector<std::string> &out_name,
			 std::string &texture
			 );

bool LoadAssImp(
	const char * path, 
	std::vector<unsigned short> & indices,
	std::vector<glm::vec3> & vertices,
	std::vector<glm::vec2> & uvs,
	std::vector<glm::vec3> & normals
);


#endif