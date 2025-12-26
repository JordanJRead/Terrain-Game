#ifndef SHADOWS_HEADER_H
#define SHADOWS_HEADER_H

#extension GL_ARB_bindless_texture : require

#include "_headeruniformbuffers.glsl"
#include "_headermath.glsl"

uniform sampler2D[CASCADECOUNT] shadowMaps;

float sampleShadowMap(vec2 sampleCoord, int i, float currDepth) {
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMaps[i], 0);
	for(int x = -3; x <= 3; ++x)
	{
		for(int y = -3; y <= 3; ++y)
		{
			float shadowDepth = texture(shadowMaps[i], sampleCoord + vec2(x, y) * texelSize).r; 
			shadow += (currDepth - 0.01 > shadowDepth) ? 1.0 : 0.0;
		}
	}
	shadow /= 49.0;
	return shadow;
}

float isPointInShadow(vec3 pos) {
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
			minScale = shadowMatrices.shadowSplits[i - 1];

		if (i == CASCADECOUNT - 1)
			maxScale = 1;
		else
			maxScale = shadowMatrices.shadowSplits[i];

		if (scale >= minScale && scale <= maxScale) {
			vec3 orthoPos = (shadowMatrices.shadowProjectionMatrices[i] * shadowMatrices.shadowViewMatrices[i] * vec4(pos, 1)).xyz;
			float currDepth = (orthoPos.z + 1) / 2;
			//float shadowDepth = texture(shadowMaps[i], (orthoPos.xy + vec2(1)) / 2).r;
			return sampleShadowMap((orthoPos.xy + vec2(1)) / 2, i, currDepth);
		}
	}	
	return 0;
}

#endif