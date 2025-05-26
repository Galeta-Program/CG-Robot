#include "FBO.h"
#include "../Utilty/Error.h"

#include <GL/glew.h>


FBO::FBO()
{
	GLCall(glGenFramebuffers(1, &id));
	//GLCall(glBindFramebuffer(GL_FRAMEBUFFER, id));
}

FBO::FBO(FBO&& other) noexcept : id(other.id)
{
	other.id = 0;
}

FBO::~FBO()
{
	GLCall(glDeleteFramebuffers(1, &id));
}

FBO& FBO::operator=(FBO&& other) noexcept
{
	if (this != &other)
	{
		if (id != 0)
		{
			GLCall(glDeleteFramebuffers(1, &id));
		}

		id = other.id;
		other.id = 0;
	}

	return *this;
}

void FBO::bind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, id));
}

void FBO::copy(const FBO& other, int width, int height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, other.id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::unbind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
