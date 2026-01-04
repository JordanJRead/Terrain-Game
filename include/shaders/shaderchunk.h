#ifndef SHADER_TERRAIN_CHUNK_I
#define SHADER_TERRAIN_CHUNK_I

#include "shaders/shaderi.h"
#include "glm/glm.hpp"

class TerrainRenderer;
class Cubemap;

class ShaderChunk : public ShaderI {
public:
	ShaderChunk(const std::string& vertPath, const std::string& fragPath);
	void setRenderData(const TerrainRenderer& terrainRenderer, float planeWidth, int instanceCount, const Cubemap& skybox);
	virtual void render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const;
	int getInstanceCount();

protected:
	int mInstanceCount{};
};

#endif