#include "ParticleSystem.h"
#include "../Utilty/Error.h" 
#include "EffectManager.h"

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
    emitters(std::move(other.emitters)),
    particlesPerEmmitter(other.particlesPerEmmitter),
    particleAmount(other.particleAmount),
    haveTexture(other.haveTexture)
{}

ParticleSystem::~ParticleSystem()
{}

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
        emitters = std::move(other.emitters);
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
		emitters.emplace_back(std::move(Emitter()));
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

void ParticleSystem::addEmitter(unsigned int particleCount, glm::vec3 initialPos)
{
    unsigned int oldParticleAmount = particleAmount;
    particleAmount += particleCount;
    emitters.emplace_back(std::move(Emitter()));
    emitters[emitters.size() - 1].setLocation(initialPos);
    emitters[emitters.size() - 1].setVelocity(200.0f);
    emitters[emitters.size() - 1].setAcceleration(5.0f);
    emitters[emitters.size() - 1].setSize(8.0f);
    emitters[emitters.size() - 1].enable();
    particlesPerEmmitter.emplace_back(particleCount);

    ssbo.setSize(particleAmount, GL_DYNAMIC_DRAW);
    
    std::vector<Particle> newParticles(particleCount, Particle{});
    for (auto& p : newParticles) {
        p.lifetime = 0.0f;
        p.color.a = 0.0f;
        p.pos = glm::vec3(0.0f, -1000.0f, 0.0f); 
        p.velocity = glm::vec4(0.0f);
        p.size = 0.0f;
    }
    ssbo.writeRange(newParticles, oldParticleAmount, newParticles.size());
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

void ParticleSystem::render(float timeNow, float deltaTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (particleAmount == 0) return;

    computeShader.use();
    ssbo.bind(ssboBindingPoint);

    int location;
    for (int i = 0; i < emitters.size(); i++)
    {
        GLCall(location = glGetUniformLocation(computeShader.getId(), "u_EmitterParticleSize"));
        if (location != -1)
        {
            GLCall(glUniform1f(location, emitters[i].getSize()));
        }

        GLCall(location = glGetUniformLocation(computeShader.getId(), "u_DeltaTime"));
        if (location != -1)
        {
            GLCall(glUniform1f(location, deltaTime));
        }

        GLCall(location = glGetUniformLocation(computeShader.getId(), "u_Velocity"));
        if (location != -1)
        {
            GLCall(glUniform1f(location, emitters[i].getVelocity()));
        }

        GLCall(location = glGetUniformLocation(computeShader.getId(), "u_VelocityDir"));
        if (location != -1)
        {
            GLCall(glUniform3fv(location, 1, glm::value_ptr(emitters[i].getVDir())));
        }

        GLCall(location = glGetUniformLocation(computeShader.getId(), "u_EmitterPos"));
        if (location != -1)
        {
            GLCall(glUniform3fv(location, 1, glm::value_ptr(emitters[i].getPos())));
        }

        GLCall(location = glGetUniformLocation(computeShader.getId(), "u_Time"));
        if (location != -1)
        {
            GLCall(glUniform1f(location, timeNow));
        }

        unsigned int fromIndex = 0, size = 0;

        if (i >= particlesPerEmmitter.size())
        {
            return;
        }

        for (unsigned int j = 0; j < i; j++)
        {
            fromIndex += particlesPerEmmitter[i];
        }
        size = particlesPerEmmitter[i];

        GLCall(location = glGetUniformLocation(computeShader.getId(), "u_FromIndex"));
        if (location != -1)
        {
            GLCall(glUniform1ui(location, fromIndex));
        }

        GLCall(location = glGetUniformLocation(computeShader.getId(), "u_Size"));
        if (location != -1)
        {
            GLCall(glUniform1ui(location, size));
        }

        computeShader.compute(fromIndex, size);
        GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));

    }


    graphicShader.use(); 

    GLCall(location = glGetUniformLocation(graphicShader.getId(), "u_UseTexture"));
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

    for (int i = 0; i < emitters.size(); i++)
    {
        if (emitters[i].isDisabled())
        {
            continue;
        }

        unsigned int emitterFromIndex = 0;
        for (unsigned int j = 0; j < i; j++)
        {
            emitterFromIndex += particlesPerEmmitter[j];
        }
        unsigned int emitterSize = particlesPerEmmitter[i];

        GLCall(location = glGetUniformLocation(graphicShader.getId(), "u_FromIndex"));
        if (location != -1)
        {
            GLCall(glUniform1ui(location, emitterFromIndex));
        }

        GLCall(location = glGetUniformLocation(graphicShader.getId(), "u_Size"));
        if (location != -1)
        {
            GLCall(glUniform1ui(location, emitterSize));
        }

        glDrawArrays(GL_POINTS, 0, emitterSize);
    }

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

void ParticleSystem::setEmitterPos(unsigned int index, glm::vec3 pos)
{
    if (index >= emitters.size())
    {
        std::cout << "Failed to set emitter position.\n";
        return;
    }

    emitters[index].setLocation(pos);
}

void ParticleSystem::setEmitterDir(unsigned int index, glm::vec3 dir)
{
    if (index >= emitters.size())
    {
        std::cout << "Failed to set emitter position.\n";
        return;
    }

    emitters[index].setVelocityDirection(dir);
    emitters[index].setAccelerationDirection(dir);
}

void ParticleSystem::setEmitterVelocity(unsigned int index, float val)
{
    if (index >= emitters.size())
    {
        return;
    }

    emitters[index].setVelocity(val);
}

void ParticleSystem::setEmitterAcceleration(unsigned int index, float val)
{
    if (index >= emitters.size())
    {
        return;
    }

    emitters[index].setAcceleration(val);
}

void ParticleSystem::setEmitterParticleSize(unsigned int index, float val)
{
    if (index >= emitters.size())
    {
        return;
    }

    emitters[index].setSize(val);
}

void ParticleSystem::disableEmitter(unsigned int idx)
{
    if (idx >= emitters.size())
    {
        return;
    }

    emitters[idx].disable();
}

void ParticleSystem::enableEmitter(unsigned int idx)
{
    if (idx >= emitters.size())
    {
        return;
    }

    emitters[idx].enable();
}

void ParticleSystem::reset()
{
    if (particleAmount > 0) {
        std::vector<Particle> clearParticles(particleAmount, Particle{});
        for (auto& p : clearParticles) {
            p.lifetime = 0.0f;
            p.color.a = 0.0f;
            p.pos = glm::vec3(0.0f, -1000.0f, 0.0f); // Move far away
            p.velocity = glm::vec4(0.0f);
            p.size = 0.0f;
        }
        ssbo.writeRange(clearParticles, 0, clearParticles.size());
    }
    
    emitters.clear();
    particlesPerEmmitter.clear();
    particleAmount = 0;
    ssbo.setSize(0, GL_DYNAMIC_DRAW);
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

glm::vec3 ParticleSystem::getEmitterPos(unsigned int index)
{
    if (index >= emitters.size())
    {
        std::cout << "Cannot get emitter position: index out of range.\n";
        return glm::vec3(0, 0, 0);
    }
    return emitters[index].getPos();
}

glm::vec3 ParticleSystem::getEmitterDir(unsigned int index)
{
    if (index >= emitters.size())
    {
        std::cout << "Cannot get emitter direction: index out of range.\n";
        return glm::vec3(0, 0, 0);
    }
    return emitters[index].getVDir();
}

bool ParticleSystem::getEmitterEnableState(unsigned int index)
{
    if (index >= emitters.size())
    {
        std::cout << "Cannot get emitter enable state: index out of range.\n";
        return false;
    }

    return !emitters[index].isDisabled();
}
