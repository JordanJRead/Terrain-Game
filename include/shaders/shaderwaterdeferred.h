#ifndef SHADER_DEFERRED_WATER
#define SHADER_DEFERRED_WATER

#include "shaderwaterchunki.h"
#include <string>

class TerrainRenderer;
class VertexArray;
class Cubemap;

class ShaderWaterDeferred : public ShaderWaterChunkI {
public:
	ShaderWaterDeferred(const std::string& vertPath, const std::string& fragPath);
	void setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, const Cubemap& skybox) const override;
	void render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const override;
};

#endif