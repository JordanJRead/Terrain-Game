#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#define IMAGECOUNT 5
#define CASCADECOUNT 3
#extension GL_ARB_bindless_texture : require

layout(std140, binding = 0) uniform TerrainParams {
	uniform int   octaveCount;
	uniform int   smoothOctaveCount;
	uniform float initialAmplitude;
	uniform float amplitudeDecay;
	uniform float spreadFactor;

	uniform float mountainFrequency;
	uniform float mountainExponent;
	uniform float antiFlatFactor;
	uniform float riverScale;
	uniform float riverStrength;
	uniform float riverExponent;
	uniform float waterEatingMountain;
	uniform float lakeScale;
	uniform float lakeStrength;
	uniform float lakeExponent;
} terrainParams;

layout(std140, binding = 1) uniform ArtisticParams {
	uniform float terrainScale;
	uniform float maxViewDistance;
	uniform float fogEncroach;
	uniform float grassDotCutoff;
	uniform float snowDotCutoff;
	uniform float shellMaxHeight;
	uniform float grassNoiseScale;
	uniform float snowNoiseScale;
	uniform float shellMaxCutoff;
	uniform float shellBaseCutoff;
	uniform float snowHeight;
	uniform float seafoamStrength;
	uniform float snowLineNoiseScale;
	uniform float snowLineNoiseAmplitude;
	uniform float mountainSnowCutoff;
	uniform float snowLineEase;
	uniform float shellAmbientOcclusion;
} artisticParams;

layout(std140, binding = 2) uniform WaterParams {
	uniform int waveCount;
	uniform float initialAmplitude;
	uniform float amplitudeMult;
	uniform float initialFreq;
	uniform float freqMult;
	uniform float initialSpeed;
	uniform float speedMult;
	uniform float specExp;
	uniform float height;
} waterParams;

layout(std140, binding = 3) uniform Colours {
	uniform vec3 dirtColour;
	uniform vec3 mountainColour;
	uniform vec3 grassColour1;
	uniform vec3 grassColour2;
	uniform vec3 snowColour;
	uniform vec3 waterColour;
	uniform vec3 sunColour;
} colours;

layout(std140, binding = 4) uniform PerFrameInfo {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	vec3 cameraPos; float g;
	vec3 dirToSun; float gg;
	float time;
	float fovX;
	float fovY;
	float yaw;
	float pitch;
	float cameraNear;
	float cameraFar;
} perFrameInfo;

layout(std430, binding = 5) buffer TerrainImagesInfo {
	vec2 imagePositions[IMAGECOUNT];
	float imageScales[IMAGECOUNT];
} terrainImagesInfo;

layout(std140, binding = 6) uniform AtmosphereInfo {
	vec3 rayleighScattering; float fregerfg;
	vec3 mieScattering; float gwrefwe;
	float maxRadius;
	float minRadius;
	float centerY;
	float rayleighDensityFalloff;
	float mieDensityFalloff;
	float rayleighDensityScale;
	float mieDensityScale;
	float rayleighG;
	float mieG;
	int rayAtmosphereStepCount;
	int raySunStepCount;
	float brightness;
} atmosphereInfo;

layout(std430, binding = 7) buffer ShadowInfo {
	mat4 viewMatrices[CASCADECOUNT];
	mat4 projectionMatrices[CASCADECOUNT];
	float splits[CASCADECOUNT - 1];
	float widths[CASCADECOUNT];
	float blurWidth;
	int blurQuality; // odd
	float blurGridSum;
	float exposure;
} shadowInfo;

layout(std430, binding = 8) buffer ChunkData {
	float data[];
} chunkData;

#endif
