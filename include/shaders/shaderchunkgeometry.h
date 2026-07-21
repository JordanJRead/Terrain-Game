#ifndef SHADER_CHUNK_GEOMETRY_H
#define SHADER_CHUNK_GEOMETRY_H

#include "shaders/shaderchunk.h"
#include <cassert>
#include "framebuffercolour.h"
#include <span>
#include <vector>

class ShaderChunkGeometry : public ShaderChunk {
public:
	ShaderChunkGeometry(const std::string& vertPath, const std::string& fragPath, std::span<const int> targetBuffers) : ShaderChunk{ vertPath, fragPath } {
		mTargetBuffers.assign(targetBuffers.begin(), targetBuffers.end());
	}

	virtual void render(const FramebufferI* const framebuffer, const PlaneGPU& plane, const TerrainImageSet& terrainImageSet, float planeWidth, int instanceCount, const Cubemap& skybox) const override {
		assert(framebuffer != nullptr);
		if (const FramebufferColour* const fbColour = dynamic_cast<const FramebufferColour* const>(framebuffer)) {
			fbColour->setTargetBuffers(mTargetBuffers);
			ShaderChunk::render(framebuffer, plane, terrainImageSet, planeWidth, instanceCount, skybox);
		}
		else {
			assert(false);
		}
	}

private:
	std::vector<int> mTargetBuffers;
};

#endif