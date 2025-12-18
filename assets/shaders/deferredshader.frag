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

vec3 getAtmosphereCenter() {
	return vec3(perFrameInfo.cameraPos.x, atmosphereInfo.centerY, perFrameInfo.cameraPos.z);
}

vec2 rayAtmosphereIntersection(vec3 pos, vec3 dir) {
	vec3 atmosphereCenter = getAtmosphereCenter();
	float a = dot(dir, dir);
	float b = 2 * (dot(dir, (pos - atmosphereCenter)));
	float c = dot((pos - atmosphereCenter), (pos - atmosphereCenter)) - pow(atmosphereInfo.maxRadius, 2);
	float disc = b * b - 4 * a * c;
	if (disc > 0) {
		return vec2((-b + sqrt(disc)) / (2 * a), (-b - sqrt(disc)) / (2 * a));
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

vec3 transmittanceFromSunToPoint(vec3 pos) {
	vec2 ts = rayAtmosphereIntersection(pos, perFrameInfo.dirToSun);
	if (ts.x < 0 && ts.y < 0)
		return vec3(1, 1 , 1);
	float t0 = min(ts.x, ts.y);
	float t1 = max(ts.x, ts.y);
	t0 = max(t0, 0);
	float dist = max(ts.x, ts.y);
	if (dist < 0) {
		return vec3(1, 1, 1);
	}

	int stepCount = 10;
	float dx = dist / stepCount;
	vec3 transmittance = vec3(1, 1, 1);
	vec3 samplePos = pos + perFrameInfo.dirToSun * dx;
	for (int n = 0; n < stepCount; ++n) {
		
		float rayleighDensity = rayleighDensityAtPoint(samplePos);
		float mieDensity = mieDensityAtPoint(samplePos);

		transmittance *= exp(-dx * (rayleighDensity * atmosphereInfo.rayleighScattering + mieDensity * atmosphereInfo.mieScattering));

		samplePos += perFrameInfo.dirToSun * dx;
	}
	return transmittance;
}

float phase(float cosTheta, float g) {
	return 1 / (4 * PI) * (1 - g * g) / pow(1 + g * g - 2 * g * cosTheta, 3/2);
}

vec3 lightReceived(vec3 rayPos, vec3 rayDir, bool isSky, bool isSun, vec3 worldPosOfVisibleObject, vec3 albedo) {
	vec2 intersectionTs = rayAtmosphereIntersection(rayPos, rayDir);
	if (intersectionTs.x < 0 && intersectionTs.y < 0)
		return albedo * transmittanceFromSunToPoint(worldPosOfVisibleObject);
	float t0;
	float t1;
	t0 = min(intersectionTs.x, intersectionTs.y);
	t0 = max(t0, 0);
	t1 = max(intersectionTs.x, intersectionTs.y);
	if (!isSky)
		t1 = min(t1, length(rayPos - worldPosOfVisibleObject));

	vec3 a = rayPos + rayDir * t0;
	vec3 b = rayPos + rayDir * t1;

	int stepCount = 10;
	float totalDistance = t1 - t0;
	float dx = totalDistance / stepCount;

	vec3 samplePos = a + rayDir * dx;
	vec3 transmittance = vec3(1, 1, 1);
	vec3 inScatteredLight = vec3(0, 0, 0);
	for (int n = 0; n < stepCount; ++n) {
		
		float rayleighDensity = rayleighDensityAtPoint(samplePos);
		float mieDensity = mieDensityAtPoint(samplePos);
		transmittance *= exp(-dx * (rayleighDensity * atmosphereInfo.rayleighScattering + mieDensity * atmosphereInfo.mieScattering));

		// In-scattering
		vec3 sunlightHittingHere = transmittanceFromSunToPoint(samplePos) * colours.sunColour;
		float cosTheta = dot(rayDir, perFrameInfo.dirToSun);
		inScatteredLight += 50 * sunlightHittingHere * (rayleighDensity * atmosphereInfo.rayleighScattering * phase(cosTheta, atmosphereInfo.rayleighG) + mieDensity * atmosphereInfo.mieScattering * phase(cosTheta, atmosphereInfo.mieG)) * transmittance * dx;
		samplePos += rayDir * dx;
	}
	if (isSky && !isSun) {
		return inScatteredLight;
	}
	return inScatteredLight + albedo * transmittanceFromSunToPoint(worldPosOfVisibleObject) * transmittance;
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
		tan(perFrameInfo.fovX / 2) * (2 * texCoord.x - 1),
		tan(perFrameInfo.fovY / 2) * (2 * texCoord.y - 1),
		-1
	);
	cameraRayDir = normalize(cameraRayDir);
	cameraRayDir = inverse(mat3(perFrameInfo.viewMatrix)) * cameraRayDir;

	if (isSky) {
		if (dot(cameraRayDir, perFrameInfo.dirToSun) > 0.999) {
			FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, true, vec3(0, 0, 0), colours.sunColour * 100), 1);
		}
		else {
			FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, false, vec3(0, 0, 0), vec3(0, 0, 0)), 1);
		}
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
			vec3 ambientColour = 0.03 * colours.sunColour * waterAlbedo;
			vec3 objectColour = diffuseColour + specularColour + ambientColour;

			
			// Reflections
			float fresnel = pow(1 - dot(viewDir, normal), 3.0);
			vec3 reflectDir = normalize(reflect(-viewDir, normal));
			vec3 reflectColour = lightReceived(worldPos, reflectDir, true, false, vec3(0), vec3(0));
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
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, false, worldPos, objectColour), 1);
			else if (fogStrength == 1)
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, false, vec3(0), vec3(0)), 1);
			else
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, false, worldPos, objectColour) * (1 - fogStrength) + lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, false, vec3(0), vec3(0)) * fogStrength, 1);
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
			vec3 ambientColour = 0.03 * colours.sunColour * terrainAlbedo;
			vec3 objectColour = diffuseColour + specularColour + ambientColour;

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
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, false, worldPos, objectColour), 1);
			else if (fogStrength == 1)
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, false, vec3(0), vec3(0)), 1);
			else
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, false, worldPos, objectColour) * (1 - fogStrength) + lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, false, vec3(0), vec3(0)) * fogStrength, 1);
		}
	}
}