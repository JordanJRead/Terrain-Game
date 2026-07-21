#ifndef SHADER_TERRAIN_DEFERRED
#define SHADER_TERRAIN_DEFERRED

#include "constants.h"
#include "shaders/shaderi.h"
#include "shadowmapper.h"

class TerrainImageSet;
class VertexArrayScreenQuad;
class Noise;
class FramebufferColour;

class ShaderTerrainDeferred : public ShaderI {
public:
	ShaderTerrainDeferred(const std::string& vertPath, const std::string& fragPath);
	void render(
		const FramebufferI* const framebuffer, 
		const VertexArrayScreenQuad& screenQuad, 
		const TerrainImageSet& terrainImageSet,
		const FramebufferColour& geometryBuffer,
		const Noise& noiseTexture,
		const ShadowMapper<CascadeCount>& shadowMapperSun,
		const ShadowMapper<CascadeCount>& shadowMapperMoon
	) const;
};

#endif