#version 460

layout (local_size_x = 256) in;

struct Particle
{
    float size;
    float lifetime;
    vec4 velocity;     
    vec4 acceleration; 
    vec4 color;
    vec3 pos;
};

layout (binding = 0, std430) buffer ParticleBuffer
{
    Particle ssbo_Particles[];
};

uniform float u_DeltaTime;
uniform vec3 u_EmitterPos;

// Random number generation
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
    uint index = gl_GlobalInvocationID.x;
    if (index >= ssbo_Particles.length())
        return;

    Particle particle = ssbo_Particles[index];

    particle.lifetime -= u_DeltaTime;

    if(particle.lifetime <= 0)
    {
        particle.pos = u_EmitterPos;
    }
    else
    {
        particle.pos += particle.velocity.xyz * particle.velocity.w * u_DeltaTime;

        vec3 velocityDir = normalize(particle.velocity.xyz);
        vec3 accelerationDir = normalize(particle.acceleration.xyz);

        vec3 velocityVec = particle.velocity.w * velocityDir;
        vec3 accelerationVec = particle.acceleration.w * accelerationDir;

        particle.velocity = vec4(normalize(velocityVec), length(velocityVec));
        particle.acceleration = vec4(normalize(accelerationVec), length(accelerationVec));

        float lifeRatio = particle.lifetime / 2.0; // Normalized lifetime
        particle.color = mix(
            vec4(1.0, 0.0, 0.0, 1.0),  // Final color (red)
            vec4(1.0, 0.5, 0.0, 1.0),  // Initial color (orange)
            lifeRatio
        );
    }
    
    particle.size *= 0.99;
    ssbo_Particles[index] = particle;
} 
