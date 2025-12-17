#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <string>
#include <string_view>

class Framebuffer;
class VertexArray;

class ShaderI {
public:
	ShaderI(const ShaderI&) = delete;
	ShaderI(ShaderI&&) = delete;

	ShaderI(const std::string& vertPath, const std::string& fragPath);
	~ShaderI() { glDeleteProgram(mID); }
	virtual void render(const Framebuffer& framebuffer, const VertexArray& vertexArray) const = 0;

protected:
	unsigned int mID;

	void internalRender(const Framebuffer& framebuffer, const VertexArray& vertexArray, bool depth, int instanceCount = 0) const;

	void internalRenderDefaultFramebuffer(const VertexArray& vertexArray, bool depth, int instanceCount = 0) const;

	void use() const { glUseProgram(mID); }
	void setMatrix4(std::string_view name, const glm::mat4& mat4) const {
		glUniformMatrix4fv(glGetUniformLocation(mID, name.data()), 1, false, glm::value_ptr(mat4));
	}
	void setVector3(std::string_view name, const glm::vec3& v) const {
		glUniform3fv(glGetUniformLocation(mID, name.data()), 1, glm::value_ptr(v));
	}
	void setVector2(std::string_view name, const glm::vec2& v) const {
		glUniform2fv(glGetUniformLocation(mID, name.data()), 1, glm::value_ptr(v));
	}
	void setFloat(std::string_view name, float f) const {
		glUniform1f(glGetUniformLocation(mID, name.data()), f);
	}
	void setUInt(std::string_view name, unsigned int n) const {
		glUniform1ui(glGetUniformLocation(mID, name.data()), n);
	}
	void setInt(std::string_view name, int n) const {
		glUniform1i(glGetUniformLocation(mID, name.data()), n);
	}
	void setBool(std::string_view name, bool b) const {
		glUniform1ui(glGetUniformLocation(mID, name.data()), b);
	}
};

#endif