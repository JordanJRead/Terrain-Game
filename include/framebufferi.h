#ifndef FRAMEBUFFER_I
#define FRAMEBUFFER_I

#include "OpenGLObjects/FBO.h"

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

protected:
	int mWidth;
	int mHeight;
	FBO mFBO;
};

#endif