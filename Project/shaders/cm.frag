#version 150

in vec3 exNormal;
in vec3 surf;
in vec3 exSurface;
out vec4 outColor;
uniform sampler2D tex;
uniform float drawSun;

float arctan(float x, float y){
	if(x > 0){
		return atan(y/x);
	}else if(x < 0){
		return 3.14 + atan(y/x);
	}else if(y > 0){
		return 3.14 / 2;
	}else if(y < 0){
		return -3.14 / 2;
	}
}

void main(void) 
{
	vec2 tc;
	float radius = sqrt(pow(surf.x, 2) + pow(surf.y, 2) + pow(surf.z, 2));

	//Spherical mapping
	tc.s = (arctan(surf.x, surf.z) + 3.14/2.0) / (2.0*3.14);
	tc.t = 1.0 - (asin(surf.y/radius) + 3.14/2) / 3.14;

	if(drawSun == 1){ //Light source
		outColor = texture(tex, tc);
	}else{
		const vec3 light = vec3(0.58, 0.58, 0.58);
		float diffuse, specular, shade;
				
		//Diffuse
		diffuse = dot(normalize(exNormal), light);
		
		//Shading + texture
		shade = 0.7 * diffuse;
		outColor = texture(tex, tc) + vec4(shade);
	}
}