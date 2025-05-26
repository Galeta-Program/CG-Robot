#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

class FBO
{
private:
	GLuint id;

public:
	FBO();
	FBO(FBO&& other) noexcept;
	FBO(const FBO& other) = delete;

	~FBO();

	FBO& operator=(const FBO& other) = delete;
	FBO& operator=(FBO&& other) noexcept;


	void bind() const;
	void unbind() const;
};