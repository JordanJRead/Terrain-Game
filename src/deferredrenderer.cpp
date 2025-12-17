#include "vertexarray.h"
#include "terrainrenderer.h"
#include "deferredrenderer.h"

DeferredRenderer::DeferredRenderer(int screenWidth, int screenHeight)
	: mFramebuffer{ 3, screenWidth, screenHeight, GL_RGBA32F }
	, mShaderTerrainDeferred{ "assets/shaders/terraingeometrypass.vert", "assets/shaders/terraingeometrypass.frag" }
	, mShaderWaterDeferred{ "assets/shaders/watergeometrypass.vert", "assets/shaders/watergeometrypass.frag" }
	, mShaderDeferred{ "assets/shaders/deferredshader.vert", "assets/shaders/deferredshader.frag" }
{
}

void DeferredRenderer::bindGBufferTexture(int i, int unit) const {
	mFramebuffer.bindColourTexture(i, unit);
}

void DeferredRenderer::doDeferredShading(const Framebuffer& targetFramebuffer, const TerrainRenderer& terrainRenderer, const VertexArray& screenQuad) {
	mShaderDeferred.setRenderData(terrainRenderer);
	mShaderDeferred.render(targetFramebuffer, screenQuad);
}