#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
out vec3 exNormal;
out vec2 outTexCoord;

uniform mat4 mdlMatrix;

void main(void)
{
	gl_Position = mdlMatrix*vec4(inPosition, 1.0);
    exNormal = inNormal;
    outTexCoord = inTexCoord; 
} 