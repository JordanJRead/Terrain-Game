#include "shaders/shaderdeferred.h"
#include "imagecount.h"
#include "deferredrenderer.h"
#include "terrainrenderer.h"

ShaderDeferred::ShaderDeferred(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	int textureUnit{ 0 };
	for (; textureUnit < ImageCount; ++textureUnit) {
		std::string indexString{ std::to_string(textureUnit) };
		setInt("images[" + indexString + "]", textureUnit);
	}
	setInt("GBuffer_GroundWorldPosShellProgress", textureUnit++);
	setInt("GBuffer_WorldPosMountain", textureUnit++);
	setInt("GBuffer_NormalDoesTexelExist", textureUnit++);
	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		std::string indexString{ std::to_string(cascadeI) };
		setInt("shadowMaps[" + indexString + "]", textureUnit++);
	}
	setInt("blueNoise", textureUnit++);
}

void ShaderDeferred::setRenderData(const TerrainRenderer& terrainRenderer) {
	int textureUnit{ 0 };
	for (; textureUnit < ImageCount; ++textureUnit) {
		terrainRenderer.bindTerrainImage(textureUnit, textureUnit);
	}

	const DeferredRenderer& deferredRenderer{ terrainRenderer.getDeferredRenderer() };
	deferredRenderer.bindGBufferTexture(0, textureUnit++);
	deferredRenderer.bindGBufferTexture(1, textureUnit++);
	deferredRenderer.bindGBufferTexture(2, textureUnit++);

	const ShadowMapper<CascadeCount>& shadowMapper{ terrainRenderer.getShadowMapper() };
	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		shadowMapper.getFramebuffer(cascadeI).getDepthTexture().use(GL_TEXTURE_2D, textureUnit++);
	}

	deferredRenderer.bindNoiseTexture(textureUnit++);
}

void ShaderDeferred::render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, false);
}