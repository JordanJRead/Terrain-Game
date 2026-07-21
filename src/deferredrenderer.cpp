#include "vertexarrayscreenquad.h"
#include "deferredrenderer.h"
#include <array>
#include "terrainimageset.h"
#include "shadowmapper.h"
#include "constants.h"

DeferredRenderer::DeferredRenderer(int screenWidth, int screenHeight)
	: mGeometryFramebuffer{ 6, screenWidth, screenHeight, GL_RGB32F }
	, mShadedTerrainFramebuffer{ 1, screenWidth, screenHeight, GL_RGB32F }
	, mShadedTerrainAndWaterFramebuffer{ 1, screenWidth, screenHeight, GL_RGB32F }
	, mShaderTerrainGeometry{ "assets/shaders/terraingeometrypass.vert", "assets/shaders/terraingeometrypass.frag", std::array{0, 1, 2, 3} }
	, mShaderWaterGeometry{ "assets/shaders/watergeometrypass.vert", "assets/shaders/watergeometrypass.frag", std::array{4, 5} }
	, mShaderTerrainDeferred{ "assets/shaders/terraindeferred.vert", "assets/shaders/terraindeferred.frag" }
	//, mShaderWaterDeferred{ "assets/shaders/waterdeferred.vert", "assets/shaders/waterdeferred.frag" }
	//, mShaderSkyAndFog{ "assets/shaders/skyfog.vert", "assets/shaders/skyfog.frag" }
{
}

void DeferredRenderer::bindGBufferTexture(int i, int unit) const {
	mGeometryFramebuffer.bindColourTexture(i, unit);
}

void DeferredRenderer::clearGeometryBuffers() const {
	glEnable(GL_DEPTH_TEST);
	mGeometryFramebuffer.setTargetBuffers(std::array{ 0, 1, 2, 3, 4, 5 });
	mGeometryFramebuffer.clear({ 0, 0, 0, 0 }, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DeferredRenderer::doDeferredShading(const FramebufferColour* const targetFramebuffer, const TerrainImageSet& terrainImageSet, const VertexArrayScreenQuad& screenQuad, const ShadowMapper<CascadeCount>& shadowMapperSun, const ShadowMapper<CascadeCount>& shadowMapperMoon) {
	glDisable(GL_DEPTH_TEST);
	mShadedTerrainFramebuffer.clear({ -1, -1, -1, -1 }, GL_COLOR_BUFFER_BIT);
	mShadedTerrainAndWaterFramebuffer.clear({ -1, -1, -1, -1 }, GL_COLOR_BUFFER_BIT);

	mShaderTerrainDeferred.render(targetFramebuffer, screenQuad, terrainImageSet, mGeometryFramebuffer, mBlueNoise, shadowMapperSun, shadowMapperMoon);
	//mShaderWaterDeferred.render(&mShadedTerrainAndWaterFramebuffer, screenQuad, mGeometryFramebuffer, mBlueNoise, shadowMapperSun, shadowMapperMoon); // Will copy shaded terrain buffer too TODO add src texture?
	//mShaderSkyAndFog.render(targetFramebuffer, screenQuad, mShadedTerrainAndWaterFramebuffer, shadowMapperSun, shadowMapperMoon);
}