#version 430 core
#extension GL_ARB_shading_language_include : require

in VertOut {
	vec3 groundWorldPos;
	vec3 worldPos;
	float shellProgress;
} fragIn;

layout(location=0) out vec4 OutGroundWorldPosShellProgress;
layout(location=1) out vec4 OutWorldPosMountain;
layout(location=2) out vec4 OutNormalDoesTexelExist;

#include "_headermath.glsl"
#include "_headeruniformbuffers.glsl"
#include "_headerterraininfo.glsl"

// Per plane

void main() {
	vec2 flatWorldPos = fragIn.groundWorldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, false);
	vec4 smoothTerrainInfo = getTerrainInfo(flatWorldPos, true);
	float mountain = terrainInfo.a;
	mountain = extreme(mountain);
	mountain = pullup(mountain);
	mountain = pullup(mountain);
	mountain = extreme(mountain);

	OutWorldPosMountain = vec4(fragIn.worldPos, mountain);
	
	bool isShell = fragIn.shellProgress > 0;

	// Terrain
	float groundHeight = fragIn.groundWorldPos.y;
	vec3 normal       = normalize(vec3(-terrainInfo.y,       1, -terrainInfo.z));
	vec3 smoothNormal = normalize(vec3(-smoothTerrainInfo.y, 1, -smoothTerrainInfo.z));

	
	vec3 groundAlbedo = colours.dirtColour * (1 - mountain) + mountain * colours.mountainColour;

	// Which shell type are we?
	float actualSnowHeight = artisticParams.snowHeight + normToNegPos(perlin(flatWorldPos * artisticParams.snowLineNoiseScale, 0).x) * artisticParams.snowLineNoiseAmplitude;
	bool isSnow = actualSnowHeight < groundHeight && mountain > artisticParams.mountainSnowCutoff;
	bool isGrass = !isSnow;
	OutGroundWorldPosShellProgress = vec4(fragIn.groundWorldPos, fragIn.shellProgress);

	// Shell blade height
	float shellScale = isGrass ? artisticParams.grassNoiseScale : artisticParams.snowNoiseScale;
	vec2 shellCoord = flatWorldPos * shellScale;
	int shellGridX = getClosestInt(floor(shellCoord.x));
	int shellGridZ = getClosestInt(floor(shellCoord.y));
	float randomTexelHeight;
	if (!isShell)
		randomTexelHeight = 1; // Ground layer should automatically pass height check
	else {
		if (isGrass) {
				randomTexelHeight = randToFloat(rand(labelPoint(shellGridX, shellGridZ)));
		}
		else {
			randomTexelHeight = 1;
		}
	}
	
	// Terrain at center of texel
	vec2 shellWorldMiddlePos = vec2(shellGridX / shellScale, shellGridZ / shellScale);
	vec4 shellMiddleTerrainInfo = getTerrainInfo(shellWorldMiddlePos, false);
	vec3 shellNormal = normalize(vec3(-shellMiddleTerrainInfo.y, 1, -shellMiddleTerrainInfo.z));

	// Wetness
	float wetHeight = 0.4;
	float distAboveWater = (isShell ? shellMiddleTerrainInfo.x : fragIn.groundWorldPos.y) - waterParams.height;
	float wet =  1 - (distAboveWater / wetHeight);
	wet = clamp(wet, 0.0, 1.0);

	// Dot cutoff
	float currDotCutoff = isGrass ? artisticParams.grassDotCutoff : artisticParams.snowDotCutoff;
	if (isSnow) {
		float snowHeightNorm = (groundHeight - actualSnowHeight) / artisticParams.snowLineEase;
		snowHeightNorm = clamp(snowHeightNorm, 0, 1);

		currDotCutoff += (1 - currDotCutoff) * (1 - snowHeightNorm);
	}

	float currDot = dot(normal, vec3(0, 1, 0));
	float smoothDot = dot(smoothNormal, vec3(0, 1, 0));
	bool shallowEnough = smoothDot >= currDotCutoff;

	// Get smaller at harder dots
	randomTexelHeight *= ((currDot - currDotCutoff) / (1 - currDotCutoff));

	float shellCutoff = artisticParams.shellBaseCutoff * int(isGrass) + fragIn.shellProgress * (artisticParams.shellMaxCutoff - artisticParams.shellBaseCutoff);
	if (isGrass)
		shellCutoff += extreme(mountain); // Grass can't grow on mountains

	bool doesShellExist = shallowEnough && randomTexelHeight >= shellCutoff && wet == 0;
	if (isShell && !doesShellExist)
		discard;
	OutNormalDoesTexelExist = vec4(isGrass && isShell ? shellNormal : normal, doesShellExist);
}