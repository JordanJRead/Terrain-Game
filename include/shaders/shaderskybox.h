#ifndef SHADER_SKYBOX
#define SHADER_SKYBOX

#include "shaders/shaderi.h"

class Cubemap;
class CubeVertices;

class ShaderSkybox : public ShaderI {
public:
	ShaderSkybox(const std::string& vertPath, const std::string& fragPath);
	void render(const FramebufferI* const framebuffer, const CubeVertices& cube, const Cubemap& cubemap) const;
};

#endif