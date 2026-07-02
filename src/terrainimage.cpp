#include "terrainimage.h"
#include <vector>
#include <iostream>
#include "vertexarray.h"
#include "shaders/shaderterrainimage.h"
#include "mathhelper.h"
#include "imgui/imgui.h"

TerrainImage::TerrainImage(int pixelDim, float worldSize, const glm::vec3& worldPos, float terrainScale)
	: mWorldSize{ worldSize }
	, mPixelDim{ pixelDim }
	, mWorldPos{ getClosestWorldPixelPos(worldPos, terrainScale) }
	, mFramebuffer{ 1, pixelDim, pixelDim, GL_RGBA32F, false }
{}

// Clears image
void TerrainImage::updatePixelDimAndClear() {
	mFramebuffer.updateDimensions(0, mPixelDim, mPixelDim);
}

void TerrainImage::bindImage(int unit) const {
	mFramebuffer.bindColourTexture(0, unit);
}

void TerrainImage::updateTexture(const VertexArray& screenQuad, const ShaderTerrainImage& terrainImageShader) {
	terrainImageShader.setRenderData(mWorldPos, mWorldSize);
	terrainImageShader.render(mFramebuffer, screenQuad);
}

glm::vec3 TerrainImage::getClosestWorldPixelPos(const glm::vec3 pos, float terrainScale) const {
	return MathHelper::getClosestWorldStepPosition(pos, mWorldSize / mPixelDim * terrainScale);
}

void TerrainImage::renderUIAndUpdate(float minSize, const glm::vec3& cameraPos, float terrainScale, bool hasTerrainParamsChanged, const VertexArray& screenQuad, const ShaderTerrainImage& terrainImageShader) {
	ImGui::PushID(this);

	float prevWorldSize{ mWorldSize };
	ImGui::DragFloat("World size", &mWorldSize, 1, 1, 100000);
	bool hasWorldSizeChanged{ prevWorldSize != mWorldSize };

	if (mWorldSize < minSize)
		mWorldSize = minSize;

	int prevPixelDim{ mPixelDim };
	ImGui::InputInt("Pixel quality", &mPixelDim, 100, 1000);
	bool hasPixelDimChanged{ prevPixelDim != mPixelDim };

	ImGui::PopID();

	if (hasPixelDimChanged) {
		updatePixelDimAndClear();
	}

	// Move image along with the player
	bool hasPositionChanged{ false };
	glm::vec2 scaledCameraPos{ glm::vec2(cameraPos.x, cameraPos.z) / terrainScale };
	double cameraDistFromImageCenter{ glm::length(scaledCameraPos - mWorldPos) };
	if (cameraDistFromImageCenter * 2 > 0.2 * mWorldSize) { // If near edge of image, update image
		glm::vec3 pixelPosition{ getClosestWorldPixelPos(cameraPos / terrainScale, terrainScale) };
		mWorldPos = glm::vec2(pixelPosition.x, pixelPosition.z);
		hasPositionChanged = true;
	}

	if (hasPixelDimChanged || hasWorldSizeChanged || hasPositionChanged || hasTerrainParamsChanged) {
		updateTexture(screenQuad, terrainImageShader);
	}
}