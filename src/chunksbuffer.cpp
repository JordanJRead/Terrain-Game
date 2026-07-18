#include "chunksbuffer.h"

int ChunksBuffer::flushChunks(const BUF& buffer) {
	if (mChunkData.empty()) {
		return 0;
	}
	buffer.bind(GL_SHADER_STORAGE_BUFFER);

	size_t byteCount{ mChunkData.size() * sizeof(float) };
	if (byteCount > mMaxBytes) {
		glBufferData(GL_SHADER_STORAGE_BUFFER, byteCount, mChunkData.data(), GL_DYNAMIC_DRAW);
		mMaxBytes = byteCount;
	}
	else {
		glBufferData(GL_SHADER_STORAGE_BUFFER, mMaxBytes, nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, byteCount, mChunkData.data());
	}

	int layerCount{ (int)mChunkData.size() / (mStoreShellData ? 3 : 2) };
	mChunkData.clear();
	return layerCount;
}

void ChunksBuffer::addChunk(const glm::vec2& worldPos, int shellCount) {
	for (int layerIndex{ shellCount }; layerIndex >= 0; --layerIndex) {
		mChunkData.push_back(worldPos.x);
		mChunkData.push_back(worldPos.y);
		if (mStoreShellData) {
			if (shellCount != 0)
				mChunkData.push_back((float)layerIndex / shellCount);
			else
				mChunkData.push_back(0);
		}
	}
}
