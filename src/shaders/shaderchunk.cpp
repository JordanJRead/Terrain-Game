#include "shaders/shaderchunk.h"
#include "shaders/shaderi.h"
#include "glm/glm.hpp"
#include "constants.h"
#include "terrainrenderer.h"
#include "cubemap.h"

ShaderChunk::ShaderChunk(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath }, mInstanceCount{ 0 } {
	use();
	for (int i{ 0 }; i < ImageCount; ++i) {
		std::string indexString{ std::to_string(i) };
		setInt("images[" + indexString + "]", i);
	}
	setInt("skybox", ImageCount);
}

void ShaderChunk::setRenderData(const TerrainRenderer& terrainRenderer, float planeWidth, int instanceCount, const Cubemap& skybox) {
	use();
	int i{ 0 };
	for (; i < ImageCount; ++i) {
		terrainRenderer.bindTerrainImage(i, i);
	}
	setFloat("planeWorldWidth", planeWidth);
	mInstanceCount = instanceCount;
	skybox.bindTexture(ImageCount);
}

void ShaderChunk::render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, true, mInstanceCount);
}

int ShaderChunk::getInstanceCount() { return mInstanceCount; }
