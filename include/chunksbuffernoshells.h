#ifndef TERRAIN_CHUNK_BUFFER_NO_SHELL_H
#define TERRAIN_CHUNK_BUFFER_NO_SHELL_H

#include "chunksbuffer.h"

class ChunksBufferNoShells : protected ChunksBuffer {
public:
	ChunksBufferNoShells() {
		mStoreShellData = false;
	}

	void addChunk(const glm::vec2& worldPos) {
		ChunksBuffer::addChunk(worldPos, 0);
	}

	int flushChunks(const BUF& buffer) {
		return ChunksBuffer::flushChunks(buffer);
	}
};

#endif