#version 430

in vec3 p;

out vec4 color;

uniform mat4 proj_matrix;

void main(void) {
	color = vec4(0, 0, p.y/2 + .5, 1.0);
}
