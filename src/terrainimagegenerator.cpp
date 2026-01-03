#include "terrainimagegenerator.h"
#include <vector>
#include <iostream>
#include "vertexarray.h"
#include "shaders/shaderterrainimage.h"

TerrainImageGenerator::TerrainImageGenerator(int pixelDim, float worldSize, int screenWidth, int screenHeight, const glm::vec2& worldPos)
	: mWorldSize{ worldSize }
	, mScreenWidth{ screenWidth }
	, mScreenHeight{ screenHeight }
	, mPixelDim{ pixelDim }
	, mWorldPos{ worldPos }
	, mFramebuffer{ 1, pixelDim, pixelDim, GL_RGBA32F, false }
{}

// Clears image
void TerrainImageGenerator::updatePixelDim(int pixelDim) {
	mPixelDim = pixelDim;
	mFramebuffer.updateDimensions(0, pixelDim, pixelDim);
}

void TerrainImageGenerator::bindImage(int unit) const {
	mFramebuffer.bindColourTexture(0, unit);
}

void TerrainImageGenerator::setWorldSize(float worldSize) {
	mWorldSize = worldSize;
}

void TerrainImageGenerator::setWorldPos(const glm::vec2& worldPos) {
	mWorldPos = worldPos;
}

void TerrainImageGenerator::updateTexture(const VertexArray& screenQuad, const ShaderTerrainImage& terrainImageShader) {
	terrainImageShader.setRenderData(mWorldPos, mWorldSize);
	terrainImageShader.render(mFramebuffer, screenQuad);
}