#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "OpenGLObjects/FBO.h"
#include "OpenGLObjects/TEX.h"
#include "OpenGLObjects/RBO.h"

class Framebuffer {
public:
	Framebuffer(int width, int height);
	void bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	}
	void bindTexture(int unit) const {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, mColourTex);
	}

private:
	FBO mFBO;
	TEX mColourTex;
	RBO mDepthTex;
};

#endif