#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "framebuffercolour.h"
#include "shaders/shaderterraindeferred.h"
#include "shaders/shaderwaterdeferred.h"
#include "shaders/shaderchunkgeometry.h"
#include "shaders/shaderskyfog.h"
#include "noise.h"

class TerrainImageSet;
class VertexArrayScreenQuad;

class DeferredRenderer {
public:
	DeferredRenderer(int screenWidth, int screenHeight);
	void bindGBufferTexture(int i, int unit) const;
	void clearGeometryBuffers() const;
	void bindNoiseTexture(int unit) const { mBlueNoise.use(unit); }
	void doDeferredShading(const FramebufferColour* const targetFramebuffer, const TerrainImageSet& terrainImageSet, const VertexArrayScreenQuad& screenQuad, const ShadowMapper<CascadeCount>& shadowMapperSun, const ShadowMapper<CascadeCount>& shadowMapperMoon);
	const Noise& getNoise() const { return mBlueNoise; }

	ShaderChunkGeometry mShaderTerrainGeometry;
	ShaderChunkGeometry mShaderWaterGeometry;
	ShaderTerrainDeferred mShaderTerrainDeferred;
	//ShaderWaterDeferred mShaderWaterDeferred;
	//ShaderSkyFog mShaderSkyAndFog;

	FramebufferColour mGeometryFramebuffer; // (terrainGroundWorldPos), (terrainWorldPos), (terrainNormal), (shellProgress, mountain, doesTexelExist), (waterWorldPos), (waterNormal)
	FramebufferColour mShadedTerrainFramebuffer;
	FramebufferColour mShadedTerrainAndWaterFramebuffer;

	Noise mBlueNoise{ "assets/bluenoise.png" };
};

#endif