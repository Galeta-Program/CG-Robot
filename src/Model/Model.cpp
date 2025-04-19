#include "Model.h"

#include "../Utilty/Error.h"
#include "../src/Utilty/OBJLoader.hpp"
#include "Part.h"

#define TOP_BODY 0
#define LEFT_UPPER_ARM 1
#define LEFT_LOWER_ARM 2
#define LEFT_HAND 3
#define HEAD 4
#define RIGHT_UPPER_ARM 5
#define RIGHT_LOWER_ARM 6
#define RIGHT_HAND 7
#define BOTTOM_BODY 8
#define LEFT_THIGH 9
#define LEFT_CALF 10
#define LEFT_FOOT 11
#define RIGHT_THIGH 12
#define RIGHT_CALF 13
#define RIGHT_FOOT 14

void Model::initialize(std::vector<std::string>& mtlPaths, std::vector<std::string>& objPaths)
{
	std::vector<glm::vec3> Kds;
	std::vector<glm::vec3> Kas;
	std::vector<glm::vec3> Kss;
	std::vector<std::string> Materials; //mtl-name
	std::string textureName;

	for (std::string path: mtlPaths)
	{
		LoadMTL(path.c_str(), Kds, Kas, Kss, Materials, textureName);
	}
	mapMtlNameToKds(Materials, Kds);

	for (int i = 0; i < objPaths.size(); i++)
	{
		parts.emplace_back(objPaths[i].c_str());
	}

	// maybe add another method to handle this
	if (!textureName.empty())
		texture.LoadTexture("../res/models2/" + textureName);
	else
		texture.LoadTexture("../res/models2/Robot_Base_color 7.png");
}

void Model::setPartsRelationship(std::vector<std::vector<unsigned int>> relationships)
{
	for (auto relationship : relationships)
	{
		std::vector<Node*> children;
		// first one is parent, and the others are its children
		for (int i = 1; i < relationship.size(); i++)
		{
			children.emplace_back(&parts[relationship[i]]);
		}

		parts[relationship[0]].addChildren(children);		
	}
}

void Model::mapMtlNameToKds(std::vector<std::string>& materials, std::vector<glm::vec3>& Kds)
{
	for (int i = 0; i < materials.size(); i++)
	{
		std::string mtlname = materials[i];
		//  name       vec3
		KDs[mtlname] = Kds[i];
	}
}

void Model::loadModel(const char* mtlPaths, const char* objPath)
{
	std::vector<glm::vec3> Kds;
	std::vector<glm::vec3> Kas;
	std::vector<glm::vec3> Kss;
	std::vector<std::string> Materials; //mtl-name
	std::string textureName;

	parts.emplace_back(objPath);
	LoadMTL(mtlPaths, Kds, Kas, Kss, Materials, textureName);
	mapMtlNameToKds(Materials, Kds);

	// maybe add another method to handle this
	if (!textureName.empty())
		texture.LoadTexture("../res/models2/" + textureName);
	else
		texture.LoadTexture("../res/models2/Robot_Base_color 7.png");
}

void Model::gatherPartsData()
{
	vao.bind();

	GLuint totalSize[3] = { 0,0,0 };
	GLuint offset[3] = { 0,0,0 };
	for (int i = 0; i < parts.size(); i++)
	{
		Part& currentPart = parts[i].getPart();
		totalSize[0] += currentPart.getVerticesSize() * sizeof(glm::vec3);
		totalSize[1] += currentPart.getUvsSize() * sizeof(glm::vec2);
		totalSize[2] += currentPart.getNormalsSize() * sizeof(glm::vec3);
	}

	vbo.initialize(totalSize[0]); // vbo initialized with size (no data yet)
	uVbo.initialize(totalSize[1]);
	nVbo.initialize(totalSize[2]);

	unsigned int vertices_size;
	unsigned int uvs_size;
	unsigned int normals_size;
	for (int i = 0; i < parts.size(); i++)
	{
		Part& currentPart = parts[i].getPart();
		vertices_size = currentPart.getVerticesSize();
		uvs_size = currentPart.getUvsSize();
		normals_size = currentPart.getNormalsSize();

		VBO<void>::copyAndWrite(currentPart.vboId(), vbo.getId(), 0, offset[0], vertices_size * sizeof(glm::vec3));
		offset[0] += vertices_size * sizeof(glm::vec3);
		VBO<void>::copyAndWrite(currentPart.uVboId(), uVbo.getId(), 0, offset[1], uvs_size * sizeof(glm::vec2));
		offset[1] += uvs_size * sizeof(glm::vec2);
		VBO<void>::copyAndWrite(currentPart.nVboId(), nVbo.getId(), 0, offset[2], normals_size * sizeof(glm::vec3));
		offset[2] += normals_size * sizeof(glm::vec3);
		currentPart.invalidBuffers();
	}
}

void Model::render(GLuint program, CG::Camera* camera)
{
	vao.bind();

	GLuint offset[3] = { 0,0,0 };//offset for vertices , uvs , normals
	for (int i = 0; i < parts.size(); i++)
	{
		Part& currentPart = parts[i].getPart();

		GLCall(GLuint ModelID = glGetUniformLocation(program, "u_Model"));
		glm::mat4 modelMat = parts[i].getModelMatrix();
		glm::mat4 parentModelMat = parts[i].getParentModelMatrix();
		glm::mat4 overallModelMat = parentModelMat * modelMat;
		GLCall(glUniformMatrix4fv(ModelID, 1, GL_FALSE, &overallModelMat[0][0]));

		GLCall(GLuint NormalMatID = glGetUniformLocation(program, "u_NormalMatrix"));
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(*(camera->GetViewMatrix()) * modelMat)));
		GLCall(glUniformMatrix3fv(NormalMatID, 1, GL_FALSE, &normalMatrix[0][0]));

		vbo.bind();
		// 1rst attribute buffer : vertices
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0,	3, GL_FLOAT, GL_FALSE, 0, (void*)offset[0]));
		offset[0] += currentPart.getVerticesSize() * sizeof(glm::vec3);

		// 2nd attribute buffer : UVs
		GLCall(glEnableVertexAttribArray(1));
		uVbo.bind();
		GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset[1]));
		offset[1] += currentPart.getUvsSize() * sizeof(glm::vec2);

		// 3rd attribute buffer : normals
		GLCall(glEnableVertexAttribArray(2));
		nVbo.bind();
		GLCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[2]));
		offset[2] += currentPart.getNormalsSize() * sizeof(glm::vec3);

		int vertexIDoffset = 0;//glVertexID's offset 
		std::string mtlname;//material name
		glm::vec3 Ks = glm::vec3(1, 1, 1);//because .mtl excluding specular , so give it here.

		std::vector<std::string>& mtls = currentPart.getMtls();

		for (int j = 0; j < mtls.size(); j++)
		{
			mtlname = mtls[j];
			//find the material diffuse color in map:KDs by material name.
			GLCall(GLuint M_KdID = glGetUniformLocation(program, "u_Material.Kd"));
			GLCall(GLuint M_KsID = glGetUniformLocation(program, "u_Material.Ks"));
			GLCall(glUniform3fv(M_KdID, 1, &KDs[mtlname][0]));
			GLCall(glUniform3fv(M_KsID, 1, &Ks[0]));
			//          (primitive   , glVertexID base , vertex count    )
			GLCall(glDrawArrays(GL_TRIANGLES, vertexIDoffset, currentPart.getFace(j + 1) * 3));
			//we draw triangles by giving the glVertexID base and vertex count is face count*3
			vertexIDoffset += currentPart.getFace(j + 1) * 3;//glVertexID's base offset is face count*3
		}//end for loop for draw one part of the robot	

	}//end for loop for updating and drawing model

	texture.bind(0);
	GLuint TextureID = glGetUniformLocation(program, "u_Texture");
	GLCall(glUniform1i(TextureID, 0));
}

