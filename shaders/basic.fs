#version 430

uniform vec3 cameraPos;
uniform vec3 scale;
uniform vec3 modelPos;

in vec3 norm;
in vec3 worldPos;

out vec4 color;

float PI = 3.14159265359;
vec3 lightDir = normalize(vec3(-1, -1, -1));

void main(void) {
	vec3 camDir = normalize(worldPos - cameraPos); // direction vector from camera to position
	float lightAngle = acos(min(dot(lightDir, norm), 1)); 
	float camAngle = acos(min(dot(norm, camDir), 1));
	
	float diffuse = lightAngle / PI;
	
	vec3 relLightPos = worldPos - lightDir; // since light is just a direction, the light "position" is always -dir away from the point
	vec3 lcDir = normalize(cameraPos - relLightPos); // direction from light position to camera position
	float d = norm.x * worldPos.x + norm.y * worldPos.y + norm.z * worldPos.z; // d is part of plane equation, the plane referred to is the surface plane
	float c = (d - (norm.x * relLightPos.x + norm.y * relLightPos.y  + norm.z * relLightPos.z)) / (norm.x*norm.x + norm.y*norm.y + norm.z*norm.z); // used to find closes point on surface plane
	vec3 llPos = relLightPos + norm * c; // lateral light pos, closest point on plane to lightPos
	vec3 llDir = lcDir - norm * dot(lcDir, norm); // lateral light->camera direction, only the component of lcDir parallel to surface plane;
	normalize(llDir);
	
	c = (d - (norm.x * cameraPos.x + norm.y * cameraPos.y  + norm.z * cameraPos.z)) / (norm.x*norm.x + norm.y*norm.y + norm.z*norm.z); // used to find closes point on surface plane
	vec3 lcPos = cameraPos + norm * c;
	
	float y = length(lcPos - relLightPos);
	float b = length(llPos - lcPos);
	float y2 = length(llPos - cameraPos);
	
	float theta = atan((y + y2) / b); // angle between perfect bounce vector from light to surface plane to camera and surface plane
	float x1 = y / tan(theta);
	vec3 hitPos = llPos + llDir * x1; // light position cast onto the surface plane + plane light->camera vector * lateral distance from light to hitPos on plane
	vec3 bounceDir = normalize(hitPos - relLightPos);
	bounceDir = normalize(bounceDir + norm * 2);
	
	float angle = acos(min(1, dot(camDir, -bounceDir)));
	
	float spectral = 1 - (min(10, angle * (180 / PI)) / 10);
	//float spectral = angle;
	
	//color = vec4(bounceDir, 0);
	
	float dist = length(worldPos - hitPos);
	
	if (dist > .1) {
		color = vec4(spectral, min(diffuse, 0), min(diffuse, 0), 0);
	} else {
		color = vec4(0, 0, 1, 0);
	}
	//color = c;
}
