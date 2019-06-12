#version 150

in vec3 in_Position;
in vec3 in_Normal;
out vec4 out_Color;

uniform mat4 translationMatrix;
uniform mat4 scaleMatrix;
uniform mat4 rotationMatrixX;
uniform mat4 rotationMatrixY;
uniform mat4 rotationMatrixZ;

void main(void)
{
	gl_Position = rotationMatrixX*rotationMatrixY*rotationMatrixZ*vec4(in_Position, 1.0);
  const vec3 light = vec3(0.58, 0.58, 0.58);
  float shade;
  shade = dot(normalize(in_Normal), light);
  shade = clamp(shade, 0, 1);
  out_Color = vec4(shade, shade, shade, 1.0);
}
