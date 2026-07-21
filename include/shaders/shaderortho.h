#ifndef SHADER_ORTHO
#define SHADER_ORTHO

#include "shaders/shaderi.h"

class TEX;

class ShaderOrtho : public ShaderI {
public:
	ShaderOrtho(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {}
	void render(const FramebufferI* const framebuffer, const VertexArray& vertexArray, const glm::vec3& colour) const {
		use();
		setVector3("colour", colour);
		internalRender(framebuffer, vertexArray, true);
	}
};

#endif