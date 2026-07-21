#ifndef SHADER_TERRAIN_IMAGE
#define SHADER_TERRAIN_IMAGE

#include "shaders/shaderi.h"
#include "glm/glm.hpp"

class VertexArrayScreenQuad;

class ShaderTerrainImage : public ShaderI {
public:
	ShaderTerrainImage(const std::string& vertPath, const std::string& fragPath);
	void render(const FramebufferI* const framebuffer, const VertexArrayScreenQuad& quad, const glm::vec2& worldPos, float worldSize) const;
};

#endif