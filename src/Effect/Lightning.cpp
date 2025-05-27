#include "Lightning.h"
#include "../Utilty/Error.h"

#include <math.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h> 
#include <time.h>   

glm::vec3 random3(glm::vec3 c) {
	float j = 4096.0 * sin(glm::dot(c, glm::vec3(17.0, 59.4, 15.0)));

	glm::vec3 r;
	r.z = 512.0 * j - floor(512.0 * j);
	j *= .125;
	r.x = 512.0 * j - floor(512.0 * j);
	j *= .125;
	r.y = 512.0 * j - floor(512.0 * j);

	return r - glm::vec3(0.5, 0.5, 0.5);
}

/* skew constants for 3d simplex functions */
const float F3 = 0.3333333;
const float G3 = 0.1666667;

/* 3d simplex noise */
float simplex3d(glm::vec3 p) {
	/* 1. find current tetrahedron T and it's four vertices */
	/* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
	/* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/

	/* calculate s and x */
	glm::vec3 s = floor(p + glm::dot(p, glm::vec3(F3)));
	glm::vec3 x = p - s + glm::dot(s, glm::vec3(G3));

	/* calculate i1 and i2 */
	glm::vec3 e = glm::step(glm::vec3(0.0), x - glm::vec3(x.y, x.z, x.x));
	glm::vec3 i1 = e * (glm::vec3(1.0, 1.0, 1.0) - glm::vec3(e.z, e.x, e.y));
	glm::vec3 i2 = glm::vec3(1.0, 1.0, 1.0) - glm::vec3(e.z, e.x, e.y) * (glm::vec3(1.0, 1.0, 1.0) - e);

	/* x1, x2, x3 */
	glm::vec3 x1 = x - i1 + G3;
	glm::vec3 x2 = x - i2 + glm::vec3(2.0, 2.0, 2.0) * G3;
	glm::vec3 x3 = x - glm::vec3(1.0, 1.0, 1.0) + glm::vec3(3.0 * G3, 3.0 * G3, 3.0 * G3);

	/* 2. find four surflets and store them in d */
	glm::vec4 w, d;

	/* calculate surflet weights */
	w.x = glm::dot(x, x);
	w.y = glm::dot(x1, x1);
	w.z = glm::dot(x2, x2);
	w.w = glm::dot(x3, x3);

	/* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
	w = glm::vec4(0.6, 0.6, 0.6, 0.6) - w;
	for (int i = 0; i < 4; i++)
	{
		if (w[i] < 0)
		{
			w[i] = 0.0f;
		}
	}

	/* calculate surflet components */
	d.x = dot(random3(s), x);
	d.y = dot(random3(s + i1), x1);
	d.z = dot(random3(s + i2), x2);
	d.w = dot(random3(s + glm::vec3(1.0, 1.0, 1.0)), x3);

	/* multiply d by w^4 */
	w *= w;
	w *= w;
	d *= w;

	/* 3. return the sum of the four surflets */
	return glm::dot(d, glm::vec4(52.0, 52.0, 52.0, 52.0));
}

float noise(glm::vec3 m) {
	return   0.5333333 * simplex3d(m)
		+ 0.2666667 * simplex3d(2.0f * m)
		+ 0.1333333 * simplex3d(4.0f * m)
		+ 0.0666667 * simplex3d(8.0f * m);
}

Lightning::Lightning() :
	gs(),
	vao(),
	vbo(),
	texture(),
	segementCount(6),
	duration(0.25),
	center(0, 0, 0),
	maxLifetime(0.25),
	displacementScale(0.4),
	maxRucursionDepth(6),
	minSegementLength(0.02),
	lightningLifetime(0.0f),
	haveTexture(false)
{
	ShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "../res/shaders/Lightning.vp"},
			{ GL_FRAGMENT_SHADER, "../res/shaders/Lightning.fp" },
			{ GL_NONE, NULL } };
	gs.load(shaders);

	GLCall(uProjLocation = glGetUniformLocation(gs.getId(), "u_Projection"));
	GLCall(uViewLocation = glGetUniformLocation(gs.getId(), "u_View"));
	GLCall(uIntensityLocation = glGetUniformLocation(gs.getId(), "u_Intensity"));

	srand(time(NULL));

	vbo.initialize(0, GL_DYNAMIC_DRAW);
}

Lightning::Lightning(Lightning&& other) noexcept:
	vao(std::move(other.vao)),
	vbo(std::move(other.vbo)),
	texture(std::move(other.texture)),
	segementCount(other.segementCount),
	duration(other.duration),
	center(other.center),
	endPoints(other.endPoints),
	lightningPoints(other.lightningPoints),
	lightningLifetime(other.lightningLifetime),
	haveTexture(other.haveTexture)
{
	srand(time(NULL));
}

Lightning& Lightning::operator=(Lightning && other) noexcept
{
	if (this != &other)
	{
		vao = std::move(other.vao);
		vbo = std::move(other.vbo);
		texture = std::move(other.texture);
		segementCount = other.segementCount;
		duration = other.duration;
		center = other.center;
		endPoints = other.endPoints;
	}

	return *this;
}

void Lightning::setCenter(glm::vec3 pos)
{
	center = pos;
}

void Lightning::setEndPoints(std::vector<glm::vec3> _endPoints)
{
	endPoints = _endPoints;
}

void Lightning::addEndPoints(glm::vec3 pos)
{
	endPoints.emplace_back(pos);
}

void Lightning::addEndPoints(std::vector<glm::vec3> positions)
{
	endPoints.insert(endPoints.end(), positions.begin(), positions.end());
}

void Lightning::updateEndPoints(unsigned int idx, glm::vec3 pos)
{
	if (endPoints.size() <= idx)
	{
		return;
	}
	endPoints[idx] = pos;
}

void Lightning::setTexture(const char* path)
{
	texture.LoadTexture(path);
	haveTexture = true;
}

void Lightning::render(float deltaTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	lightningLifetime -= deltaTime;

	gs.use();

	vao.bind();
	vbo.bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

	float intensity = glm::max(0.0f, lightningLifetime / maxLifetime);
	GLCall(glUniform1f(uIntensityLocation, intensity));
	GLCall(glUniformMatrix4fv(uProjLocation, 1, false, glm::value_ptr(projectionMatrix)));
	GLCall(glUniformMatrix4fv(uViewLocation, 1, false, glm::value_ptr(viewMatrix)));

	for (int i = 0; i < endPoints.size(); i++)
	{
		generateAndActivateLightning(center, endPoints[i]);
		addWidth(10);
		glDrawArrays(GL_LINE_STRIP, 0, lightningPoints.size()); // 頂點數量 = 數據長度 / 每個頂點的分量數
	}
}

void Lightning::generateLightningPathRecursive(glm::vec3 p1, glm::vec3 p2, unsigned int depth, float displacementScale) {

	if (depth <= 0 || glm::length(p2 - p1) < minSegementLength) {
        lightningPoints.emplace_back(p2);
        return;
    }

    glm::vec3 midPoint = (p1 + p2) * 0.5f;

	glm::vec3 segmentVector = p2- p1;
	// choose 2 simple prependicular vectors
	glm::vec3 prep1(-segmentVector.y, segmentVector.x, 0);
	glm::vec3 prep2(0, -segmentVector.z, segmentVector.y);

	float w1 = (float)rand(), w2 = (float)rand();

	glm::vec3 perpendicularVector = glm::normalize(prep1 * w1 + prep2 * w2);

    float randomOffset = noise(p2 - p1) * glm::length(p2 - p1) * displacementScale;
    glm::vec3 displacedMidPoint = midPoint +  perpendicularVector * randomOffset;

    generateLightningPathRecursive(p1, displacedMidPoint, depth - 1, displacementScale * 0.7);
    generateLightningPathRecursive(displacedMidPoint, p2, depth - 1, displacementScale * 0.7);
}

void Lightning::generateAndActivateLightning(glm::vec3 startPoint, glm::vec3 endPoint) {
	lightningPoints.clear();

    lightningPoints.emplace_back(startPoint); 
    generateLightningPathRecursive(startPoint, endPoint, maxRucursionDepth, displacementScale);

    vbo.bind();
	vbo.setData(lightningPoints, GL_DYNAMIC_DRAW);

    lightningLifetime = maxLifetime;
}

void Lightning::addWidth(unsigned int width)
{
	for (int i = 0; i < lightningPoints.size(); i++)
	{

	}
}

