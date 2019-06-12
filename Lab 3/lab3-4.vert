#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
out vec3 ex_Normal; 
out vec2 outTexCoord;
out vec3 surf;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToProjectionMatrix;

void main(void)
{
	gl_Position =  worldToProjectionMatrix * modelToWorldMatrix * vec4(inPosition, 1.0);
	ex_Normal = mat3(modelToWorldMatrix)*inNormal;
    surf = inPosition; 
	outTexCoord = inTexCoord;
}


