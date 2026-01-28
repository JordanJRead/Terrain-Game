#ifndef SHADOWS_HEADER_H
#define SHADOWS_HEADER_H

#extension GL_ARB_bindless_texture : require

#include "_headeruniformbuffers.glsl"
#include "_headermath.glsl"

uniform sampler2D[CASCADECOUNT] shadowMapsSun;
uniform sampler2D[CASCADECOUNT] shadowMapsMoon;

int taxicabDist(ivec2 p1, ivec2 p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

float sampleShadowMapSun(vec2 sampleCoord, int i, float currDepth, bool blur = false) {
	if (!blur) {
		return (currDepth > texture(shadowMapsSun[i], sampleCoord).r) ? 1 : 0.0;
	}

	float shadow = 0.0;
	float gridPixelWorldSize = shadowInfo.blurWidth / shadowInfo.blurQuality;
	float gridPixelTexSize = gridPixelWorldSize / shadowInfo.widthsSun[i];
	for (int x = 0; x < shadowInfo.blurQuality; ++x) {
		for (int y = 0; y < shadowInfo.blurQuality; ++y) {
			vec2 sampleOffset = gridPixelTexSize * vec2(x, y) - vec2(shadowInfo.blurQuality / 2);
			float weight = shadowInfo.blurQuality - taxicabDist(ivec2(x, y), ivec2(shadowInfo.blurQuality / 2));
			weight /= shadowInfo.blurGridSum;
			float shadowDepth = texture(shadowMapsSun[i], sampleCoord + vec2(x, y) * gridPixelTexSize).r; 
			shadow += (currDepth > shadowDepth) ? weight : 0.0;
		}
	}
	return shadow;
}

float isPointInSunShadow(vec3 pos, vec3 normal, bool blur = false) {
	vec3 viewSpacePos = (perFrameInfo.viewMatrix * vec4(pos, 1)).xyz;
	float scale = ((-viewSpacePos.z) - perFrameInfo.cameraNear) / (perFrameInfo.cameraFar - perFrameInfo.cameraNear);
	if (scale < 0 || scale > 1)
		return 0;
	for (int i = 0; i < CASCADECOUNT; ++i) {
		float minScale;
		float maxScale;
		if (i == 0)
			minScale = 0;
		else
			minScale = shadowInfo.splits[i - 1];

		if (i == CASCADECOUNT - 1)
			maxScale = 1;
		else
			maxScale = shadowInfo.splits[i];

		if (scale >= minScale && scale <= maxScale) {
			float bias = shadowInfo.minBias + (shadowInfo.maxBias - shadowInfo.minBias) * (1 - dot(normal, perFrameInfo.dirToSun));
			vec3 orthoPos = (shadowInfo.projectionMatricesSun[i] * (shadowInfo.viewMatricesSun[i] * vec4(pos, 1) + vec4(0, 0, bias, 0))).xyz;
			float currDepth = (orthoPos.z + 1) / 2;
			//float shadowDepth = texture(shadowMaps[i], (orthoPos.xy + vec2(1)) / 2).r;
			return sampleShadowMapSun((orthoPos.xy + vec2(1)) / 2, i, currDepth, blur);
		}
	}	
	return 0;
}

float sampleShadowMapMoon(vec2 sampleCoord, int i, float currDepth, bool blur = false) {
	if (!blur) {
		return (currDepth > texture(shadowMapsMoon[i], sampleCoord).r) ? 1 : 0.0;
	}

	float shadow = 0.0;
	float gridPixelWorldSize = shadowInfo.blurWidth / shadowInfo.blurQuality;
	float gridPixelTexSize = gridPixelWorldSize / shadowInfo.widthsMoon[i];
	for (int x = 0; x < shadowInfo.blurQuality; ++x) {
		for (int y = 0; y < shadowInfo.blurQuality; ++y) {
			vec2 sampleOffset = gridPixelTexSize * vec2(x, y) - vec2(shadowInfo.blurQuality / 2);
			float weight = shadowInfo.blurQuality - taxicabDist(ivec2(x, y), ivec2(shadowInfo.blurQuality / 2));
			weight /= shadowInfo.blurGridSum;
			float shadowDepth = texture(shadowMapsMoon[i], sampleCoord + vec2(x, y) * gridPixelTexSize).r; 
			shadow += (currDepth > shadowDepth) ? weight : 0.0;
		}
	}
	return shadow;
}

float isPointInMoonShadow(vec3 pos, vec3 normal, bool blur = false) {
	vec3 viewSpacePos = (perFrameInfo.viewMatrix * vec4(pos, 1)).xyz;
	float scale = ((-viewSpacePos.z) - perFrameInfo.cameraNear) / (perFrameInfo.cameraFar - perFrameInfo.cameraNear);
	if (scale < 0 || scale > 1)
		return 0;
	for (int i = 0; i < CASCADECOUNT; ++i) {
		float minScale;
		float maxScale;
		if (i == 0)
			minScale = 0;
		else
			minScale = shadowInfo.splits[i - 1];

		if (i == CASCADECOUNT - 1)
			maxScale = 1;
		else
			maxScale = shadowInfo.splits[i];

		if (scale >= minScale && scale <= maxScale) {
			float bias = shadowInfo.minBias + (shadowInfo.maxBias - shadowInfo.minBias) * (1 - dot(normal, -perFrameInfo.dirToSun));
			vec3 orthoPos = (shadowInfo.projectionMatricesMoon[i] * (shadowInfo.viewMatricesMoon[i] * vec4(pos, 1) + vec4(0, 0, bias, 0))).xyz;
			float currDepth = (orthoPos.z + 1) / 2;
			//float shadowDepth = texture(shadowMaps[i], (orthoPos.xy + vec2(1)) / 2).r;
			return sampleShadowMapMoon((orthoPos.xy + vec2(1)) / 2, i, currDepth, blur);
		}
	}	
	return 0;
}

#endif