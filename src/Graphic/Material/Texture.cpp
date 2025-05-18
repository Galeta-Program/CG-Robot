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

void Texture::bind(unsigned int textureUnit /* = 0 */, GLint bindType) const
{
	if (textureUnit >= 32)
	{
		return;
	}

	GLCall(glActiveTexture(GL_TEXTURE0 + textureUnit));
	GLCall(glBindTexture(bindType, id));
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

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		stbi_image_free(data);
	}
	else
	{
		std::cout << "load failed: " << filename << std::endl;
		glDeleteTextures(1, &id);
		id = 0;
	}

	return id;
}

GLuint Texture::LoadCubeMap(std::vector<std::string> faces)
{
	GLCall(glGenTextures(1, &id));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, id));

	int width, height, channels;
	for (GLuint i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
		if (data)
		{
			GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
			stbi_image_free(data);
		}
		else
		{
			std::cout << "failed to load at path: " << faces[i] << std::endl;
			glDeleteTextures(1, &id);
			id = 0;
			return 0;
		}
	}

	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	return id;
}