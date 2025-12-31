#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

out VertOut {
	vec3 worldPos;
	vec3 viewPos;
} vertOut;

// Per plane
uniform float planeWorldWidth;

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
		//waterInfo.x += amplitude * sin(dot(waterDir, pos) * freq + perFrameInfo.time * speed);
		waterInfo.x += amplitude * (exp(sin(dot(waterDir, pos) * freq + perFrameInfo.time * speed)) - 1.4);
		waterInfo.yz += amplitude * exp(sin(dot(waterDir, pos) * freq + perFrameInfo.time * speed)) * cos(dot(waterDir, pos) * freq + perFrameInfo.time * speed) * freq * waterDir;

		amplitude *= waterParams.amplitudeMult;
		freq *= waterParams.freqMult;
		speed *= waterParams.speedMult;
	}
	return waterInfo / amplitudeSum * waterParams.initialAmplitude;
}

void main() {
	vec2 flatPlanePos = vec2(chunkData.data[gl_InstanceID * 2 + 0], chunkData.data[gl_InstanceID * 2 + 1]);
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + flatPlanePos.x, waterParams.height, vPos.y * planeWorldWidth + flatPlanePos.y, 1);
	vec2 flatWorldPos = worldPos.xz;
	vec3 waterInfo = getWaterHeight(flatWorldPos);
	worldPos.y += waterInfo.x;

	vertOut.worldPos = worldPos.xyz;
	vertOut.viewPos = (perFrameInfo.viewMatrix * worldPos).xyz;
	gl_Position = perFrameInfo.projectionMatrix * (vec4(vertOut.viewPos, 1));
}