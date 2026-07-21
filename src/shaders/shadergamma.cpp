#include "shaders/shadergamma.h"
#include "OpenGLObjects/TEX.h"
#include "noise.h"
#include "vertexarrayscreenquad.h"

ShaderGamma::ShaderGamma(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	use();
	setInt("source", 0);
	setInt("noise", 1);
}

void ShaderGamma::render(const FramebufferI* const framebuffer, const VertexArrayScreenQuad& screenQuad, const TEX& sourceTexture, const Noise& noise) const {
	sourceTexture.bind(GL_TEXTURE_2D, 0);
	noise.use(1);
	internalRender(framebuffer, screenQuad.getVertexArray(), false);
}