#ifndef SHADER_LINE_H
#define SHADER_LINE_H

#include "shaderi.h"
#include "vertexarray.h"
#include "inputspace.h"

class FramebufferColour;

class ShaderLine : public ShaderI {
public:
	ShaderLine(const std::string& vertPath, const std::string& fragPath);
	void render(const FramebufferColour* const framebuffer, const glm::vec3& linePos1, const glm::vec3& linePos2, InputSpace inputSpace, const glm::vec3& startColour, const glm::vec3& endColour) const;

private:
	VertexArray mLineVertexArray;
};

#endif