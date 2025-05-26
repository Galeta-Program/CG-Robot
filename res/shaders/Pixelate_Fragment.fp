#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D u_ScreenTexture;
uniform float u_PixelSize;
uniform vec2 u_ScreenSize;
uniform float u_Alpha;
uniform sampler2D u_DepthTexture;

uniform bool u_usePixelate;
uniform bool u_useToonshader;

float getDepth(vec2 uv) 
{
    return texture(u_DepthTexture, uv).r;
}

vec3 quantizeColor(vec3 color, int levels)
{
    return floor(color * float(levels)) / float(levels);
}

void main() 
{
    vec2 pixelatedUV;
    if (u_usePixelate == true)
        pixelatedUV = floor(TexCoords * u_ScreenSize / u_PixelSize) * u_PixelSize / u_ScreenSize;
    else
        pixelatedUV = TexCoords;

    vec3 color = texture(u_ScreenTexture, pixelatedUV).rgb;

    if (u_useToonshader == true)
    {
        vec2 texelSize = 1.0 / u_ScreenSize;
        float center = getDepth(pixelatedUV);

        float threshold = 0.000005;
        bool isDrawLine = false;

        if (abs(center - getDepth(pixelatedUV + vec2(-texelSize.x, 0.0))) > threshold) isDrawLine = true;
        if (abs(center - getDepth(pixelatedUV + vec2( texelSize.x, 0.0))) > threshold) isDrawLine = true;
        if (abs(center - getDepth(pixelatedUV + vec2(0.0,  texelSize.y))) > threshold) isDrawLine = true;
        if (abs(center - getDepth(pixelatedUV + vec2(0.0, -texelSize.y))) > threshold) isDrawLine = true;

        if (isDrawLine)
        {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);  // √‰ΩtΩu
        }
        else
        {
            int levels = 16;
            vec3 quantizedColor = quantizeColor(color, levels);
            FragColor = vec4(quantizedColor, u_Alpha);
        }
    }
    else
    {
        FragColor = vec4(color, u_Alpha);
    }
}
