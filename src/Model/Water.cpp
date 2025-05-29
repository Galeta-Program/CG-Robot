#include "../Model/Water.h"

Water::Water()
{
}

void Water::initialize(const glm::vec3& color, const char* filePath)
{
    float size = 100.0f;
    float y = 0.0f;
    points.push_back({ -size, y, -size });
    points.push_back({ -size, y,  size });
    points.push_back({  size, y, -size });
    points.push_back({  size, y, -size });
    points.push_back({ -size, y,  size });
    points.push_back({  size, y,  size });

    std::vector<glm::vec3> colors(points.size(), color);
    waterObj.initialize(points, colors);
    waterObj.gatherData();
    waterObj.computeNormal();

    // 高斯水波圖
    waterTexture.LoadTexture(filePath);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


    reflectionFBO.bind();

    glGenTextures(1, &reflectionTexture);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WATER_WIDTH, WATER_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);

    glGenTextures(1, &reflectionDepthBuffer);
    glBindTexture(GL_TEXTURE_2D, reflectionDepthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WATER_WIDTH, WATER_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, reflectionDepthBuffer, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

   
    refractionFBO.bind();

    glGenTextures(1, &refractionTexture);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WATER_WIDTH, WATER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);

    glGenTextures(1, &refractionDepthBuffer);
    glBindTexture(GL_TEXTURE_2D, refractionDepthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WATER_WIDTH, WATER_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionDepthBuffer, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Water::reflectSet()
{
    reflectionFBO.bind();
    glViewport(0, 0, WATER_WIDTH, WATER_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Water::refractSet()
{
    refractionFBO.bind();
    glViewport(0, 0, WATER_WIDTH, WATER_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Water::render(CG::Camera* camera)
{
    waterObj.getShaderProgram().use();

    glUniform1f(glGetUniformLocation(waterObj.getShaderProgram().getId(), "u_Time"), glfwGetTime());
    
    glActiveTexture(GL_TEXTURE0);
    waterTexture.bind();
    glUniform1i(glGetUniformLocation(waterObj.getShaderProgram().getId(), "u_WaterTexture"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glUniform1i(glGetUniformLocation(waterObj.getShaderProgram().getId(), "u_ReflectionTexture"), 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glUniform1i(glGetUniformLocation(waterObj.getShaderProgram().getId(), "u_RefractionTexture"), 2);

    waterObj.render(camera);
}

void Water::setReflectShader(const char* vert, const char* frag)
{
    ShaderInfo shaders[] =
    {
        { GL_VERTEX_SHADER,		vert },
        { GL_FRAGMENT_SHADER,	frag },
        { GL_NONE, NULL }
    };
    reflectProgram.load(shaders);
}