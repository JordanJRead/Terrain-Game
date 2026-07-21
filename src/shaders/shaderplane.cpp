#include "shaders/shaderplane.h"
#include "planegpu.h"

ShaderPlane::ShaderPlane(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath }
{}

void ShaderPlane::render(const FramebufferI* const framebuffer, const PlaneGPU& plane) const {
	internalRender(framebuffer, plane.getVertexArray(), false);
}
