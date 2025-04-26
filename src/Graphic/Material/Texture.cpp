#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"
#include "../../Utilty/Error.h"
#include "../../src/stb_image.h"

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

	GLCall(glActiveTexture(GL_TEXTURE0 + textureUnit));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
}

GLuint Texture::LoadTexture(std::string filename)
{
	
	GLCall(glGenTextures(1, &id));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
	stbi_set_flip_vertically_on_load(1);

	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
	if (data)
	{
		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "load failed: " << filename << std::endl;
	}

	stbi_image_free(data);

	return id;
}