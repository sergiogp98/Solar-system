#version 150

in  vec3 inPosition;
in  vec2 inTexCoord;
out vec2 texCoord;

uniform mat4 projectionMatrix;
uniform mat4 worldToViewMatrix;

void main(void)
{
	texCoord = inTexCoord;
	
	gl_Position = projectionMatrix * worldToViewMatrix * vec4(inPosition, 1.0);
}
