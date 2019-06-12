#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D grassTex, rockTex;
uniform float MIN_Y_HEIGHT = 0.001;
in vec3 surf;

void main(void)
{
    float shade = clamp(2, 2, 2);
    vec4 grass = texture(grassTex, texCoord.st);
    vec4 rock = texture(rockTex, texCoord.st);
    if(surf.y >= MIN_Y_HEIGHT){
        outColor = shade * sin(surf.x * 5.0) * grass + (1.0 - sin(surf.x * 5.0)) * rock;
    }else{
        outColor = vec4(0.0, 0.0, 1.0, 0.0);
    }
}
