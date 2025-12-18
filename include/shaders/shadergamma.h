#ifndef SHADER_GAMMA
#define SHADER_GAMMA

#include "shaders/shaderi.h"

class TEX;

class ShaderGamma : public ShaderI {
public:
	ShaderGamma(const std::string& vertPath, const std::string& fragPath);
	void setRenderData(const TEX& sourceTexture);
	void render(const FramebufferI& framebuffer, const VertexArray& vertexArray) const override;
	void render(const VertexArray& vertexArray) const;
};

#endif