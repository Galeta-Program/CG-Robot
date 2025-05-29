#version 460

in vec3 VertexColor;
in vec3 v_LightDir;
in vec3 v_ViewDir;
in vec3 v_Normal;

in vec4 worldPos;

out vec4 outColor;

uniform samplerCube u_Skybox;
uniform vec3 u_CameraPos;

uniform sampler2D u_ShadowMap;
uniform mat4 u_LightSpaceMatrix;

uniform u_Light
{
    float u_Shininess;
    vec3 u_LightColor;
};

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.x < 0.0 || projCoords.x > 1.0 ||
       projCoords.y < 0.0 || projCoords.y > 1.0 ||
       projCoords.z > 1.0)
    {
        return 0.0;
    }

    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // 防止shadow acne
    float bias = max(0.002 * (1.0 - dot(normalize(v_Normal), normalize(v_LightDir))), 0.0005);

    float intensity = clamp(1.0 - u_Shininess / 100.0, 0.0, 1.0);
    float baseShadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    float shadow = baseShadow * intensity;

    return shadow;
}


void main()
{
    vec3 I = normalize(worldPos.xyz - u_CameraPos);
    vec3 R = reflect(I, normalize(v_Normal));   // 反射

    vec3 reflectionColor = texture(u_Skybox, R).rgb; // 去cubemap取樣

    vec4 lightSpacePos = u_LightSpaceMatrix * worldPos;

    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(v_LightDir);
    vec3 viewDir = normalize(v_ViewDir);

    float shadow = ShadowCalculation(lightSpacePos);
    
    vec3 ambient = 1.0 * u_LightColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    vec3 specular = spec * u_LightColor;
    
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular); 
    
    //outColor = vec4(result, 1.0) * vec4(VertexColor, 1.0) * vec4(reflectionColor, 1.0);

    outColor = vec4(reflectionColor, 1.0);
}