#include "ParticleSystem.h"
#include "../Utilty/Error.h" 

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

ParticleSystem::ParticleSystem(): particleAmount(0), ssboBindingPoint(0), ssbo(0), haveTexture(false)
{
}

ParticleSystem::ParticleSystem(ParticleSystem&& other) noexcept:
    ssbo(std::move(other.ssbo)),
    graphicShader(std::move(other.graphicShader)),
    computeShader(std::move(other.computeShader)),
    ubo(std::move(other.ubo)),
    texture(std::move(other.texture)),
    ssboBindingPoint(other.ssboBindingPoint),
    emitters(other.emitters),
    particlesPerEmmitter(other.particlesPerEmmitter),
    particleAmount(other.particleAmount),
    haveTexture(other.haveTexture)
{
}

ParticleSystem::~ParticleSystem()
{
}

ParticleSystem& ParticleSystem::operator=(ParticleSystem&& other) noexcept
{
    if (this != &other)
    {
        ssbo = std::move(other.ssbo);
        graphicShader = std::move(other.graphicShader);
        computeShader = std::move(other.computeShader);
        ubo = std::move(other.ubo);
        texture = std::move(other.texture);
        ssboBindingPoint = other.ssboBindingPoint;
        emitters = other.emitters;
        particlesPerEmmitter = other.particlesPerEmmitter;
        particleAmount = other.particleAmount;
        haveTexture = other.haveTexture;
    }
    
    return *this;
}


void ParticleSystem::init(std::vector<int> particlesInEmitter, const char* vs, const char* fs, const char* cs)
{
	for (unsigned int i = 0; i < particlesInEmitter.size(); i++)
	{
		emitters.emplace_back(Emitter());
		particleAmount += particlesInEmitter[i];
		particlesPerEmmitter.emplace_back(particlesInEmitter[i]);
	}
	ssbo.initialize(particleAmount * sizeof(Particle), GL_DYNAMIC_DRAW);

    ShaderInfo shaders[] = {
            { GL_VERTEX_SHADER, vs },
            { GL_FRAGMENT_SHADER, fs },
            { GL_NONE, NULL } };
    graphicShader.load(shaders);

    computeShader.load(cs);

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

void ParticleSystem::render(float timeNow, float deltaTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, unsigned int emitter /*= -1*/)
{
    if (particleAmount == 0) return;

    computeShader.use();

    GLCall(int location = glGetUniformLocation(computeShader.getId(), "u_EmitterParticleSize"));
    if (location != -1)
    {
        GLCall(glUniform1f(location, emitters[0].getSize()));
    }

    GLCall(location = glGetUniformLocation(computeShader.getId(), "u_DeltaTime")); 
    if (location != -1)
    {
        GLCall(glUniform1f(location, deltaTime));
    }

    GLCall(location = glGetUniformLocation(computeShader.getId(), "u_Velocity"));
    if (location != -1)
    {
        GLCall(glUniform1f(location, emitters[0].getVelocity()));
    }

    GLCall(location = glGetUniformLocation(computeShader.getId(), "u_VelocityDir"));
    if (location != -1)
    {
        GLCall(glUniform3fv(location, 1, glm::value_ptr(emitters[0].getVDir())));
    }

    GLCall(location = glGetUniformLocation(computeShader.getId(), "u_EmitterPos"));
    if (location != -1)
    {
        GLCall(glUniform3fv(location, 1, glm::value_ptr(emitters[0].getPos())));
    }

    GLCall(location = glGetUniformLocation(computeShader.getId(), "u_Time"));
    if (location != -1)
    {
        GLCall(glUniform1f(location, timeNow));
    }

    GLCall(location = glGetUniformLocation(computeShader.getId(), "u_FromIndex"));
    if (location != -1)
    {
        if (emitter == -1)
        {
            GLCall(glUniform1ui(location, 0));
        }
        else
        {
            if (emitter >= particlesPerEmmitter.size())
            {
                return;
            }

            unsigned int count = 0;
            for (unsigned int i = 0; i < emitter; i++)
            {
                count += particlesPerEmmitter[i];
            }

            GLCall(glUniform1ui(location, count));
        }
    }

    GLCall(location = glGetUniformLocation(computeShader.getId(), "u_Size"));
    if (location != -1)
    {
        unsigned int count = 0;
        if (emitter == -1)
        {
            for (unsigned int i = 0; i < particlesPerEmmitter.size(); i++)
            {
                count += particlesPerEmmitter[i];
            }
            GLCall(glUniform1ui(location, count));
        }
        else
        {
            if (emitter >= particlesPerEmmitter.size())
            {
                return;
            }

            GLCall(glUniform1ui(location, particlesPerEmmitter[emitter]));
        }
    }

    
    ssbo.bind(ssboBindingPoint); 

    computeShader.compute();

    GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));

    graphicShader.use(); 

    GLCall(location = glGetUniformLocation(computeShader.getId(), "u_UseTexture"));
    if (location != -1)
    {
        if (haveTexture)
        {
            GLCall(glUniform1i(location, 1));
        }
        else
        {
            GLCall(glUniform1i(location, 0));
        }
    }
    

    ubo.fillInData(0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));
    ubo.fillInData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix));

    ubo.bind();

    if (haveTexture)
    {
        texture.bind(20);
        GLint tex_loc = glGetUniformLocation(graphicShader.getId(), "u_Texture");
    }

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);            

    ssbo.bind(ssboBindingPoint);


    glDrawArrays(GL_POINTS, 0, particleAmount);

    glDisable(GL_PROGRAM_POINT_SIZE);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void ParticleSystem::setupEmitter(std::vector<EmitterSettings> settings)
{
    for (int i = 0; i < settings.size(); i++)
    {
        if (settings[i].idx >= emitters.size())
        {
            std::cout << "Emitter index out of range.\n";
            return;
        }

        emitters[settings[i].idx].init(settings[i]._location, settings[i].vDir, settings[i].aDir, settings[i].v, settings[i].a, settings[i].s);
    }
	
}

void ParticleSystem::setTexture(std::string path)
{
    GLuint texID = texture.LoadTexture(path);
    if (texID != 0) {
        haveTexture = true;
    } else {
        haveTexture = false;
        std::cerr << "Error: Failed to load texture: " << path << std::endl;
    }
}

void ParticleSystem::setParticleAmount(unsigned int amount)
{
	particleAmount = amount;
}