#ifndef SHADER_GAMMA
#define SHADER_GAMMA

#include "shaders/shaderi.h"

class TEX;
class Noise;
class FramebufferI;
class VertexArrayScreenQuad;

class ShaderGamma : public ShaderI {
public:
	ShaderGamma(const std::string& vertPath, const std::string& fragPath);
	void render(const FramebufferI* const framebuffer, const VertexArrayScreenQuad& screenQuad, const TEX& sourceTexture, const Noise& noiseTexture) const;
};

#endif