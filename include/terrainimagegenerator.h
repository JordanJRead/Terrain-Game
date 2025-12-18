#ifndef TERRAIN_H
#define TERRAIN_H

#include "OpenGLObjects/FBO.h"
#include "glm/glm.hpp"
#include "framebuffercolour.h"
#include "shaders/shaderterrainimage.h"

class VertexArray;
class ShaderI;

class TerrainImageGenerator {
public:
	TerrainImageGenerator(int pixelDim, float worldSize, int screenWidth, int screenHeight, const glm::vec2& worldPos);
	void bindImage(int unit) const;

	int getPixelDim() { return mPixelDim; }
	float getWorldSize() { return mWorldSize; }
	const glm::vec2& getWorldPos() { return mWorldPos; }

	void updatePixelDim(int pixelDim);
	void setWorldSize(float worldSize);
	void setWorldPos(const glm::vec2& worldPos);
	void updateTexture(const VertexArray& screenQuad, const ShaderTerrainImage& terrainImageShader); // TODO make shader static?

private:
	FramebufferColour mFramebuffer;

	int mPixelDim;
	float mWorldSize;

	int mScreenWidth;
	int mScreenHeight;
	glm::vec2 mWorldPos;
};

#endif