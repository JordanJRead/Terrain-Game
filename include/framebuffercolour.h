#ifndef FRAMEBUFFER_COLOUR_H
#define FRAMEBUFFER_COLOUR_H

#include "OpenGLObjects/FBO.h"
#include "OpenGLObjects/TEX.h"
#include "OpenGLObjects/RBO.h"
#include <vector>
#include "framebufferi.h"

class FramebufferColour : public FramebufferI {
public:
	FramebufferColour(FramebufferColour&&) = default;
	FramebufferColour& operator=(FramebufferColour&&) = default;
	FramebufferColour(int colourTextureCount, int width, int height, int glInternalFormat, bool hasDepth = true);
	void bindColourTexture(int colourTextureIndex, int unit) const;
	void updateDimensions(int index, int width, int height);
	const TEX& getColourTex(int i);

private:
	int mColourTextureCount;
	std::vector<TEX> mColourTextures;
	RBO mDepthStencil;
};

#endif