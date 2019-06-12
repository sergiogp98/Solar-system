#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
out vec3 exNormal;
out vec2 outTexCoord;

uniform mat4 translationMatrix;
uniform mat4 scaleMatrix;
uniform mat4 rotationMatrixX;
uniform mat4 rotationMatrixY;
uniform mat4 rotationMatrixZ;

void main(void)
{
	gl_Position = rotationMatrixZ*rotationMatrixY*rotationMatrixX*scaleMatrix*translationMatrix*vec4(inPosition, 1.0);
    exNormal = inNormal;
    outTexCoord = inTexCoord; 
}   
