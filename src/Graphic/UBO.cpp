#include "UBO.h"

UBO::UBO(unsigned int _size)
{
	initialize(_size);
}

UBO::UBO(UBO&& other) noexcept:
	id(other.id),
	size(other.size)
{
	other.id = 0;
}

UBO::~UBO()
{
	glDeleteBuffers(1, &id);
}

UBO& UBO::operator=(UBO&& other) noexcept
{
	if (this != &other)
	{
		if (id != 0)
		{
			glDeleteBuffers(1, &id);
		}
		size = other.size;
		id = other.id;
		other.id = 0;
	}
	return *this;
}

void UBO::associateWithShaderBlock(unsigned int program, const char* uniformBlockName, unsigned int bindingPoint)
{
	int UBOsize = 0;
	int idx = glGetUniformBlockIndex(program, uniformBlockName);

	glGetActiveUniformBlockiv(program, idx, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);
	//bind UBO to its idx
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, id, 0, UBOsize);
	glUniformBlockBinding(program, idx, bindingPoint);
}

void UBO::fillInData(GLintptr offset, GLintptr size, const void* data)
{
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}


void UBO::initialize(unsigned int _size)
{
	glGenBuffers(1, &id);
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferData(GL_UNIFORM_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	size = _size;
}

void UBO::bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
}

void UBO::unbind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
