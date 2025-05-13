#pragma once
#include "StorageBuffer.h"
#include "../Utilty/Error.h"
#include <vector>
#include <GL/glew.h>
#include <iostream>

template<class T>
class SSBO : public StorageBuffer<T>
{
	unsigned int bindingPoint;

public:
	SSBO() : StorageBuffer<T>() {}
	SSBO(unsigned int _bindingPoint) : StorageBuffer<T>(), bindingPoint(_bindingPoint) {}
	SSBO(const std::vector<T>& v);

	SSBO(SSBO&& other) noexcept;
	SSBO(const SSBO& other) = delete;

	~SSBO();

	SSBO& operator=(const SSBO& other) = delete;
	SSBO& operator=(SSBO&& other) noexcept;

	void initialize(unsigned int _size, GLuint usageMode = GL_STATIC_DRAW) override;
	void initialize(const std::vector<T>& v, GLuint usageMode = GL_STATIC_DRAW) override;

	void writeRange(std::vector<T> vec, unsigned int start, unsigned int count);

	void bind() const override;
	void bind(unsigned int bindingPoint) const;
	void unbind() const override;
};

template<class T>
SSBO<T>::SSBO(const std::vector<T>& v): StorageBuffer<T>()
{
	initialize(v);
}

template<class T>
SSBO<T>::SSBO(SSBO<T>&& other) noexcept : StorageBuffer<T>(std::move(other))
{
}


template<class T>
SSBO<T>::~SSBO()
{}

template<class T>
SSBO<T>& SSBO<T>::operator=(SSBO<T>&& other) noexcept
{
	if (this != &other)
	{
		StorageBuffer<T>::operator=(std::move(other));
	}

	return *this;
}

template<class T>
void SSBO<T>::initialize(unsigned int _size, GLuint usageMode)
{
	GLCall(glGenBuffers(1, &this->id));
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->id));
	GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, _size, NULL, usageMode));
	this->size = _size;
}

template<class T>
void SSBO<T>::initialize(const std::vector<T>& v, GLuint usageMode)
{
	if (v.size() == 0)
	{
		std::cout << "SSBO initialization fails: Empty vector." << std::endl;
		return;
	}

	GLCall(glGenBuffers(1, &this->id));
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->id));
	GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, v.size() * sizeof(T), &v[0], usageMode));
	this->size = v.size();
}

template<class T>
void SSBO<T>::writeRange(std::vector<T> vec, unsigned int startIndex, unsigned int count)
{
	bind();
	GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, startIndex * sizeof(T), count * sizeof(T), vec.data()));
	unbind();
}

template<class T>
inline void SSBO<T>::bind() const
{
	GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->id));
}

template<class T>
void SSBO<T>::bind(unsigned int bindingPoint) const
{
	GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint,  this->id));
}

template<class T>
void SSBO<T>::unbind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}
