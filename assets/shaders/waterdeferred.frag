#version 430 core

#include "_headeruniformbuffers.glsl"

in vec2 texCoord;
out vec4 FragColour;

uniform sampler2D GBuffer_TerrainGroundWorldPos;
uniform sampler2D GBuffer_TerrainWorldPos;
uniform sampler2D GBuffer_TerrainNormal;
uniform sampler2D GBuffer_ShellProgressMountainDoesTexelExist;
uniform sampler2D GBuffer_WaterWorldPos;
uniform sampler2D GBuffer_WaterNormal;

uniform sampler2D sceneSource;

void main() {
	vec3 terrainColour = texture(sceneSource, texCoord).rgb;
	vec3 terrainWorldPos = texture(GBuffer_TerrainWorldPos, texCoord).xyz;
	vec3 waterWorldPos = texture(GBuffer_WaterWorldPos, texCoord).xyz;
	vec3 terrainNormal = texture(GBuffer_TerrainNormal, texCoord).xyz;
	vec3 waterNormal = texture(GBuffer_WaterNormal, texCoord).xyz;

	if (waterNormal == vec3(0)) {
		FragColour = vec4(terrainColour, 1);
	}
	else {
		float waterDistance = length(perFrameInfo.cameraPos - waterWorldPos);
		float terrainDistance = length(perFrameInfo.cameraPos - terrainWorldPos);
		float depth = abs(waterDistance - terrainDistance);
		float transmittance = exp(-depth / 5);
		vec3 colour = terrainColour * transmittance + colours.waterColour * (1 - transmittance);
		FragColour = vec4(colour, 1);
	}
}