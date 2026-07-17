#version 430 core
#include "_headeruniformbuffers.glsl"
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D source;
uniform sampler2D noise;

void main() {
	vec3 color = texture(source, texCoord).xyz;

	// Tone mapping
	color = vec3(1) - exp(-color * shadowInfo.exposure);

	// Gamma
	color = pow(color, vec3(1/2.2));
	
	// Noise
	vec2 noiseSamplePos = gl_FragCoord.xy / textureSize(noise, 0).xy;
	float noiseValue = texture(noise, noiseSamplePos).r * (1 / 256.0 - 1 / 512.0) * 3;
	color = color + vec3(noiseValue);

	FragColor = vec4(color, 1);

}