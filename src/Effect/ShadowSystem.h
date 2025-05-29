#pragma once 
#include "Emitter.h"
#include "../Graphic/ShaderProgram/ComputeShader.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"
#include "../Graphic/FBO.h"
#include "../Graphic/UBO.h"
#include "../Graphic/Material/Texture.h"

#include <map>

class ShadowSystem
{
private:
	FBO shadowFBO;
	GLuint shadowMap;
	GraphicShader shadowShader;

	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
public:
	ShadowSystem();
	ShadowSystem(ShadowSystem&& other) noexcept;

	glm::mat4 set(glm::vec3 lightPos);
	void setShader(const char* vert, const char* frag);
	inline GLuint getShadowMap() { return shadowMap; }
	inline const ShaderProgram& getShaderProgram() { return shadowShader; }
};