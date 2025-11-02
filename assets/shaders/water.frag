#version 430 core

in vec3 worldPos3;
in vec3 viewPos;
out vec4 FragColour;

uniform samplerCube skybox;

uniform vec3 planePos;

#include "_headeruniformbuffers.glsl";
#include "_headermath.glsl";
#include "_headerterraininfo.glsl";

vec3 getWaterHeight(vec2 pos) {
	vec3 waterInfo = vec3(0, 0, 0);

	float amplitude = waterParams.initialAmplitude;
	float freq = waterParams.initialFreq;
	float speed = waterParams.initialSpeed;

	float amplitudeSum = 0;

	for (int i = 0; i < waterParams.waveCount; ++i) {
		amplitudeSum += amplitude;
		float randNum = randToFloat(rand(i));
		vec2 waterDir = randUnitVector(randNum);
		//waterInfo.x += amplitude * sin(dot(waterDir, pos) * freq + perFrameInfo.time * speed);
		waterInfo.x += amplitude * (exp(sin(dot(waterDir, pos) * freq + perFrameInfo.time * speed)) - 1.4);
		waterInfo.yz += amplitude * exp(sin(dot(waterDir, pos) * freq + perFrameInfo.time * speed)) * cos(dot(waterDir, pos) * freq + perFrameInfo.time * speed) * freq * waterDir;

		amplitude *= waterParams.amplitudeMult;
		freq *= waterParams.freqMult;
		speed *= waterParams.speedMult;
	}
	return waterInfo / amplitudeSum * waterParams.initialAmplitude;
}

void main() {
	// Water
	vec2 flatWorldPos = worldPos3.xz;
	vec3 waterInfo = getWaterHeight(flatWorldPos);
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, false);

	// Lighting
	vec3 normal = normalize(vec3(-waterInfo.y, 1, -waterInfo.z));
	float diffuse = max(0, dot(perFrameInfo.dirToSun, normal));

	vec3 viewDir = normalize(perFrameInfo.cameraPos - worldPos3);
	vec3 halfWay = normalize(viewDir + perFrameInfo.dirToSun);
	float spec = pow(max(dot(normal, halfWay), 0), waterParams.specExp);

	float ambient = 0.2;


	vec3 albedo = colours.waterColour;
	float nearTerrainFactor = 1 - (abs(waterInfo.x + planePos.y - terrainInfo.x)) / artisticParams.seafoamStrength;
	nearTerrainFactor = clamp(nearTerrainFactor, 0.0, 1.0);
	if (nearTerrainFactor > 0) {
		float whiteStrength = nearTerrainFactor;
		//float whiteStrength = perlin(flatWorldPos, 0).x;
		//whiteStrength = easeInOutQuint(whiteStrength) * nearTerrainFactor;
		albedo = vec3(0.7) * whiteStrength + albedo * (1 - whiteStrength);
	}

	float distFromCamera = length(viewPos);
	float fogStart = artisticParams.maxViewDistance - artisticParams.fogEncroach;
	float fogStrength;

	if (distFromCamera < fogStart)
		fogStrength = 0;
	else if (distFromCamera > artisticParams.maxViewDistance)
		fogStrength = 1;
	else
		fogStrength = (distFromCamera - fogStart) / artisticParams.fogEncroach;
		
	vec3 skyboxSample = worldPos3 - perFrameInfo.cameraPos;
	vec3 litAlbedo = (diffuse + ambient) * albedo;
	
	float fresnel = pow(1 - dot(viewDir, normal), 3.0);
	vec3 reflectDir = normalize(reflect(-viewDir, normal));
	vec3 reflectColour = texture(skybox, reflectDir).xyz;

	fresnel = clamp(fresnel, 0.0, 1.0);
	litAlbedo = fresnel * reflectColour + (1 - fresnel) * litAlbedo + spec * colours.sunColour;

	vec3 finalColour = (1 - fogStrength) * litAlbedo + fogStrength * texture(skybox, skyboxSample).xyz;

	FragColour = vec4(finalColour, 1);
}