#version 150

uniform sampler2D texUnit;
in vec2 outTexCoord;
in float shade;
out vec4 outColor;

void main(void)
{
    outColor = shade*texture(texUnit, outTexCoord);
}
