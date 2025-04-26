#include "Model.h"

#include "../Utilty/Error.h"
#include "../src/Utilty/OBJLoader.hpp"
#include "Part.h"
#include "../Graphic/Material/MaterialManager.h"

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
	for ( int i = 0; i < objPaths.size(); i++ )
	{
		parts.emplace_back( objPaths[i].c_str(), mtlPaths[i].c_str());
	}
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

void Model::mapMtlNameToKds( std::vector<std::string>& materials, 
							 std::map<std::string, glm::vec3>& mapping, 
							 std::vector<glm::vec3>& content )
{
	for ( int i = 0; i < materials.size(); i++ )
	{
		std::string mtlname = materials[i];
		mapping[mtlname] = content[i];
	}
}

void Model::loadModel(const char* mtlPath, const char* objPath)
{
	parts.emplace_back(objPath, mtlPath);
}

void Model::gatherPartsData()
{
	vao.bind();
	vbo.bind();
	ebo.bind();
	instancingVbo.bind();

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

	instancingVbo.initialize(instancingCount * sizeof(glm::mat4), GL_DYNAMIC_DRAW);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * 4 * i));
		glVertexAttribDivisor(3 + i, 1);
	}
}

void Model::render( GLuint program, CG::Camera* camera )
{
	instancingVbo.bind();

	int n, p = 0, limit = 0, count = 0, u = 0;
	glm::mat4* matrices = (glm::mat4*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (n = 0; n < instancingCount; n++)
	{
		float x = p * 30;
		float y = 0;
		float z = -u * 25;
		matrices[n] = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
		p += 2;
			
		if (p > limit)
		{
			u += 1;
			count += u;
			p = -u;
			limit = u;
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	vao.bind();

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
		const std::vector<std::string>& mtls = currentPart.getMtlNames();
		MaterialManager& mtlManager = MaterialManager::getInstance();
		for ( int j = 0; j < mtls.size(); j++ )
		{
			mtlname = mtls[j];
			mtlManager.use(program, mtlname);
		}

		GLCall(glDrawElementsInstanced(GL_TRIANGLES, drawSize, GL_UNSIGNED_INT, (void*)offset, instancingCount));

		offset += drawSize * sizeof(unsigned int);
	}
}

void Model::modifyInstance(unsigned int count)
{
	instancingCount = count;
	instancingVbo.bind();
	instancingVbo.initialize(instancingCount * sizeof(glm::mat4), GL_DYNAMIC_DRAW);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * 4 * i));
		glVertexAttribDivisor(3 + i, 1);
	}
}

