#include "shaders/shaderterrainforward.h"
#include "imagecount.h"
#include "terrainrenderer.h"
#include "cubemap.h"

ShaderTerrainForward::ShaderTerrainForward(const std::string& vertPath, const std::string& fragPath) : ShaderTerrainChunkI{ vertPath, fragPath } {
	setInt("skybox", ImageCount);
}

void ShaderTerrainForward::setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, int shellCount, const Cubemap& skybox) {
	use();
	int i{ 0 };
	for (; i < ImageCount; ++i) {
		terrainRenderer.bindTerrainImage(i, i);
	}
	setVector3("planePos", { planePos.x, 0, planePos.z });
	setFloat("planeWorldWidth", planeWidth);
	mShellCount = shellCount;
	skybox.bindTexture(ImageCount);
}

void ShaderTerrainForward::render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, true, mShellCount + 1);
}