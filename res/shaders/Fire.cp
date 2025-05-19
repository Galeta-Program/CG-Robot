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
        // random spread
        float randXOffset = (hash11(float(index) * 1.23) * 2.0 - 1.0) * 0.1;
        float randYOffset = (hash11(float(index) * 2.34) * 2.0 - 1.0) * 0.1;
        float randZOffset = (hash11(float(index) * 3.45) * 2.0 - 1.0) * 0.1; 
        particle.pos = u_EmitterPos + vec3(randXOffset, randYOffset, randZOffset); 
        
        float lifetimeSeed = float(index) * 0.763 + u_Time * 0.01; 
        float baseInitialLifetime = 1.0 + hash11(lifetimeSeed) * 5.0;    // Range: 1.0 to 6.0 seconds

        particle.initialLifetime = baseInitialLifetime;

        // Make particles start at a slightly varied point in their new lifetime (e.g., 0% to 20% "aged")
        // This helps to smooth out the visual "pulse" of newly emitted particles.
        float startLifeProgress = hash11(float(index) * 5.55 + u_Time * 0.02) * 0.20; // 0.0 to 0.20 (i.e. 0% to 20%)
        particle.lifetime = particle.initialLifetime * (1.0 - startLifeProgress);
        
        particle.size = u_EmitterParticleSize + hash11(float(index) * 1.41) * 5.0; // 8 ~ 13
        particle.initialSize = particle.size; // 8 ~ 13
        
        vec3 noiseSpawnDir = hash31(float(index) * 4.56 + u_Time * 0.01) * 0.1;
        vec3 initialDir = normalize(u_VelocityDir + noiseSpawnDir);
        
        float newV = u_Velocity + hash11(float(index) * 7.89 + u_Time * 0.01) * 3;
        particle.velocity = vec4(initialDir, newV);

        particle.color = vec4(1.0, 1.0, 1.0, 1.0); 
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

        // --- Particle Scaling over Lifetime ---
        // Scaled using a quadratic: starts at 0.2 * initialSize,
        // peaks at 1.5 * initialSize (mid-life), ends at 0.01 * initialSize.
        float f_invLifeRatio = invLifeRatio; // Alias for conciseness
        float scaleMultiplier = -5.58 * f_invLifeRatio * f_invLifeRatio + 5.39 * f_invLifeRatio + 0.2;
        particle.size = particle.initialSize * scaleMultiplier;
        particle.size = max(0.01, particle.size); // Ensure min size, though it should stay above with these coeffs.

        vec3 hotColor = vec3(1.0, 1.0, 0.6);    // Bright yellow-white
        vec3 midColor = vec3(1.0, 0.5, 0.0);    // Orange
        vec3 coolColor = vec3(0.8, 0.2, 0.0);   // Reddish

        vec3 finalColorVal;
        if (invLifeRatio < 0.3f) 
        { 
            finalColorVal = mix(hotColor, midColor, invLifeRatio / 0.3f);
        } 
        else if (invLifeRatio < 0.7f)
        { 
            finalColorVal = mix(midColor, coolColor, (invLifeRatio - 0.3f) / 0.4f);
        } 
        else 
        { 
            vec3 veryCoolColor = vec3(0.05, 0.05, 0.05);
            float t = clamp((invLifeRatio - 0.7f) / 0.3f, 0.0, 1.0);
            finalColorVal = mix(coolColor, veryCoolColor, t);
        }
        particle.color.rgb = finalColorVal;

        float fadeInAlpha = 1.0f;
        if (invLifeRatio < 0.15f) 
        { 
            fadeInAlpha = smoothstep(0.0f, 0.15f, invLifeRatio);
        }

        float fadeOutAlpha = 1.0f;
        // lifeRatio = 1.0 (new) -> 0.0 (dead)
        if (lifeRatio < 0.6f) 
        { // Last 60% of life (lifeRatio: 0.6 -> 0)
            fadeOutAlpha = smoothstep(0.0f, 0.6f, lifeRatio);
        }
        
        float currentAlpha = min(fadeInAlpha, fadeOutAlpha);
        currentAlpha *= 0.75f; // Overall translucency

        particle.color.a = currentAlpha;
        particle.color.a = max(0.0f, particle.color.a); // Ensure alpha is not negative
    }
    
    ssbo_Particles[index] = particle;
}