#ifndef SHADER_WATER_DEFERRED_H
#define SHADER_WATER_DEFERRED_H

#include "constants.h"
#include "shaders/shaderi.h"
#include "shadowmapper.h"

class VertexArrayScreenQuad;
class Noise;
class FramebufferI;
class FramebufferColour;

class ShaderWaterDeferred : public ShaderI {
public:
	ShaderWaterDeferred(const std::string& vertPath, const std::string& fragPath);
	void render(const FramebufferI* const framebuffer, const VertexArrayScreenQuad& screenQuad, const FramebufferColour& geometryBuffer, const Noise& noiseTexture,
		const ShadowMapper<CascadeCount>& shadowMapperSun, const ShadowMapper<CascadeCount>& shadowMapperMoon) const;
};


#endif