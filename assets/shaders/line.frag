#version 430 core

in float t;
out vec4 FragColour;

uniform vec3 lineStartColour;
uniform vec3 lineEndColour;

void main() {
	vec3 blendedColour = mix(lineStartColour, lineEndColour, t);
	FragColour = vec4(blendedColour, 1);
}