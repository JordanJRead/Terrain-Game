#version 430 core
#extension GL_ARB_shading_language_include : require
#define PI 3.141592653589793238462
#define IMAGECOUNT 4

layout(location = 0) in vec2 vPos;

out vec3 viewPos;
out vec3 groundWorldPos;
out vec3 shellWorldPos;

// Per app
uniform int imageCount;
uniform sampler2D images[IMAGECOUNT];

// Per whenever they get changed
uniform float imageScales[IMAGECOUNT];
uniform vec2 imagePositions[IMAGECOUNT];

#include "_headeruniformbuffers.glsl"
#include "_headerterraininfo.glsl"

// Per frame
uniform mat4 view;
uniform mat4 proj;

// Per plane
uniform float planeWorldWidth;
uniform vec3 planePos;
uniform int instanceID;
out flat int shellIndex;

void main() {
	vec4 worldPos = vec4(vPos.x * planeWorldWidth + planePos.x, planePos.y, vPos.y * planeWorldWidth + planePos.z, 1);
	vec2 flatWorldPos = worldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, false);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	worldPos.y += terrainInfo.x;

	groundWorldPos = worldPos.xyz;
	shellIndex = artisticParams.shellCount - 1 - instanceID; // -1 to shellCount - 1, in reverse order to minimize overdraw
	if (shellIndex >= 0) {
		float shellProgress = float(shellIndex + 1) / artisticParams.shellCount;
		worldPos.xyz += normal * shellProgress * artisticParams.shellMaxHeight;
	}
	shellWorldPos = worldPos.xyz;
	
	viewPos = (view * worldPos).xyz;
	gl_Position = proj * vec4(viewPos, 1);
}