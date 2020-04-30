#version 430

uniform vec3 cameraPos;
uniform vec3 scale;
uniform vec3 modelPos;
uniform int wireframeEnabled;
uniform vec3 wireframeColor;
uniform float transparency;


in vec3 norm;
in vec3 worldPos;

out vec4 color;

float PI = 3.14159265359;
vec3 lightDir = normalize(vec3(-1, -2, -1.5));
float SHININESS = 16;
vec4 AMBIENT = vec4(0, .2, .2, 0);

float shininessFactor = 1;
vec4 diffuseColor = vec4(0, 1, 1, 1);
vec4 specularColor = vec4(1, 1, 1, 1);

void main(void) {
	if (wireframeEnabled == 0) {
		vec3 camDir = normalize(worldPos - cameraPos); // direction vector from camera to position
		float lightAngle = max(-1, dot(-lightDir, norm)); 
		
		float diffuse = lightAngle;

		
		float specular = 0;
		if (dot(-lightDir, norm) > 0); {
			vec3 v = (cameraPos - worldPos);
			vec3 h = normalize(lightDir + v);
			float specAngle = max(0, dot(h, reflect(lightDir, norm)));
			specular = pow(specAngle, SHININESS); 
		}
		
		AMBIENT = vec4(0, 0, 0, 0);
		color = AMBIENT + vec4(specular, min(diffuse + specular, 1), min(diffuse + specular, 1), 1 - transparency);
		
	} else {
		color = vec4(wireframeColor, 1);
	}
}
