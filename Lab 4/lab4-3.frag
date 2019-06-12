#version 150

in vec2 outTexCoord;
in vec3 ex_Normal;
in vec3 surf;
out vec4 outColor;

uniform sampler2D tex;

uniform vec3 cameraVector;
uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];

void main(void)
{


    vec3 total_light_color = vec3(0);

	for (int i = 0; i < 4; i++) {
		vec3 lightDirection = vec3(0, 0, 0);
		if (isDirectional[i])
			lightDirection = lightSourcesDirPosArr[i];
		else
			lightDirection = normalize(lightSourcesDirPosArr[i] - surf);
		
		//Diffuse
		vec3 lightColor = lightSourcesColorArr[i];
		float shade = dot(normalize(ex_Normal), lightDirection);
		shade = clamp(shade, 0, 1);

		// Specular
		vec3 reflectedLightDirection = reflect(-lightDirection, ex_Normal);
		vec3 eyeDirection = normalize(cameraVector - surf);
		float specularStrength = 0.0;
		if (dot(lightDirection, ex_Normal) > 0.0) {
			specularStrength = dot(reflectedLightDirection, eyeDirection);
			specularStrength = max(specularStrength, 0);
			specularStrength = pow(specularStrength, specularExponent[i]);
		}

		total_light_color += lightColor * (shade + specularStrength);
	}
	vec3 normalized = normalize(ex_Normal);
	outColor = vec4(normalized.x, normalized.x, normalized.x, 1.0);
}
