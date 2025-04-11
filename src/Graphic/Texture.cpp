#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"
#include "../src/stb_image.h"

#include <iostream>

Texture::Texture(Texture&& other) noexcept :
	id(other.id),
	width(other.width),
	height(other.height),
	channels(other.channels)
{
	other.id = 0;
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	if (this != &other)
	{
		id = other.id;
		other.id = 0;

		width = other.width;
		height = other.height;
		channels = other.channels;
	}

	return *this;
}

void Texture::bind(unsigned int textureUnit /* = 0 */) const
{
	if (textureUnit >= 32)
	{
		return;
	}

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, id);
}

GLuint Texture::LoadTexture(std::string filename)
{
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
	if (data)
	{
		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "load failed: " << filename << std::endl;
	}
	stbi_image_free(data);

	return texture;
	
	return 0;
}