#version 150

out vec4 out_Color_Fragment;
in vec4 out_Color;

void main(void)
{
	out_Color_Fragment = out_Color;
}
