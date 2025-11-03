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
		for (int i{ 0 }; i < 4; ++i) {
			std::string indexString{ std::to_string(i) };

			mShaderDeferred.use();
			mShaderDeferred.setInt("GBuffer_GroundWorldPosShellIndex", 4);
			mShaderDeferred.setInt("GBuffer_WorldPosMountain", 5);
			mShaderDeferred.setInt("GBUFFER_NormalDoesTexelExist", 6); // TOD Make lowercase, it's like this in the shader too
			mShaderDeferred.setInt("images[" + indexString + "]", i);
			mShaderTerrainGeometry.use();
			mShaderTerrainGeometry.setInt("images[" + indexString + "]", i);
			mShaderWaterGeometry.use();
			mShaderWaterGeometry.setInt("images[" + indexString + "]", i);
		}
	}
	void useFramebuffer() { mFramebuffer.use(); }
	void useShaderTerrainGeometryPass() { mShaderTerrainGeometry.use(); }
	void useShaderWaterGeometryPass() { mShaderWaterGeometry.use(); }
	void setTerrainPlaneInfo(const glm::vec3& planePos, float planeWidth) {
		mShaderTerrainGeometry.setVector3("planePos", planePos);
		mShaderTerrainGeometry.setFloat("planeWorldWidth", planeWidth);
	}
	void setWaterPlaneInfo(const glm::vec3& planePos, float planeWidth) {
		mShaderWaterGeometry.setVector3("planePos", planePos);
		mShaderWaterGeometry.setFloat("planeWorldWidth", planeWidth);
	}
	void doDeferredShading(const Framebuffer<1>& targetFramebuffer, const VertexArray& screenQuad) {
		mFramebuffer.use();
		mFramebuffer.bindColourTexture(0, 4);
		mFramebuffer.bindColourTexture(1, 5);
		mFramebuffer.bindColourTexture(2, 6);

		mShaderDeferred.use();
		targetFramebuffer.use();
		screenQuad.use();
		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, screenQuad.getIndexCount(), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
	}

private:
	Shader mShaderTerrainGeometry;
	Shader mShaderWaterGeometry;
	Shader mShaderDeferred;
	Framebuffer<3> mFramebuffer;
};

#endif