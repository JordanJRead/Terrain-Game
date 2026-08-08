#include "shaders/shaderline.h"

#include "framebuffercolour.h"

ShaderLine::ShaderLine(const std::string& vertPath, const std::string& fragPath) : ShaderI{ vertPath, fragPath } {
	std::vector<float> vertexData{ 0, 1 };
	std::vector<unsigned int> vertexIndices{ 0, 1 };
	std::vector<int> vertexLayout{ 1 };
	mLineVertexArray.create(vertexData, vertexIndices, vertexLayout);
}

void ShaderLine::render(const FramebufferColour* const framebuffer, const glm::vec3& linePos1, const glm::vec3& linePos2, InputSpace inputSpace, const glm::vec3& startColour, const glm::vec3& endColour) const {
	use();
	setVector3("linePos1", linePos1);
	setVector3("linePos2", linePos2);
	setInt("spaceType", (int)inputSpace);
	setVector3("lineStartColour", startColour);
	setVector3("lineEndColour", endColour);
	internalRender(framebuffer, mLineVertexArray, false, -1, GL_LINES);
}