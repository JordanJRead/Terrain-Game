#version 430 core
#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec2 vPos;

#include "_headerterraininfo.glsl"
#include "_headeruniformbuffers.glsl"

// Per plane
uniform float planeWorldWidth;
uniform vec3 planePos;

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + planePos.x, planePos.y, vPos.y * planeWorldWidth + planePos.z, 1);
	vec2 flatWorldPos = worldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, false);
	worldPos.y += terrainInfo.x;
	gl_Position = perFrameInfo.projectionMatrix * perFrameInfo.viewMatrix * worldPos;
}