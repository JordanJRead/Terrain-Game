#version 430 core
#include "_headeruniformbuffers.glsl"
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D source;

void main() {
	vec3 color = texture(source, texCoord).xyz;
	//color.x = 2/(1 + pow(1.1, -color.x)) - 1;
	//color.y = 2/(1 + pow(1.1, -color.y)) - 1;
	//color.z = 2/(1 + pow(1.1, -color.z)) - 1;
	color = vec3(1) - exp(-color * 0.1);
	FragColor = vec4(pow(color, vec3(1/2.2)), 1);
}