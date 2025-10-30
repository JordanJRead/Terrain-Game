#ifndef TERRAIN_INFO_HEADER
#define TERRAIN_INFO_HEADER

#include "_headeruniformbuffers.glsl"
#include "_headermath.glsl"

vec4 getTerrainInfo(vec2 worldPos, bool smoothTerrain) {
	for (int i = 0; i < IMAGECOUNT; ++i) {
		vec2 sampleCoord = ((worldPos / artisticParams.terrainScale - imagePositions[i]) / imageScales[i]) + vec2(0.5);
		
		if (!(sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0)) {
			vec4 terrainInfo = texture(images[i], sampleCoord);
			if (smoothTerrain) {
				terrainInfo.y = unpackFloats(terrainInfo.y).y;
				terrainInfo.z = unpackFloats(terrainInfo.z).y;
			}
			else {
				terrainInfo.y = unpackFloats(terrainInfo.y).x;
				terrainInfo.z = unpackFloats(terrainInfo.z).x; // ?
			}
			terrainInfo.yz /= imageScales[i] * artisticParams.terrainScale;
			return terrainInfo;
		}
	}
	return vec4(0, 0, 0, 0);
}

#endif
