#version 150

uniform sampler2D texUnit;
in vec2 outTexCoord;
in vec3 exNormal;
out vec4 outColor;

void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58);
 	float shade;
 	shade = dot(normalize(exNormal), light);
 	shade = clamp(shade, 0, 1);
	outColor = shade*texture(texUnit, outTexCoord*10);
}
