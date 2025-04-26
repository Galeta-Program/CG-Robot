#pragma once

#include <string>
#include <GL/glew.h>

struct ShaderInfo
{
	GLenum type;
	const char* filename;
};

class ShaderProgram
{
private:
	unsigned int program;

	const GLchar* ReadShader(const char* filename);

public:
	ShaderProgram() : program(0) {}
	~ShaderProgram();

	GLuint load(ShaderInfo* shaders);

	void use() const;
	void unUse() const;

	inline unsigned int getId() const { return program; }
};