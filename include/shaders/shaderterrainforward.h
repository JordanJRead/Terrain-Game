#ifndef SHADER_FORWARD_TERRAIN
#define SHADER_FORWARD_TERRAIN

#include "shaders/shaderi.h"
#include "shaderterrainchunki.h"

class ShaderTerrainForward : public ShaderTerrainChunkI {
public:
	ShaderTerrainForward(const std::string& vertPath, const std::string& fragPath);
	void setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, int shellCount, const Cubemap& skybox) override;
	void render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const override;

private:
	int mShellCount{ 0 };
};

#endif