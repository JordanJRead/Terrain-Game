#version 430 core
#include "_headeruniformbuffers.glsl"
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D source;

void main() {
	vec3 color = texture(source, texCoord).xyz;
	color = vec3(1) - exp(-color * shadowInfo.exposure);
	FragColor = vec4(pow(color, vec3(1/2.2)), 1);
}