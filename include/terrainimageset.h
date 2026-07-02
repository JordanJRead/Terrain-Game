#ifndef TERRAIN_IMAGE_SET_H
#define TERRAIN_IMAGE_SET_H

#include "terrainimage.h"
#include <vector>
#include "glm/glm.hpp"
#include <cassert>

class VertexArray;
class ShaderTerrainImage;

class TerrainImageSet {
public:
	TerrainImageSet(int imageCount, const glm::vec3& cameraPos, float terrainScale, const VertexArray& screenQuad, const ShaderTerrainImage& imageShader);
	void renderUIAndUpdate(bool hasTerrainParamsChanged, const glm::vec3& cameraPos, float terrainScale, const VertexArray& screenQuad, const ShaderTerrainImage& terrainImageShader);
	const TerrainImage& getImage(size_t i) const;
	std::vector<glm::vec2> getImagePositions() const;
	std::vector<float> getImageWorldWidths() const;
	size_t getImageCount() const { return mImages.size(); }

private:
	std::vector<TerrainImage> mImages;
};

#endif