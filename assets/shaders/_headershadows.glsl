#ifndef SHADOWS_HEADER_H
#define SHADOWS_HEADER_H

#extension GL_ARB_bindless_texture : require

#include "_headeruniformbuffers.glsl"
#include "_headermath.glsl"

uniform sampler2D[CASCADECOUNT] shadowMaps;

int taxicabDist(ivec2 p1, ivec2 p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

float sampleShadowMap(vec2 sampleCoord, int i, float currDepth, bool blur = false) {

/*
	float weights[7][7] = {
		float[7](1, 2, 3, 4, 3, 2, 1),
		float[7](2, 3, 4, 5, 4, 3, 2),
		float[7](3, 4, 5, 6, 5, 4, 3),
		float[7](4, 5, 6, 7, 6, 5, 4),
		float[7](3, 4, 5, 6, 5, 4, 3),
		float[7](2, 3, 4, 5, 4, 3, 2),
		float[7](1, 2, 3, 4, 3, 2, 1)
	};

	float weights[7][7] = {
		float[7](1.0/188, 2.0/188, 3.0/188, 4.0/188, 3.0/188, 2.0/188, 1.0/188),
		float[7](2.0/188, 3.0/188, 4.0/188, 5.0/188, 4.0/188, 3.0/188, 2.0/188),
		float[7](3.0/188, 4.0/188, 5.0/188, 6.0/188, 5.0/188, 4.0/188, 3.0/188),
		float[7](4.0/188, 5.0/188, 6.0/188, 7.0/188, 6.0/188, 5.0/188, 4.0/188),
		float[7](3.0/188, 4.0/188, 5.0/188, 6.0/188, 5.0/188, 4.0/188, 3.0/188),
		float[7](2.0/188, 3.0/188, 4.0/188, 5.0/188, 4.0/188, 3.0/188, 2.0/188),
		float[7](1.0/188, 2.0/188, 3.0/188, 4.0/188, 3.0/188, 2.0/188, 1.0/188)
	};
*/
	if (!blur) {
		return (currDepth - 0.005 > texture(shadowMaps[i], sampleCoord).r) ? 1 : 0.0;
	}

	float shadow = 0.0;
	float gridPixelWorldSize = shadowInfo.blurWidth / shadowInfo.blurQuality;
	float gridPixelTexSize = gridPixelWorldSize / shadowInfo.widths[i];
	for (int x = 0; x < shadowInfo.blurQuality; ++x) {
		for (int y = 0; y < shadowInfo.blurQuality; ++y) {
			vec2 sampleOffset = gridPixelTexSize * vec2(x, y) - vec2(shadowInfo.blurQuality / 2);
			float weight = shadowInfo.blurQuality - taxicabDist(ivec2(x, y), ivec2(shadowInfo.blurQuality / 2));
			weight /= shadowInfo.blurGridSum;
			float shadowDepth = texture(shadowMaps[i], sampleCoord + vec2(x, y) * gridPixelTexSize).r; 
			shadow += (currDepth - 0.005 > shadowDepth) ? weight : 0.0;
		}
	}
	return shadow;
}

float isPointInShadow(vec3 pos, bool blur = false) {
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
			vec3 orthoPos = (shadowInfo.projectionMatrices[i] * shadowInfo.viewMatrices[i] * vec4(pos, 1)).xyz;
			float currDepth = (orthoPos.z + 1) / 2;
			//float shadowDepth = texture(shadowMaps[i], (orthoPos.xy + vec2(1)) / 2).r;
			return sampleShadowMap((orthoPos.xy + vec2(1)) / 2, i, currDepth, blur);
		}
	}	
	return 0;
}

#endif