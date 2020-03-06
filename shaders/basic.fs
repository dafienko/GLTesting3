#version 430

uniform vec3 cameraPos;
uniform vec3 scale;
uniform vec3 modelPos;

in vec3 norm;
in vec3 worldPos;

out vec4 color;

float PI = 3.14159265359;
vec3 lightDir = normalize(vec3(-1, -2, -1.5));
float SHININESS = 16;

void main(void) {
	vec3 camDir = normalize(worldPos - cameraPos); // direction vector from camera to position
	float lightAngle = acos(min(dot(lightDir, norm), 1)); 
	float camAngle = acos(min(dot(norm, camDir), 1));
	
	float diffuse = lightAngle / PI;
	
	float spectral = 0;
	if (dot(-lightDir, norm) > 0); {
		vec3 v = (cameraPos - worldPos);
		vec3 h = normalize(lightDir + v);
		float specAngle = max(0, dot(h, reflect(lightDir, norm)));
		spectral = pow(specAngle, SHININESS); 
	}
	
	color = vec4(spectral, min(diffuse + spectral, 1) , min(diffuse + spectral, 1), 0);
}
