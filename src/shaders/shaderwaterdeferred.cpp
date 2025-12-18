#include "shaders/shaderwaterdeferred.h"
#include "imagecount.h"
#include "terrainrenderer.h"
#include "cubemap.h"
#include "vertexarray.h"
#include "framebufferi.h"

#include <string>
#include "shaders/shaderwaterchunki.h"
#include "glm/glm.hpp"

ShaderWaterDeferred::ShaderWaterDeferred(const std::string& vertPath, const std::string& fragPath) : ShaderWaterChunkI{ vertPath, fragPath } {}

void ShaderWaterDeferred::setRenderData(const TerrainRenderer& terrainRenderer, const glm::vec3& planePos, float planeWidth, const Cubemap& skybox) const {
	use();
	int i{ 0 };
	for (; i < ImageCount; ++i) {
		terrainRenderer.bindTerrainImage(i, i);
	}
	setVector3("planePos", planePos);
	setFloat("planeWorldWidth", planeWidth);
}

void ShaderWaterDeferred::render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, true);
}