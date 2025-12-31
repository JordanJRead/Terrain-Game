#ifndef TERRAIN_CHUNK_BUFFER_H
#define TERRAIN_CHUNK_BUFFER_H

#include "glm/glm.hpp"
#include <vector>
#include <array>
#include "OpenGLObjects/BUF.h"

template <int ChunkQualityCount>
class TerrainChunkBuffer {
public:
	TerrainChunkBuffer(int bindingIndex) {

		mBuffer.use(GL_SHADER_STORAGE_BUFFER);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_STATIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, mBuffer);
	
	}
	int flush(int qualityIndex) {
		mBuffer.use(GL_SHADER_STORAGE_BUFFER);
		glBufferData(GL_SHADER_STORAGE_BUFFER, mVectors[qualityIndex].size() * sizeof(float), mVectors[qualityIndex].data(), GL_DYNAMIC_DRAW);

		int layerCount{ mVectors[qualityIndex].size() / 3 };
		mVectors[qualityIndex].clear();
		return layerCount;
	}

	void addChunk(int qualityIndex, const glm::vec2& worldPos, int shellCount) {
		for (int shellI{ -1 }; shellI < shellCount; ++shellI) {
			addChunkShell(qualityIndex, worldPos, (shellI + 1) / shellCount);
		}
	}

private:
	BUF mBuffer;
	std::array<std::vector<float>, ChunkQualityCount> mVectors; // each vector is: x, y, shell, x, y, shell, ...

	void addChunkShell(int qualityIndex, const glm::vec2& worldPos, float shellProgress) {
		mVectors[qualityIndex].push_back(worldPos.x);
		mVectors[qualityIndex].push_back(worldPos.y);
		mVectors[qualityIndex].push_back(shellProgress);
	}
};

#endif