#version 430 core

in vec2 texCoord;
out vec4 FragColor;

#include "_headeruniformbuffers.glsl"
#include "_headerterraininfo.glsl"

vec3 getWaterAlbedo(vec3 worldPos) {
	// Water / terrain info
	vec2 flatWorldPos = worldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, false);
	
	// Albedo
	vec3 albedo = colours.waterColour;
	float nearTerrainFactor = 1 - (abs(worldPos.y - terrainInfo.x)) / artisticParams.seafoamStrength;
	nearTerrainFactor = clamp(nearTerrainFactor, 0.0, 1.0);
	if (nearTerrainFactor > 0) {
		float whiteStrength = nearTerrainFactor;
		albedo = vec3(0.7) * whiteStrength + albedo * (1 - whiteStrength);
	}
	return albedo;
}

uniform sampler2D GBuffer_GroundWorldPosShellProgress;
uniform sampler2D GBuffer_WorldPosMountain;
uniform sampler2D GBuffer_NormalDoesTexelExist;

vec4 getTerrainAlbedoWet(vec3 groundWorldPos, float shellProgress, float mountain, bool doesShellExist) {
	vec2 flatWorldPos = groundWorldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, true);
	
	bool isShell = shellProgress != 0;

	// Terrain
	float groundHeight = groundWorldPos.y;
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	vec4 smoothTerrainInfo = getTerrainInfo(flatWorldPos, true);
	vec3 smoothNormal = normalize(vec3(-smoothTerrainInfo.y, 1, -smoothTerrainInfo.z));
	
	vec3 groundAlbedo = colours.dirtColour * (1 - mountain) + mountain * colours.mountainColour;

	// Which shell type are we?
	float actualSnowHeight = artisticParams.snowHeight + normToNegPos(perlin(flatWorldPos * artisticParams.snowLineNoiseScale, 0).x) * artisticParams.snowLineNoiseAmplitude;
	bool isSnow = actualSnowHeight < groundHeight && mountain > artisticParams.mountainSnowCutoff;
	bool isGrass = !isSnow;
	float grassperlin = perlin(flatWorldPos * 0.1, 0).x;
	vec3 shellAlbedo = isSnow ? colours.snowColour : (colours.grassColour1 * grassperlin + colours.grassColour2 * (1 - grassperlin));
	shellAlbedo = shellAlbedo - shellAlbedo * (1 - shellProgress) * artisticParams.shellAmbientOcclusion;
	
	// Terrain at center of texel
	float shellScale = isGrass ? artisticParams.grassNoiseScale : artisticParams.snowNoiseScale;
	vec2 shellCoord = flatWorldPos * shellScale;
	int shellGridX = getClosestInt(floor(shellCoord.x));
	int shellGridZ = getClosestInt(floor(shellCoord.y));
	vec2 shellWorldMiddlePos = vec2(shellGridX / shellScale, shellGridZ / shellScale);
	vec4 shellMiddleTerrainInfo = getTerrainInfo(shellWorldMiddlePos, false);

	// Wetness
	float wetHeight = 0.4;
	float distAboveWater = (isShell ? shellMiddleTerrainInfo.x : groundWorldPos.y) - waterParams.height;
	float wet =  1 - (distAboveWater / wetHeight);
	wet = clamp(wet, 0.0, 1.0);

	// Albedo
	vec3 albedo;
	if (!isShell) {
		if (wet == 0)
			albedo = doesShellExist ? shellAlbedo : groundAlbedo;
		else {
			albedo = groundAlbedo - wet * groundAlbedo * 0.4;
		}
	}
	else {
		albedo = shellAlbedo;
	}

	return vec4(albedo, wet);
}

void main() {
	vec4 groundWorldPosShellProgress = texture(GBuffer_GroundWorldPosShellProgress, texCoord);
	vec4 worldPosMountain = texture(GBuffer_WorldPosMountain, texCoord);
	vec4 normalDoesTexelExist = texture(GBuffer_NormalDoesTexelExist, texCoord);

	vec3 normal = normalDoesTexelExist.xyz;
	vec3 worldPos = worldPosMountain.xyz;
	vec3 groundWorldPos = groundWorldPosShellProgress.xyz;
	bool doesTexelExist = bool(normalDoesTexelExist.w);
	float mountain = worldPosMountain.w;
	float shellProgress = groundWorldPosShellProgress.w;

	bool isWater = groundWorldPosShellProgress.w == -2;
	bool isSky = groundWorldPosShellProgress.w == -3;

	if (isSky) {
		FragColor = vec4(0.2, 0.2, 1, 1);
	}
	else {
		if (isWater) {
			vec3 waterAlbedo = getWaterAlbedo(worldPos);

			// Specular
			vec3 viewDir = normalize(perFrameInfo.cameraPos - worldPos);
			vec3 halfWay = normalize(viewDir + perFrameInfo.dirToSun);
			float spec = pow(max(dot(normal, halfWay), 0), waterParams.specExp);
			
			vec3 diffuseColour = max(dot(normal, perFrameInfo.dirToSun), 0) * colours.sunColour * waterAlbedo;
			vec3 specularColour = spec * colours.sunColour;
			vec3 ambientColour = 0.05 * colours.sunColour * waterAlbedo;

			FragColor = vec4(diffuseColour + specularColour + ambientColour, 1);
		}
		else {
			vec4 terrainAlbedoWet = getTerrainAlbedoWet(groundWorldPosShellProgress.xyz, groundWorldPosShellProgress.w, worldPosMountain.w, bool(normalDoesTexelExist.w));
			vec3 terrainAlbedo = terrainAlbedoWet.xyz;
			float wet = terrainAlbedoWet.w;
			
			vec3 viewDir = normalize(perFrameInfo.cameraPos - groundWorldPos);
			vec3 halfWay = normalize(viewDir + perFrameInfo.dirToSun);
			bool isShell = shellProgress != 0;
			float spec = isShell ? 0 : pow(max(dot(normal, halfWay), 0), waterParams.specExp);
			spec *= wet * wet;
			
			vec3 diffuseColour = max(dot(normal, perFrameInfo.dirToSun), 0) * colours.sunColour * terrainAlbedo;
			vec3 specularColour = spec * colours.sunColour;
			vec3 ambientColour = 0.05 * colours.sunColour * terrainAlbedo;

			FragColor = vec4(diffuseColour + specularColour + ambientColour, 1);
		}
		vec3 normal = normalDoesTexelExist.xyz;
	}
}