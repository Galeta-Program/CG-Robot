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

/*
void MainScene::UpdateModel()
{
	glm::mat4 Rotatation[PARTSNUM];
	glm::mat4 Translation[PARTSNUM];
	for (int i = 0; i < PARTSNUM; i++)
	{
		Models[i] = glm::mat4(1.0f);
		Rotatation[i] = glm::mat4(1.0f);
		Translation[i] = glm::mat4(1.0f);
	}
	float r, pitch, yaw, roll;
	float alpha, beta, gamma;

	//身體=======================================================
	//== 上身
	beta = angle;
	Rotatation[0] = rotate(beta, 0, 1, 0);
	Translation[0] = translate(0, 15.8 + position, 0);
	Models[0] = Translation[0] * Rotatation[0];

	//== 下身
	Translation[8] = translate(0, -5.3, 0);
	Models[8] = Models[0] * Translation[8] * Rotatation[8];
	//=============================================================

	//頭==========================================================
	Translation[4] = translate(0, 3.2, -0.5);
	Models[4] = Models[0] * Translation[4] * Rotatation[4];
	//============================================================

	//左手=======================================================
	//== 左上手臂
	yaw = glm::radians(beta); r = 3.7;
	alpha = angles[1];
	gamma = 10;
	Rotatation[1] = rotate(alpha, 1, 0, 0) * rotate(gamma, 0, 0, 1);//向前旋轉*向右旋轉
	Translation[1] = translate(7.3, 0, 0);

	Models[1] = Models[0] * Translation[1] * Rotatation[1];

	//== 左下手臂
	pitch = glm::radians(alpha); r = 3;
	roll = glm::radians(gamma);
	static int i = 0;
	i += 5;
	alpha = angles[2] - 0;
	//上手臂+下手臂向前旋轉*向右旋轉
	Rotatation[2] = rotate(alpha, 1, 0, 0);
	//延x軸位移以上手臂為半徑的圓周長:translate(0,r*cos,r*sin)
	//延z軸位移以上手臂為半徑角度:translate(r*sin,-rcos,0)
	Translation[2] = translate(3, -6.5, 0);

	Models[2] = Models[1] * Translation[2] * Rotatation[2];

	//== 左手掌
	pitch = glm::radians(alpha);
	//b = glm::radians(angles[2]);
	roll = glm::radians(gamma);
	//手掌角度與下手臂相同
	//Rotatation[3] = Rotatation[2];
	//延x軸位移以上手臂為半徑的圓周長:translate(0,r*cos,r*sin) ,角度為上手臂+下手臂
	Translation[3] = translate(3.7, -8.6, 4);
	Models[3] = Models[2] * Translation[3] * Rotatation[3];
	//============================================================

	//右手=========================================================
	//== 右上手臂
	gamma = -10; alpha = angles[5] = -angles[1];
	Rotatation[5] = rotate(alpha, 1, 0, 0) * rotate(gamma, 0, 0, 1);
	Translation[5] = translate(-7.3, 0, 0);
	Models[5] = Models[0] * Translation[5] * Rotatation[5];

	//== 右下手臂
	angles[6] = angles[2];
	pitch = glm::radians(alpha); r = -3;
	roll = glm::radians(gamma);
	alpha = angles[6] - 0;
	Rotatation[6] = rotate(alpha, 1, 0, 0);
	Translation[6] = translate(-3, -6.5, 0);
	Models[6] = Models[5] * Translation[6] * Rotatation[6];

	//== 右手掌
	pitch = glm::radians(alpha);
	//b = glm::radians(angles[7]);
	roll = glm::radians(gamma);
	Translation[7] = translate(-3.7, -8.6, 4);
	Models[7] = Models[6] * Translation[7] * Rotatation[7];
	//=============================================================

	//左腳=========================================================
	//== 左大腿
	alpha = angles[9]; gamma = 0;
	Rotatation[9] = rotate(alpha, 1, 0, 0) * rotate(gamma, 0, 0, 1);
	Translation[9] = translate(3.3, -7, 2.5);
	Models[9] = Models[8] * Translation[9] * Rotatation[9];

	//== 左小腿
	pitch = glm::radians(alpha);
	roll = glm::radians(gamma);
	alpha = angles[10];
	Translation[10] = translate(3, -9.5, 1.2);
	Rotatation[10] = rotate(alpha, 1, 0, 0);
	Models[10] = Models[9] * Translation[10] * Rotatation[10];

	//== 左腳
	alpha = angles[11];
	Translation[11] = translate(1.5, -17, -1.5);
	Rotatation[11] = rotate(alpha, 1, 0, 0);
	Models[11] = Models[10] * Translation[11] * Rotatation[11];
	//=============================================================

	//右腳=========================================================
	//== 右大腿
	alpha = angles[12] = -angles[9];
	gamma = -0;
	Rotatation[12] = rotate(alpha, 1, 0, 0) * rotate(gamma, 0, 0, 1);
	Translation[12] = translate(-3.3, -7, 2.5);
	Models[12] = Models[8] * Translation[12] * Rotatation[12];

	//== 右小腿
	pitch = glm::radians(alpha);
	roll = glm::radians(gamma);
	alpha = angles[13] = angles[10];
	Translation[13] = translate(-3, -9.5, 1.2);
	Rotatation[13] = rotate(alpha, 1, 0, 0);
	Models[13] = Models[12] * Translation[13] * Rotatation[13];

	//== 右腳
	alpha = angles[14];
	Translation[14] = translate(-1.5, -17, -1.5);
	Rotatation[14] = rotate(alpha, 1, 0, 0);
	Models[14] = Models[13] * Translation[14] * Rotatation[14];
	//=============================================================
}

*/

void Model::render(GLuint program)
{
	vao.bind();

	GLuint offset[3] = { 0,0,0 };//offset for vertices , uvs , normals
	for (int i = 0; i < parts.size(); i++)
	{
		Part& currentPart = parts[i].getPart();

		GLCall(GLuint ModelID = glGetUniformLocation(program, "Model"));

		glm::mat4 modelMat = parts[i].getModelMatrix();
		glm::mat4 parentModelMat = parts[i].getParentModelMatrix();
		glm::mat4 overallModelMat = parentModelMat * modelMat;

		GLCall(glUniformMatrix4fv(ModelID, 1, GL_FALSE, &overallModelMat[0][0]));

		vbo.bind();
		// 1rst attribute buffer : vertices
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0,				//location
			3,				//vec3
			GL_FLOAT,			//type
			GL_FALSE,			//not normalized
			0,				//strip
			(void*)offset[0]));//buffer offset
		//(location,vec3,type,固定點,連續點的偏移量,buffer point)
		offset[0] += currentPart.getVerticesSize() * sizeof(glm::vec3);

		// 2nd attribute buffer : UVs
		GLCall(glEnableVertexAttribArray(1));//location 1 :vec2 UV
		uVbo.bind();
		GLCall(glVertexAttribPointer(1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)offset[1]));
		//(location,vec2,type,固定點,連續點的偏移量,point)
		offset[1] += currentPart.getUvsSize() * sizeof(glm::vec2);

		// 3rd attribute buffer : normals
		GLCall(glEnableVertexAttribArray(2));//location 2 :vec3 Normal
		nVbo.bind();
		GLCall(glVertexAttribPointer(2,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)offset[2]));
		//(location,vec3,type,固定點,連續點的偏移量,point)
		offset[2] += currentPart.getNormalsSize() * sizeof(glm::vec3);

		int vertexIDoffset = 0;//glVertexID's offset 
		std::string mtlname;//material name
		glm::vec3 Ks = glm::vec3(1, 1, 1);//because .mtl excluding specular , so give it here.

		std::vector<std::string>& mtls = currentPart.getMtls();

		for (int j = 0; j < mtls.size(); j++)
		{
			mtlname = mtls[j];
			//find the material diffuse color in map:KDs by material name.
			GLCall(GLuint M_KdID = glGetUniformLocation(program, "Material.Kd"));
			GLCall(GLuint M_KsID = glGetUniformLocation(program, "Material.Ks"));
			GLCall(glUniform3fv(M_KdID, 1, &KDs[mtlname][0]));
			GLCall(glUniform3fv(M_KsID, 1, &Ks[0]));
			//          (primitive   , glVertexID base , vertex count    )
			GLCall(glDrawArrays(GL_TRIANGLES, vertexIDoffset, currentPart.getFace(j + 1) * 3));
			//we draw triangles by giving the glVertexID base and vertex count is face count*3
			vertexIDoffset += currentPart.getFace(j + 1) * 3;//glVertexID's base offset is face count*3
		}//end for loop for draw one part of the robot	

	}//end for loop for updating and drawing model

	texture.bind(0);
	GLuint TextureID = glGetUniformLocation(program, "Texture");
	GLCall(glUniform1i(TextureID, 0));
}

void Model::stand()
{
	parts[TOP_BODY].setTranslate(glm::vec3(0.000000, 0.000000, 0.000000));		// top_body
	parts[LEFT_UPPER_ARM].setTranslate(glm::vec3(7.650000, 0.000000, 0.000000));		// left_upper_arm
	parts[LEFT_LOWER_ARM].setTranslate(glm::vec3(2.900000, -6.400000, 0.000000));	// left_lower_arm
	parts[LEFT_HAND].setTranslate(glm::vec3(3.650000, -8.500000, 3.900000));	// left_hand
	parts[HEAD].setTranslate(glm::vec3(0.000000, 3.100000, 0.000000));		// head
	parts[RIGHT_UPPER_ARM].setTranslate(glm::vec3(-7.650000, 0.050000, 0.000000));	// right_upper_arm
	parts[RIGHT_LOWER_ARM].setTranslate(glm::vec3(-2.950000, -6.400000, 0.000000));	// right_lower_arm
	parts[RIGHT_HAND].setTranslate(glm::vec3(-3.650000, -8.500000, 3.900000));	// right_hand	
	parts[BOTTOM_BODY].setTranslate(glm::vec3(0.000000, -3.550000, 0.000000));	// bottom_body
	parts[LEFT_THIGH].setTranslate(glm::vec3(3.250000, -6.650000, 2.200000));	// left_thigh
	parts[LEFT_CALF].setTranslate(glm::vec3(3.200000, -13.100000, 1.150000));	// left_calf
	parts[LEFT_FOOT].setTranslate(glm::vec3(1.350000, -16.799999, -1.150000));	// left_foot
	parts[RIGHT_THIGH].setTranslate(glm::vec3(-3.250000, -6.650000, 2.200000));	// right_thigh
	parts[RIGHT_CALF].setTranslate(glm::vec3(-3.350000, -13.100000, 1.150000));	// right_calf
	parts[RIGHT_FOOT].setTranslate(glm::vec3(-1.350000, -16.799999, -1.150000));// right_foot
}