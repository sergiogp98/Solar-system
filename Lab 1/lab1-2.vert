#version 150

in  vec3 in_Position;
uniform mat4 translationMatrix;
uniform mat4 scaleMatrix;
uniform mat4 rotationMatrixX;
uniform mat4 rotationMatrixY;
uniform mat4 rotationMatrixZ;

void main(void)
{
	gl_Position = translationMatrix*vec4(in_Position, 1.0);
}
