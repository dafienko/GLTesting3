#version 430

layout (location=0) in vec3 position;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

out vec3 p;

void main(void) {
	p = position;
	gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}
