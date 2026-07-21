#include "shaders/shaderskyfog.h"
#include "framebuffercolour.h"

ShaderSkyFog::ShaderSkyFog(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	int textureUnit{ 0 };

	setInt("shadedSceneTexture", textureUnit++);

	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		std::string indexString{ std::to_string(cascadeI) };
		setInt("shadowMapsSun[" + indexString + "]", textureUnit++);
	}

	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		std::string indexString{ std::to_string(cascadeI) };
		setInt("shadowMapsMoon[" + indexString + "]", textureUnit++);
	}
}

void ShaderSkyFog::render(const FramebufferColour* const framebuffer, const VertexArrayScreenQuad& screenQuad,
	const FramebufferColour& shadedSceneBuffer,
	const ShadowMapper<CascadeCount>& shadowMapperSun, const ShadowMapper<CascadeCount>& shadowMapperMoon)
{
	int textureUnit{ 0 };

	shadedSceneBuffer.bindColourTexture(0, textureUnit++);

	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		shadowMapperSun.getFramebuffer(cascadeI).getDepthTexture().bind(GL_TEXTURE_2D, textureUnit++);
	}

	for (int cascadeI{ 0 }; cascadeI < CascadeCount; ++cascadeI) {
		shadowMapperMoon.getFramebuffer(cascadeI).getDepthTexture().bind(GL_TEXTURE_2D, textureUnit++);
	}

}