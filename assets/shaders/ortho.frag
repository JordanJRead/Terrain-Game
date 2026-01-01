#version 430 core

out vec4 FragColor;
uniform vec3 colour;

void main() {
	FragColor = vec4(colour, 1);
}