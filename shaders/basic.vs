#version 430

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;

uniform vec3 cameraPos;
uniform vec3 scale;
uniform vec3 modelPos;
uniform mat4 proj_matrix;
uniform mat4 mv_matrix;
uniform mat4 m_matrix;
uniform float transparency;

out vec3 norm;
out vec3 worldPos;

vec3 lightDir = normalize(vec3(-1, -1, -1));



void main(void) {
	norm = (m_matrix * vec4(normal, 0)).xyz;
	worldPos = (m_matrix * vec4(position * scale, 1)).xyz; // world position
	
	gl_Position = proj_matrix * mv_matrix * vec4(position * scale, 1.0);
}
