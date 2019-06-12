#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;

void main(void)
{
	float shade = clamp(2, 2, 2);
	outColor = shade * texture(tex, texCoord);
}
