#pragma once

#include "../Utilty/Error.h"
#include <vector>
#include <GL/glew.h>
#include <iostream>

template<class T>
class VBO
{
private:
	GLuint id;
	unsigned int size;

public:
	VBO() : id(0), size(0) {}
	VBO(const std::vector<T>& v);

	VBO(VBO&& other) noexcept;
	VBO(const VBO& other) = delete;

	~VBO();

	VBO& operator=(const VBO& other) = delete;
	VBO& operator=(VBO&& other) noexcept;

	void initialize(unsigned int size);
	void initialize(const std::vector<T>& v);

	void bind() const;
	void unbind() const;
	void invalid() const;

	static void copyAndWrite(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, unsigned int size);

	inline unsigned int getSize() const { return size; }
	inline unsigned int getId() const { return id; }
};

template< typename T >
void VBO<T>::copyAndWrite(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, unsigned int size)
{
	GLCall(glBindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer));
	GLCall(glBindBuffer(GL_COPY_READ_BUFFER, readBuffer));
	GLCall(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size));
	
	GLCall(glBindBuffer(GL_COPY_WRITE_BUFFER, 0));
	GLCall(glBindBuffer(GL_COPY_READ_BUFFER, 0));
}

template<class T>
VBO<T>::VBO(const std::vector<T>& v)
{
	initialize(v);
}

template<class T>
VBO<T>::VBO(VBO<T>&& other) noexcept : id(other.id), size(other.size)
{
	other.id = 0;
}


template<class T>
VBO<T>::~VBO()
{
	if (id != 0)
	{
		GLCall(glDeleteBuffers(1, &id));
	}
}

template<class T>
VBO<T>& VBO<T>::operator=(VBO<T>&& other) noexcept
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
void VBO<T>::initialize(unsigned int size)
{
	GLCall(glGenBuffers(1, &id));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW));
	size = size;
}

template<class T>
void VBO<T>::initialize(const std::vector<T>& v)
{
	if (v.size() == 0)
	{
		std::cout << "VBO initialization fails: Empty vector." << std::endl;
		return;
	}

	GLCall(glGenBuffers(1, &id));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
	GLCall(glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(T), &v[0], GL_STATIC_DRAW));
	size = v.size();
}

template<class T>
void VBO<T>::bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
}

template<class T>
void VBO<T>::unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

template<class T>
void VBO<T>::invalid() const
{
	GLCall(glInvalidateBufferData(id));
}
