#ifndef SHADOWS_HEADER_H
#define SHADOWS_HEADER_H

#extension GL_ARB_bindless_texture : require

#include "_headeruniformbuffers.glsl"
#include "_headermath.glsl"

uniform sampler2D[CASCADECOUNT] shadowMaps;

float sampleShadowMap(vec2 sampleCoord, int i, float currDepth) {
	float weights[7][7] = {
		float[7](1.0/188, 2.0/188, 3.0/188, 4.0/188, 3.0/188, 2.0/188, 1.0/188),
		float[7](2.0/188, 3.0/188, 4.0/188, 5.0/188, 4.0/188, 3.0/188, 2.0/188),
		float[7](3.0/188, 4.0/188, 5.0/188, 6.0/188, 5.0/188, 4.0/188, 3.0/188),
		float[7](4.0/188, 5.0/188, 6.0/188, 7.0/188, 6.0/188, 5.0/188, 4.0/188),
		float[7](3.0/188, 4.0/188, 5.0/188, 6.0/188, 5.0/188, 4.0/188, 3.0/188),
		float[7](2.0/188, 3.0/188, 4.0/188, 5.0/188, 4.0/188, 3.0/188, 2.0/188),
		float[7](1.0/188, 2.0/188, 3.0/188, 4.0/188, 3.0/188, 2.0/188, 1.0/188)
	};
	float shadow = 0.0;
	float gridPixelSize = 0.5; // TODO: sampleWidth / sampleQuality
	float gridPixelSizeInTex = gridPixelSize / shadowInfo.shadowWidths[i];
	for(int x = -3; x <= 3; ++x) // TODO: [-3, 3] -> [-(sampleQuality - 1)/2, (sampleQuality - 1)/2, or just do [0, sampleQuality-1]
	{
		for(int y = -3; y <= 3; ++y)
		{
			float shadowDepth = texture(shadowMaps[i], sampleCoord + vec2(x, y) * gridPixelSizeInTex).r; 
			shadow += (currDepth - 0.005 > shadowDepth) ? weights[x + 3][y + 3] : 0.0;
		}
	}
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
			minScale = shadowInfo.shadowSplits[i - 1];

		if (i == CASCADECOUNT - 1)
			maxScale = 1;
		else
			maxScale = shadowInfo.shadowSplits[i];

		if (scale >= minScale && scale <= maxScale) {
			vec3 orthoPos = (shadowInfo.shadowProjectionMatrices[i] * shadowInfo.shadowViewMatrices[i] * vec4(pos, 1)).xyz;
			float currDepth = (orthoPos.z + 1) / 2;
			//float shadowDepth = texture(shadowMaps[i], (orthoPos.xy + vec2(1)) / 2).r;
			return sampleShadowMap((orthoPos.xy + vec2(1)) / 2, i, currDepth);
		}
	}	
	return 0;
}

#endif