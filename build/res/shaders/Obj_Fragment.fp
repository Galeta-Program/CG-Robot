#version 460

in vec3 VertexColor;
in vec3 v_LightDir;
in vec3 v_ViewDir;
in vec3 v_Normal;

out vec4 outColor;

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
    
    vec3 result = ambient + diffuse + specular;    
    outColor = vec4(result, 1.0) * vec4(VertexColor, 1.0);
}