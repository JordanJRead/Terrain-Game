#ifndef TERRAIN_CHUNK_BUFFER_H
#define TERRAIN_CHUNK_BUFFER_H

#include "glm/glm.hpp"
#include <vector>
#include <array>
#include "OpenGLObjects/BUF.h"
#include <iostream>
#include "gpubuffer.h"

template <int ChunkQualityCount>
class ChunkBuffers {
public:
	ChunkBuffers(int startBindingIndex) {
		for (int i{ 0 }; i < ChunkQualityCount; ++i) {
			mTerrainBuffers[i].initialize(startBindingIndex + i);
		}
		mWaterBuffer.initialize(startBindingIndex + ChunkQualityCount);
	}

	int flushTerrain(int qualityIndex) {
		mTerrainBuffers[qualityIndex].setData(mTerrainVectors[qualityIndex]);
		int layerCount{ (int)mTerrainVectors[qualityIndex].size() / 3 };
		mTerrainVectors[qualityIndex].clear();
		return layerCount;
	}

	int flushWater() {
		mWaterBuffer.setData(mWaterData);
		int chunkCount{ (int)mWaterData.size() / 2 };
		mWaterData.clear();
		return chunkCount;
	}

	void addTerrainChunk(int qualityIndex, const glm::vec2& worldPos, int shellCount) {
		for (int layerIndex{ shellCount }; layerIndex >= 0; --layerIndex) {
			mTerrainVectors[qualityIndex].push_back(worldPos.x);
			mTerrainVectors[qualityIndex].push_back(worldPos.y);
			mTerrainVectors[qualityIndex].push_back((float)layerIndex / shellCount);
		}
	}

	void addWaterChunk(const glm::vec2& worldPos) {
		mWaterData.push_back(worldPos.x);
		mWaterData.push_back(worldPos.y);
	}

private:
	std::array<GPUBuffer, ChunkQualityCount> mTerrainBuffers;
	GPUBuffer mWaterBuffer;
	std::array<std::vector<float>, ChunkQualityCount> mTerrainVectors; // each vector is: x, y, shell, x, y, shell, ...
	std::vector<float> mWaterData; // x, y, x, y, ...
};

#endif