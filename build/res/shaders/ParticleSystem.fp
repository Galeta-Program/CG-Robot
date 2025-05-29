#version 460

in vec4 v_Color;

layout (binding = 20) uniform sampler2D u_Texture;

out vec4 out_FragColor;

void main(void)
{ 
    out_FragColor = texture(u_Texture, gl_PointCoord) * v_Color;
}
	
    