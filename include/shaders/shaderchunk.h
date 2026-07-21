#ifndef SHADER_TERRAIN_CHUNK_I
#define SHADER_TERRAIN_CHUNK_I

#include "shaders/shaderi.h"
#include "glm/glm.hpp"

class TerrainImageSet;
class Cubemap;
class PlaneGPU;

class ShaderChunk : public ShaderI {
public:
	ShaderChunk(const std::string& vertPath, const std::string& fragPath);
	virtual void render(const FramebufferI* const framebuffer, const PlaneGPU& plane, const TerrainImageSet& terrainImageSet, float planeWidth, int instanceCount, const Cubemap& skybox) const;
};

#endif