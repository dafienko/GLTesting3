#version 430

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;

uniform vec3 cameraPos;
uniform vec3 scale;
uniform vec3 modelPos;
uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

out vec3 norm;
out vec3 worldPos;

vec3 lightDir = normalize(vec3(-1, -1, -1));



void main(void) {
	norm = normal;
	worldPos = position * scale + modelPos; // world position
	
	gl_Position = proj_matrix * mv_matrix * vec4(position * scale, 1.0);
}
