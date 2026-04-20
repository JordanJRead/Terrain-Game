#ifndef TERRAIN_CHUNK_BUFFER_H
#define TERRAIN_CHUNK_BUFFER_H

#include "glm/glm.hpp"
#include <vector>
#include <array>
#include "OpenGLObjects/BUF.h"
#include <iostream>

template <int ChunkQualityCount>
class ChunkBuffers {
public:
	ChunkBuffers(int bindingIndex) {
		mBuffer.bind(GL_SHADER_STORAGE_BUFFER);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, mBuffer);
	}

	int flushTerrain(int qualityIndex) {
		mBuffer.bind(GL_SHADER_STORAGE_BUFFER);

		size_t byteCount{ mTerrainVectors[qualityIndex].size() * sizeof(float) };
		if (byteCount > mMaxBytes) {
			glBufferData(GL_SHADER_STORAGE_BUFFER, byteCount, mTerrainVectors[qualityIndex].data(), GL_DYNAMIC_DRAW);
			mMaxBytes = byteCount;
		}
		else {
			glBufferData(GL_SHADER_STORAGE_BUFFER, mMaxBytes, nullptr, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, byteCount, mTerrainVectors[qualityIndex].data());
		}

		int layerCount{ (int)mTerrainVectors[qualityIndex].size() / 2 };
		mTerrainVectors[qualityIndex].clear();
		return layerCount;
	}

	int flushWater() {
		mBuffer.bind(GL_SHADER_STORAGE_BUFFER);

		size_t byteCount{ mWaterData.size() * sizeof(float) };
		if (byteCount > mMaxBytes) {
			glBufferData(GL_SHADER_STORAGE_BUFFER, byteCount, mWaterData.data(), GL_DYNAMIC_DRAW);
			mMaxBytes = byteCount;
		}
		else {
			glBufferData(GL_SHADER_STORAGE_BUFFER, mMaxBytes, nullptr, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, byteCount, mWaterData.data());
		}

		int chunkCount{ (int)mWaterData.size() / 2 };
		mWaterData.clear();
		return chunkCount;
	}

	void addTerrainChunk(int qualityIndex, const glm::vec2& worldPos, int shellCount) {
		mTerrainVectors[qualityIndex].push_back(worldPos.x);
		mTerrainVectors[qualityIndex].push_back(worldPos.y);
	}

	void addWaterChunk(const glm::vec2& worldPos) {
		mWaterData.push_back(worldPos.x);
		mWaterData.push_back(worldPos.y);
	}

private:
	BUF mBuffer;
	std::array<std::vector<float>, ChunkQualityCount> mTerrainVectors; // each vector is: x, y, x, y, ...
	std::vector<float> mWaterData; // x, y, x, y, ...
	int mMaxBytes{ 0 };
};

#endif