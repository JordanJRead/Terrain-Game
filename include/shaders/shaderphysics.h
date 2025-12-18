#ifndef SHADER_PHYSICS
#define SHADER_PHYSICS

#include "shaders/shaderi.h"

class ShaderPhysics : public ShaderI {
public:
	ShaderPhysics(const std::string& vertPath, const std::string& fragPath);
	void render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const override;
	void render(const VertexArray& vertexArray) const;
};

#endif