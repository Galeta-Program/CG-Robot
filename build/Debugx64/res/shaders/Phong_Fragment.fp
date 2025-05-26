#version 460
struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

in vec3 v_Normal;
in vec3 v_LightDir;
in vec3 v_ViewDir;
in vec2 v_TexCoord;
in vec4 v_WorldPos;

uniform MaterialInfo u_Material;
uniform sampler2D u_Texture;

uniform sampler2D u_ShadowMap;
uniform mat4 u_LightSpaceMatrix;

uniform u_Light
{
    float u_Shininess;
    vec3 u_LightColor;
};

out vec4 out_FragColor;

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

    // Bias¨¾¤îshadow acne
    float bias = max(0.002 * (1.0 - dot(normalize(v_Normal), normalize(v_LightDir))), 0.0005);

    float intensity = clamp(1.0 - u_Shininess / 100.0, 0.0, 1.0);
    float baseShadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    float shadow = baseShadow * intensity;

    return shadow;
}

void main(void)
{ 
    vec4 lightSpacePos = u_LightSpaceMatrix * v_WorldPos;
    
    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(v_LightDir);
    vec3 viewDir = normalize(v_ViewDir);
    vec3 halfwayDir = normalize(lightDir + viewDir); // Blinn Phong

    vec4 texColor = texture(u_Texture, v_TexCoord);
    
    float shadow = ShadowCalculation(lightSpacePos);

    vec3 ambient = u_Material.Ka * u_LightColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_Material.Kd * u_LightColor;

    vec3 reflectDir = reflect(-lightDir, normal); // Phong
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), u_Shininess);
    vec3 specular = spec * u_Material.Ks * u_LightColor;
    
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);       
    out_FragColor = vec4(result, 1.0) * texColor;
}
	
    