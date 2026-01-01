#ifndef SHADER_ORTHO
#define SHADER_ORTHO

#include "shaders/shaderi.h"

class TEX;

class ShaderOrtho : public ShaderI {
public:
	ShaderOrtho(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {}
	void setColour(const glm::vec3& colour) {
		use();
		setVector3("colour", colour);
	}
	void render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const override {
		use();
		internalRender(framebuffer, vertexArray, true);
	}
};

#endif