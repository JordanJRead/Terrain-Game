#ifndef SHADER_PLANE
#define SHADER_PLANE

#include "shaders/shaderi.h"

class PlaneGPU;

class ShaderPlane : public ShaderI {
public:
	ShaderPlane(const std::string& vertPath, const std::string& fragPath);
	void render(const FramebufferI* const framebuffer, const PlaneGPU& plane) const;
};

#endif