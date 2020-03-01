#version 430

in float angle;
in vec4 c;

out vec4 color;

float PI = 3.14159265359;

void main(void) {
	color = vec4(0, ((.5 *angle)/(.5 *PI)), ((.5 *angle)/(.5 *PI)), 1.0);
	//color = c;
}
