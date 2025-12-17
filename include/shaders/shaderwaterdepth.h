#ifndef SHADER_DEPTH_WATER
#define SHADER_DEPTH_WATER

#include "shaders/shaderi.h"
#include "shaderwaterchunki.h"

class Cubemap;
class TerrainRenderer;

class ShaderWaterDepth : public ShaderWaterChunkI {
public:
	ShaderWaterDepth(const std::string& vertPath, const std::string& fragPath);
	void setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, const Cubemap& skybox) const override;
	void render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const override;
};

#endif