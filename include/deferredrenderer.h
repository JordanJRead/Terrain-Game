#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "OpenGLObjects/TEX.h"
#include "OpenGLObjects/RBO.h"
#include "shader.h"
#include "framebuffer.h"
#include "vertexarray.h"
#include "glad/glad.h"
#include "imagecount.h"

class DeferredRenderer {
public:
	DeferredRenderer(int screenWidth, int screenHeight)
		: mFramebuffer{ screenWidth, screenHeight, GL_RGBA32F }
		, mShaderTerrainGeometry{ "assets/shaders/terraingeometrypass.vert", "assets/shaders/terraingeometrypass.frag" }
		, mShaderWaterGeometry{ "assets/shaders/watergeometrypass.vert", "assets/shaders/watergeometrypass.frag" }
		, mShaderDeferred{ "assets/shaders/deferredshader.vert", "assets/shaders/deferredshader.frag" }
	{
		mShaderDeferred.use();
		mShaderDeferred.setInt("GBuffer_GroundWorldPosShellProgress", 5);
		mShaderDeferred.setInt("GBuffer_WorldPosMountain", 6);
		mShaderDeferred.setInt("GBuffer_NormalDoesTexelExist", 7);
		for (int i{ 0 }; i < ImageCount; ++i) {
			std::string indexString{ std::to_string(i) };

			mShaderDeferred.use();
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
		mFramebuffer.bindColourTexture(0, 5); // TODO
		mFramebuffer.bindColourTexture(1, 6);
		mFramebuffer.bindColourTexture(2, 7);

		mShaderDeferred.use();
		targetFramebuffer.use();
		screenQuad.use();
		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLE_STRIP, screenQuad.getIndexCount(), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
	}

private:
	Shader mShaderTerrainGeometry;
	Shader mShaderWaterGeometry;
	Shader mShaderDeferred;
	Framebuffer<3> mFramebuffer;
};

#endif