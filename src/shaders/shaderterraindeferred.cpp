#include "shaders/shaderterraindeferred.h"
#include "framebuffercolour.h"
#include "terrainimageset.h"
#include "deferredrenderer.h"
#include "shadowmapper.h"
#include "vertexarrayscreenquad.h"
#include "noise.h"

ShaderTerrainDeferred::ShaderTerrainDeferred(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	int textureUnit{ 0 };
	for (; textureUnit < ImageCount; ++textureUnit) {
		std::string indexString{ std::to_string(textureUnit) };
		setInt("images[" + indexString + "]", textureUnit);
	}

	setInt("GBuffer_TerrainGroundWorldPos", textureUnit++);
	setInt("GBuffer_TerrainWorldPos", textureUnit++);
	setInt("GBuffer_TerrainNormal", textureUnit++);
	setInt("GBuffer_ShellProgressMountainDoesTexelExist", textureUnit++);

	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		std::string indexString{ std::to_string(cascadeI) };
		setInt("shadowMapsSun[" + indexString + "]", textureUnit++);
	}

	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		std::string indexString{ std::to_string(cascadeI) };
		setInt("shadowMapsMoon[" + indexString + "]", textureUnit++);
	}

	setInt("blueNoise", textureUnit++);
}

void ShaderTerrainDeferred::render(const FramebufferI* const framebuffer, const VertexArrayScreenQuad& screenQuad, const TerrainImageSet& terrainImageSet, const FramebufferColour& geometryBuffer, const Noise& noiseTexture, const ShadowMapper<CascadeCount>& shadowMapperSun, const ShadowMapper<CascadeCount>& shadowMapperMoon) const
{
	int textureUnit{ 0 };
	for (; textureUnit < ImageCount; ++textureUnit) {
		terrainImageSet.getImage(textureUnit).bindTexture(textureUnit);
	}

	geometryBuffer.bindColourTexture(0, textureUnit++);
	geometryBuffer.bindColourTexture(1, textureUnit++);
	geometryBuffer.bindColourTexture(2, textureUnit++);
	geometryBuffer.bindColourTexture(3, textureUnit++);

	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		shadowMapperSun.getFramebuffer(cascadeI).getDepthTexture().bind(GL_TEXTURE_2D, textureUnit++);
	}

	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		shadowMapperMoon.getFramebuffer(cascadeI).getDepthTexture().bind(GL_TEXTURE_2D, textureUnit++);
	}
	noiseTexture.use(textureUnit++);
	internalRender(framebuffer, screenQuad.getVertexArray(), false);
}