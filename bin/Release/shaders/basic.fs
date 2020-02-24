#version 430

in vec3 p;

out vec4 color;

void main(void) {
	color = vec4(0, 0, p.y/2 + .5, 1.0);
}
