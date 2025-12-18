#ifndef SHADER_TERRAIN_IMAGE
#define SHADER_TERRAIN_IMAGE

#include "shaders/shaderi.h"
#include "glm/glm.hpp"

class ShaderTerrainImage : public ShaderI {
public:
	ShaderTerrainImage(const std::string& vertPath, const std::string& fragPath);
	void setRenderData(const glm::vec2& worldPos, float worldSize) const;
	void render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const override;
};

#endif