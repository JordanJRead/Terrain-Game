#version 430 core
#extension GL_ARB_shading_language_include : require
#define PI 3.141592653589793238462
#define IMAGECOUNT 4

in vec3 viewPos;
in vec3 groundWorldPos;
in vec3 shellWorldPos;
out vec4 FragColor;

// Per app probably
uniform sampler2D images[IMAGECOUNT];
uniform samplerCube skybox;
uniform vec3 cameraPos;
uniform vec3 dirToLight;

// Per whenever they get changed
uniform float imageScales[IMAGECOUNT];
uniform vec2 imagePositions[IMAGECOUNT];
#include "_headermath.glsl" //    '/' is required but can't be used for intellisense?
#include "_headeruniformbuffers.glsl"
#include "_headerterraininfo.glsl"

// Per plane
in flat int shellIndex;

uniform float waterHeight;

void main() {
	vec2 flatWorldPos = groundWorldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, true);

	// Cool stuff
	//float myperlin = perlin(flatWorldPos / vec2(cos(atan(terrainInfo.y)), cos(atan(terrainInfo.z))), 0).x;
	//FragColor = vec4(myperlin.xxx, 1);
	//return;
	
	bool isShell = shellIndex >= 0;

	// Terrain
	float groundHeight = groundWorldPos.y;
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	vec4 smoothTerrainInfo = getTerrainInfo(flatWorldPos, true);
	vec3 smoothNormal = normalize(vec3(-smoothTerrainInfo.y, 1, -smoothTerrainInfo.z));

	float mountain = terrainInfo.a;
	mountain = extreme(mountain);
	mountain = pullup(mountain);
	mountain = pullup(mountain);
	mountain = extreme(mountain);
	
	vec3 groundAlbedo = colours.dirtColour * (1 - mountain) + mountain * colours.mountainColour;

	// Which shell type are we?
	float actualSnowHeight = artisticParams.snowHeight + normToNegPos(perlin(flatWorldPos * artisticParams.snowLineNoiseScale, 0).x) * artisticParams.snowLineNoiseAmplitude;
	bool isSnow = actualSnowHeight < groundHeight && mountain > artisticParams.mountainSnowCutoff;
	bool isGrass = !isSnow;
	float grassperlin = perlin(flatWorldPos * 0.1, 0).x;
	vec3 shellAlbedo = isSnow ? colours.snowColour : (colours.grassColour1 * grassperlin + colours.grassColour2 * (1 - grassperlin));
	float shellProgress = float(shellIndex + 1) / artisticParams.shellCount;
	shellAlbedo = shellAlbedo - shellAlbedo * (1 - shellProgress) * artisticParams.shellAmbientOcclusion;

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
	float distAboveWater = (isShell ? shellMiddleTerrainInfo.x : groundWorldPos.y) - waterHeight;
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
	bool shallowEnough = currDot >= currDotCutoff;

	// Get smaller at harder dots
	randomTexelHeight *= ((currDot - currDotCutoff) / (1 - currDotCutoff));

	float shellCutoff = artisticParams.shellBaseCutoff * int(isGrass) + shellProgress * (artisticParams.shellMaxCutoff - artisticParams.shellBaseCutoff);
	if (isGrass)
		shellCutoff += extreme(mountain); // Grass can't grow on mountains

	bool doesShellExist = shallowEnough && randomTexelHeight >= shellCutoff && wet == 0;

	// Albedo
	vec3 albedo;
	// Ground
	if (!isShell) {
		if (wet == 0)
			albedo = doesShellExist ? shellAlbedo : groundAlbedo;
		else {
			albedo = groundAlbedo - wet * groundAlbedo * 0.4;
		}
	}

	// Shell
	else {
		if (!doesShellExist)
			discard;
		albedo = shellAlbedo;
	}


	// Fog
	float distFromCamera = length(viewPos);
	float fogStart = artisticParams.maxViewDistance - artisticParams.fogEncroach;
	float fogStrength;

	if (distFromCamera < fogStart)
		fogStrength = 0;
	else if (distFromCamera > artisticParams.maxViewDistance)
		fogStrength = 1;
	else
		fogStrength = (distFromCamera - fogStart) / artisticParams.fogEncroach;

	// Lighting
	vec3 currNormal = isGrass && isShell ? shellNormal : normal;
	if (isSnow)
		currNormal = smoothNormal;
	float diffuse = max(0, dot(dirToLight, currNormal));
	float ambient = 0.03;
	vec3 viewDir = normalize(cameraPos - groundWorldPos);
	vec3 halfWay = normalize(viewDir + dirToLight);
	float spec = isShell ? 0 : pow(max(dot(normal, halfWay), 0), waterParams.specExp);
	spec *= wet * wet;

	vec3 litAlbedo = (diffuse + ambient) * albedo + spec * colours.sunColour;
	vec3 skyboxSample = shellWorldPos - cameraPos;
	vec3 finalColor = (1 - fogStrength) * litAlbedo + fogStrength * texture(skybox, skyboxSample).xyz;
	FragColor = vec4(finalColor, 1);
}