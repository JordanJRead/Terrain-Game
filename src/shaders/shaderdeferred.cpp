#include "shaders/shaderdeferred.h"
#include "imagecount.h"
#include "deferredrenderer.h"
#include "terrainrenderer.h"

ShaderDeferred::ShaderDeferred(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	int i{ 0 };
	for (; i < ImageCount; ++i) {
		std::string indexString{ std::to_string(i) };
		setInt("images[" + indexString + "]", i);
	}
	setInt("GBuffer_GroundWorldPosShellProgress", i++);
	setInt("GBuffer_WorldPosMountain", i++);
	setInt("GBuffer_NormalDoesTexelExist", i++);
}

void ShaderDeferred::setRenderData(const TerrainRenderer& terrainRenderer) {
	int i{ 0 };
	for (; i < ImageCount; ++i) {
		terrainRenderer.bindTerrainImage(i, i);
	}

	const DeferredRenderer& deferredRenderer{ terrainRenderer.getDeferredRenderer() };
	deferredRenderer.bindGBufferTexture(0, i++);
	deferredRenderer.bindGBufferTexture(1, i++);
	deferredRenderer.bindGBufferTexture(2, i++);
}

void ShaderDeferred::render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, false);
}