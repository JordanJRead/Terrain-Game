#ifndef HEADER_WATER_INFO_GLSL
#define HEADER_WATER_INFO_GLSL

vec3 getWaterInfo(vec2 pos) {
	vec3 waterInfo = vec3(0, 0, 0);

	float amplitude = waterParams.initialAmplitude;
	float freq = waterParams.initialFreq;
	float speed = waterParams.initialSpeed;

	for (int i = 0; i < waterParams.waveCount; ++i) {
		float randNum = randToFloat(rand(i));
		vec2 waterDir = randUnitVector(randNum);
		waterInfo.x += amplitude * (exp(sin(dot(waterDir, pos) * freq + perFrameInfo.time * speed)));
		waterInfo.yz += amplitude * exp(sin(dot(waterDir, pos) * freq + perFrameInfo.time * speed)) * cos(dot(waterDir, pos) * freq + perFrameInfo.time * speed) * freq * waterDir;

		amplitude *= waterParams.amplitudeMult;
		freq *= waterParams.freqMult;
		speed *= waterParams.speedMult;
	}
	return waterInfo;
}

#endif