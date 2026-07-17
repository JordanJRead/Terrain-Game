#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include "chunksbuffer.h"
#include "chunksbuffernoshells.h"
#include "planequalityset.h"
#include "shellqualityset.h"
#include <vector>
#include "vertexarray.h"
#include <optional>
#include "glm/glm.hpp"
#include "commonbuffertypes.h"
#include "OpenGLObjects/BUF.h"

class CameraI;

class ChunkManager {
public:
	ChunkManager(int qualityCount, int bufferBindingIndex, float terrainSpan, int chunkCount, const CommonBufferTypes::TerrainParams& terrainParams);
	void renderUI();

	float getChunkWidth() const { return mTerrainSpan / mChunkCount; }
	float getTerrainSpan() const { return mTerrainSpan; }
	void populateBuffers(const CameraI& camera, bool skipShells = false, bool forceLowQuality = false, bool frustumCulling = true, bool simpleWater = true);
	std::optional<std::pair<const VertexArray&, int>> flushSomeTerrain();
	std::optional<std::pair<const VertexArray&, int>> flushSomeWater();

private:
	BUF mChunkDataBuffer;

	std::vector<ChunksBuffer> mTerrainChunkBuffers;
	std::vector<ChunksBufferNoShells> mWaterChunkBuffers;
	PlaneQualitySet mTerrainPlanes;
	PlaneQualitySet mWaterPlanes;
	ShellQualitySet mShellQualities;
	size_t mCurrentTerrainQualityIndex{ 0 };
	size_t mCurrentWaterQualityIndex{ 0 };

	float mTerrainSpan;
	int mChunkCount;
	float mMinTerrainHeight;
	float mMaxTerrainHeight;
};

#endif
