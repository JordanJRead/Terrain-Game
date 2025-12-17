#include "shaders/shaderphysics.h"

ShaderPhysics::ShaderPhysics(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath }
{
}

void ShaderPhysics::render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const {
	internalRender(framebuffer, vertexArray, false);
}

void ShaderPhysics::render(const VertexArray& vertexArray) const {
	internalRenderDefaultFramebuffer(vertexArray, false);
}