#include "Part.h"
#include <glm/glm.hpp>

#include "../Graphic/Material/MaterialManager.h"


template class VBO<glm::vec3>;
template class VBO<glm::vec2>;

Part::Part(const char* obj, const char* mtl)
{
	loadToBuffer(obj);

	MaterialManager& manager = MaterialManager::getInstance();
	manager.registerMaterial(mtl, "../res/models2/");
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
{}

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


void Part::loadToBuffer(const char* obj)
{
	bool res = loadOBJ(obj);
	if (!res) printf("load failed\n");

	vbo.initialize(arrangedVertex);
	ebo.initialize(elementIndex);

	vertexSize = arrangedVertex.size();
	elementSize = elementIndex.size();
}

bool Part::loadOBJ(const char* path)
{
	printf("Loading OBJ file %s...\n", path);

	std::vector<glm::vec3> vertices, normals;
	std::vector<glm::vec2> textCoords;
	std::map<std::string, int> faceToIndex;

	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		printf("Impossible to open the .obj file! Are you in the right path ? See Tutorial 1 for details\n");
		return false;
	}

	int fcount = 0; // face count for material
	while (1)
	{
		char lineHeader[128];

		int result = fscanf(file, "%s", lineHeader);
		if (result == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.emplace_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 tCoord;
			fscanf(file, "%f %f\n", &tCoord.x, &tCoord.y);
			textCoords.emplace_back(tCoord);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normals.emplace_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3], texCoordIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &texCoordIndex[0], &normalIndex[0],
				&vertexIndex[1], &texCoordIndex[1], &normalIndex[1],
				&vertexIndex[2], &texCoordIndex[2], &normalIndex[2]);

			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}

			std::string keys[3] = {
				std::to_string(vertexIndex[0]) + "/" + std::to_string(texCoordIndex[0]) + "/" + std::to_string(normalIndex[0]),
				std::to_string(vertexIndex[1]) + "/" + std::to_string(texCoordIndex[1]) + "/" + std::to_string(normalIndex[1]),
				std::to_string(vertexIndex[2]) + "/" + std::to_string(texCoordIndex[2]) + "/" + std::to_string(normalIndex[2]),
			};

			Vertex tmpVertex[3]; // one face have three vertex
			for (int i = 0; i < 3; i++)
			{
				tmpVertex[i].v = vertices[vertexIndex[i] - 1];
				tmpVertex[i].t = textCoords[texCoordIndex[i] - 1];
				tmpVertex[i].n = normals[normalIndex[i] - 1];

				if (faceToIndex.find(keys[i]) == faceToIndex.end())
				{
					arrangedVertex.emplace_back(tmpVertex[i]);
					faceToIndex[keys[i]] = arrangedVertex.size() - 1;
				}

				elementIndex.emplace_back(faceToIndex[keys[i]]);
			}

			fcount++;
		}
		else if (strcmp(lineHeader, "usemtl") == 0)
		{
			faces.push_back(fcount);
			char material[50];
			fscanf(file, "%s", material);
			mtlNames.push_back(material);
			fcount = 0;
		}
		else
		{
			char trash[1000];
			fgets(trash, 1000, file);
		}
	}

	faces.push_back(fcount);

	return true;
}