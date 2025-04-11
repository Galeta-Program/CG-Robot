#pragma once

#include <GL/glew.h>
#include <string>

class Texture
{
private:
	GLuint id;
	int width, height, channels;

public:
	Texture();
	Texture(Texture&& other);
	Texture(const Texture& other) = delete;
	~Texture();

	Texture& operator=(Texture&& other);
	Texture& operator=(const Texture& other) = delete;

	GLuint LoadTexture(std::string filename);

	void bind(unsigned int textureUnit = 0) const;

	inline GLuint getId() { return id; }
};

