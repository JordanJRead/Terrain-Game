#ifndef HEADER_SCREEN_MARCH_GLSL
#define HEADER_SCREEN_MARCH_GLSL


#include "_headeruniformbuffers.glsl"

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

vec2 screenSpaceMarch(vec2 fragStart, vec3 worldStart, vec3 worldEnd, int stepCount, int binaryStepCount, sampler2D sceneWorldTexture, sampler2D sceneNormalTexture) {
	float depthNear = worldToDepth(worldStart);
	float depthFar = worldToDepth(worldEnd);

	vec2 textureDims = textureSize(sceneWorldTexture, 0);
	vec2 fragEnd = worldToUV(worldEnd) * textureDims;

	float tVisible = 0;
	float tInvisible = 0;

	bool hit = false;
	float currentDeltaDepth = -1;

	for (int i = 0; i < stepCount; ++i) {
		tInvisible = float(i) / (stepCount - 1);

		vec2 fragCurrent = mix(fragStart, fragEnd, tInvisible);
		vec2 uvCurrent = fragCurrent / textureDims;

		if (uvCurrent.x > 1 || uvCurrent.x < 0 || uvCurrent.y > 1 || uvCurrent.y < 0) {
			// OOB uv
			break;
		}

		vec3 worldCurrent = mix(worldStart / depthNear, worldEnd / depthFar, tInvisible) / mix(1 / depthNear, 1 / depthFar, tInvisible);

		float currentDepth = worldToDepth(worldCurrent);
		if (currentDepth < perFrameInfo.cameraNear || currentDepth > perFrameInfo.cameraFar) {
			// OOB near/far
			break;
		}

		vec3 currentSceneWorld = texture(sceneWorldTexture, uvCurrent).xyz;

		if (texture(sceneNormalTexture, uvCurrent).xyz == vec3(0)) {
			// No terrain
			continue;
		}

		float currentSceneDepth = worldToDepth(currentSceneWorld);

		currentDeltaDepth = currentDepth - currentSceneDepth;

		if (currentDeltaDepth > 0) {
			// Now behind terrain
			hit = true;
			break;
		}
		else {
			tVisible = tInvisible;
		}
	}

	if (!hit) {
		return vec2(-1, -1);
	}

	vec3 currentSceneWorld;
	float tSearch = (tInvisible + tVisible) / 2;
	float tMoveAmount = (tInvisible - tVisible) / 4;
	vec2 finalUV;
	int debugPointCount = 0;
	for (int i = 0; i < binaryStepCount; ++i) {
		vec2 fragCurrent = mix(fragStart, fragEnd, tSearch);
		finalUV = fragCurrent / textureDims;
		if (finalUV.x > 1 || finalUV.x < 0 || finalUV.y > 1 || finalUV.y < 0) {
			break;
		}

		vec3 worldCurrent = mix(worldStart / depthNear, worldEnd / depthFar, tSearch) / mix(1 / depthNear, 1 / depthFar, tSearch);

		float currentDepth = worldToDepth(worldCurrent);
		if (currentDepth < perFrameInfo.cameraNear || currentDepth > perFrameInfo.cameraFar) {
			break;
		}

		currentSceneWorld = texture(sceneWorldTexture, finalUV).xyz;

		// No terrain
		if (texture(sceneNormalTexture, finalUV).xyz == vec3(0)) {
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

	return finalUV;
}

#endif