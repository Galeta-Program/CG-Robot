#version 460

layout (local_size_x = 256) in;

struct Particle
{
    double size;
    double lifetime;
    vec4 velocity;     
    vec4 acceleration; 
    vec4 color;
    vec3 pos;
};

layout (binding = 0, std430) buffer ParticleBuffer
{
    Particle ssbo_Particles[];
};

uniform float deltaTime;

void main()
{
    uint index = gl_GlobalInvocationID.x;
    if (index >= ssbo_Particles.length())
        return;

    // Particle update logic will go here
} 
