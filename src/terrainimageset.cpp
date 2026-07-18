#include "terrainimageset.h"
#include <array>
#include "vertexarray.h"
#include "shaders/shaderterrainimage.h"
#include "imgui/imgui.h"
/*
{ 2500, 1,   cameraPos, mArtisticParams.mValue.terrainScale },
{ 2500, 4,   cameraPos, mArtisticParams.mValue.terrainScale },
{ 2500, 12,  cameraPos, mArtisticParams.mValue.terrainScale },
{ 5000, 64,  cameraPos, mArtisticParams.mValue.terrainScale },
{ 5000, 300, cameraPos, mArtisticParams.mValue.terrainScale }
*/

TerrainImageSet::TerrainImageSet(int imageCount, const glm::vec3& cameraPos, float terrainScale, const VertexArray& screenQuad, const ShaderTerrainImage& imageShader) {
	static std::array<int, 5>   defaultDimensions{ {2500, 2500, 2500, 5000, 5000} };
	static std::array<float, 5> defaultWorldSizes{ {1, 4, 12, 64, 300} };

	mImages.reserve(imageCount);
	for (size_t i{ 0 }; i < imageCount; ++i) {
		mImages.emplace_back(
			i < 5 ? defaultDimensions[i] : defaultDimensions[4],
			i < 5 ? defaultWorldSizes[i] : defaultWorldSizes[4],
			cameraPos,
			terrainScale
		);
		mImages.back().updateTexture(screenQuad, imageShader);
	}
}

const TerrainImage& TerrainImageSet::getImage(size_t i) const {
	assert(i < mImages.size());
	return mImages[i];
}

void TerrainImageSet::renderUIAndUpdate(bool hasTerrainParamsChanged, const glm::vec3& cameraPos, float terrainScale, const VertexArray& screenQuad, const ShaderTerrainImage& terrainImageShader, bool renderUI) {
	// UI
	for (int i{ 0 }; i < mImages.size(); ++i) {
		std::string label{ "Image " };
		label += std::to_string(i + 1);
		if (renderUI)
		ImGui::SeparatorText(label.c_str());
		mImages[i].renderUIAndUpdate(i == 0 ? 0 : mImages[i - 1].getWorldSize(), cameraPos, terrainScale, hasTerrainParamsChanged, screenQuad, terrainImageShader, renderUI);
	}
}

std::vector<glm::vec2> TerrainImageSet::getImagePositions() const {
	std::vector<glm::vec2> positions;
	positions.reserve(mImages.size());
	for (const TerrainImage& image : mImages) {
		positions.push_back(image.getPosition());
	}
	return positions;
}

std::vector<float> TerrainImageSet::getImageWorldWidths() const {
	std::vector<float> widths;
	widths.reserve(mImages.size());
	for (const TerrainImage& image : mImages) {
		widths.push_back(image.getWorldSize());
	}
	return widths;
}