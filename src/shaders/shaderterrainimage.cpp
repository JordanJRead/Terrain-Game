#include "shaders/shaderterrainimage.h"
#include "glm/glm.hpp"

ShaderTerrainImage::ShaderTerrainImage(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {}

void ShaderTerrainImage::setRenderData(const glm::vec2& worldPos, float worldSize) const {
	use();
	setVector2("worldPos", worldPos);
	setFloat("scale", worldSize);
}

void ShaderTerrainImage::render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const {
	glDisable(GL_BLEND);
	internalRender(framebuffer, vertexArray, false);
}