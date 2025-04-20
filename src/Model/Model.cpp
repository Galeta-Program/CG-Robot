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

void Model::initialize( std::vector<std::string>& mtlPaths, std::vector<std::string>& objPaths )
{
	std::vector<glm::vec3> Kds;
	std::vector<glm::vec3> Kas;
	std::vector<glm::vec3> Kss;
	std::vector<std::string> Materials; //mtl-name
	std::string textureName;

	for ( std::string path: mtlPaths )
	{
		LoadMTL( path.c_str(), Kds, Kas, Kss, Materials, textureName );
	}
	mapMtlNameToKds( Materials, Kds );

	for ( int i = 0; i < objPaths.size(); i++ )
	{
		parts.emplace_back( objPaths[i].c_str() );
	}

	// maybe add another method to handle this
	if (!textureName.empty())
		texture.LoadTexture( "../res/models2/" + textureName );
	else
		texture.LoadTexture( "../res/models2/Robot_Base_color 7.png" );
}

void Model::setPartsRelationship( std::vector<std::vector<unsigned int>> relationships )
{
	for ( auto relationship : relationships )
	{
		std::vector<Node*> children;
		// first one is parent, and the others are its children
		for ( int i = 1; i < relationship.size(); i++ )
		{
			children.emplace_back( &parts[relationship[i]] );
		}

		parts[relationship[0]].addChildren( children );		
	}
}

void Model::mapMtlNameToKds( std::vector<std::string>& materials, std::vector<glm::vec3>& Kds )
{
	for ( int i = 0; i < materials.size(); i++ )
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

	parts.emplace_back( objPath );
	LoadMTL( mtlPaths, Kds, Kas, Kss, Materials, textureName );
	mapMtlNameToKds( Materials, Kds );

	// maybe add another method to handle this
	if (!textureName.empty())
		texture.LoadTexture( "../res/models2/" + textureName );
	else
		texture.LoadTexture( "../res/models2/Robot_Base_color 7.png" );
}

void Model::gatherPartsData()
{
	vao.bind();
	vbo.bind();
	ebo.bind();

	GLuint totalVertexSize = 0;
	GLuint vertexSizeOffset = 0;
	GLuint totalElementSize = 0;
	GLuint elementSizeOffset = 0;

	for (int i = 0; i < parts.size(); i++)
	{
		Part& currentPart = parts[i].getPart();
		totalVertexSize += currentPart.getVertexSize();
		totalElementSize += currentPart.getElementSize();
	}

	vbo.initialize(totalVertexSize * sizeof(Vertex));
	ebo.initialize(totalElementSize * sizeof(unsigned int));

	GLuint elementLoaded = 0;
	for ( int i = 0; i < parts.size(); i++ )
	{
		Part& currentPart = parts[i].getPart();
		unsigned int currentVertexSize = currentPart.getVertexSize();
		unsigned int currentElementSize = currentPart.getElementSize();

		VBO<void>::copyAndWrite( currentPart.vboId(), vbo.getId(), 0, vertexSizeOffset, currentVertexSize * sizeof(Vertex) );
		vertexSizeOffset += currentVertexSize * sizeof(Vertex);

		if (i == 0) 
		{
			EBO::copyAndWrite(currentPart.eboId(), ebo.getId(), 0, elementSizeOffset, currentElementSize * sizeof(unsigned int));
		}
		else
		{
			std::vector<unsigned int> indices(currentElementSize);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentPart.eboId());
			glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, currentElementSize * sizeof(unsigned int), indices.data());
			for ( int j = 0; j < currentElementSize; j++ )
			{
				indices[j] += elementLoaded;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.getId());
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, elementSizeOffset, currentElementSize * sizeof(unsigned int), indices.data());
		}
		elementSizeOffset += currentElementSize * sizeof(unsigned int);
		elementLoaded += currentVertexSize;
	}

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))));
}

void Model::render( GLuint program, CG::Camera* camera )
{
	vao.bind();
	
	texture.bind( 0 );
	GLuint TextureID = glGetUniformLocation( program, "u_Texture" );
	GLCall( glUniform1i( TextureID, 0 ) );

	GLuint offset = 0;
	for ( int i = 0; i < parts.size(); i++ )
	{
		Part& currentPart = parts[i].getPart();

		GLCall( GLuint ModelID = glGetUniformLocation( program, "u_Model" ) );
		glm::mat4 modelMat = parts[i].getModelMatrix();
		glm::mat4 parentModelMat = parts[i].getParentModelMatrix();
		glm::mat4 overallModelMat = parentModelMat * modelMat;
		GLCall( glUniformMatrix4fv( ModelID, 1, GL_FALSE, &overallModelMat[0][0] ) );

		GLCall( GLuint NormalMatID = glGetUniformLocation( program, "u_NormalMatrix" ) );
		glm::mat3 normalMatrix = glm::transpose( glm::inverse( glm::mat3( *(camera->GetViewMatrix()) * modelMat ) ) );
		GLCall( glUniformMatrix3fv( NormalMatID, 1, GL_FALSE, &normalMatrix[0][0] ) );

		GLuint drawSize = currentPart.getElementSize();

		std::string mtlname;
		glm::vec3 Ks = glm::vec3( 1, 1, 1 );//because .mtl excluding specular , so give it here.

		const std::vector<std::string>& mtls = currentPart.getMtlNames();

		for ( int j = 0; j < mtls.size(); j++ )
		{
			mtlname = mtls[j];
			//find the material diffuse color in map:KDs by material name.
			GLCall( GLuint M_KdID = glGetUniformLocation( program, "u_Material.Kd" ) );
			GLCall( GLuint M_KsID = glGetUniformLocation( program, "u_Material.Ks" ) );
			GLCall( glUniform3fv( M_KdID, 1, &KDs[mtlname][0] ) );
			GLCall( glUniform3fv( M_KsID, 1, &Ks[0] ) );
		}
		GLCall(glDrawElements(GL_TRIANGLES, drawSize, GL_UNSIGNED_INT, (void*)offset));

		offset += drawSize * sizeof(unsigned int);
	}//end for loop for updating and drawing model
}

