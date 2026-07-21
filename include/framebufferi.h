#ifndef FRAMEBUFFER_I
#define FRAMEBUFFER_I

#include "OpenGLObjects/FBO.h"
#include <iostream>
#include <glm/glm.hpp>

class FramebufferI {
public:
	FramebufferI(FramebufferI&&) = default;
	FramebufferI& operator=(FramebufferI&&) = default;
	FramebufferI(int width, int height) : mWidth{ width }, mHeight{ height } {}
	virtual ~FramebufferI() = 0;
	void use() const {
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
		glViewport(0, 0, mWidth, mHeight);
	}
	void clear(const glm::vec4 colour, GLbitfield mask) const {
		use();
		glClearColor(colour.x, colour.y, colour.z, colour.w);
		glClear(mask);
	}

protected:
	int mWidth;
	int mHeight;
	FBO mFBO;
};

#endif