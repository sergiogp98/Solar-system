#version 150

in vec2 outTexCoord;
in vec3 exNormal;
out vec4 outColor;

void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58);
 	float shade;
 	shade = dot(normalize(exNormal), light);
 	shade = clamp(shade, 0, 1);
	float a = sin(outTexCoord.s*30)/2+0.5;
	float b = sin(outTexCoord.t*30)/2+0.5;
	outColor = shade*vec4(a, b, 1.0, 0.0);
}