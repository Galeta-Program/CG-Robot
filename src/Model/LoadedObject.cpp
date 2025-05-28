#include "LoadedObject.h"

#include "../Graphic/Material/MaterialManager.h"


LoadedObject::LoadedObject()
{
	if (instancingCount > instancingOffests.size())
	{
		int diff = instancingCount - instancingOffests.size();
		for (int i = 0; i < diff; i++)
			instancingOffests.push_back({0.0, 0.0, 0.0});
	}		
	else if (instancingCount < instancingOffests.size())
	{
		instancingCount = instancingOffests.size();
	}
}

void LoadedObject::initialize(const char* mtlPath, const char* objPath, const char* prefix)
{
	bool res = loadOBJ(objPath);
	if (!res) printf("load failed\n");

	vao.bind();
	vbo.initialize(arrangedVertex);
	ebo.initialize(elementIndex);

	vertexSize = arrangedVertex.size();
	elementSize = elementIndex.size();

	MaterialManager& manager = MaterialManager::getInstance();
	manager.registerMaterial(mtlPath, "../res/models2/");
}

void LoadedObject::gatherData()
{
	vao.bind();
	vbo.bind();
	ebo.bind();

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))));

	instancingVbo.bind();
	instancingVbo.initialize(instancingCount * sizeof(glm::mat4), GL_DYNAMIC_DRAW);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * 4 * i));
		glVertexAttribDivisor(3 + i, 1);
	}
}

void LoadedObject::render(CG::Camera* camera, const ShaderProgram* inProgram, GLint type)
{
	if (!isDisplay)
		return;

	if (inProgram == nullptr)
	{
		inProgram = &program;
	}

	inProgram->use();
	instancingVbo.bind();

	glm::mat4* matrices = (glm::mat4*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (int n = 0; n < instancingCount; n++)
	{
		matrices[n] = glm::translate(glm::mat4(1.0f), instancingOffests[n]);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	vao.bind();

	GLCall(GLuint ModelID = glGetUniformLocation(inProgram->getId(), "u_Model"));
	GLCall(glUniformMatrix4fv(ModelID, 1, GL_FALSE, &modelMatrix[0][0]));

	GLCall(GLuint NormalMatID = glGetUniformLocation(inProgram->getId(), "u_NormalMatrix"));
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(*(camera->GetViewMatrix()) * modelMatrix)));
	GLCall(glUniformMatrix3fv(NormalMatID, 1, GL_FALSE, &normalMatrix[0][0]));

	MaterialManager& mtlManager = MaterialManager::getInstance();
	mtlManager.use(inProgram->getId(), mtlName);

	GLCall(glDrawElementsInstanced(type, elementSize, GL_UNSIGNED_INT, 0, instancingCount));
	program.unUse();
}

bool LoadedObject::loadOBJ(const char* path)
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
			mtlName = material;
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