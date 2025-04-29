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

uniform MaterialInfo u_Material;
uniform sampler2D u_Texture;

uniform u_Light
{
    float u_Shininess;
    vec3 u_LightColor;
};

out vec4 out_FragColor;

void main(void)
{ 
    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(v_LightDir);
    vec3 viewDir = normalize(v_ViewDir);

    vec4 texColor = texture(u_Texture, v_TexCoord);
    
    vec3 ambient = u_Material.Ka * u_LightColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_Material.Kd * u_LightColor;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    vec3 specular = spec * u_Material.Ks * u_LightColor;
    
    vec3 result = ambient + diffuse + specular;    
    out_FragColor = vec4(result, 1.0) * texColor;
}
	
    