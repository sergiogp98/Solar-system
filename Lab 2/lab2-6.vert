#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
out vec2 outTexCoord;
out float shade;

uniform mat4 mdlMatrix;
uniform mat4 worldToViewMatrix;
mat3 normalMatrix1;

void main(void)
{
    gl_Position = mdlMatrix*worldToViewMatrix*vec4(inPosition, 1.0);
    normalMatrix1 = mat3(mdlMatrix);
    vec3 transformedNormal = normalMatrix1*inNormal;
    const vec3 light = vec3(0.58, 0.58, 0.58);
 	shade = dot(normalize(transformedNormal), light);
 	shade = clamp(shade, 0, 1);
    outTexCoord = inTexCoord;
}