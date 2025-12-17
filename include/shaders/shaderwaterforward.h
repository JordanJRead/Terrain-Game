#ifndef SHADER_FORWARD_WATER
#define SHADER_FORWARD_WATER

#include "shaders/shaderi.h"
#include "shaderwaterchunki.h"

class ShaderWaterForward : public ShaderWaterChunkI {
public:
	ShaderWaterForward(const std::string& vertPath, const std::string& fragPath);
	void setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, const Cubemap& skybox) const override;
	void render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const override;
};

#endif