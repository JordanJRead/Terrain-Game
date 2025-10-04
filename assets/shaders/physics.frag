#version 430 core

in flat vec3 fragColor;
out vec4 FragColor;

void main() {
	FragColor = vec4(fragColor, 1);
}