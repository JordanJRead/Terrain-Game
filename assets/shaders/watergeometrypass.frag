#version 430 core

in VertOut {
	vec3 worldPos;
} fragIn;

layout(location=0) out vec4 OutGroundWorldPosShellIndex;
layout(location=1) out vec4 OutWorldPosMountain;
layout(location=2) out vec4 OutNormalDoesTexelExist;

uniform vec3 planePos;

#include "_headeruniformbuffers.glsl";
#include "_headermath.glsl";
#include "_headerterraininfo.glsl";

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
		waterInfo.x += amplitude * (exp(sin(dot(waterDir, pos) * freq + perFrameInfo.time * speed)) - 1.4);
		waterInfo.yz += amplitude * exp(sin(dot(waterDir, pos) * freq + perFrameInfo.time * speed)) * cos(dot(waterDir, pos) * freq + perFrameInfo.time * speed) * freq * waterDir;

		amplitude *= waterParams.amplitudeMult;
		freq *= waterParams.freqMult;
		speed *= waterParams.speedMult;
	}
	return waterInfo / amplitudeSum * waterParams.initialAmplitude;
}

void main() {
	vec2 flatWorldPos = fragIn.worldPos.xz;
	vec3 waterInfo = getWaterHeight(flatWorldPos);
	vec3 normal = normalize(vec3(-waterInfo.y, 1, -waterInfo.z));

	OutGroundWorldPosShellIndex = vec4(fragIn.worldPos, -2);
	OutWorldPosMountain = vec4(fragIn.worldPos, 0);
	OutNormalDoesTexelExist = vec4(normal, 0);
}