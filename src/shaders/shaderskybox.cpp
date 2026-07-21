#include "shaders/shaderskybox.h"
#include "cubemap.h"
#include "framebufferi.h"
#include "cubevertices.h"

ShaderSkybox::ShaderSkybox(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	setInt("skybox", 0);
}

void ShaderSkybox::render(const FramebufferI* const framebuffer, const CubeVertices& cube, const Cubemap& cubemap) const {
	cubemap.bindTexture(0);
	internalRender(framebuffer, cube.getVertexArray(), false);
}