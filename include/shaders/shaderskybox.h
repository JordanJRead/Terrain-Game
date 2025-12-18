#ifndef SHADER_SKYBOX
#define SHADER_SKYBOX

#include "shaders/shaderi.h"

class Cubemap;

class ShaderSkybox : public ShaderI {
public:
	ShaderSkybox(const std::string& vertPath, const std::string& fragPath);

	void setRenderData(const Cubemap& cubemap);

	void render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const override;
};

#endif