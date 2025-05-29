#version 460

in vec3 VertexColor;
in vec3 v_LightDir;
in vec3 v_ViewDir;
in vec3 v_Normal;
in vec4 worldPos;
in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D u_WaterTexture;
uniform sampler2D u_ReflectionTexture;
uniform sampler2D u_RefractionTexture;
uniform float u_Time;
uniform vec3 u_CameraPos;

uniform u_Light
{
    float u_Shininess;
    vec3 u_LightColor;
};

void main()
{
    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(v_LightDir);
    vec3 viewDir = normalize(v_ViewDir);

    vec3 ambient = 1.0 * u_LightColor;
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    vec3 specular = spec * u_LightColor;
    vec3 lightingResult = ambient + diffuse + specular;

    vec2 minXZ = vec2(-100.0, -100.0);
    vec2 rangeXZ = vec2(200.0, 200.0);
    vec2 worldUV = (worldPos.xz - minXZ) / rangeXZ;

    float waveSpeed = 0.1;
    vec2 offset = vec2(u_Time * waveSpeed, u_Time * waveSpeed * 0.7);
    float wave = texture(u_WaterTexture, worldUV + offset).r;

    float distortionStrength = 0.02;
    vec2 distortion = vec2(wave, wave) * distortionStrength;

    vec3 I = normalize(worldPos.xyz - u_CameraPos);
    vec3 R = reflect(I, normal);
    vec3 T = refract(I, normal, 1.0 / 1.33);

    vec2 reflectUV = clamp(worldUV + R.xz * 0.05 + distortion, 0.001, 0.999);
    vec2 refractUV = clamp(worldUV + T.xz * 0.05 + distortion, 0.001, 0.999);

    vec3 reflectionColor = texture(u_ReflectionTexture, reflectUV).rgb;
    vec3 refractionColor = texture(u_RefractionTexture, refractUV).rgb;

    float fresnel = pow(1.0 - max(dot(normal, -I), 0.0), 3.0) * 0.7 + 0.3;
    vec3 surfaceColor = mix(refractionColor, reflectionColor, fresnel);

    vec3 waveDarken = VertexColor - wave * vec3(0.0, 0.2, 0.4);
    waveDarken = clamp(waveDarken, 0.0, 1.0);
    vec3 finalWater = mix(surfaceColor, waveDarken, 0.25);

    outColor = vec4(finalWater * lightingResult, 1.0);
}
