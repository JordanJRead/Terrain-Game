#include "shaders/shadercircle.h"

#include "framebuffercolour.h"

ShaderCircle::ShaderCircle(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath }
{}

void ShaderCircle::render(const FramebufferColour* const framebuffer, const glm::vec3& circleCenter, float radius, InputSpace inputSpace, const glm::vec3& colour, float framebufferAspectRatio) const {
	use();
	setVector3("circleCenter", circleCenter);
	setFloat("radius", radius);
	setInt("spaceType", (int)inputSpace);
	setVector3("colour", colour);
	setFloat("framebufferAspectRatio", framebufferAspectRatio);
	internalRender(framebuffer, mScreenQuad.getVertexArray(), false);
}