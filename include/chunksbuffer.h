#ifndef TERRAIN_CHUNK_BUFFER_H
#define TERRAIN_CHUNK_BUFFER_H

#include "glm/glm.hpp"
#include <vector>
#include <array>
#include "OpenGLObjects/BUF.h"
#include <iostream>

class ChunksBuffer {
public:
	int flushChunks(const BUF& buffer);
	void addChunk(const glm::vec2& worldPos, int additionalShellCount);

private:
	std::vector<float> mChunkData; // x, y, [shellProgress], x, y, [shellProgress], ...
	int mMaxBytes{ 0 };
	int mBufferBindingIndex;

protected:
	bool mStoreShellData{ true };
};

#endif
