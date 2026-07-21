#version 430 core

in flat vec3 fragColor;
out vec4 FragColour;

void main() {
	FragColour = vec4(fragColor, 1);
}