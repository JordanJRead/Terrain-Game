#include "shaders/shaderterrainimage.h"
#include "glm/glm.hpp"
#include "vertexarrayscreenquad.h"

ShaderTerrainImage::ShaderTerrainImage(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {}

void ShaderTerrainImage::render(const FramebufferI* const framebuffer, const VertexArrayScreenQuad& quad, const glm::vec2& worldPos, float worldSize) const {
	use();
	setVector2("worldPos", worldPos);
	setFloat("scale", worldSize);
	glDisable(GL_BLEND);
	internalRender(framebuffer, quad.getVertexArray(), false);
}