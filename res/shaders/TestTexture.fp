#version 460

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D testTextureSampler; // Unique name for the sampler

void main()
{
    FragColor = texture(testTextureSampler, TexCoord);
} 