#pragma once

#include "../Scene/SceneGraph/Node.h"
#include "../Scene/Camera.h"
#include "../Scene/SceneGraph/Node.h"
#include "../Graphic/VAO.h"
#include "../Graphic/FBO.h"
#include "../Graphic/Material/Texture.h"
#include "../Scene/Camera.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"

#include <string>
#include <Vector>
#include <map>

class ScreenRenderer
{
private:
	VAO vao;
	VBO<float> vbo;

	int width, height;
	FBO fbo;
	GLuint texColorBuffer;
	GLuint rbo;

	GLuint depthTexture;

	GraphicShader program;

	bool usePixelate = true;
	bool useToonshader = true;
	bool useMotionBlur = true;

	float pixelSize;

	static const int HISTORY_COUNT = 20;
	GLuint historyTextures[HISTORY_COUNT];
	FBO historyFBOs[HISTORY_COUNT];
	int currentHistoryIndex = 0;

	GLuint getTexture() const { return texColorBuffer; }

public:
	ScreenRenderer() {}
	~ScreenRenderer() {}

	void initialize(int newWidth, int newHeight);
	void resize(int newWidth, int newHeight);
	void set();
	void render();
	void setShader(const char* vert, const char* frag);
	void setPixelate(bool isUsed);
	void setToonshader(bool isUsed);
	void setMotionBlur(bool isUsed);
	void setPixelSize(float size);
	float getPixelSize() { return pixelSize; }
};

