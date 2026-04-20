#include "shaders/shadergamma.h"
#include "OpenGLObjects/TEX.h"
#include "noise.h"

ShaderGamma::ShaderGamma(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	setInt("source", 0);
	setInt("noise", 1);
}

void ShaderGamma::setRenderData(const TEX& sourceTexture, const Noise& noise) {
	sourceTexture.bind(GL_TEXTURE_2D, 0);
	noise.use(1);
}

void ShaderGamma::render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, false);
}

void ShaderGamma::render(const VertexArray& vertexArray) const {
	internalRenderDefaultFramebuffer(vertexArray, false);
}