#pragma once
#include "../Graphic/VAO.h"
#include "../Graphic/VBO.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"
#include "../Graphic/Material/Texture.h"

#include <glm/glm.hpp>
#include <vector>

class Lightning
{
private:
	GraphicShader gs;
	VAO vao;
	VBO<glm::vec3> vbo;
	Texture texture;

	unsigned int segementCount;
	unsigned int duration;
	unsigned int maxRucursionDepth;
	float displacementScale;;
	float minSegementLength;
	float lightningLifetime;
	float maxLifetime;
	glm::vec3 center;
	std::vector<glm::vec3> endPoints;
	std::vector<glm::vec3> lightningPoints;


	bool haveTexture;

	GLuint uProjLocation;
	GLuint uViewLocation;
	GLuint uIntensityLocation;


public:
	Lightning();
	Lightning(Lightning&& other) noexcept;
	Lightning(const Lightning& other) = delete;
	~Lightning() {}

	Lightning& operator=(Lightning&& other) noexcept;
	Lightning& operator=(const Lightning& other) = delete;

	void setCenter(glm::vec3 pos);
	void setEndPoints(std::vector<glm::vec3> _endPoints);
	void addEndPoints(glm::vec3 pos);
	void addEndPoints(std::vector<glm::vec3> positions);
	void updateEndPoints(unsigned int idx, glm::vec3 pos);
	void clearEndPoints() { endPoints.clear(); }

	void setTexture(const char* path);

	void render(float deltaTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	void generateLightningPathRecursive(glm::vec3 p1, glm::vec3 p2, unsigned int depth, float displacementScale);
	void generateAndActivateLightning(glm::vec3 startPoint, glm::vec3 endPoint);

	void addWidth(unsigned int width = 1);

	inline glm::vec3 getCenter() { return center; }
	inline std::vector<glm::vec3> getEndPoints() { return endPoints; }
};