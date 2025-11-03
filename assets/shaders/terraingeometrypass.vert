#version 430 core
#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec2 vPos;

out VertOut {
	vec3 groundWorldPos;
	vec3 worldPos;
	flat int shellIndex;
} vertOut;

#include "_headeruniformbuffers.glsl"
#include "_headerterraininfo.glsl"

// Per plane
uniform float planeWorldWidth; 
uniform vec3 planePos;

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + planePos.x, planePos.y, vPos.y * planeWorldWidth + planePos.z, 1);
	vec2 flatWorldPos = worldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, false);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	worldPos.y += terrainInfo.x;

	vertOut.groundWorldPos = worldPos.xyz;
	vertOut.shellIndex = artisticParams.shellCount - 1 - gl_InstanceID; // -1 to shellCount - 1, in reverse order to minimize overdraw
	if (vertOut.shellIndex >= 0) {
		float shellProgress = float(vertOut.shellIndex + 1) / artisticParams.shellCount;
		worldPos.xyz += normal * shellProgress * artisticParams.shellMaxHeight;
	}

	vertOut.worldPos = worldPos.xyz;

	gl_Position = perFrameInfo.projectionMatrix * perFrameInfo.viewMatrix * worldPos;
}