#include "ParticleSystem.h"
#include "../Utilty/Error.h" 

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

ParticleSystem::ParticleSystem(): particleAmount(0), ssboBindingPoint(0), ssbo(0)
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::init(std::vector<int> particlesInEmitter)
{
	for (unsigned int i = 0; i < particlesInEmitter.size(); i++)
	{
		emitters.emplace_back(Emitter());
		particleAmount += particlesInEmitter[i];
		particlesPerEmmitter.emplace_back(particlesInEmitter[i]);
	}
	ssbo.initialize(particleAmount * sizeof(Particle), GL_DYNAMIC_DRAW);

	ShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "../res/shaders/ParticleSystem.vp" },
			{ GL_FRAGMENT_SHADER, "../res/shaders/ParticleSystem.fp" },
			{ GL_NONE, NULL } };
	graphicShader.load(shaders);

	computeShader.load("../res/shaders/Fire.cp");
	computeShader.setGroupAmount(std::floor(particleAmount / 256.0) + 1, 1, 1);

    ubo.initialize(sizeof(glm::mat4) * 2);
    ubo.associateWithShaderBlock(graphicShader.getId(), "u_MatVP", 0);
}

void ParticleSystem::emit()
{
	unsigned int currentOffset = 0;
	for (int i = 0; i < emitters.size(); i++)
	{
		emitters[i].emit(ssbo, currentOffset, currentOffset + particlesPerEmmitter[i] - 1);
		currentOffset += particlesPerEmmitter[i];
	}
}

void ParticleSystem::render(float deltaTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (particleAmount == 0) return;

    computeShader.use(); 

    GLCall(int location = glGetUniformLocation(computeShader.getId(), "u_DeltaTime")); 
    if (location != -1)
    {
        GLCall(glUniform1f(location, deltaTime));
    }

    GLCall(location = glGetUniformLocation(computeShader.getId(), "u_EmitterPos"));
    if (location != -1)
    {
        GLCall(glUniform3fv(location, 1, glm::value_ptr(emitters[0].getPos())));
    }

    ssbo.bind(ssboBindingPoint); 

    GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));

    graphicShader.use(); 

    ubo.fillInData(0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));
    ubo.fillInData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix));

    ubo.bind();

    glm::mat4 modelMatrix = glm::mat4(1.0f); 
    

    glEnable(GL_BLEND | GL_VERTEX_PROGRAM_POINT_SIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    glDepthMask(GL_FALSE);            
    // glEnable(GL_DEPTH_TEST); // Keep depth test if they should be occluded

    // If your vertex shader uses gl_PointSize:
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Bind the SSBO for the graphics shader to read from (vertex shader)
    // The SSBO is already bound from the compute pass, but explicit binding for clarity
    // or if you unbound it is fine. It should be bound to the same binding point
    // as declared in the vertex shader (e.g., binding = 0).
    ssbo.bind(ssboBindingPoint);


    // Issue Draw Call
    glDrawArrays(GL_POINTS, 0, particleAmount);

    // Restore OpenGL States
    glBindVertexArray(0); // Unbind VAO
    glDisable(GL_PROGRAM_POINT_SIZE);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void ParticleSystem::setupEmitter(unsigned int idx, glm::vec3 _location, glm::vec3 vDirA, glm::vec3 vDirB, glm::vec3 vDirC, glm::vec3 aDirA, glm::vec3 aDirB, glm::vec3 aDirC, glm::vec3 _color, float vMin, float vMax, float aMin, float aMax, float sMin, float sMax, float lMin, float lMax)
{
	if (idx >= emitters.size())
	{
		std::cout << "Emitter index out of range.\n";
		return;
	}

	emitters[idx].init(_location, vDirA, vDirB, vDirC, aDirA, aDirB, aDirC, _color, vMin, vMax, aMin, aMax, sMin, sMax, lMin, lMax);
}

void ParticleSystem::setParticleAmount(unsigned int amount)
{
	particleAmount = amount;
}