#include "shaders/shadergamma.h"
#include "OpenGLObjects/TEX.h"

ShaderGamma::ShaderGamma(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	setInt("source", 0);
}

void ShaderGamma::setRenderData(const TEX& sourceTexture) {
	sourceTexture.use(GL_TEXTURE_2D, 0);
}

void ShaderGamma::render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, false);
}

void ShaderGamma::render(const VertexArray& vertexArray) const {
	internalRenderDefaultFramebuffer(vertexArray, false);
}