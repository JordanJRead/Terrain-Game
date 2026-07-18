#include "heightfunction.h"

float HeightFunction::getHeightAtPoint(const glm::vec2& worldPos, const CommonBufferTypes::TerrainParams& terrainParams, float terrainScale) {
	glm::vec2 pos = worldPos / terrainScale;
	float mountain = MathHelper::perlin(pos * terrainParams.mountainFrequency, 0);
	mountain = pow(mountain, terrainParams.mountainExponent);

	mountain = mountain * (1 - terrainParams.antiFlatFactor) + terrainParams.antiFlatFactor;

	// Rivers
	float river = MathHelper::perlin(pos * terrainParams.riverScale, 1);

	river *= 2;
	river -= 1;
	river = abs(river);
	river = 1 - river;

	river = pow(river, terrainParams.riverExponent);

	river *= terrainParams.riverStrength;
	river *= (mountain * terrainParams.waterEatingMountain + 1);

	// Lakes
	float lake = MathHelper::perlin(pos * terrainParams.lakeScale, 1);

	lake = MathHelper::extreme(lake);

	lake = pow(lake, terrainParams.lakeExponent);

	lake *= terrainParams.lakeStrength;
	lake *= (mountain * terrainParams.waterEatingMountain + 1);

	float terrainInfo = 0;

	float amplitude = terrainParams.initialAmplitude;
	float spread = 1;

	for (int i = 0; i < terrainParams.octaveCount; ++i) {
		glm::vec2 samplePos = pos * spread;
		float perlinData = MathHelper::perlin(samplePos, 0);

		terrainInfo += amplitude * perlinData;
		amplitude *= terrainParams.amplitudeDecay;
		spread *= terrainParams.spreadFactor;
	}

	terrainInfo *= mountain;

	terrainInfo -= river;

	terrainInfo -= lake;

	return terrainInfo;
}

float HeightFunction::getHeightWithPerlin(const CommonBufferTypes::TerrainParams& terrainParams, const std::array<float, 4>& perlinValues) {
	float mountain = perlinValues[0];
	mountain = pow(mountain, terrainParams.mountainExponent);

	mountain = mountain * (1 - terrainParams.antiFlatFactor) + terrainParams.antiFlatFactor;

	// Rivers
	float river = perlinValues[1];

	river *= 2;
	river -= 1;
	river = abs(river);
	river = 1 - river;

	river = pow(river, terrainParams.riverExponent);

	river *= terrainParams.riverStrength;
	river *= (mountain * terrainParams.waterEatingMountain + 1);

	// Lakes
	float lake = perlinValues[2];

	lake = MathHelper::extreme(lake);

	lake = pow(lake, terrainParams.lakeExponent);

	lake *= terrainParams.lakeStrength;
	lake *= (mountain * terrainParams.waterEatingMountain + 1);

	float terrainInfo = 0;

	float amplitude = terrainParams.initialAmplitude;
	float spread = 1;

	for (int i = 0; i < terrainParams.octaveCount; ++i) {
		float perlinData = perlinValues[3];

		terrainInfo += amplitude * perlinData;
		amplitude *= terrainParams.amplitudeDecay;
		spread *= terrainParams.spreadFactor;
	}

	terrainInfo *= mountain;

	terrainInfo -= river;

	terrainInfo -= lake;

	return terrainInfo;
}

std::array<float, 4> HeightFunction::getMinHeightPerlinValues(const CommonBufferTypes::TerrainParams& terrainParams) {
	std::array<float, 4> minPerlinValues{ 0, 1, 1, 0 };
	float minHeight{ getHeightWithPerlin(terrainParams, minPerlinValues) };
	for (int i1{ 0 }; i1 <= 100; ++i1) {
		for (int i2{ 0 }; i2 <= 100; ++i2) {
			for (int i3{ 0 }; i3 <= 100; ++i3) {
				for (int i4{ 0 }; i4 <= 100; ++i4) {
					std::array<float, 4> testPerlinValues{ {i1 / 100.0f, i2 / 100.0f, i3 / 100.0f, i4 / 100.0f} };
					float height{ getHeightWithPerlin(terrainParams, testPerlinValues) };
					if (height < minHeight) {
						minPerlinValues = testPerlinValues;
						minHeight = height;
					}
				}
			}
		}
	}
	return minPerlinValues;
}

std::array<float, 4> HeightFunction::getMaxHeightPerlinValues(const CommonBufferTypes::TerrainParams& terrainParams) {
	std::array<float, 4> maxPerlinValues{ 1, 0, 0, 1 };
	float maxHeight{ getHeightWithPerlin(terrainParams, maxPerlinValues) };
	for (int i1{ 0 }; i1 <= 100; ++i1) {
		for (int i2{ 0 }; i2 <= 100; ++i2) {
			for (int i3{ 0 }; i3 <= 100; ++i3) {
				for (int i4{ 0 }; i4 <= 100; ++i4) {
					std::array<float, 4> testPerlinValues{ {i1 / 100.0f, i2 / 100.0f, i3 / 100.0f, i4 / 100.0f} };
					float height{ getHeightWithPerlin(terrainParams, testPerlinValues) };
					if (height > maxHeight) {
						maxPerlinValues = testPerlinValues;
						maxHeight = height;
					}
				}
			}
		}
	}
	return maxPerlinValues;
}