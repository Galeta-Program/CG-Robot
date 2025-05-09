#pragma once

#include "../Utilty/Error.h"
#include <vector>
#include <GL/glew.h>
#include <iostream>

template<class T>
class SSBO
{
private:
	GLuint id;
	unsigned int size;

public:
	SSBO() : id(0), size(0) {}
	SSBO(const std::vector<T>& v);

	SSBO(SSBO&& other) noexcept;
	SSBO(const SSBO& other) = delete;

	~SSBO();

	SSBO& operator=(const SSBO& other) = delete;
	SSBO& operator=(SSBO&& other) noexcept;

	void initialize(unsigned int _size, GLuint usageMode = GL_STATIC_DRAW);
	void initialize(const std::vector<T>& v, GLuint usageMode = GL_STATIC_DRAW);

	void bind() const;
	void unbind() const;
	void invalid() const;

	static void copyAndWrite(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, unsigned int size);

	inline unsigned int getSize() const { return size; }
	inline unsigned int getId() const { return id; }
};

template< typename T >
void SSBO<T>::copyAndWrite(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, unsigned int size)
{
	GLCall(glBindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer));
	GLCall(glBindBuffer(GL_COPY_READ_BUFFER, readBuffer));
	GLCall(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size));

	GLCall(glBindBuffer(GL_COPY_WRITE_BUFFER, 0));
	GLCall(glBindBuffer(GL_COPY_READ_BUFFER, 0));
}

template<class T>
SSBO<T>::SSBO(const std::vector<T>& v)
{
	initialize(v);
}

template<class T>
SSBO<T>::SSBO(SSBO<T>&& other) noexcept : id(other.id), size(other.size)
{
	other.id = 0;
}


template<class T>
SSBO<T>::~SSBO()
{
	if (id != 0)
	{
		GLCall(glDeleteBuffers(1, &id));
	}
}

template<class T>
SSBO<T>& SSBO<T>::operator=(SSBO<T>&& other) noexcept
{
	if (this != &other)
	{
		if (id != 0)
		{
			GLCall(glDeleteBuffers(1, &id));
		}

		id = other.id;
		other.id = 0;
	}

	return *this;
}

template<class T>
void SSBO<T>::initialize(unsigned int _size, GLuint usageMode)
{
	GLCall(glGenBuffers(1, &id));
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, id));
	GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, _size, NULL, usageMode));
	size = _size;
}

template<class T>
void SSBO<T>::initialize(const std::vector<T>& v, GLuint usageMode)
{
	if (v.size() == 0)
	{
		std::cout << "SSBO initialization fails: Empty vector." << std::endl;
		return;
	}

	GLCall(glGenBuffers(1, &id));
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, id));
	GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, v.size() * sizeof(T), &v[0], usageMode));
	size = v.size();
}

template<class T>
void SSBO<T>::bind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, id));
}

template<class T>
void SSBO<T>::unbind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

template<class T>
void SSBO<T>::invalid() const
{
	GLCall(glInvalidateBufferData(id));
}
