#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

layout(std140, binding = 0) uniform terrainParams {
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
};

layout(std140, binding = 1) uniform ArtisticParams {
	uniform float terrainScale;
	uniform float maxViewDistance;
	uniform float fogEncroach;
	uniform float grassDotCutoff;
	uniform float snowDotCutoff;
	uniform int   shellCount;
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
	vec4 cameraPos;
	vec4 dirToSun;
	float time;
} perFrameInfo;

#endif