#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
out vec2 outTexCoord;
out vec3 transformedNormal;

uniform mat4 mdlMatrix;
uniform mat4 worldToViewMatrix;
mat3 normalMatrix1;

void main(void)
{
    gl_Position = mdlMatrix*worldToViewMatrix*vec4(inPosition, 1.0);
    normalMatrix1 = mat3(mdlMatrix);
    transformedNormal = normalMatrix1*inNormal;
    outTexCoord = inTexCoord; 
} 
