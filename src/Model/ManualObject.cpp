#include "ManualObject.h"


ManualObject::ManualObject()
{
	if (instancingCount > instancingOffests.size())
	{
		int diff = instancingCount - instancingOffests.size();
		for (int i = 0; i < diff; i++)
			instancingOffests.push_back({ 0.0, 0.0, 0.0 });
	}
	else if (instancingCount < instancingOffests.size())
	{
		instancingCount = instancingOffests.size();
	}
}

void ManualObject::initialize(std::vector<glm::vec3>& objPoints, std::vector<glm::vec3>& objColors)
{
	points = objPoints;
	colors = objColors;
	pVbo.initialize(points, GL_DYNAMIC_DRAW);
	cVbo.initialize(colors, GL_DYNAMIC_DRAW);
}

void ManualObject::gatherData()
{
	vao.bind();

	pVbo.bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	cVbo.bind();
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	nVbo.bind();
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);


	instancingVbo.bind();
	instancingVbo.initialize(instancingCount * sizeof(glm::mat4), GL_DYNAMIC_DRAW);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * 4 * i));
		glVertexAttribDivisor(3 + i, 1);
	}
	vao.unbind();
}

void ManualObject::render(CG::Camera* camera, const ShaderProgram* inProgram, GLint type)
{
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
	updateModelMatrix();

	GLCall(GLuint ModelID = glGetUniformLocation(inProgram->getId(), "u_Model"));
	GLCall(glUniformMatrix4fv(ModelID, 1, GL_FALSE, &modelMatrix[0][0]));
	GLCall(glDrawArraysInstanced(type, 0, points.size(), instancingCount));
	vao.unbind();
}

void ManualObject::setPoints(std::vector<glm::vec3>& objPoints)
{
	points = objPoints;
	pVbo.initialize(points);
}

void ManualObject::setColors(std::vector<glm::vec3>& objColors)
{
	colors = objColors;
	cVbo.initialize(colors);
}

void ManualObject::setNormals(std::vector<glm::vec3>& objNormals)
{
	normals = objNormals;
	nVbo.initialize(normals);
}


void ManualObject::computeNormal(GLint type)
{
	normals.clear();
	normals = std::vector<glm::vec3>(points.size(), glm::vec3(0.0f));
	if (type == GL_TRIANGLES)
	{
		for (int i = 0; i + 3 <= points.size(); i += 3)
		{
			glm::vec3 AB = points[i + 1] - points[i];
			glm::vec3 AC = points[i + 2] - points[i];
			normals[i] = normals[i + 1] = normals[i + 2] = glm::normalize(glm::cross(AB, AC));
		}
	}
	else if (type == GL_QUADS)
	{
		for (int i = 0; i + 4 <= points.size(); i += 4)
		{
			glm::vec3 AB = points[i + 1] - points[i];
			glm::vec3 AC = points[i + 2] - points[i];
			glm::vec3 normal1 = glm::normalize(glm::cross(AB, AC));

			glm::vec3 CD = points[i + 3] - points[i + 2];
			glm::vec3 CA = points[i] - points[i + 2];
			glm::vec3 normal2 = glm::normalize(glm::cross(CD, CA));

			glm::vec3 avgNormal = glm::normalize(normal1 + normal2);
			normals[i] = normals[i + 1] = normals[i + 2] = normals[i + 3] = avgNormal;
		}
	}

	nVbo.initialize(normals);
}

// sectorCount竒絬计秖XZキちだ蛾㏄计
// stackCount 絥絬计秖パ伐玭伐蔼糷计
void ManualObject::generateSphere(float radius, int sectorCount, int stackCount, glm::vec3 color)
{
	points.clear();
	normals.clear();
	colors.clear();

	std::vector<glm::vec3> tmpPoints;
	std::vector<glm::vec3> tmpColors;
	std::vector<glm::vec3> tmpNormals;

	for (int i = 0; i <= stackCount; ++i)
	{
		float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount;
		float xy = radius * cos(stackAngle);
		float z = radius * sin(stackAngle);

		for (int j = 0; j <= sectorCount; ++j)
		{
			float sectorAngle = j * 2 * glm::pi<float>() / sectorCount;

			float x = xy * cos(sectorAngle); // x = r * cos(phi) * cos(theta)
			float y = xy * sin(sectorAngle); // y = r * cos(phi) * sin(theta)

			glm::vec3 pos(x, y, z);
			tmpPoints.push_back(pos);
			tmpNormals.push_back(glm::normalize(pos));
			tmpColors.push_back(color);
		}
	}

	// 锣传à
	for (int i = 0; i < stackCount; ++i)
	{
		for (int j = 0; j < sectorCount; ++j)
		{
			int cur = i * (sectorCount + 1) + j;
			int next = cur + sectorCount + 1;

			points.push_back(tmpPoints[cur]);
			points.push_back(tmpPoints[next]);
			points.push_back(tmpPoints[cur + 1]);

			normals.push_back(tmpNormals[cur]);
			normals.push_back(tmpNormals[next]);
			normals.push_back(tmpNormals[cur + 1]);

			colors.push_back(tmpColors[cur]);
			colors.push_back(tmpColors[next]);
			colors.push_back(tmpColors[cur + 1]);

			points.push_back(tmpPoints[cur + 1]);
			points.push_back(tmpPoints[next]);
			points.push_back(tmpPoints[next + 1]);

			normals.push_back(tmpNormals[cur + 1]);
			normals.push_back(tmpNormals[next]);
			normals.push_back(tmpNormals[next + 1]);

			colors.push_back(tmpColors[cur + 1]);
			colors.push_back(tmpColors[next]);
			colors.push_back(tmpColors[next + 1]);
		}
	}

	pVbo.initialize(points, GL_DYNAMIC_DRAW);
	nVbo.initialize(normals);
	cVbo.initialize(colors, GL_DYNAMIC_DRAW);
}