#version 430 core

#include "_headeruniformbuffers.glsl"

in vec2 texCoord;
layout(pixel_center_integer) in vec4 gl_FragCoord;
out vec4 FragColour;

uniform sampler2D GBuffer_TerrainGroundWorldPos;
uniform sampler2D GBuffer_TerrainWorldPos;
uniform sampler2D GBuffer_TerrainNormal;
uniform sampler2D GBuffer_ShellProgressMountainDoesTexelExist;
uniform sampler2D GBuffer_WaterWorldPos;
uniform sampler2D GBuffer_WaterNormal;

uniform sampler2D sceneSource;
uniform ivec2 debugFragCoord;

vec2 worldToUV(vec3 worldPos) {
	vec4 clipPos = perFrameInfo.projectionMatrix * perFrameInfo.viewMatrix * vec4(worldPos, 1);
	vec3 ndcPos = clipPos.xyz / clipPos.w;
	vec2 uvPos = (ndcPos.xy + vec2(1)) / vec2(2);
	return uvPos;
}

vec3 worldToView(vec3 worldPos) {
	return (perFrameInfo.viewMatrix * vec4(worldPos, 1)).xyz;
}

float worldToDepth(vec3 worldPos) {
	return -worldToView(worldPos).z;
}

void main() {
	vec3 waterWorldPos = texture(GBuffer_WaterWorldPos, texCoord).xyz;
	vec3 waterNormal = normalize(texture(GBuffer_WaterNormal, texCoord).xyz);

	if (isnan(waterNormal.x)) {
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

	vec3 worldStart = waterWorldPos;// + reflectedDirection * 1;
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

	float depthNear = worldToDepth(worldStart);
	float depthFar = worldToDepth(worldEnd);

	vec2 textureDims = textureSize(sceneSource, 0);

	vec2 fragStart = texCoord * textureDims;
	vec2 fragEnd = worldToUV(worldEnd) * textureDims;

	float tVisible = 0;
	float tInvisible = 0;

	bool hit = false;
	vec2 uvCurrent;
	float currentDeltaDepth = -1;
	for (int i = 0; i < screenSpaceReflectionParams.stepCount; ++i) {
		tInvisible = float(i) / (screenSpaceReflectionParams.stepCount - 1);

		vec2 fragCurrent = mix(fragStart, fragEnd, tInvisible);
		uvCurrent = fragCurrent / textureDims;
		if (uvCurrent.x > 1 || uvCurrent.x < 0 || uvCurrent.y > 1 || uvCurrent.y < 0) {
			break;
		}

		vec3 worldCurrent = mix(worldStart / depthNear, worldEnd / depthFar, tInvisible) / mix(1 / depthNear, 1 / depthFar, tInvisible);

		float currentDepth = worldToDepth(worldCurrent);
		if (currentDepth < perFrameInfo.cameraNear || currentDepth > perFrameInfo.cameraFar) {
			break;
		}

		vec3 currentSceneWorld = texture(GBuffer_TerrainWorldPos, uvCurrent).xyz;

		// No terrain
		if (texture(GBuffer_TerrainNormal, uvCurrent).xyz == vec3(0)) {
			continue;
		}
		float currentSceneDepth = worldToDepth(currentSceneWorld);

		currentDeltaDepth = currentDepth - currentSceneDepth;

		// Now behind terrain
		if (currentDeltaDepth > 0) {
			hit = true;
			break;
		}
		else {
			tVisible = tInvisible;
		}
	}

	vec3 currentSceneWorld;
	if (hit) {
		float tSearch = (tInvisible + tVisible) / 2;
		float tMoveAmount = (tInvisible - tVisible) / 4;
		int debugPointCount = 0;
		for (int i = 0; i < screenSpaceReflectionParams.binarySearchStepCount; ++i) {
			vec2 fragCurrent = mix(fragStart, fragEnd, tSearch);
			uvCurrent = fragCurrent / textureDims;
			if (uvCurrent.x > 1 || uvCurrent.x < 0 || uvCurrent.y > 1 || uvCurrent.y < 0) {
				break;
			}
			
			vec3 worldCurrent = mix(worldStart / depthNear, worldEnd / depthFar, tSearch) / mix(1 / depthNear, 1 / depthFar, tSearch);

			float currentDepth = worldToDepth(worldCurrent);
			if (currentDepth < perFrameInfo.cameraNear || currentDepth > perFrameInfo.cameraFar) {
				break;
			}

			currentSceneWorld = texture(GBuffer_TerrainWorldPos, uvCurrent).xyz;

			// No terrain
			if (texture(GBuffer_TerrainNormal, uvCurrent).xyz == vec3(0)) {
				break;
			}
			float currentSceneDepth = worldToDepth(currentSceneWorld);

			currentDeltaDepth = currentDepth - currentSceneDepth;
			if (currentDeltaDepth < 0) {
				tSearch += tMoveAmount;
			}
			else {
				tSearch -= tMoveAmount;
			}
			tMoveAmount /= 2;
		}
	}

	bool doesTerrainExist = !(texture(GBuffer_TerrainNormal, uvCurrent).xyz == vec3(0));
	bool OOB = uvCurrent.x < 0 || uvCurrent.x > 1 || uvCurrent.y < 0 || uvCurrent.y > 1;

	float visibility =
		(hit ? 1 : 0)
	  * (doesTerrainExist ? 1 : 0)
	  * (screenSpaceReflectionParams.fadeOutTowardsCamera ? (max(0, dot(-incomingDir, reflectedDirection))) : 1)
	  * (screenSpaceReflectionParams.fadeOutDistance ? (1 - clamp(length(currentSceneWorld - waterWorldPos) / screenSpaceReflectionParams.maxWorldDistance, 0, 1)) : 1)
	  * (OOB ? 0 : 1)
	;
	visibility = clamp(visibility, 0, 1);
	vec3 finalColour = mix(colours.waterColour, texture(sceneSource, uvCurrent).rgb, hit);
	FragColour = vec4(finalColour, 1);
}