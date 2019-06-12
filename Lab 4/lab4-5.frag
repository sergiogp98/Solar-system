#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D grassTex, rockTex;
in vec3 surf;

void main(void)
{
	float shade = clamp(2, 2, 2);
    vec4 grass = texture(grassTex, texCoord.st);
    vec4 rock = texture(rockTex, texCoord.st);
	outColor = shade * sin(surf.x*5.0) * grass + (1.0 - sin(surf.x*5.0)) * rock;
}
