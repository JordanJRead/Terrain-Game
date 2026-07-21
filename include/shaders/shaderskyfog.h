#ifndef SHADER_SKY_FOG_H
#define SHADER_SKY_FOG_H

#include "shaderi.h"
#include "constants.h"
#include "shadowmapper.h"

class FramebufferColour;
class VertexArrayScreenQuad;

class ShaderSkyFog : public ShaderI {
public:
	ShaderSkyFog(const std::string& vertPath, const std::string& fragPath);

	void render(const FramebufferColour* const framebuffer, const VertexArrayScreenQuad& screenQuad,
		const FramebufferColour& shadedSceneBuffer,
		const ShadowMapper<CascadeCount>& shadowMapperSun, const ShadowMapper<CascadeCount>& shadowMapperMoon);
};

#endif