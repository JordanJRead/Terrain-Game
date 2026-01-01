#ifndef TERRAIN_CHUNK_BUFFER_H
#define TERRAIN_CHUNK_BUFFER_H

#include "glm/glm.hpp"
#include <vector>
#include <array>
#include "OpenGLObjects/BUF.h"

template <int ChunkQualityCount>
class ChunkBuffers {
public:
	ChunkBuffers(int bindingIndex) {

		mBuffer.use(GL_SHADER_STORAGE_BUFFER);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_STATIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, mBuffer);
	}

	int flushTerrain(int qualityIndex) {
		mBuffer.use(GL_SHADER_STORAGE_BUFFER);
		glBufferData(GL_SHADER_STORAGE_BUFFER, mTerrainVectors[qualityIndex].size() * sizeof(float), mTerrainVectors[qualityIndex].data(), GL_DYNAMIC_DRAW);

		int layerCount{ (int)mTerrainVectors[qualityIndex].size() / 3 };
		mTerrainVectors[qualityIndex].clear();
		return layerCount;
	}

	int flushWater() {
		mBuffer.use(GL_SHADER_STORAGE_BUFFER);
		glBufferData(GL_SHADER_STORAGE_BUFFER, mWaterData.size() * sizeof(float), mWaterData.data(), GL_DYNAMIC_DRAW);

		int chunkCount{ (int)mWaterData.size() / 2 };
		mWaterData.clear();
		return chunkCount;
	}

	void addTerrainChunk(int qualityIndex, const glm::vec2& worldPos, int shellCount) {
		for (int layerIndex{ 0 }; layerIndex <= shellCount; ++layerIndex) {
			if (shellCount == 0)
				addTerrainChunkShell(qualityIndex, worldPos, 0);
			else
				addTerrainChunkShell(qualityIndex, worldPos, (float)layerIndex / shellCount);
		}
	}

	void addWaterChunk(const glm::vec2& worldPos) {
		mWaterData.push_back(worldPos.x);
		mWaterData.push_back(worldPos.y);
	}

private:
	BUF mBuffer;
	std::array<std::vector<float>, ChunkQualityCount> mTerrainVectors; // each vector is: x, y, shell, x, y, shell, ...
	std::vector<float> mWaterData; // x, y, x, y, ...

	void addTerrainChunkShell(int qualityIndex, const glm::vec2& worldPos, float shellProgress) {
		mTerrainVectors[qualityIndex].push_back(worldPos.x);
		mTerrainVectors[qualityIndex].push_back(worldPos.y);
		mTerrainVectors[qualityIndex].push_back(shellProgress);
	}
};

#endif