#version 460

layout (local_size_x = 256) in;

struct Particle
{
    float size;
    float initialSize;      // <<-- 已新增: 儲存粒子初始大小
    float lifetime;
    float initialLifetime;  // <<-- 已新增: 儲存粒子初始生命週期
    vec4 velocity;          // .w for speed
    vec4 acceleration;      // .w for magnitude
    vec3 pos;
    vec4 color;             // <<-- 已新增: 儲存粒子顏色（即使來自貼圖，也需要alpha）
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
    if (index >= ssbo_Particles.length())
        return;

    Particle particle = ssbo_Particles[index];
    
    particle.lifetime -= u_DeltaTime; 

    if(particle.lifetime <= 0.0) 
    {
        float randXOffset = (hash11(float(index) * 1.23) * 2.0 - 1.0) * 0.1;
        float randYOffset = (hash11(float(index) * 2.34) * 2.0 - 1.0) * 0.1;
        float randZOffset = (hash11(float(index) * 3.45) * 2.0 - 1.0) * 0.1; // Reduced spread
        particle.pos = u_EmitterPos + vec3(randXOffset, randYOffset, randZOffset); 
        
        // Generate a base initial lifetime. Adding u_Time to the seed helps ensure
        // that particles reset at different actual times get a different range of lifetimes,
        // further desynchronizing batches over longer periods.
        float lifetimeSeed = float(index) * 0.763 + u_Time * 0.01; // Small factor for u_Time to avoid overly rapid changes
        float baseInitialLifetime = 1.0 + hash11(lifetimeSeed) * 5.0;    // Range: 1.0 to 6.0 seconds

        particle.initialLifetime = baseInitialLifetime;

        // Make particles start at a slightly varied point in their new lifetime (e.g., 0% to 20% "aged")
        // This helps to smooth out the visual "pulse" of newly emitted particles.
        float startLifeProgress = hash11(float(index) * 5.55 + u_Time * 0.02) * 0.20; // 0.0 to 0.20 (i.e. 0% to 20%)
        particle.lifetime = particle.initialLifetime * (1.0 - startLifeProgress);
        
        particle.size = u_EmitterParticleSize + hash11(float(index) * 1.41) * 5.0; // 8 ~ 13
        particle.initialSize = particle.size; // 8 ~ 13
        
        vec3 noiseSpawnDir = hash31(float(index) * 4.56) * 0.1;
        vec3 initialDir = normalize(u_VelocityDir + noiseSpawnDir);
        
        float newV = u_Velocity + hash11(float(index) * 7.89) * 3;
        particle.velocity = vec4(initialDir, newV);

        particle.color = vec4(1.0, 1.0, 1.0, 1.0); 
        particle.acceleration = vec4(0.0, 0.0, 0.0, 0.0); // Initialize acceleration
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
        
        // --- 湍流 / 橫向擴散 ---
        float height = particle.pos.z - u_EmitterPos.z;
        if(height > 0.0) { 
            float turbulenceStrength = mix(0.05, 0.25, invLifeRatio) * min(1.0, height * 0.05);
            
            vec2 noiseCoord = particle.pos.xy * 0.1 + u_Time * 0.05; 
            float noiseValX = noise2D(noiseCoord, u_Time); 
            float noiseValY = noise2D(noiseCoord + vec2(100.0, 200.0), u_Time); 

            newVelocity.x += noiseValX * turbulenceStrength * u_DeltaTime;
            newVelocity.y += noiseValY * turbulenceStrength * u_DeltaTime; 
        }
        
        particle.pos += newVelocity * u_DeltaTime;
        
        particle.velocity = vec4(normalize(newVelocity), length(newVelocity));
        
        if (particle.velocity.w <= 0.01) {
            particle.lifetime = 0;
        }

        // --- Particle Scaling over Lifetime ---
        // Scaled using a quadratic: starts at 0.2 * initialSize,
        // peaks at 1.5 * initialSize (mid-life), ends at 0.01 * initialSize.
        float f_invLifeRatio = invLifeRatio; // Alias for conciseness
        float scaleMultiplier = -5.58 * f_invLifeRatio * f_invLifeRatio + 5.39 * f_invLifeRatio + 0.2;
        particle.size = particle.initialSize * scaleMultiplier;
        particle.size = max(0.01, particle.size); // Ensure min size, though it should stay above with these coeffs.

        // --- Color Over Lifetime ---
        vec3 hotColor = vec3(1.0, 1.0, 0.6);    // Bright yellow-white
        vec3 midColor = vec3(1.0, 0.5, 0.0);    // Orange
        vec3 coolColor = vec3(0.8, 0.2, 0.0);   // Reddish

        vec3 finalColorVal;
        if (invLifeRatio < 0.3f) { // First 30% of life
            finalColorVal = mix(hotColor, midColor, invLifeRatio / 0.3f);
        } else if (invLifeRatio < 0.7f) { // Next 40% of life
            finalColorVal = mix(midColor, coolColor, (invLifeRatio - 0.3f) / 0.4f);
        } else { // Last 30% of life (invLifeRatio from 0.7 to 1.0)
            vec3 veryCoolColor = vec3(0.05, 0.05, 0.05); // Very dark, desaturated
            // As invLifeRatio goes from 0.7 to 1.0, t goes from 0 to 1
            float t = clamp((invLifeRatio - 0.7f) / 0.3f, 0.0, 1.0); // Ensure t is [0,1]
            finalColorVal = mix(coolColor, veryCoolColor, t);
        }
        particle.color.rgb = finalColorVal;

        // --- Alpha Over Lifetime ---
        float fadeInAlpha = 1.0f;
        if (invLifeRatio < 0.15f) { // First 15% of life (invLifeRatio: 0 -> 0.15)
            fadeInAlpha = smoothstep(0.0f, 0.15f, invLifeRatio);
        }

        float fadeOutAlpha = 1.0f;
        // lifeRatio = 1.0 (new) -> 0.0 (dead)
        if (lifeRatio < 0.6f) { // Last 60% of life (lifeRatio: 0.6 -> 0)
            fadeOutAlpha = smoothstep(0.0f, 0.6f, lifeRatio);
        }
        
        float currentAlpha = min(fadeInAlpha, fadeOutAlpha);
        currentAlpha *= 0.75f; // Overall translucency

        particle.color.a = currentAlpha;
        particle.color.a = max(0.0f, particle.color.a); // Ensure alpha is not negative
    }
    
    ssbo_Particles[index] = particle;
}