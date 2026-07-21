#ifndef TERRAIN_IMAGE_H
#define TERRAIN_IMAGE_H

#include "OpenGLObjects/FBO.h"
#include "glm/glm.hpp"
#include "framebuffercolour.h"
#include "shaders/shaderterrainimage.h"

class VertexArrayScreenQuad;
class ShaderI;

class TerrainImage {
public:
	TerrainImage(int pixelDim, float worldSize, const glm::vec3& worldPos, float terrainScale);
	void bindTexture(int unit) const;

	int getPixelDim() const { return mPixelDim; }
	float getWorldSize() const { return mWorldSize; }
	glm::vec2 getPosition() const { return mWorldPos; }
	const glm::vec2& getWorldPos() { return mWorldPos; }
	glm::vec3 getClosestWorldPixelPos(const glm::vec3 pos, float terrainScale) const;

	void updatePixelDimAndClear();
	void updateTexture(const VertexArrayScreenQuad& screenQuad, const ShaderTerrainImage& terrainImageShader);
	void renderUIAndUpdate(float minSize, const glm::vec3& cameraPos, float terrainScale, bool hasTerrainParamsChanged, const VertexArrayScreenQuad& screenQuad, const ShaderTerrainImage& terrainImageShader, bool renderUI);

private:
	FramebufferColour mFramebuffer;

	int mPixelDim;
	float mWorldSize;
	glm::vec2 mWorldPos;
};

#endif