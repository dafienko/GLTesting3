#version 430

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;

uniform vec3 modelPos;
uniform mat4 proj_matrix;
uniform mat4 mv_matrix;
uniform vec3 cameraPos;

out float angle;
out vec4 c;

void main(void) {
	vec3 p = position + modelPos;
	vec3 dir = normalize(p - cameraPos);
	float a = acos(dot(dir, normal)); 
	angle = a;
	c = vec4(dir, 1);
	gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}
