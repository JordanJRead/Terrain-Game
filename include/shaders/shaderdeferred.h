#ifndef SHADER_DEFERRED
#define SHADER_DEFERRED

#include "shaders/shaderi.h"

class TerrainRenderer;

class ShaderDeferred : public ShaderI {
public:
	ShaderDeferred(const std::string& vertPath, const std::string& fragPath);
	void setRenderData(const TerrainRenderer& terrainRenderer);
	void render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const override;
};

#endif