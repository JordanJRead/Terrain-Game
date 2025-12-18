#include "shaders/shaderwaterdepth.h"
#include "imagecount.h"

ShaderWaterDepth::ShaderWaterDepth(const std::string& vertPath, const std::string& fragPath) : ShaderWaterChunkI{ vertPath, fragPath } {}

void ShaderWaterDepth::setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, const Cubemap& skybox) const {
	use();
	setVector3("planePos", planePos);
	setFloat("planeWorldWidth", planeWidth);
}

void ShaderWaterDepth::render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, true);
}