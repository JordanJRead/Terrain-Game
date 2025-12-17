#include "shaders/shaderterraindepth.h"
#include "imagecount.h"
#include "terrainrenderer.h"

ShaderTerrainDepth::ShaderTerrainDepth(const std::string& vertPath, const std::string& fragPath) : ShaderTerrainChunkI{ vertPath, fragPath } {}

void ShaderTerrainDepth::setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, int shellCount, const Cubemap& skybox) {
	use();
	int i{ 0 };
	for (; i < ImageCount; ++i) {
		terrainRenderer.bindTerrainImage(i, i);
	}
	setVector3("planePos", { planePos.x, 0, planePos.z });
	setFloat("planeWorldWidth", planeWidth);
}

void ShaderTerrainDepth::render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, true, 1);
}