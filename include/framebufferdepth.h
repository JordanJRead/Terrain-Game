#ifndef FRAMEBUFFER_DEPTH_H
#define FRAMEBUFFER_DEPTH_H

#include "OpenGLObjects/TEX.h"
#include "framebufferi.h"

class FramebufferDepth : public FramebufferI {
public:
	FramebufferDepth(FramebufferDepth&&) = default;
	FramebufferDepth& operator=(FramebufferDepth&&) = default;
	FramebufferDepth(int width, int height);
	const TEX& getDepthTexture() const;

private:
	TEX mDepthTexture;
};

#endif