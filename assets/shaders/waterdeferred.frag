#version 430 core

uniform sampler2D blueNoise;
layout(pixel_center_integer) in vec4 gl_FragCoord;

#include "_headeruniformbuffers.glsl"
#include "_headerscreenmarch.glsl"
#include "_headeratmosphere.glsl"

in vec2 texCoord;
out vec4 FragColour;

uniform sampler2D GBuffer_TerrainGroundWorldPos;
uniform sampler2D GBuffer_TerrainWorldPos;
uniform sampler2D GBuffer_TerrainNormal;
uniform sampler2D GBuffer_ShellProgressMountainDoesTexelExist;
uniform sampler2D GBuffer_WaterWorldPos;
uniform sampler2D GBuffer_WaterNormal;

uniform sampler2D sceneSource;
uniform ivec2 debugFragCoord;

vec3 fresnelColour(vec3 normal, vec3 direction) {
	float n = 1.333;
	float f0 = pow((n - 1) / (n + 1), 5.0);
	return vec3(f0 + (1 - f0) * pow(max(1 - dot(normal, direction), 0), 5.0));
}

void main() {
	vec3 waterWorldPos = texture(GBuffer_WaterWorldPos, texCoord).xyz;
	vec3 waterNormal = normalize(texture(GBuffer_WaterNormal, texCoord).xyz);

	if (isnan(waterNormal.x)) {
		// No water
		FragColour = vec4(texture(sceneSource, texCoord).rgb, 1);
		return;
	}

	// TODO
	/*
	Go through rest of SSRe pipeline
	Add sky reflection (in this pass)
	Add SSRa / water colour
	*/

	vec3 incomingDir = normalize(waterWorldPos - perFrameInfo.cameraPos);
	vec3 reflectedDirection = normalize(reflect(incomingDir, waterNormal));

	vec3 worldStart = waterWorldPos;
	vec3 worldEnd = worldStart + reflectedDirection * screenSpaceReflectionParams.maxWorldDistance;

	// Fix ray going behind camera
	{
		float endDepth = worldToDepth(worldEnd);
		if (endDepth < perFrameInfo.cameraNear) {
			float depthBehind = abs(endDepth - perFrameInfo.cameraNear);
			float entireYLength = abs(worldToDepth(worldStart) - endDepth);
			float fractionBehind = depthBehind / entireYLength;
			worldEnd = worldStart + reflectedDirection * screenSpaceReflectionParams.maxWorldDistance * (1 - fractionBehind);
		}
	}

	vec2 reflectedUV = screenSpaceMarch(texCoord * textureSize(GBuffer_TerrainNormal, 0), worldStart, worldEnd, screenSpaceReflectionParams.stepCount, screenSpaceReflectionParams.binarySearchStepCount, GBuffer_TerrainWorldPos, GBuffer_TerrainNormal);

	bool doesTerrainExist = !(texture(GBuffer_TerrainNormal, reflectedUV).xyz == vec3(0));
	bool OOB = reflectedUV.x < 0 || reflectedUV.x > 1 || reflectedUV.y < 0 || reflectedUV.y > 1;

	float ssrConfidence =
	    (doesTerrainExist ? 1 : 0)
	 // * (screenSpaceReflectionParams.fadeOutTowardsCamera ? (max(0, dot(-incomingDir, reflectedDirection))) : 1)
	 // * (screenSpaceReflectionParams.fadeOutDistance ? (1 - clamp(length(currentSceneWorld - waterWorldPos) / screenSpaceReflectionParams.maxWorldDistance, 0, 1)) : 1)
	  * (OOB ? 0 : 1)
	;
	ssrConfidence = clamp(ssrConfidence, 0, 1);

	// Sky
	vec3 starColour = getStarColour(reflectedDirection);
	vec3 skyColour = lightReceived(waterWorldPos, reflectedDirection, true, vec3(0), starColour);

	vec3 reflectedColour = mix(skyColour, texture(sceneSource, reflectedUV).rgb, ssrConfidence);

	vec3 fresnel = fresnelColour(waterNormal, reflectedDirection);

	vec3 finalColour = reflectedColour * fresnel + colours.waterColour * (vec3(1) - fresnel);

	FragColour = vec4(finalColour, 1);
}