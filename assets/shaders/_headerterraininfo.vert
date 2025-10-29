#ifndef TERRAIN_INFO_HEADER
#define TERRAIN_INFO_HEADER

#include "_headeruniformbuffers.vert"
#include "_headermath.vert"

float packFloats(vec2 v) {
	return uintBitsToFloat(packHalf2x16(v));
}

vec2 unpackFloats(float v) {
	return unpackHalf2x16(floatBitsToUint(v));
}

vec4 getTerrainInfo(vec2 pos) {
	vec3 mountain = perlin(pos * mountainFrequency, 0);
	mountain.yz *= scale;
	mountain.yz *= mountainFrequency;
	
	mountain.yz = mountainExponent * pow(mountain.x, mountainExponent - 1) * mountain.yz;
	mountain.x = pow(mountain.x, mountainExponent);
	
	mountain.yz = (1 - antiFlatFactor) * mountain.yz;
	mountain.x = mountain.x * (1 - antiFlatFactor) + antiFlatFactor;

	// Rivers
	vec3 river = perlin(pos * riverScale, 1);

	river *= 2;
	river.x -= 1;
	river.yz *= sign(river.x);
	river.x = abs(river.x);
	river.x = 1 - river.x;
	river.yz *= -1;
	
	river.yz = riverExponent * pow(river.x, riverExponent - 1) * river.yz;
	river.x = pow(river.x, riverExponent);

	river.yz *= scale;
	river.yz *= riverScale;
	river *= riverStrength;
	river.yz = river.yz * (mountain.x * 5 + 1) + 5 * mountain.yz * river.x;
	river.x *= (mountain.x * waterEatingMountain + 1);

	// Lakes
	vec3 lake = perlin(pos * lakeScale, 1);

	lake.yz *= dextreme(lake.x);
	lake.x = extreme(lake.x);
	
	lake.yz = lakeExponent * pow(lake.x, lakeExponent - 1) * lake.yz;
	lake.x = pow(lake.x, lakeExponent);

	lake.yz *= scale;
	lake.yz *= lakeScale;
	lake *= lakeStrength;
	lake.yz = lake.yz * (mountain.x * 5 + 1) + 5 * mountain.yz * lake.x;
	lake.x *= (mountain.x * waterEatingMountain + 1);

	vec3 terrainInfo = vec3(0, 0, 0);
	vec3 smoothTerrainInfo = vec3(0, 0, 0);

	float amplitude = initialAmplitude;
	float spread = 1;

	for (int i = 0; i < octaveCount; ++i) {
		vec2 samplePos = pos * spread;
		vec3 perlinData = perlin(samplePos, 0);

		terrainInfo.x += amplitude * perlinData.x;
		terrainInfo.yz += amplitude * perlinData.yz * spread;
		
		if (i < smoothOctaveCount) {
			smoothTerrainInfo.x += amplitude * perlinData.x;
			smoothTerrainInfo.yz += amplitude * perlinData.yz * spread;
		}

		amplitude *= amplitudeDecay;
		spread *= spreadFactor;
	}
	terrainInfo.yz *= scale;
	smoothTerrainInfo.yz *= scale;
	
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

	vec4 combinedTerrainInfo = vec4(terrainInfo.x, 0, 0, mountain);
	combinedTerrainInfo.y = packFloats(vec2(terrainInfo.y, smoothTerrainInfo.y));
	combinedTerrainInfo.z = packFloats(vec2(terrainInfo.z, smoothTerrainInfo.z));

	return combinedTerrainInfo;
}

#endif