#pragma once

#include <GL/glew.h>
#include <string>

class Texture
{
private:
	GLuint id;
	int width, height, channels;

public:
	Texture(): id(0), width(0), height(0), channels(0) {}
	Texture(Texture&& other) noexcept;
	Texture(const Texture& other) = delete;
	~Texture();

	Texture& operator=(Texture&& other) noexcept;
	Texture& operator=(const Texture& other) = delete;

	GLuint LoadTexture(std::string filename);

	void bind(unsigned int textureUnit = 0) const;

	inline GLuint getId() { return id; }
};

