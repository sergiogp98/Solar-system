#version 150

uniform sampler2D texUnit;
uniform samplerCube skybox;
in vec2 outTexCoord;
in float shade;
out vec4 outColor;

uniform float disabledZBuffer;

void main(void)
{
	if(disabledZBuffer == 1)
	{
		outColor = texture(skybox, outTexCoord);
	}
    else
	{
		outColor = shade*texture(texUnit, outTexCoord);
	}
}

