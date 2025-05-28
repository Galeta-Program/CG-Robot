#version 460

layout (local_size_x = 256) in;

struct Particle
{
    float size;
    float initialSize;     
    float lifetime;
    float initialLifetime; 
    vec4 velocity;         
    vec4 acceleration;     
    vec3 pos;
    vec4 color;            
};

layout (binding = 0, std430) buffer ParticleBuffer
{
    Particle ssbo_Particles[];
};

uniform float u_EmitterParticleSize; 
uniform float u_DeltaTime;
uniform float u_Velocity;            
uniform vec3 u_VelocityDir;          
uniform vec3 u_EmitterPos;
uniform float u_Time;

uniform uint u_FromIndex;              
uniform uint u_Size;                



float hash11(float p) {
    p = fract(p * 0.1031);
    p *= p + 33.33;
    p = p * p + p; 
    return fract(p);
}

vec3 hash31(float seed) {
    return vec3(
        hash11(seed * 1.23) * 2.0 - 1.0,
        hash11(seed * 2.34) * 2.0 - 1.0,
        hash11(seed * 3.45) * 2.0 - 1.0
    );
}

float noise2D(vec2 coord, float time) {
    float n = fract(sin(dot(coord + time, vec2(12.9898, 78.233))) * 43758.5453);
    return n * 2.0 - 1.0; 
}


void main()
{
    uint index = gl_GlobalInvocationID.x;

    if(index < u_FromIndex || index > u_FromIndex + u_Size - 1)
    {
        return;
    }

    if (index >= ssbo_Particles.length())
    {
        return;
    }

    Particle particle = ssbo_Particles[index];
    
    particle.lifetime -= u_DeltaTime; 

    if(particle.lifetime <= 0.0) 
    {
        particle.pos = u_EmitterPos; 
        
        float lifetimeSeed = float(index) * 0.763 + u_Time * 0.01; 
        float baseInitialLifetime = 6;    // Range: 1.0 to 6.0 seconds

        particle.initialLifetime = baseInitialLifetime;
        particle.lifetime = particle.initialLifetime;
        
        particle.size = u_EmitterParticleSize + hash11(float(index) * 1.41) * 5.0; // 8 ~ 13
        particle.initialSize = particle.size; // 8 ~ 13
        
        vec3 noiseSpawnDir = hash31(float(index) * 4.56 + u_Time * 0.01) * 0.1;
        vec3 initialDir = normalize(noiseSpawnDir);
        
        float newV = u_Velocity + hash11(float(index) * 7.89 + u_Time * 0.01) * 3;
        particle.velocity = vec4(initialDir, newV);

        particle.color = vec4(hash31(float(index) * 5.67 + u_Time * 0.01), 1.0); 
        particle.acceleration = vec4(0.0, 0.0, 0.0, 0.0); 
    }
    else 
    {
        float lifeRatio = clamp(particle.lifetime / particle.initialLifetime, 0.0, 1.0); 
        float invLifeRatio = 1.0 - lifeRatio;

        vec3 velocityDir = normalize(particle.velocity.xyz);
        float currentSpeed = particle.velocity.w;

        currentSpeed *= (1.0 - invLifeRatio * 0.5); 
        currentSpeed = max(0.0, currentSpeed); 

        vec3 accelerationVec = particle.acceleration.w * normalize(particle.acceleration.xyz);
        vec3 newVelocity = velocityDir * currentSpeed + accelerationVec * u_DeltaTime;
        
        particle.pos += newVelocity * u_DeltaTime;
        
        particle.velocity = vec4(normalize(newVelocity), length(newVelocity));
        
        if (particle.velocity.w <= 1) {
            particle.lifetime = 0;
        }

        particle.color.rgb = vec3(hash31(float(index) * 5.67 + u_Time * 0.01));

        float fadeInAlpha = 1.0f;
        if (invLifeRatio < 0.15f) 
        { 
            fadeInAlpha = smoothstep(0.0f, 0.15f, invLifeRatio);
        }

        float fadeOutAlpha = 1.0f;
        if (lifeRatio < 0.6f) 
        { 
            fadeOutAlpha = smoothstep(0.0f, 0.6f, lifeRatio);
        }
        
        float currentAlpha = min(fadeInAlpha, fadeOutAlpha);
        currentAlpha *= 0.75f; // Overall translucency

        particle.color.a = currentAlpha;
        particle.color.a = max(0.0f, particle.color.a); // Ensure alpha is not negative
    }
    
    ssbo_Particles[index] = particle;
}