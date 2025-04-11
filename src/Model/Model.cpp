#include "Model.h"

#include "../src/Utilty/OBJLoader.hpp"

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
		parts.emplace_back(Part(objPaths[i].c_str()));
	}

	// maybe add another method to handle this
	if (!textureName.empty())
		texture.LoadTexture("../res/models2/" + textureName);
	else
		texture.LoadTexture("../res/models2/Robot_Base_color 7.png");
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
	std::string textureName;

	parts.emplace_back(Part(objPath));
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
		totalSize[0] += parts[i].getVerticesSize() * sizeof(glm::vec3);
		totalSize[1] += parts[i].getUvsSize() * sizeof(glm::vec2);
		totalSize[2] += parts[i].getNormalsSize() * sizeof(glm::vec3);
	}

	vbo.initialize(totalSize[0]); // vbo initialized with size (no data yet)
	uVbo.initialize(totalSize[1]);
	nVbo.initialize(totalSize[2]);

	unsigned int vertices_size;
	unsigned int uvs_size;
	unsigned int normals_size;
	for (int i = 0; i < parts.size(); i++)
	{
		vertices_size = parts[i].getVerticesSize();
		uvs_size = parts[i].getUvsSize();
		normals_size = parts[i].getNormalsSize();

		VBO<void>::copyAndWrite(parts[i].vboId(), vbo.getId(), 0, offset[0], vertices_size * sizeof(glm::vec3));
		VBO<void>::copyAndWrite(parts[i].uVboId(), uVbo.getId(), 0, offset[0], uvs_size * sizeof(glm::vec3));
		VBO<void>::copyAndWrite(parts[i].nVboId(), nVbo.getId(), 0, offset[0], normals_size * sizeof(glm::vec3));
		parts[i].invalidBuffers();
	}
}

void Model::render(GLuint program)
{
	vao.bind();

	GLuint offset[3] = { 0,0,0 };//offset for vertices , uvs , normals
	for (int i = 0; i < parts.size(); i++)
	{
		GLuint ModelID = glGetUniformLocation(program, "Model");
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Models[i][0][0]);

		vbo.bind();
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,				//location
			3,				//vec3
			GL_FLOAT,			//type
			GL_FALSE,			//not normalized
			0,				//strip
			(void*)offset[0]);//buffer offset
		//(location,vec3,type,固定點,連續點的偏移量,buffer point)
		offset[0] += parts[i].getVerticesSize() * sizeof(glm::vec3);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);//location 1 :vec2 UV
		uVbo.bind();
		glVertexAttribPointer(1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)offset[1]);
		//(location,vec2,type,固定點,連續點的偏移量,point)
		offset[1] += parts[i].getUvsSize() * sizeof(glm::vec2);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);//location 2 :vec3 Normal
		nVbo.bind();
		glVertexAttribPointer(2,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)offset[2]);
		//(location,vec3,type,固定點,連續點的偏移量,point)
		offset[2] += parts[i].getNormalsSize() * sizeof(glm::vec3);

		int vertexIDoffset = 0;//glVertexID's offset 
		std::string mtlname;//material name
		glm::vec3 Ks = glm::vec3(1, 1, 1);//because .mtl excluding specular , so give it here.

		std::vector<std::string>* mtls = &(parts[i].getMtls());
		for (int j = 0; j < mtls->size(); j++)
		{
			mtlname = (*mtls)[j];
			//find the material diffuse color in map:KDs by material name.
			GLuint M_KdID = glGetUniformLocation(program, "Material.Kd");
			GLuint M_KsID = glGetUniformLocation(program, "Material.Ks");
			glUniform3fv(M_KdID, 1, &KDs[mtlname][0]);
			glUniform3fv(M_KsID, 1, &Ks[0]);
			//          (primitive   , glVertexID base , vertex count    )
			glDrawArrays(GL_TRIANGLES, vertexIDoffset, parts[i].getFace(j + 1) * 3);
			//we draw triangles by giving the glVertexID base and vertex count is face count*3
			vertexIDoffset += parts[i].getFace(j + 1) * 3;//glVertexID's base offset is face count*3
		}//end for loop for draw one part of the robot	

	}//end for loop for updating and drawing model

	texture.bind(0);
	glUniform1i(texture.getId(), 0);
}
