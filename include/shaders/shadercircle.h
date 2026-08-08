#ifndef SHADER_CIRCLE_H
#define SHADER_CIRCLE_H

#include "shaderi.h"
#include "vertexarrayscreenquad.h"
#include "inputspace.h"

class FramebufferColour;

class ShaderCircle : public ShaderI {
public:
	ShaderCircle(const std::string& vertPath, const std::string& fragPath);
	void render(const FramebufferColour* const framebuffer, const glm::vec3& circleCenter, float radius, InputSpace inputSpace, const glm::vec3& colour, float framebufferAspectRatio) const;

private:
	VertexArrayScreenQuad mScreenQuad;
};

#endif#pragma once
