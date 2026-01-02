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
	//return vec3(0, atmosphereInfo.centerY + 50, 0);
}

// returns a new ray position and a ray length through the atmosphere / to the visible object
// the ray position with be the original position if the ray is inside the sphere
vec4 rayAtmosphere(vec3 pos, vec3 dir, bool isObject, vec3 posOfObject) {
	vec3 atmosphereCenter = getAtmosphereCenter();
	float a = dot(dir, dir);
	float b = 2 * (dot(dir, (pos - atmosphereCenter)));
	float c = dot((pos - atmosphereCenter), (pos - atmosphereCenter)) - pow(atmosphereInfo.maxRadius, 2);
	float disc = b * b - 4 * a * c;
	if (disc <= 0)
		return vec4(pos, 0);

	float objectDist = length(pos - posOfObject);

	vec2 ts = vec2((-b + sqrt(disc)) / (2 * a), (-b - sqrt(disc)) / (2 * a));
	float entranceDist = min(ts.x, ts.y);
	float exitDist = max(ts.x, ts.y);
	if (isObject)
		exitDist = min(exitDist, objectDist);

	if (exitDist <= 0)
		return vec4(pos, 0);

	if (entranceDist <= 0)
		return vec4(pos, exitDist);

	return vec4(pos + dir * entranceDist, exitDist - entranceDist);
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

float getOpticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLength) {
	vec3 samplePos = rayOrigin;
	int sampleCount = 20;
	float dx = rayLength / (sampleCount - 1);

	float opticalDepth = 0;

	for (int i = 0; i < sampleCount; ++i) {
		opticalDepth += rayleighDensityAtPoint(samplePos) * dx;
		samplePos += rayDir * dx;
	}

	return opticalDepth;
}

float phase(float cosTheta, float g) {
	return 1 / (4 * PI) * (1 - g * g) / pow(1 + g * g - 2 * g * cosTheta, 3/2);
}

vec3 lightReceived(vec3 rayPos, vec3 rayDir, bool isObject, vec3 posOfObject, vec3 albedo, vec3 normal = vec3(0)) {
	vec4 rayAtmosphereInfo = rayAtmosphere(rayPos, rayDir, isObject, posOfObject);
	vec3 samplePos = rayAtmosphereInfo.xyz;
	float dist = rayAtmosphereInfo.w;

	int stepCount = atmosphereInfo.rayAtmosphereStepCount;
	float dx = dist / (stepCount - 1);

	vec3 inScatteredLight = vec3(0);
	float cameraOpticalDepth = 0;

	for (int n = 0; n < stepCount; ++n) {
		
		float rayleighDensity = rayleighDensityAtPoint(samplePos);
		cameraOpticalDepth += rayleighDensity * dx;

		float rayToSunDist = rayAtmosphere(samplePos, perFrameInfo.dirToSun, false, vec3(0)).w;
		float opticalDepthToSun = getOpticalDepth(samplePos, perFrameInfo.dirToSun, rayToSunDist);
		
		inScatteredLight += rayleighDensity * exp(-(opticalDepthToSun + cameraOpticalDepth) * atmosphereInfo.rayleighScattering);

		//vec3 sunlightHittingHere = transmittanceFromSunToPoint(samplePos) * colours.sunColour;
		//float cosTheta = dot(rayDir, perFrameInfo.dirToSun);
		samplePos += rayDir * dx;
	}
	inScatteredLight *= atmosphereInfo.rayleighScattering * dx * atmosphereInfo.brightness;

	if (!isObject) {
		return inScatteredLight + albedo * exp(-cameraOpticalDepth * atmosphereInfo.rayleighScattering);
	}
	float objectToSunDist = rayAtmosphere(posOfObject, perFrameInfo.dirToSun, false, vec3(0)).w;
	float opticalDepthToSun = getOpticalDepth(posOfObject, perFrameInfo.dirToSun, objectToSunDist);
	return inScatteredLight + albedo * exp(-(opticalDepthToSun + cameraOpticalDepth) * atmosphereInfo.rayleighScattering);
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

	vec3 starColour = dot(cameraRayDir, perFrameInfo.dirToSun) > 0.999 ? colours.sunColour : vec3(0);

	if (isSky) {
		FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, vec3(0), starColour), 1);
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
			vec3 reflectColour = lightReceived(worldPos, reflectDir, false, vec3(0), starColour);
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
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, worldPos, objectColour, normal), 1);
			else if (fogStrength == 1)
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, vec3(0), vec3(0)), 1); // sun colour?
			else
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, worldPos, objectColour, normal), 1) * (1 - fogStrength) + vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, vec3(0), starColour), 1);
		}
		else { // terrain
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
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, worldPos, objectColour, normal), 1);
			else if (fogStrength == 1)
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, vec3(0), starColour), 1);
			else
				FragColor = vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, true, worldPos, objectColour, normal), 1) * (1 - fogStrength) + vec4(lightReceived(perFrameInfo.cameraPos, cameraRayDir, false, vec3(0), starColour), 1);
			}
	}
}