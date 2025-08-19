#version 430 core

layout(location = 0) in vec3 vWorldPos;

out vec3 fragColor;

uniform mat4 view;
uniform mat4 proj;

uint rand(uint n) {
	uint state = n * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	word = (word >> 22u) ^ word;
	return word;
}

float randToFloat(uint n) {
	return float(n) / 4294967296.0;
}

void main() {
	gl_Position = proj * view * vec4(vWorldPos, 1);
	fragColor = vec3(randToFloat(rand(gl_VertexID)), randToFloat(rand(gl_VertexID * 10)), randToFloat(rand(gl_VertexID * 20)));
}