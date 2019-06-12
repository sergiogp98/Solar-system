#version 150

in  vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;
out vec2 outTexCoord;
out vec3 surf;
out vec3 ex_Normal;

// NY
uniform mat4 projMatrix;
uniform mat4 mdlMatrix;

void main(void)
{
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
	ex_Normal = mat3(mdlMatrix)*inNormal;
	surf = inPosition;
	outTexCoord = inTexCoord;
}
