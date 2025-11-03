#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "OpenGLObjects/TEX.h"
#include "OpenGLObjects/RBO.h"
#include "shader.h"
#include "framebuffer.h"
#include "vertexarray.h"
#include "glad/glad.h"

class DeferredRenderer {
public:
	DeferredRenderer(int screenWidth, int screenHeight)
		: mFramebuffer{ screenWidth, screenHeight }
		, mShaderTerrainGeometry{ "assets/shaders/terraingeometrypass.vert", "assets/shaders/terraingeometrypass.frag" }
		, mShaderWaterGeometry{ "assets/shaders/watergeometrypass.vert", "assets/shaders/watergeometrypass.frag" }
		, mShaderDeferred{ "assets/shaders/deferredshader.vert", "assets/shaders/deferredshader.frag" }
	{
	}
	void useFramebuffer() { mFramebuffer.use(); }
	void useTerrainGeometryShaderPass() { mShaderTerrainGeometry.use(); }
	void useWaterGeometryShaderPass() { mShaderWaterGeometry.use(); }
	void doDeferredShading(const Framebuffer<1>& targetFramebuffer, const VertexArray& screenQuad) {
		mFramebuffer.use();
		mFramebuffer.bindColourTexture(0, 4);
		mFramebuffer.bindColourTexture(1, 5);
		mFramebuffer.bindColourTexture(2, 6);
		mShaderDeferred.use();
		mShaderDeferred.setInt("GBuffer_GroundWorldPosShellIndex", 4);
		mShaderDeferred.setInt("GBuffer_WorldPosMountain", 5);
		mShaderDeferred.setInt("GBUFFER_NormalDoesTexelExist", 6);

		targetFramebuffer.use();
		screenQuad.use();
		glDrawElements(GL_TRIANGLES, screenQuad.getIndexCount(), GL_UNSIGNED_INT, 0);
	}

private:
	Shader mShaderTerrainGeometry;
	Shader mShaderWaterGeometry;
	Shader mShaderDeferred;
	Framebuffer<3> mFramebuffer;
};

#endif