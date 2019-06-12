#version 150
	
in  vec3 inPosition;
in  vec3 inNormal;
in  vec2 inTexCoord;
out vec3 exNormal;
out vec3 surf;
out vec3 exSurface;

uniform mat4 projectionMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 modelToWorldMatrix;

void main(void)
{
	mat3 normalMatrix1 = mat3(worldToViewMatrix * modelToWorldMatrix);
	exNormal = normalMatrix1 * inNormal;
	surf = inPosition;
	exSurface = vec3(worldToViewMatrix * modelToWorldMatrix * vec4(inPosition, 1.0));
	gl_Position = projectionMatrix * worldToViewMatrix * modelToWorldMatrix * vec4(inPosition, 1.0);
}
