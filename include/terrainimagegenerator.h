#ifndef TERRAIN_H
#define TERRAIN_H

#include "OpenGLObjects/FBO.h"
#include "OpenGLObjects/TEX.h"
#include "glm/glm.hpp"

class VertexArray;
class Shader;

class TerrainImageGenerator {
public:
	TerrainImageGenerator(int pixelDim, float worldSize, int screenWidth, int screenHeight, const glm::vec2& worldPos, bool isLast = false);
	void bindImage(int unit);

	int getPixelDim() { return mPixelDim; }
	float getWorldSize() { return mWorldSize; }
	const glm::vec2& getWorldPos() { return mWorldPos; }
	float getMaxHeight() { return mMaxHeight; }

	void updatePixelDim(int pixelDim);
	void setWorldSize(float worldSize);
	void setWorldPos(const glm::vec2& worldPos);
	void updateTexture(const VertexArray& screenQuad, const Shader& terrainImageShader);
	unsigned long long getTextureHandle() { return mTextureHandle; }

private:
	FBO mFBO;
	TEX mColorTex;
	unsigned long long mTextureHandle;

	int mPixelDim;
	float mWorldSize;

	int mScreenWidth;
	int mScreenHeight;
	glm::vec2 mWorldPos;

	float mMaxHeight;
	bool mIsLast;
};

#endif