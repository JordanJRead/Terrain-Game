#ifndef SHADER_DEPTH_TERRAIN
#define SHADER_DEPTH_TERRAIN

#include "shaderterrainchunki.h"

class TerrainRenderer;
class Cubemap;

class ShaderTerrainDepth : public ShaderTerrainChunkI {
public:
	ShaderTerrainDepth(const std::string& vertPath, const std::string& fragPath);
	void setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, int shellCount, const Cubemap& skybox) override;
	void render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const override;
};

#endif