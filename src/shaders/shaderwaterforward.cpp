#include "shaders/shaderwaterforward.h"
#include "imagecount.h"
#include "terrainrenderer.h"
#include "cubemap.h"

ShaderWaterForward::ShaderWaterForward(const std::string& vertPath, const std::string& fragPath) : ShaderWaterChunkI{ vertPath, fragPath } {
	use();
	int i{ 0 };
	for (; i < ImageCount; ++i) {
		std::string indexString{ std::to_string(i) };
		setInt("images[" + indexString + "]", i);
	}
	setInt("skybox", ImageCount);
}

void ShaderWaterForward::setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, const Cubemap& skybox) const {
	use();
	int i{ 0 };
	for (; i < ImageCount; ++i) {
		terrainRenderer.bindTerrainImage(i, i);
	}
	setVector3("planePos", planePos);
	setFloat("planeWorldWidth", planeWidth);
	skybox.bindTexture(ImageCount);
}

void ShaderWaterForward::render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, true);
}