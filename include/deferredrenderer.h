#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "framebuffercolour.h"
#include "shaders/shaderdeferred.h"
#include "shaders/shaderchunk.h"

class TerrainRenderer;
class VertexArray;

class DeferredRenderer {
public:
	DeferredRenderer(int screenWidth, int screenHeight);
	void bindGBufferTexture(int i, int unit) const;
	void doDeferredShading(const FramebufferColour& targetFramebuffer, const TerrainRenderer& terrainRenderer, const VertexArray& screenQuad);

	ShaderChunk mShaderTerrainDeferred;
	ShaderChunk mShaderWaterDeferred;
	ShaderDeferred mShaderDeferred;
	FramebufferColour mFramebuffer;
};

#endif