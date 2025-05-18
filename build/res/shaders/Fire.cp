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


vec3 limitDeviation(vec3 vector, vec3 reference, float maxAngle) {
    if(length(vector) < 0.001 || length(reference) < 0.001)
        return vector;
        
    vec3 normVector = normalize(vector);
    vec3 normReference = normalize(reference);
    
    float angle = acos(dot(normVector, normReference));
    
    if(angle <= maxAngle)
        return vector;
        
    vec3 perp = normVector - normReference * dot(normVector, normReference);
    if(length(perp) > 0.001) {
        perp = normalize(perp);
        return length(vector) * (cos(maxAngle) * normReference + sin(maxAngle) * perp);
    }
    return vector;
}

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
        float randXOffset = (hash11(float(index) * 1.23) * 2.0 - 1.0) * 0.3;
        float randYOffset = (hash11(float(index) * 2.34) * 2.0 - 1.0) * 0.3;
        float randZOffset = (hash11(float(index) * 3.45) * 2.0 - 1.0) * 0.3;
        particle.pos = u_EmitterPos + vec3(randXOffset, randYOffset, randZOffset); 
        
        particle.lifetime = 1.0 + hash11(float(index) * 0.763) * 5.0; // 1 ~ 6
        particle.initialLifetime = particle.lifetime; // 1 ~ 6
        
        particle.size = u_EmitterParticleSize + hash11(float(index) * 1.41) * 5.0; // 8 ~ 13
        particle.initialSize = particle.size; // 8 ~ 13
        
        vec3 noiseSpawnDir = hash31(float(index) * 4.56) * 0.3; 
        vec3 initialDir = normalize(u_VelocityDir + noiseSpawnDir);
        
        particle.velocity = vec4(initialDir, u_Velocity);

        particle.color = vec4(1.0, 1.0, 1.0, 1.0); 
    }
    else 
    {

        // 計算生命週期比例 (1.0:新 -> 0.0:死) 和反向比例 (0.0:新 -> 1.0:死)
        float lifeRatio = clamp(particle.lifetime / particle.initialLifetime, 0.0, 1.0); 
        float invLifeRatio = 1.0 - lifeRatio;

        vec3 velocityDir = normalize(particle.velocity.xyz);
        float currentSpeed = particle.velocity.w;

        // 隨著粒子老化/上升，速度逐漸減慢 (末端減速高達 50%)
        currentSpeed *= (1.0 - invLifeRatio * 0.5); 
        currentSpeed = max(0.0, currentSpeed); // 防止速度為負

        vec3 accelerationVec = particle.acceleration.w * normalize(particle.acceleration.xyz);
        vec3 newVelocity = velocityDir * currentSpeed + accelerationVec * u_DeltaTime;
        
        vec3 mainUpDirection = vec3(0, 0, 1); 
        newVelocity = limitDeviation(newVelocity, mainUpDirection, 0.785); // 0.785 弧度 = 45 度
        
        // --- 湍流 / 橫向擴散 ---
        float height = particle.pos.z - u_EmitterPos.z; // <<-- 根據 mainUpDirection 調整這裡的軸！
        if(height > 0.0) { 
            // 湍流強度隨生命週期和高度增加
            float turbulenceStrength = mix(0.1, 0.8, invLifeRatio) * min(1.0, height * 0.1); 
            
            vec2 noiseCoord = particle.pos.xy * 0.1 + u_Time * 0.05; // <<-- 使用 u_Time
            float noiseValX = noise2D(noiseCoord, u_Time); // <<-- 使用 u_Time
            float noiseValY = noise2D(noiseCoord + vec2(100.0, 200.0), u_Time); // <<-- 使用 u_Time

            newVelocity.x += noiseValX * turbulenceStrength * u_DeltaTime;
            newVelocity.y += noiseValY * turbulenceStrength * u_DeltaTime; // <<-- 根據 mainUpDirection 調整這裡的軸！
        }
        
        // 更新粒子位置
        particle.pos += newVelocity * u_DeltaTime;
        
        // 更新粒子速度 (方向和大小)
        particle.velocity = vec4(normalize(newVelocity), length(newVelocity));
        
        

        // --- 透明度隨生命週期變化 (實現平滑淡出) ---
        float alpha = 1.0;
        if (lifeRatio < 0.3) { // 在生命週期最後 30% 快速淡出
            alpha = smoothstep(0.0, 0.3, lifeRatio); // 從 0.0 (完全透明) 到 1.0 (完全不透明)
        }
        particle.color.a = alpha; // <<-- 重要: 更新粒子的 Alpha 值
        particle.color.a = max(0.0, particle.color.a); // 確保透明度不為負
    }
    
    ssbo_Particles[index] = particle;
}