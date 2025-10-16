#version 430 core
#define PI 3.141592653589793238462
#define IMAGECOUNT 4

in vec3 viewPos;
in vec3 groundWorldPos;
in vec3 shellWorldPos;
out vec4 FragColor;

// Per app probably
uniform sampler2D images[IMAGECOUNT];
uniform samplerCube skybox;
uniform vec3 cameraPos;
uniform vec3 dirToLight;

// Per whenever they get changed
uniform float imageScales[IMAGECOUNT];
uniform vec2 imagePositions[IMAGECOUNT];

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
};

layout(std140, binding = 3) uniform Colours {
	uniform vec3 dirtColor;
	uniform vec3 mountainColor;
	uniform vec3 grassColor;
	uniform vec3 snowColor;
	uniform vec3 waterColor;
	uniform vec3 sunColour;
};

layout(std140, binding = 2) uniform WaterParams {
	uniform int waveCount;
	uniform float initialAmplitude;
	uniform float amplitudeMult;
	uniform float initialFreq;
	uniform float freqMult;
	uniform float initialSpeed;
	uniform float speedMult;
	uniform float specExp;
};

uniform float waterHeight;

// Per plane
in flat int shellIndex;

vec4 getTerrainInfo(vec2 worldPos) {
	for (int i = 0; i < IMAGECOUNT; ++i) {
		vec2 sampleCoord = ((worldPos / terrainScale - imagePositions[i]) / imageScales[i]) + vec2(0.5);
		
		if (!(sampleCoord.x > 1 || sampleCoord.x < 0 || sampleCoord.y > 1 || sampleCoord.y < 0)) {
			vec4 terrainInfo = texture(images[i], sampleCoord);
			terrainInfo.yz /= imageScales[i] * terrainScale;
			return terrainInfo;
		}
	}
	return vec4(0, 0, 0, 0);
}


float easeInExpo(float x) {
	return x == 0 ? 0 : pow(2.0, 10 * x - 10);
}

float easeInOutQuint(float x) {
	return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2.0 * x + 2, 5.0) / 2;
}

uint rand(uint n) {
	uint state = n * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	word = (word >> 22u) ^ word;
	return word;
}

float randToFloat(uint n) {
	return float(n) / 4294967296.0;
}

uint labelPoint(int x, int y) {
	if (x == 0 && y == 0)
		return 0;

	int n = max(abs(x), abs(y));
	int width = 2 * n + 1;
	int startingIndex = (width - 2) * (width - 2);

	if (n == y) { // top row
		return startingIndex + x + n;
	}
	if (n == -y) { // bottom row
		return startingIndex + width + x + n;
	}
	if (n == x) { // right col
		return startingIndex + width * 2 + y - 1 + n;
	}
	if (n == -x) { // right col
		return startingIndex + width * 2  + width - 2 + y - 1 + n;
	}
	return 0;
}

vec2 randUnitVector(float randNum) {
	float theta = 2 * PI * randNum;
	return normalize(vec2(cos(theta), sin(theta)));
}

int getClosestInt(float x) {
	return int(round(x) + 0.1 * (x < 0 ? -1 : 1));
}

vec2 quinticInterpolation(vec2 t) {
	return t * t * t * (t * (t * vec2(6) - vec2(15)) + vec2(10));
}

vec2 quinticDerivative(vec2 t) {
	return vec2(30) * t * t * (t * (t - vec2(2)) + vec2(1));
}

vec3 perlin(vec2 pos, int reroll) {
	int x0 = getClosestInt(floor(pos.x));
	int x1 = getClosestInt(ceil(pos.x));
	int y0 = getClosestInt(floor(pos.y));
	int y1 = getClosestInt(ceil(pos.y));

	vec2 p00 = vec2(x0, y0);
	
	vec2 relPoint =  pos - p00;

	uint rui00 = rand(labelPoint(x0, y0));
	uint rui10 = rand(labelPoint(x1, y0));
	uint rui01 = rand(labelPoint(x0, y1));
	uint rui11 = rand(labelPoint(x1, y1));

	for (int i = 0; i < reroll; ++i) {
		rui00 = rand(rui00);
		rui10 = rand(rui10);
		rui01 = rand(rui01);
		rui11 = rand(rui11);
	}

	float r00 = randToFloat(rui00);
	float r10 = randToFloat(rui10);
	float r01 = randToFloat(rui01);
	float r11 = randToFloat(rui11);

	vec2 g00 = randUnitVector(r00);
	vec2 g10 = randUnitVector(r10);
	vec2 g01 = randUnitVector(r01);
	vec2 g11 = randUnitVector(r11);

	vec2 v00 = relPoint;
	vec2 v11 = relPoint - vec2(1, 1);
	vec2 v10 = relPoint - vec2(1, 0);
	vec2 v01 = relPoint - vec2(0, 1);
	
	float d00 = dot(v00, g00);
	float d10 = dot(v10, g10);
	float d01 = dot(v01, g01);
	float d11 = dot(v11, g11);

	// From https://iquilezles.org/articles/gradientnoise/ and Acerola's github
	vec2 u = quinticInterpolation(relPoint);
	vec2 du = quinticDerivative(relPoint);
	float noise = d00 + u.x * (d10 - d00) + u.y * (d01 - d00) + u.x * u.y * (d00 - d10 - d01 + d11);
	noise = noise / 1.414 + 0.5;
	vec2 tangents = g00 + u.x * (g10 - g00) + u.y * (g01 - g00) + u.x * u.y * (g00 - g10 - g01 + g11) + du * (u.yx * (d00 - d10 - d01 + d11) + vec2(d10, d01) - d00);
	tangents /= 1.414;

	return vec3(noise, tangents.x, tangents.y);
}

float normToNegPos(float x) {
	return x * 2 - 1;
}

float extreme(float x) {
	return x < 0.5 ? (16 * x * x * x * x * x) : 1 - pow(-2 * x + 2, 5.0) / 2.0;
}

float pullup(float x) {
	return x == 1 ? 1 : 1 - pow(2.0, -10.0 * x);
}

void main() {
	bool isShell = shellIndex >= 0;

	// Terrain
	vec2 flatWorldPos = groundWorldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos);
	float groundHeight = groundWorldPos.y;
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));

	float mountain = terrainInfo.a;
	mountain = extreme(mountain);
	mountain = pullup(mountain);
	mountain = pullup(mountain);
	mountain = extreme(mountain);
	
	vec3 groundAlbedo = dirtColor * (1 - mountain) + mountain * mountainColor;

	// Which shell type are we?
	float actualSnowHeight = snowHeight + normToNegPos(perlin(flatWorldPos * snowLineNoiseScale, 0).x) * snowLineNoiseAmplitude;
	bool isSnow = actualSnowHeight < groundHeight && mountain > mountainSnowCutoff;
	bool isGrass = !isSnow;
	vec3 shellAlbedo = isSnow ? snowColor : grassColor;

	// Shell blade height
	float shellScale = isGrass ? grassNoiseScale : snowNoiseScale;
	vec2 shellCoord = flatWorldPos * shellScale;
	int shellGridX = getClosestInt(floor(shellCoord.x));
	int shellGridZ = getClosestInt(floor(shellCoord.y));
	float randomTexelHeight;
	if (!isShell)
		randomTexelHeight = 1; // Ground layer should automatically pass height check
	else {
		if (isGrass) {
				randomTexelHeight = randToFloat(rand(labelPoint(shellGridX, shellGridZ)));
		}
		else {
			randomTexelHeight = 1;
		}
	}
	
	// Terrain at center of texel
	vec2 shellWorldMiddlePos = vec2(shellGridX / shellScale, shellGridZ / shellScale);
	vec4 shellMiddleTerrainInfo = getTerrainInfo(shellWorldMiddlePos);
	vec3 shellNormal = normalize(vec3(-shellMiddleTerrainInfo.y, 1, -shellMiddleTerrainInfo.z));

	// Wetness
	float wetHeight = 0.4;
	float distAboveWater = (isShell ? shellMiddleTerrainInfo.x : groundWorldPos.y) - waterHeight;
	float wet =  1 - (distAboveWater / wetHeight);
	wet = clamp(wet, 0.0, 1.0);

	// Should shell exist?
	float currDotCutoff = isGrass ? grassDotCutoff : snowDotCutoff;
	bool shallowEnough = dot(normal, vec3(0, 1, 0)) >= currDotCutoff;

	float shellProgress = float(shellIndex + 1) / shellCount;
	float shellCutoff = shellBaseCutoff + shellProgress * (shellMaxCutoff - shellBaseCutoff);
	if (isGrass)
		shellCutoff += extreme(mountain); // Grass can't grow on mountains

	bool doesShellExist = shallowEnough && randomTexelHeight >= shellCutoff && wet == 0;

	// Albedo
	vec3 albedo;
	// Ground
	if (!isShell) {
		if (wet == 0)
			albedo = doesShellExist ? shellAlbedo : groundAlbedo;
		else {
			albedo = groundAlbedo - wet * groundAlbedo * 0.4;
		}
	}

	// Shell
	else {
		if (!doesShellExist)
			discard;
		albedo = shellAlbedo + shellAlbedo * shellProgress * 0.2;
	}


	// Fog
	float distFromCamera = length(viewPos);
	float fogStart = maxViewDistance - fogEncroach;
	float fogStrength;

	if (distFromCamera < fogStart)
		fogStrength = 0;
	else if (distFromCamera > maxViewDistance)
		fogStrength = 1;
	else
		fogStrength = (distFromCamera - fogStart) / fogEncroach;

	// Lighting
	vec3 currNormal = isGrass && isShell ? shellNormal : normal;
	float diffuse = max(0, dot(dirToLight, currNormal));
	float ambient = 0.03;
	vec3 viewDir = normalize(cameraPos - groundWorldPos);
	vec3 halfWay = normalize(viewDir + dirToLight);
	float spec = isShell ? 0 : pow(max(dot(normal, halfWay), 0), specExp);
	spec *= wet * wet;

	vec3 litAlbedo = (diffuse + ambient) * albedo + spec * sunColour;
	vec3 skyboxSample = shellWorldPos - cameraPos;
	vec3 finalColor = (1 - fogStrength) * litAlbedo + fogStrength * texture(skybox, skyboxSample).xyz;
	FragColor = vec4(finalColor, 1);
}