#include "shaders/shaderwaterdeferred.h"
#include "framebuffercolour.h"
#include "deferredrenderer.h"
#include "shadowmapper.h"
#include "vertexarrayscreenquad.h"
#include "noise.h"

ShaderWaterDeferred::ShaderWaterDeferred(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	int textureUnit{ 0 };

	setInt("GBuffer_TerrainGroundWorldPos", textureUnit++);
	setInt("GBuffer_TerrainWorldPos", textureUnit++);
	setInt("GBuffer_TerrainNormal", textureUnit++);
	setInt("GBuffer_ShellProgressMountainDoesTexelExist", textureUnit++);
	setInt("GBuffer_WaterWorldPos", textureUnit++);
	setInt("GBuffer_WaterNormal", textureUnit++);
	setIVector2("debugFragCoord", { -1, -1 });

	setInt("sceneSource", textureUnit++);

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

void ShaderWaterDeferred::render(const FramebufferI* const framebuffer, const VertexArrayScreenQuad& screenQuad, const TEX& sourceSceneTexture, const FramebufferColour& geometryBuffer, const Noise& noiseTexture,
	const ShadowMapper<CascadeCount>& shadowMapperSun, const ShadowMapper<CascadeCount>& shadowMapperMoon, const glm::ivec2& debugFragPos) const
{
	use();
	setIVector2("debugFragCoord", debugFragPos);
	int textureUnit{ 0 };

	geometryBuffer.bindColourTexture(0, textureUnit++);
	geometryBuffer.bindColourTexture(1, textureUnit++);
	geometryBuffer.bindColourTexture(2, textureUnit++);
	geometryBuffer.bindColourTexture(3, textureUnit++);
	geometryBuffer.bindColourTexture(4, textureUnit++);
	geometryBuffer.bindColourTexture(5, textureUnit++);
	sourceSceneTexture.bind(GL_TEXTURE_2D, textureUnit++);

	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		shadowMapperSun.getFramebuffer(cascadeI).getDepthTexture().bind(GL_TEXTURE_2D, textureUnit++);
	}

	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		shadowMapperMoon.getFramebuffer(cascadeI).getDepthTexture().bind(GL_TEXTURE_2D, textureUnit++);
	}

	noiseTexture.use(textureUnit++);
	internalRender(framebuffer, screenQuad.getVertexArray(), false);
}