#version 430 core

in vec2 texCoord;
out vec4 FragColor;

#include "_headeruniformbuffers.glsl"
#include "_headerterraininfo.glsl"
#include "_headershadows.glsl"

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
uniform sampler2D blueNoise;

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

vec3 getAtmosphereCenter() {
	return vec3(perFrameInfo.cameraPos.x, atmosphereInfo.centerY, perFrameInfo.cameraPos.z);
	//return vec3(0, 50, 0);
}

vec2 rayAtmosphereIntersection(vec3 pos, vec3 dir) {
	vec3 atmosphereCenter = getAtmosphereCenter();
	float a = dot(dir, dir);
	float b = 2 * (dot(dir, (pos - atmosphereCenter)));
	float c = dot((pos - atmosphereCenter), (pos - atmosphereCenter)) - pow(atmosphereInfo.maxRadius, 2);
	float disc = b * b - 4 * a * c;
	if (disc > 0) {
		vec2 ts = vec2((-b + sqrt(disc)) / (2 * a), (-b - sqrt(disc)) / (2 * a));
		ts = vec2(min(ts.x, ts.y), max(ts.x, ts.y));
		if (ts.x < 0)
			ts.x = 0;
		return ts;
	}
	return vec2(-1, -1);
}

bool isPointInAtmosphere(vec3 pos) {
	vec3 atmosphereCenter = getAtmosphereCenter();
	return length(pos - atmosphereCenter) <= atmosphereInfo.maxRadius;
}

float rayleighDensityAtPoint(vec3 pos) {
	vec3 atmosphereCenter = getAtmosphereCenter();
	float distFromCenter = length(atmosphereCenter - pos);
	if (distFromCenter < atmosphereInfo.minRadius) {
		return atmosphereInfo.rayleighDensityScale;
	}
	if (distFromCenter > atmosphereInfo.maxRadius) {
		return 0;
	}
	float norm = (distFromCenter - atmosphereInfo.minRadius) / (atmosphereInfo.maxRadius - atmosphereInfo.minRadius);
	return exp(-atmosphereInfo.rayleighDensityFalloff * norm) * (1 - norm) * atmosphereInfo.rayleighDensityScale;
}

float mieDensityAtPoint(vec3 pos) {
	vec3 atmosphereCenter = getAtmosphereCenter();
	float distFromCenter = length(atmosphereCenter - pos);
	if (distFromCenter < atmosphereInfo.minRadius) {
		return atmosphereInfo.mieDensityScale;
	}
	if (distFromCenter > atmosphereInfo.maxRadius) {
		return 0;
	}
	float norm = (distFromCenter - atmosphereInfo.minRadius) / (atmosphereInfo.maxRadius - atmosphereInfo.minRadius);
	return exp(-atmosphereInfo.mieDensityFalloff * norm) * (1 - norm) * atmosphereInfo.mieDensityScale;
}

float opticalDepth(vec3 pos, vec3 dir, bool isRayleigh) {
	vec2 ts = rayAtmosphereIntersection(pos, dir);
	if (ts.x == -1) {
		return 0;
	}
	
	int stepCount = atmosphereInfo.raySunStepCount;
	float totalDistance = ts.y - ts.x;
	float dx = totalDistance / stepCount;
	vec3 samplePos = pos;

	float depth = 0;
	for (int i = 0; i < stepCount; ++i) {
		float density = isRayleigh ? rayleighDensityAtPoint(samplePos) : mieDensityAtPoint(samplePos);
		depth += density * dx;
		samplePos += dir * dx;
	}
	return depth;
}

float phase(float cosTheta, float g) {
	return 1 / (4 * PI) * (1 - g * g) / pow(1 + g * g - 2 * g * cosTheta, 3/2);
}

vec3 lightReceived(vec3 rayPos, vec3 rayDir, bool isSky, vec3 worldPosOfVisibleObject, vec3 albedo, vec3 normal = vec3(0), bool doShadows = true) {
	vec3 colourOfObject = albedo;
	if (!isSky) {
		float sunShadow  = isPointInSunShadow(worldPosOfVisibleObject, normal, true);
		float moonShadow  = isPointInMoonShadow(worldPosOfVisibleObject, normal, true);
		
		vec3 sunColourHittingHere  = (1 - sunShadow)  * colours.sunColour  * exp(-(opticalDepth(worldPosOfVisibleObject,  perFrameInfo.dirToSun, true) * atmosphereInfo.rayleighScattering + opticalDepth(worldPosOfVisibleObject,  perFrameInfo.dirToSun, false) * atmosphereInfo.mieScattering));
		vec3 moonColourHittingHere = (1 - moonShadow) * colours.moonColour * exp(-(opticalDepth(worldPosOfVisibleObject, -perFrameInfo.dirToSun, true) * atmosphereInfo.rayleighScattering + opticalDepth(worldPosOfVisibleObject, -perFrameInfo.dirToSun, false) * atmosphereInfo.mieScattering));

		float sunDot = dot(normal, perFrameInfo.dirToSun);
		float moonDot = dot(normal, -perFrameInfo.dirToSun);
		sunDot = max(sunDot, 0);
		moonDot = max(moonDot, 0);

		float ambientDot = mix((moonDot + 1) / 2, (sunDot + 1) / 2, perFrameInfo.nightStrength);

		vec3 dayAmbient = vec3(1);
		vec3 nightAmbient = vec3(0.02);
		vec3 ambient = perFrameInfo.nightStrength * nightAmbient + (1 - perFrameInfo.nightStrength) * dayAmbient;

		colourOfObject = albedo * (sunDot * sunColourHittingHere + moonDot * moonColourHittingHere + ambientDot * ambient);
	}

	vec2 ts = rayAtmosphereIntersection(rayPos, rayDir);
	if (ts.x < 0 && ts.y < 0)
		return colourOfObject;
	if (!isSky)
		ts.y = min(ts.y, length(rayPos - worldPosOfVisibleObject));
	if (ts.x > ts.y)
		return colourOfObject;

	vec3 a = rayPos + rayDir * ts.x;
	vec3 b = rayPos + rayDir * ts.y;

	int stepCount = atmosphereInfo.rayAtmosphereStepCount;
	float totalDistance = ts.y - ts.x;
	float dx = totalDistance / stepCount;
	
	vec2 noiseSamplePos = gl_FragCoord.xy / textureSize(blueNoise, 0).xy;
	vec3 samplePos = a + rayDir * dx - texture(blueNoise, noiseSamplePos).r * atmosphereInfo.ditherStrength;

	float currentOpticalDepthRayleigh = 0;
	float currentOpticalDepthMie = 0;
	vec3 inScatteredLight = vec3(0, 0, 0);

	for (int n = 0; n < stepCount; ++n) {
		bool inSunShadow  = isPointInSunShadow(samplePos, normal) > 0.5;
		bool inMoonShadow  = isPointInMoonShadow(samplePos, normal) > 0.5;

		float rayleighDensity = rayleighDensityAtPoint(samplePos);
		float mieDensity = mieDensityAtPoint(samplePos);

		currentOpticalDepthRayleigh += rayleighDensity * dx;
		currentOpticalDepthMie += mieDensity * dx;

		// todo dont calculate if in shadow
		float toSunRayleighOpticalDepth  = opticalDepth(samplePos,  perFrameInfo.dirToSun, true);
		float toMoonRayleighOpticalDepth = opticalDepth(samplePos, -perFrameInfo.dirToSun, true);
		float toSunMieOpticalDepth        = opticalDepth(samplePos,  perFrameInfo.dirToSun, false);
		float toMoonMieOpticalDepth       = opticalDepth(samplePos, -perFrameInfo.dirToSun, false);
		
		float cosThetaSun  = dot(rayDir,  perFrameInfo.dirToSun);
		float cosThetaMoon = dot(rayDir, -perFrameInfo.dirToSun);

		// In-scattering from sun
		vec3 inScatteredFromSunRayleigh  = (inSunShadow ? 0 : 1)  * colours.sunColour  * rayleighDensity * phase(cosThetaSun,  atmosphereInfo.rayleighG) * atmosphereInfo.rayleighScattering * exp(-(currentOpticalDepthRayleigh + toSunRayleighOpticalDepth)  * atmosphereInfo.rayleighScattering);
		vec3 inScatteredFromMoonRayleigh = (inMoonShadow ? 0 : 1) * colours.moonColour  * rayleighDensity * phase(cosThetaMoon, atmosphereInfo.rayleighG) * atmosphereInfo.rayleighScattering * exp(-(currentOpticalDepthRayleigh + toMoonRayleighOpticalDepth) * atmosphereInfo.rayleighScattering);
		vec3 inScatteredFromSunMie       = (inSunShadow ? 0 : 1)  * colours.sunColour  * mieDensity * phase(cosThetaSun,  atmosphereInfo.mieG)      * atmosphereInfo.mieScattering      * exp(-(currentOpticalDepthMie      + toSunMieOpticalDepth)       * atmosphereInfo.mieScattering);
		vec3 inScatteredFromMoonMie      = (inMoonShadow ? 0 : 1) * colours.moonColour  * mieDensity  * phase(cosThetaMoon, atmosphereInfo.mieG)      * atmosphereInfo.mieScattering      * exp(-(currentOpticalDepthMie      + toMoonMieOpticalDepth)      * atmosphereInfo.mieScattering);

		inScatteredLight += (inScatteredFromSunRayleigh + inScatteredFromMoonRayleigh + inScatteredFromSunMie + inScatteredFromMoonMie) * atmosphereInfo.brightness * dx;

		samplePos += rayDir * dx;
	}
	if (isSky)
		return inScatteredLight + colourOfObject;

	return inScatteredLight + colourOfObject * exp(-(currentOpticalDepthRayleigh * atmosphereInfo.rayleighScattering + currentOpticalDepthMie * atmosphereInfo.mieScattering));
 }

 int getSplitIndex(vec3 dir) {
	return int((dir.y + 1) / 2 / (1.0 / STARYSPLITCOUNT));
}

bool isStarVisibleInSplit(vec3 dir, int i) {
	ivec2 indexInfo = starData.indexData[i];
	for (int starI = indexInfo.x; starI < indexInfo.x + indexInfo.y; ++starI) {
		vec4 star = starData.stars[starI];
		vec3 starDir = star.xyz;
		float starSize = star.w;

		if (dot(dir, starDir) > cos(radians(starSize))) {
			return true;
		}
	}
	return false;
}

 vec3 getStarColor(vec3 dir) {
	float theta = -perFrameInfo.dayTime * PI;
	vec3 rotatedDir = mat3(perFrameInfo.starRotationMatrix) * dir;

	if (dot(dir, perFrameInfo.dirToSun) > cos(radians(atmosphereInfo.sunSizeDeg))) {
		return colours.sunColour;
	}

	if (dot(dir, -perFrameInfo.dirToSun) > cos(radians(atmosphereInfo.sunSizeDeg))) {
		return colours.moonColour;
	}

	int ySplitIndex = clamp(getSplitIndex(rotatedDir), 0, STARYSPLITCOUNT - 1);

	if (isStarVisibleInSplit(rotatedDir, ySplitIndex))
		return colours.starColour;

	if (ySplitIndex != 0)
		ySplitIndex -= 1;
		
	if (isStarVisibleInSplit(rotatedDir, ySplitIndex))
		return colours.starColour;

	ySplitIndex++;
	if (ySplitIndex != STARYSPLITCOUNT - 1)
		ySplitIndex += 1;
		
	if (isStarVisibleInSplit(rotatedDir, ySplitIndex))
		return colours.starColour;
	
	return vec3(0);
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

	vec3 cameraRayDir = vec3(
		//tan(perFrameInfo.fovX / 2) * (2 * texCoord.x - 1),
		//tan(perFrameInfo.fovY / 2) * (2 * texCoord.y - 1),
		perFrameInfo.tanHalfFOVX * (2 * texCoord.x - 1),
		perFrameInfo.tanHalfFOVY * (2 * texCoord.y - 1),
		-1
	);
	cameraRayDir = normalize(cameraRayDir);
	cameraRayDir = inverse(mat3(perFrameInfo.viewMatrix)) * cameraRayDir;
	
	vec3 starColour = getStarColor(cameraRayDir);

	vec3 skyColour = lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, vec3(0), starColour);

	if (isSky) {
		FragColor = vec4(skyColour, 1);
	}
	else {
		if (isWater) {
			vec3 waterAlbedo = getWaterAlbedo(worldPos);

			// Specular
			vec3 viewDir = normalize(perFrameInfo.cameraPos - worldPos);
			vec3 halfWay = normalize(viewDir + perFrameInfo.dirToSun);
			float spec = pow(max(dot(normal, halfWay), 0), waterParams.specExp);
			
			vec3 objectColour = waterAlbedo;

			
			// Reflections
			float fresnel = pow(1 - dot(viewDir, normal), 3.0);
			vec3 reflectDir = normalize(reflect(-viewDir, normal));
			vec3 reflectStarColor = getStarColor(reflectDir);
			vec3 reflectColour = lightReceived(worldPos, reflectDir, true, vec3(0), reflectStarColor, vec3(0), false);
			fresnel = clamp(fresnel, 0.0, 1.0);
			objectColour = fresnel * reflectColour + (1 - fresnel) * objectColour;

			// Fog
			float distFromCamera = length(worldPos - perFrameInfo.cameraPos);
			float fogStart = artisticParams.maxViewDistance - artisticParams.fogEncroach;
			float fogStrength;

			if (distFromCamera < fogStart)
				fogStrength = 0;
			else if (distFromCamera > artisticParams.maxViewDistance)
				fogStrength = 1;
			else
				fogStrength = (distFromCamera - fogStart) / artisticParams.fogEncroach;

			if (fogStrength == 0)
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, worldPos, objectColour, normal), 1);
			else if (fogStrength == 1)
				FragColor = vec4(skyColour, 1);
			else
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, worldPos, objectColour, normal) * (1 - fogStrength) + skyColour * fogStrength, 1);
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
			
			vec3 objectColour = terrainAlbedo;

			// Fog
			float distFromCamera = length(worldPos - perFrameInfo.cameraPos);
			float fogStart = artisticParams.maxViewDistance - artisticParams.fogEncroach;
			float fogStrength;

			if (distFromCamera < fogStart)
				fogStrength = 0;
			else if (distFromCamera > artisticParams.maxViewDistance)
				fogStrength = 1;
			else
				fogStrength = quinticInterpolationF((distFromCamera - fogStart) / artisticParams.fogEncroach);

			if (fogStrength == 0)
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, worldPos, objectColour, normal), 1);
			else if (fogStrength == 1)
				FragColor = vec4(skyColour, 1);
			else
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, worldPos, objectColour, normal) * (1 - fogStrength) + skyColour * fogStrength, 1);
		}
	}
}