#include "framebufferdepth.h"
#include <iostream>

FramebufferDepth::FramebufferDepth(int width, int height) :FramebufferI{ width, height } {
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Depth stencil
	glBindTexture(GL_TEXTURE_2D, mDepthTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);

	// Finish
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::FRAMEBUFFERDEPTH:: Framebuffer is not complete!" << "\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const TEX& FramebufferDepth::getDepthTexture() const {
	return mDepthTexture;
}