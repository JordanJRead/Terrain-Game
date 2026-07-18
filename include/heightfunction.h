#ifndef HEIGHT_FUNCTION_H
#define HEIGHT_FUNCTION_H

#include "glm/glm.hpp"
#include "mathhelper.h"
#include "commonbuffertypes.h"

namespace HeightFunction {
	inline std::array<float, 4> gMinPerlinValues{ {1, 0, 1, 0} };
	inline std::array<float, 4> gMaxPerlinValues{ {1, 0, 0, 1} };

	float getHeightAtPoint(const glm::vec2& worldPos, const CommonBufferTypes::TerrainParams& terrainParams, float terrainScale);
	float getHeightWithPerlin(const CommonBufferTypes::TerrainParams& terrainParams, const std::array<float, 4>& perlinValues);
	std::array<float, 4> getMinHeightPerlinValues(const CommonBufferTypes::TerrainParams& terrainParams);
	std::array<float, 4> getMaxHeightPerlinValues(const CommonBufferTypes::TerrainParams& terrainParams);
}

#endif