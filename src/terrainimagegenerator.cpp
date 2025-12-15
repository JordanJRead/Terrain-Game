#include "terrainimagegenerator.h"
#include <vector>
#include <iostream>
#include "vertexarray.h"
#include "shader.h"

TerrainImageGenerator::TerrainImageGenerator(int pixelDim, float worldSize, int screenWidth, int screenHeight, const glm::vec2& worldPos)
	: mWorldSize{ worldSize }
	, mScreenWidth{ screenWidth }
	, mScreenHeight{ screenHeight }
	, mPixelDim{ pixelDim }
	, mWorldPos{ worldPos }
	, mFramebuffer{ pixelDim, pixelDim, GL_RGBA32F, false }
{}

// Clears image
void TerrainImageGenerator::updatePixelDim(int pixelDim) {
	mPixelDim = pixelDim;
	mFramebuffer.updateDimensions(0, pixelDim, pixelDim);
}

void TerrainImageGenerator::bindImage(int unit) {
	mFramebuffer.bindColourTexture(0, unit);
}

void TerrainImageGenerator::setWorldSize(float worldSize) {
	mWorldSize = worldSize;
}

void TerrainImageGenerator::setWorldPos(const glm::vec2& worldPos) {
	mWorldPos = worldPos;
}

void TerrainImageGenerator::updateTexture(const VertexArray& screenQuad, const Shader& terrainImageShader) {
	glViewport(0, 0, mPixelDim, mPixelDim);
	mFramebuffer.use();
	glClear(GL_COLOR_BUFFER_BIT);
	screenQuad.use();
	terrainImageShader.use();

	terrainImageShader.setFloat("scale", mWorldSize);
	terrainImageShader.setVector2("worldPos", mWorldPos);

	glDisable(GL_BLEND);
	glDrawElements(GL_TRIANGLES, screenQuad.getIndexCount(), GL_UNSIGNED_INT, 0);
	glEnable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, mScreenWidth, mScreenHeight);
}