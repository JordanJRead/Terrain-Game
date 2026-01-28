#version 430 core
#define PI 3.141592653589793238462

in vec2 latticePos;
out vec4 OutTerrainData;

uniform float imageScale;

#include "_headeruniformbuffers.glsl"
#include "_headermath.glsl"

vec3 getNonMountainTerrainInfo(vec2 pos) {
	vec3 mountain = vec3(0);//perlin(pos * terrainParams.mountainFrequency, 0);
	mountain.yz *= imageScale;
	mountain.yz *= terrainParams.mountainFrequency;
	
	mountain.yz = terrainParams.mountainExponent * pow(mountain.x, terrainParams.mountainExponent - 1) * mountain.yz;
	mountain.x = pow(mountain.x, terrainParams.mountainExponent);
	
	mountain.yz = (1 - terrainParams.antiFlatFactor) * mountain.yz;
	mountain.x = mountain.x * (1 - terrainParams.antiFlatFactor) + terrainParams.antiFlatFactor;

	// Rivers
	vec3 river = perlin(pos * terrainParams.riverScale, 1);
	river.yz *= terrainParams.riverScale;
	river.yz *= imageScale;

	river *= 2;
	river.x -= 1;

	river.yz *= sign(river.x);
	river.x = abs(river.x);

	river.x = 1 - river.x;
	river.yz *= -1;
	
	river.yz = terrainParams.riverExponent * pow(river.x, terrainParams.riverExponent - 1) * river.yz;
	river.x = pow(river.x, terrainParams.riverExponent);

	river *= terrainParams.riverStrength;

	river.yz = river.yz * (mountain.x * terrainParams.waterEatingMountain + 1) + terrainParams.waterEatingMountain * mountain.yz * river.x;
	river.x *= (mountain.x * terrainParams.waterEatingMountain + 1);

	// Lakes
	vec3 lake = perlin(pos * terrainParams.lakeScale, 1);

	lake.yz *= dextreme(lake.x);
	lake.x = extreme(lake.x);
	
	lake.yz = terrainParams.lakeExponent * pow(lake.x, terrainParams.lakeExponent - 1) * lake.yz;
	lake.x = pow(lake.x, terrainParams.lakeExponent);

	lake.yz *= imageScale;
	lake.yz *= terrainParams.lakeScale;
	lake *= terrainParams.lakeStrength;
	lake.yz = lake.yz * (mountain.x * terrainParams.waterEatingMountain + 1) + terrainParams.waterEatingMountain * mountain.yz * lake.x;
	lake.x *= (mountain.x * terrainParams.waterEatingMountain + 1);

	vec3 terrainInfo = vec3(0, 0, 0);
	vec3 smoothTerrainInfo = vec3(0, 0, 0);

	float amplitude = terrainParams.initialAmplitude;
	float spread = 1;

	for (int i = 0; i < terrainParams.octaveCount; ++i) {
		vec2 samplePos = pos * spread;
		vec3 perlinData = perlin(samplePos, 0);

		terrainInfo.x += amplitude * perlinData.x;
		terrainInfo.yz += amplitude * perlinData.yz * spread;
		
		if (i < terrainParams.smoothOctaveCount) {
			smoothTerrainInfo.x += amplitude * perlinData.x;
			smoothTerrainInfo.yz += amplitude * perlinData.yz * spread;
		}

		amplitude *= terrainParams.amplitudeDecay;
		spread *= terrainParams.spreadFactor;
	}
	terrainInfo.yz *= imageScale;
	smoothTerrainInfo.yz *= imageScale;
	
	terrainInfo.yz = terrainInfo.x * mountain.yz + mountain.x * terrainInfo.yz;
	terrainInfo.x *= mountain.x;
	smoothTerrainInfo.yz = smoothTerrainInfo.x * mountain.yz + mountain.x * smoothTerrainInfo.yz;
	smoothTerrainInfo.x *= mountain.x;

	terrainInfo.x -= river.x;
	terrainInfo.yz -= river.yz;
	smoothTerrainInfo.x -= river.x;
	smoothTerrainInfo.yz -= river.yz;
	
	terrainInfo.x -= lake.x;
	terrainInfo.yz -= lake.yz;
	smoothTerrainInfo.x -= lake.x;
	smoothTerrainInfo.yz -= lake.yz;

	return terrainInfo;
}

vec3 getRiverNoise(vec2 pos, float spread, float amplitude, float exponent) {
	vec3 river = perlin(pos * spread, 1);
	river.yz *= spread;
	river.yz *= imageScale;

	river *= 2;
	river.x -= 1;

	river.yz *= river.x / sqrt(river.x * river.x + 0.01);
	river.x = sqrt(river.x * river.x + 0.01);

	river.x = 1 - river.x;
	river.yz *= -1;
	
	river.yz = exponent * pow(river.x, exponent - 1) * river.yz;
	river.x = pow(river.x, exponent);

	river *= amplitude;

	return river;
}

vec3 getMountainTerrainInfo(vec2 pos) {

	vec3 terrainInfo = vec3(0, 0, 0);

	float amplitude = terrainParams.riverStrength;
	float spread = terrainParams.riverScale;

	vec3 baseRiver = getRiverNoise(pos, spread, 1, terrainParams.riverExponent);
	terrainInfo += getRiverNoise(pos, spread * waterParams.freqMult, amplitude * waterParams.amplitudeMult, terrainParams.riverExponent) * terrainInfo.x;

	amplitude = terrainParams.initialAmplitude * 3;
	spread = 0.1;

	for (int i = 0; i < terrainParams.octaveCount; ++i) {
		vec2 samplePos = pos * spread;
		vec3 perlinData = perlin(samplePos, 0);
		
		terrainInfo.x += amplitude * perlinData.x;
		terrainInfo.yz += amplitude * perlinData.yz * spread * imageScale;

		amplitude *= terrainParams.amplitudeDecay;
		spread *= terrainParams.spreadFactor;
	}

	return terrainInfo;
}

vec4 createTerrainInfo(vec2 pos) {
	return vec4(getMountainTerrainInfo(pos), 0);
}

void main() {
	OutTerrainData = createTerrainInfo(latticePos);
}