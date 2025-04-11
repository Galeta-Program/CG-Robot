#include "VAO.h"

#include <GL/glew.h>


VAO::VAO()
{
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
}

VAO::VAO(VAO&& other) noexcept : id(other.id)
{
	other.id = 0;
}

VAO::~VAO()
{
	glDeleteVertexArrays(1, &id);
}

VAO& VAO::operator=(VAO&& other) noexcept
{
	if (this != &other)
	{
		if (id != 0)
		{
			glDeleteVertexArrays(1, &id);
		}

		id = other.id;
		other.id = 0;
	}

	return *this;
}

void VAO::bind() const
{
	glBindVertexArray(id);
}

void VAO::unbind() const
{
	glBindVertexArray(0);
}
