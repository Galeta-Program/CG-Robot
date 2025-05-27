#include "SkyBox.h"


bool SkyBox::loadFaces(std::vector<std::string> faces)
{
	if (faces.size() == 6)
	{
		Texture* texture = new Texture();
		texture->LoadCubeMap(faces);
		textures.push_back(texture);
		currentTextureIndex = textures.size() - 1;
		return true;
	}
	return false;
}

void SkyBox::updateDate()
{
	float size = 100;
	std::vector<glm::vec3> points({
			glm::vec3(-size, size, size),
			glm::vec3(size, size, size),
			glm::vec3(size, -size, size),
			glm::vec3(-size, -size, size),

			glm::vec3(-size, size, -size),
			glm::vec3(-size, size, size),
			glm::vec3(-size, -size, size),
			glm::vec3(-size, -size, -size),	

			glm::vec3(size, size, size),
			glm::vec3(size, size, -size),
			glm::vec3(size, -size, -size),		
			glm::vec3(size, -size, size),

			glm::vec3(size, size, -size),
			glm::vec3(-size, size, -size),
			glm::vec3(-size, -size, -size),			
			glm::vec3(size, -size, -size),

			glm::vec3(-size, size, -size),
			glm::vec3(size, size, -size),		
			glm::vec3(size, size, size),
			glm::vec3(-size, size, size),

			glm::vec3(size, -size, size),
			glm::vec3(size, -size, -size),
			glm::vec3(-size, -size, -size),	
			glm::vec3(-size, -size, size),
			});
	std::vector<glm::vec3> dummyColors(points.size(), glm::vec3(1.0f));
	std::vector<glm::vec3> colors;
		for (int i = 0; i < points.size(); i++)
		{
			double v1 = rand() % 100;
			double v2 = rand() % 100;
			double v3 = rand() % 100;
			v1 = v1 / 100;
			v2 = v2 / 100;
			v3 = v3 / 100;
			colors.push_back(glm::vec3(v1, v2, v3));
		}

	skyBoxObj.initialize(points, colors);
	skyBoxObj.gatherData();
}

void SkyBox::switchTexture(unsigned int index)
{
	if (index < textures.size())
	{
		currentTextureIndex = index;
	}
}	

void SkyBox::render(CG::Camera* camera, GLint type)
{
	skyBoxObj.getShaderProgram().use();
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);     

	textures[currentTextureIndex]->bind(0, GL_TEXTURE_CUBE_MAP);
	GLuint TextureID = glGetUniformLocation(skyBoxObj.getShaderProgram().getId(), "skybox");
	GLCall(glUniform1i(TextureID, 0));
	skyBoxObj.render(camera, nullptr, type);

	glDepthFunc(GL_LESS);             
	glDepthMask(GL_TRUE);
}