#include "../Scene/ScreenRenderer.h"


void ScreenRenderer::initialize(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;
    fbo.bind();

    // color texture
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    // depth texture
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // ¸j¨ì§Aªº framebuffer
    fbo.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    fbo.unbind();

    for (int i = 0; i < HISTORY_COUNT; ++i) {
        glGenTextures(1, &historyTextures[i]);
        glBindTexture(GL_TEXTURE_2D, historyTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        historyFBOs[i].bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, historyTextures[i], 0);
        historyFBOs[i].unbind();
    }


    std::vector<float> quadVertices = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    vao.bind();
    vbo.initialize(quadVertices);
    vbo.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}


void ScreenRenderer::resize(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;

    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, newWidth, newHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
}



void ScreenRenderer::set()
{
    fbo.bind();
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FBO is incomplete! Status: " << std::hex << status << std::endl;
    }

}

void ScreenRenderer::render()
{
    program.use();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    vao.bind();

    glUniform1f(glGetUniformLocation(program.getId(), "u_PixelSize"), pixelSize);
    glUniform2f(glGetUniformLocation(program.getId(), "u_ScreenSize"), width, height);
    if (useMotionBlur == true)
        glUniform1f(glGetUniformLocation(program.getId(), "u_Alpha"), 0.8f);
    else
        glUniform1f(glGetUniformLocation(program.getId(), "u_Alpha"), 1.0f);
    glUniform1i(glGetUniformLocation(program.getId(), "u_usePixelate"), usePixelate);
    glUniform1i(glGetUniformLocation(program.getId(), "u_useToonshader"), useToonshader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glUniform1i(glGetUniformLocation(program.getId(), "u_ScreenTexture"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(glGetUniformLocation(program.getId(), "u_DepthTexture"), 1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (useMotionBlur == true)
    {
        for (int i = 0; i < HISTORY_COUNT; ++i)
        {
            int texIndex = (currentHistoryIndex + i) % HISTORY_COUNT;
            float alpha = 1.0f / sqrt(float(i) + 1);
            //alpha = 0.8f - float(i) / HISTORY_COUNT;
            //alpha = pow(0.7, float(HISTORY_COUNT - i));
            

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, historyTextures[texIndex]);
            glUniform1i(glGetUniformLocation(program.getId(), "u_ScreenTexture"), 0);
            glUniform1f(glGetUniformLocation(program.getId(), "u_PixelSize"), pixelSize);
            glUniform2f(glGetUniformLocation(program.getId(), "u_ScreenSize"), width, height);
            glUniform1f(glGetUniformLocation(program.getId(), "u_Alpha"), alpha);
            glUniform1i(glGetUniformLocation(program.getId(), "u_usePixelate"), usePixelate);
            glUniform1i(glGetUniformLocation(program.getId(), "u_useToonshader"), useToonshader);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }       

        historyFBOs[currentHistoryIndex].copy(fbo, width, height);
        currentHistoryIndex = (currentHistoryIndex + 1) % HISTORY_COUNT;
    }
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    vao.unbind();
    program.unUse();
}

void ScreenRenderer::setShader(const char* vert, const char* frag)
{
    ShaderInfo shaders[] =
    {
        { GL_VERTEX_SHADER,		vert },
        { GL_FRAGMENT_SHADER,	frag },
        { GL_NONE, NULL }
    };
    program.load(shaders);
}


void ScreenRenderer::setPixelate(bool isUsed) 
{ 
    usePixelate = isUsed; 
}

void ScreenRenderer::setToonshader(bool isUsed) 
{ 
    useToonshader = isUsed; 
}

void ScreenRenderer::setMotionBlur(bool isUsed) 
{ 
    useMotionBlur = isUsed; 
}

void ScreenRenderer::setPixelSize(float size)
{
    if (size > 0)
        pixelSize = size;
}