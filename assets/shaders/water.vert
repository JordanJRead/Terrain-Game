#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

out vec3 worldPos3;
out vec3 viewPos;

// Per frame
uniform mat4 view;
uniform mat4 proj;
uniform float time;

// Per plane
uniform float planeWorldWidth;
uniform vec3 planePos;

#include "_headeruniformbuffers.glsl";
#include "_headermath.glsl";

vec3 getWaterHeight(vec2 pos) {
	vec3 waterInfo = vec3(0, 0, 0);

	float amplitude = waterParams.initialAmplitude;
	float freq = waterParams.initialFreq;
	float speed = waterParams.initialSpeed;

	float amplitudeSum = 0;

	for (int i = 0; i < waterParams.waveCount; ++i) {
		amplitudeSum += amplitude;
		float randNum = randToFloat(rand(i));
		vec2 waterDir = randUnitVector(randNum);
		//waterInfo.x += amplitude * sin(dot(waterDir, pos) * freq + time * speed);
		waterInfo.x += amplitude * (exp(sin(dot(waterDir, pos) * freq + time * speed)) - 1.4);
		waterInfo.yz += amplitude * exp(sin(dot(waterDir, pos) * freq + time * speed)) * cos(dot(waterDir, pos) * freq + time * speed) * freq * waterDir;

		amplitude *= waterParams.amplitudeMult;
		freq *= waterParams.freqMult;
		speed *= waterParams.speedMult;
	}
	return waterInfo / amplitudeSum * waterParams.initialAmplitude;
}

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + planePos.x, planePos.y, vPos.y * planeWorldWidth + planePos.z, 1);
	vec2 flatWorldPos = worldPos.xz;
	vec3 waterInfo = getWaterHeight(flatWorldPos);
	worldPos.y += waterInfo.x;

	worldPos3 = worldPos.xyz;
	viewPos = (view * worldPos).xyz;
	//worldPos.y = planePos.y;
	gl_Position = proj * vec4((view * worldPos).xyz, 1);
}