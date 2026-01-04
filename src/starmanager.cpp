#include "starmanager.h"
#include <array>
#include <vector>
#include "glm/glm.hpp"
#include "constants.h"
#include "mathhelper.h"
#include "random.h"

StarManager::StarManager(int bufferUnit) : mPrevParameters{ -1, -1, -1 }
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bufferUnit, mBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

int StarManager::getSplitIndex(const glm::vec3& dir) {
	return (int)((dir.y + 1) / 2 / (1.0f / StarYSplitCount));
}

void StarManager::update(const StarParameters& starParamters) {
	if (starParamters == mPrevParameters)
		return;
	mPrevParameters = starParamters;

	std::array<std::vector<glm::vec4>, StarYSplitCount> splitStars;
	std::vector<glm::vec4> concatStars;

	for (int i{ 0 }; i < starParamters.count; ++i) {
		glm::vec3 dir{ MathHelper::randUnitVector3() };
		float size{ starParamters.minSize + Random::getFloat() * (starParamters.maxSize - starParamters.minSize) };
		int splitIndex{ getSplitIndex(dir) };
		splitStars[splitIndex].push_back({ dir, size });
	}

	int totalStarIndex{ 0 };
	std::array<glm::ivec2, StarYSplitCount> indexData;
	for (int splitI{ 0 }; splitI < StarYSplitCount; ++splitI) {

		int splitStartIndex{ totalStarIndex };
		int splitStarCount{ static_cast<int>(splitStars[splitI].size()) };
		indexData[splitI] = glm::ivec2{ splitStartIndex, splitStarCount };
		for (int i{ 0 }; i < splitStarCount; ++i) {
			concatStars.push_back(splitStars[splitI][i]);
			++totalStarIndex;
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBuffer);

	int indexDataByteSize{ static_cast<int>(indexData.size() * sizeof(glm::ivec2)) };
	int concatStarsByteSize{ static_cast<int>(concatStars.size() * sizeof(glm::vec4)) };
	glBufferData(GL_SHADER_STORAGE_BUFFER, indexDataByteSize + concatStarsByteSize, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, indexDataByteSize, indexData.data());
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, indexDataByteSize, concatStarsByteSize, concatStars.data());

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}