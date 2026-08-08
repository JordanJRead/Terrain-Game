#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#define IMAGECOUNT 5
#define CASCADECOUNT 4
#define STARYSPLITCOUNT 10
#extension GL_ARB_bindless_texture : require

layout(std140, binding = 0) uniform TerrainParams {
	int   octaveCount;
	int   smoothOctaveCount;
	float initialAmplitude;
	float amplitudeDecay;
	float spreadFactor;

	float mountainFrequency;
	float mountainExponent;
	float antiFlatFactor;
	float riverScale;
	float riverStrength;
	float riverExponent;
	float waterEatingMountain;
	float lakeScale;
	float lakeStrength;
	float lakeExponent;
} terrainParams;

layout(std140, binding = 1) uniform ArtisticParams {
	float terrainScale;
	float maxViewDistance;
	float fogEncroach;
	float grassDotCutoff;
	float snowDotCutoff;
	float shellMaxHeight;
	float grassNoiseScale;
	float snowNoiseScale;
	float shellMaxCutoff;
	float shellBaseCutoff;
	float snowHeight;
	float seafoamStrength;
	float snowLineNoiseScale;
	float snowLineNoiseAmplitude;
	float mountainSnowCutoff;
	float snowLineEase;
	float shellAmbientOcclusion;
} artisticParams;

layout(std140, binding = 2) uniform WaterParams {
	int waveCount;
	float initialAmplitude;
	float amplitudeMult;
	float initialFreq;
	float freqMult;
	float initialSpeed;
	float speedMult;
	float specExp;
	float height;
} waterParams;

layout(std140, binding = 3) uniform Colours {
	vec3 dirtColour;
	vec3 mountainColour;
	vec3 grassColour1;
	vec3 grassColour2;
	vec3 snowColour;
	vec3 waterColour;
	vec3 sunColour;
	float sunBrightness;
	vec3 moonColour;
	float moonBrightness;
	vec3 starColour;
	float starBrightness;
} colours;

layout(std140, binding = 4) uniform PerFrameInfo {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 starRotationMatrix;
	vec3 cameraPos;
	vec3 dirToSun;
	float time;
	float fovX;
	float fovY;
	float yaw;
	float pitch;
	float cameraNear;
	float cameraFar;
	float tanHalfFOVX;
	float tanHalfFOVY;
	float dayTime;
	float nightStrength;
} perFrameInfo;

layout(std430, binding = 5) buffer TerrainImagesInfo {
	vec2 imagePositions[IMAGECOUNT];
	float imageScales[IMAGECOUNT];
} terrainImagesInfo;

layout(std140, binding = 6) uniform AtmosphereInfo {
	vec3 rayleighScattering;
	vec3 mieScattering;
	float maxRadius;
	float minRadius;
	float centerY;
	float rayleighDensityFalloff;
	float mieDensityFalloff;
	float rayleighDensity;
	float mieDensity;
	float rayleighG;
	float mieG;
	int rayAtmosphereStepCount;
	int raySunStepCount;
	float brightness;
	float ditherStrength;
	float sunSizeDeg;
} atmosphereInfo;

layout(std430, binding = 7) buffer ShadowInfo {
	mat4 viewMatricesSun[CASCADECOUNT];
	mat4 projectionMatricesSun[CASCADECOUNT];
	mat4 viewMatricesMoon[CASCADECOUNT];
	mat4 projectionMatricesMoon[CASCADECOUNT];
	float splits[CASCADECOUNT - 1];
	float widthsSun[CASCADECOUNT];
	float widthsMoon[CASCADECOUNT];
	float blurWidth;
	int blurQuality; // odd
	float blurGridSum;
	float exposure;
	float minBias;
	float maxBias;
} shadowInfo;

layout(std430, binding = 8) buffer ChunkData {
	float data[];
} chunkData;

layout(std430, binding = 9) buffer StarData {
	ivec2 indexData[STARYSPLITCOUNT]; // (starting index for split i, number of stars in split i)
	vec4 stars[]; // (x dir, y dir, z dir, size in deg)
} starData;

layout(std140, binding = 10) uniform ScreenSpaceReflectionParams {
	float maxWorldDistance;
	int stepCount;
	int binarySearchStepCount;
	bool fadeOutTowardsCamera;
	bool fadeOutDepth;
	bool fadeOutDistance;
} screenSpaceReflectionParams;

layout(std430, binding = 11) buffer DebugData {
	bool hasData;
	vec3 worldStart;
	vec3 worldEnd;
	int maxPointCount;
	int currentPointCount;
	float tCurrent;
	float tVisible;
	vec4 pointLine[1000];
	vec4 pointScene[1000];
	vec2 pointUV[1000];
} debugData;

#endif
