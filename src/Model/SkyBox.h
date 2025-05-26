#pragma once

#include "../Scene/SceneGraph/Node.h"
#include "../Graphic/VAO.h"
#include "../Graphic/Material/Texture.h"
#include "../Scene/Camera.h"
#include "../Model/ManualObject.h"

#include <string>
#include <Vector>
#include <map>

class SkyBox
{
private:
	ManualObject skyBoxObj;
	Texture texture;
	std::vector<std::vector<std::string>> skyBoxFaces;

public:
	SkyBox() {}
	~SkyBox() {}
	SkyBox(SkyBox&& other) noexcept;
	SkyBox(const SkyBox& other) = delete;

	SkyBox& operator=(SkyBox&& other) noexcept;
	SkyBox& operator=(const SkyBox& other) = delete;

	ManualObject& getObject() { return skyBoxObj; }
	bool loadFaces(std::vector<std::string> faces);
	void updateDate();
	void render(CG::Camera* camera, GLint type = GL_TRIANGLES);

	inline Texture& getTexture() { return texture; }
};

