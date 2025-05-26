#include "ShadowSystem.h"
#include "../Graphic/ShaderProgram/ComputeShader.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"
#include "../Graphic/FBO.h"
#include "../Graphic/UBO.h"
#include "../Graphic/Material/Texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ShadowSystem::ShadowSystem()
{
	shadowFBO.bind();
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 遠方陰影
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


	// 掛到framebuffer的depth attachment
	shadowFBO.bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

	//	只需要深度資訊
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	shadowFBO.unbind();
}


glm::mat4 ShadowSystem::set(glm::vec3 lightPos)
{
	float near_plane = 10.0f, far_plane = 5000.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	shadowShader.use();
	glUniformMatrix4fv(glGetUniformLocation(shadowShader.getId(), "u_LightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	shadowFBO.bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	
	return lightSpaceMatrix;
}


void ShadowSystem::setShader(const char* vert, const char* frag)
{
	ShaderInfo shaders[] =
	{
		{ GL_VERTEX_SHADER,		vert },
		{ GL_FRAGMENT_SHADER,	frag },
		{ GL_NONE, NULL }
	};
	shadowShader.load(shaders);
}