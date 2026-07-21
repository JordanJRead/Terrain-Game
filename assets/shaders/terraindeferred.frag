#version 430 core

#include "_headeruniformbuffers.glsl"
#include "_headerterraininfo.glsl"
#include "_headershadows.glsl"
#include "_headervolumetrics.glsl"

in vec2 texCoord;
out vec4 FragColour;

uniform sampler2D GBuffer_TerrainGroundWorldPos;
uniform sampler2D GBuffer_TerrainWorldPos;
uniform sampler2D GBuffer_TerrainNormal;
uniform sampler2D GBuffer_ShellProgressMountainDoesTexelExist;

vec4 getTerrainAlbedoWet(vec3 groundWorldPos, float shellProgress, float mountain, bool doesShellExist) {
	vec2 flatWorldPos = groundWorldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, true); // TODO
	
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
	vec3 groundWorldPos = texture(GBuffer_TerrainGroundWorldPos, texCoord).xyz;
	vec3 worldPos = texture(GBuffer_TerrainWorldPos, texCoord).xyz;
	vec3 normal = normalize(texture(GBuffer_TerrainNormal, texCoord).xyz);
	if (normal == vec3(0, 0, 0)) {
		discard;
	}
	vec3 artData = texture(GBuffer_ShellProgressMountainDoesTexelExist, texCoord).xyz;
	float shellProgress = artData.x;
	float mountain = artData.y;
	bool doesTexelExist = artData.z > 0.5;

	vec3 cameraRayDir = vec3(
		//tan(perFrameInfo.fovX / 2) * (2 * texCoord.x - 1),
		//tan(perFrameInfo.fovY / 2) * (2 * texCoord.y - 1),
		perFrameInfo.tanHalfFOVX * (2 * texCoord.x - 1),
		perFrameInfo.tanHalfFOVY * (2 * texCoord.y - 1),
		-1
	);
	cameraRayDir = normalize(cameraRayDir);
	cameraRayDir = inverse(mat3(perFrameInfo.viewMatrix)) * cameraRayDir;

	vec4 albedoWet = getTerrainAlbedoWet(groundWorldPos, shellProgress, mountain, doesTexelExist); // here
	vec3 albedo = albedoWet.xyz;
	float wet = albedoWet.w;
			
	vec3 viewDir = normalize(perFrameInfo.cameraPos - groundWorldPos);
	vec3 halfWay = normalize(viewDir + perFrameInfo.dirToSun);
	bool isShell = shellProgress != 0;
	float spec = isShell ? 0 : pow(max(dot(normal, halfWay), 0), waterParams.specExp); // TODO spec
	spec *= wet * wet;

	vec3 sunColour = colours.sunColour * colours.sunBrightness;



	float sunShadow  = isPointInSunShadow(worldPos, normal, true);
	float moonShadow  = isPointInMoonShadow(worldPos, normal, true);
		
	vec3 sunColourHittingHere  = (1 - sunShadow)  * colours.sunColour * colours.sunBrightness  * exp(-(opticalDepth(worldPos,  perFrameInfo.dirToSun, true) * atmosphereInfo.rayleighScattering + opticalDepth(worldPos,  perFrameInfo.dirToSun, false) * atmosphereInfo.mieScattering));
	vec3 moonColourHittingHere = (1 - moonShadow) * colours.moonColour * colours.moonBrightness * exp(-(opticalDepth(worldPos, -perFrameInfo.dirToSun, true) * atmosphereInfo.rayleighScattering + opticalDepth(worldPos, -perFrameInfo.dirToSun, false) * atmosphereInfo.mieScattering));

	float sunDot = max(0, dot(normal, perFrameInfo.dirToSun));
	float moonDot = max(0, dot(normal, -perFrameInfo.dirToSun));

	float ambientDot = mix((moonDot + 1) / 2, (sunDot + 1) / 2, perFrameInfo.nightStrength);

	vec3 dayAmbient = vec3(1);
	vec3 nightAmbient = vec3(0.02);
	vec3 ambientLight = mix(nightAmbient, dayAmbient, perFrameInfo.nightStrength);

	vec3 colourOfObject = albedo * (sunDot * sunColourHittingHere + moonDot * moonColourHittingHere + ambientDot * ambientLight);

	FragColour = vec4(colourOfObject, 1);
}