#include "shaders/shaderskybox.h"
#include "cubemap.h"
#include "framebuffer.h"
#include "vertexarray.h"

ShaderSkybox::ShaderSkybox(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	setInt("skybox", 0);
}

void ShaderSkybox::setRenderData(const Cubemap& cubemap) {
	cubemap.bindTexture(0);
}

void ShaderSkybox::render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, false);
}