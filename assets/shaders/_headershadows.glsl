#ifndef SHADOWS_HEADER_H
#define SHADOWS_HEADER_H

#extension GL_ARB_bindless_texture : require

#include "_headeruniformbuffers.glsl"
#include "_headermath.glsl"

uniform sampler2D[CASCADECOUNT] shadowMaps;

bool isPointInShadow(vec3 pos) {
	vec3 viewSpacePos = (perFrameInfo.viewMatrix * vec4(pos, 1)).xyz;
	float scale = ((-viewSpacePos.z) - perFrameInfo.cameraNear) / (perFrameInfo.cameraFar - perFrameInfo.cameraNear);
	if (scale < 0 || scale > 1)
		return false;
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
			float shadowDepth = texture(shadowMaps[i], (orthoPos.xy + vec2(1)) / 2).r;
			return currDepth - 0.01 > shadowDepth; // todo better bias
		}
	}	
	return false;
}

#endif