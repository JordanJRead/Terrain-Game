#ifndef SHADER_DEFERRED_TERRAIN
#define SHADER_DEFERRED_TERRAIN

#include "shaders/shaderi.h"
#include "shaderterrainchunki.h"

class ShaderTerrainDeferred : public ShaderTerrainChunkI {
public:
	ShaderTerrainDeferred(const std::string& vertPath, const std::string& fragPath);
	void setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, int shellCount, const Cubemap& skybox) override;
	void render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const override;

private:
	int mShellCount{ 0 };
};

#endif