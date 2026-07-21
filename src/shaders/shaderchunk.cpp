#include "shaders/shaderchunk.h"
#include "shaders/shaderi.h"
#include "glm/glm.hpp"
#include "constants.h"
#include "terrainimageset.h"
#include "cubemap.h"
#include "planegpu.h"

ShaderChunk::ShaderChunk(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	for (int i{ 0 }; i < ImageCount; ++i) {
		std::string indexString{ std::to_string(i) };
		setInt("images[" + indexString + "]", i);
	}
	setInt("skybox", ImageCount);
}

void ShaderChunk::render(const FramebufferI* const framebuffer, const PlaneGPU& plane, const TerrainImageSet& terrainImageSet, float planeWidth, int instanceCount, const Cubemap& skybox) const {
	use();
	int i{ 0 };
	for (; i < ImageCount; ++i) {
		terrainImageSet.getImage(i).bindTexture(i);
	}
	skybox.bindTexture(ImageCount);

	setFloat("planeWorldWidth", planeWidth);
	internalRender(framebuffer, plane.getVertexArray(), true, instanceCount);
}
