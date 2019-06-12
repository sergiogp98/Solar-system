#version 150

in vec3 in_Position;
out vec4 out_Color;
in vec4 in_Color;
uniform mat4 translationMatrix;
uniform mat4 scaleMatrix;
uniform mat4 rotationMatrixX;
uniform mat4 rotationMatrixY;
uniform mat4 rotationMatrixZ;

void main(void)
{
	gl_Position = rotationMatrixX*rotationMatrixY*rotationMatrixZ*vec4(in_Position, 1.0);
    out_Color = in_Color*vec4(1.0);
}
