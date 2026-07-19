#version 430 core
#define PI 3.141592653589793238462

layout(location = 0) in vec2 vPos;

uniform float planeWorldWidth;

#include "_headeruniformbuffers.glsl"
#include "_headermath.glsl"
#include "_headerwaterinfo.glsl"

void main() {
	vec2 flatPlanePos = vec2(chunkData.data[gl_InstanceID * 2 + 0], chunkData.data[gl_InstanceID * 2 + 1]);
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + flatPlanePos.x, waterParams.height, vPos.y * planeWorldWidth + flatPlanePos.y, 1);
	vec2 flatWorldPos = worldPos.xz;
	vec3 waterInfo = getWaterInfo(flatWorldPos);
	worldPos.y += waterInfo.x;
	gl_Position = perFrameInfo.projectionMatrix * perFrameInfo.viewMatrix * worldPos;
}