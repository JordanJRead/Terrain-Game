#include "chunkmanager.h"

#include "imgui/imgui.h"
#include "camerai.h"
#include "mathhelper.h"
#include "heightfunction.h"
#include <iostream>

ChunkManager::ChunkManager(int qualityCount, int bufferBindingIndex, float terrainSpan, int chunkCount, const CommonBufferTypes::TerrainParams& terrainParams)
	: mTerrainSpan{ terrainSpan }
	, mChunkCount{ chunkCount }
	, mTerrainPlanes{ 4.0f, std::array{1.0f / 6, 1.0f / 25}, std::array{ 256.0f, 1000.0f }, terrainSpan / chunkCount }
	//, mWaterPlanes{ 0.0f, std::array<float, 0>{}, std::array<float, 0>{}, terrainSpan / chunkCount }
	, mWaterPlanes{ 2.0f, std::array<float, 1>{0.5f}, std::array<float, 1>{240.0f}, terrainSpan / chunkCount }
	, mShellQualities{ 30, std::array{ 0.7f }, std::array{ 200.0f } }
{
	mMinTerrainHeight = HeightFunction::getHeightWithPerlin(terrainParams, HeightFunction::gMinPerlinValues);
	mMaxTerrainHeight = HeightFunction::getHeightWithPerlin(terrainParams, HeightFunction::gMaxPerlinValues);

	mChunkDataBuffer.bind(GL_SHADER_STORAGE_BUFFER);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bufferBindingIndex, mChunkDataBuffer);
}

void ChunkManager::renderUI() {
	ImGui::DragFloat("Span", &mTerrainSpan, 1, 1, 100000);
	ImGui::DragInt("Count", &mChunkCount, 1, 1, 1000);

	float chunkWidth{ mTerrainSpan / mChunkCount };

	ImGui::SeparatorText("Terrain planes");
	mTerrainPlanes.renderUI(chunkWidth);

	ImGui::SeparatorText("Water planes");
	mWaterPlanes.renderUI(chunkWidth);

	ImGui::SeparatorText("Shells");
	mShellQualities.renderUI();

	// Update buffers
	if (mTerrainChunkBuffers.size() != mTerrainPlanes.getPlaneCount()) {
		mTerrainChunkBuffers.resize(mTerrainPlanes.getPlaneCount());
	}
	if (mWaterChunkBuffers.size() != mWaterPlanes.getPlaneCount()) {
		mWaterChunkBuffers.resize(mWaterPlanes.getPlaneCount());
	}
}

void ChunkManager::populateBuffers(const CameraI& camera, bool skipShells, bool forceLowQuality, bool frustumCulling, bool simpleWater) {
	for (int x{ -mChunkCount / 2 }; x <= mChunkCount / 2; ++x) {
		for (int z{ -mChunkCount / 2 }; z <= mChunkCount / 2; ++z) {
			float chunkWidth{ mTerrainSpan / mChunkCount };
			glm::vec3 chunkPos{ MathHelper::getClosestWorldStepPosition(camera.getPosition(), chunkWidth) + glm::vec3(x * chunkWidth, 0, z * chunkWidth) };

			// Frustum culling
			std::array<float, 2> xVals{ chunkPos.x - chunkWidth / 2.0f, chunkPos.x + chunkWidth / 2.0f };
			std::array<float, 2> yVals{ mMinTerrainHeight, mMaxTerrainHeight };
			std::array<float, 2> zVals{ chunkPos.z - chunkWidth / 2.0f, chunkPos.z + chunkWidth / 2.0f };

			bool isVisible{ true };
			if (frustumCulling)
				isVisible = camera.isAABBVisible({ {chunkPos.x - chunkWidth / 2.0f, mMinTerrainHeight, chunkPos.z - chunkWidth / 2.0f}, {chunkPos.x + chunkWidth / 2.0f, mMaxTerrainHeight, chunkPos.z + chunkWidth / 2.0f} });

			if (isVisible) {
				bool isNeighbourChunck{ x >= -1 && x <= 1 && z >= -1 && z <= 1 };
				float chunkDistance{ glm::length(chunkPos - camera.getPosition()) };

				int shellCount{ isNeighbourChunck ? mShellQualities.getHighQualityShellCount() : mShellQualities.getShellCountAtDistance(chunkDistance) };
				if (skipShells) {
					shellCount = 0;
				}
				size_t terrainQualityIndex{ forceLowQuality ? mTerrainPlanes.getPlaneCount() - 1 : mTerrainPlanes.getPlaneIndexAtDistance(chunkDistance) };
				size_t waterQualityIndex{ forceLowQuality ? mWaterPlanes.getPlaneCount() - 1 : mWaterPlanes.getPlaneIndexAtDistance(chunkDistance) };

				mTerrainChunkBuffers[terrainQualityIndex].addChunk(glm::vec2{ chunkPos.x, chunkPos.z }, shellCount);
				if (!simpleWater || (x == 0 && z == 0))
					mWaterChunkBuffers[waterQualityIndex].addChunk(glm::vec2{ chunkPos.x, chunkPos.z });
			}
		}
	}
}

std::optional<std::pair<const VertexArray&, int>> ChunkManager::flushSomeTerrain() {
	if (mCurrentTerrainQualityIndex >= mTerrainPlanes.getPlaneCount()) {
		mCurrentTerrainQualityIndex = 0;
		return std::nullopt;
	}
	const VertexArray& planeVertexArray{ mTerrainPlanes.getPlaneAtIndex(mCurrentTerrainQualityIndex).getVertexArray() };
	int instanceCount{ mTerrainChunkBuffers[mCurrentTerrainQualityIndex].flushChunks(mChunkDataBuffer) };
	mCurrentTerrainQualityIndex++;
	return std::pair<const VertexArray&, int>{ planeVertexArray, instanceCount };
}

std::optional<std::pair<const VertexArray&, int>> ChunkManager::flushSomeWater() {
	if (mCurrentWaterQualityIndex >= mWaterPlanes.getPlaneCount()) {
		mCurrentWaterQualityIndex = 0;
		return std::nullopt;
	}
	const VertexArray& planeVertexArray{ mWaterPlanes.getPlaneAtIndex(mCurrentWaterQualityIndex).getVertexArray() };
	int instanceCount{ mWaterChunkBuffers[mCurrentWaterQualityIndex].flushChunks(mChunkDataBuffer) };
	mCurrentWaterQualityIndex++;
	return std::pair<const VertexArray&, int>{ planeVertexArray, instanceCount };
}
