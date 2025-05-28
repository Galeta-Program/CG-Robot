#pragma once

#include "../Scene/Camera.h"
#include "../Graphic/VAO.h"
#include "../Graphic/FBO.h"
#include "../Graphic/Material/Texture.h"
#include "../Scene/Camera.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"
#include "../Model/ManualObject.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <string>
#include <Vector>
#include <map>

class Water
{
private:
    ManualObject waterObj;

    FBO reflectionFBO;
    FBO refractionFBO;
    GLuint reflectionTexture;
    GLuint refractionTexture;
    GLuint reflectionDepthBuffer;
    GLuint refractionDepthBuffer;

	Texture waterTexture;

    GraphicShader reflectProgram;

    std::vector<glm::vec3> points;

    const unsigned int WATER_WIDTH = 4096, WATER_HEIGHT = 4096;
public:
    
    Water();
    ~Water() {}

    void initialize(const glm::vec3& color, const char* filePath);
    void reflectSet();
    void refractSet();

    void render(CG::Camera* camera);

    void setShader(const char* vert, const char* frag);
    void setReflectShader(const char* vert, const char* frag);
    ManualObject& getObject() { return waterObj; }
    inline const ShaderProgram& getReflectShaderProgram() { return reflectProgram; }

};

