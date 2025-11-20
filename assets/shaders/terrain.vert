#version 430 core
#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec2 vPos;

out VertOut {
	vec3 viewPos;
	vec3 groundWorldPos;
	vec3 worldPos;
	flat bool isEdge;
} vertOut;

#include "_headeruniformbuffers.glsl"
#include "_headerterraininfo.glsl"

// Per plane
uniform float planeWorldWidth;
uniform vec3 planePos;
out flat int shellIndex;

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + planePos.x, planePos.y, vPos.y * planeWorldWidth + planePos.z, 1);
	vec2 flatWorldPos = worldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, false);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	worldPos.y += terrainInfo.x;

	vertOut.groundWorldPos = worldPos.xyz;
	shellIndex = artisticParams.shellCount - 1 - gl_InstanceID; // -1 to shellCount - 1, in reverse order to minimize overdraw
	if (shellIndex >= 0) {
		float shellProgress = float(shellIndex + 1) / artisticParams.shellCount;
		worldPos.xyz += normal * shellProgress * artisticParams.shellMaxHeight;
	}

	vertOut.worldPos = worldPos.xyz;
	vertOut.viewPos = (perFrameInfo.viewMatrix * worldPos).xyz;
	
	if ((0.5 - abs(vPos.x)) < 0.01 || (0.5 - abs(vPos.y)) < 0.01)
		vertOut.isEdge = true;
	else
		vertOut.isEdge = false;
	gl_Position = perFrameInfo.projectionMatrix * vec4(vertOut.viewPos, 1);
}