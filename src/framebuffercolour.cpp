#include "OpenGLObjects/FBO.h"
#include "OpenGLObjects/TEX.h"
#include "OpenGLObjects/RBO.h"
#include <vector>
#include <iostream>
#include "framebuffercolour.h"

FramebufferColour::FramebufferColour(int colourTextureCount, int width, int height, int glInternalFormat, bool hasDepth)
	: FramebufferI{ width, height }
	, mColourTextureCount{ colourTextureCount }
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	// Colour textures
	if (mColourTextureCount > 0) {
		std::vector<unsigned int> attachments;
		attachments.reserve(mColourTextureCount);
		for (int i{ 0 }; i < mColourTextureCount; ++i) {
			mColourTextures.emplace_back();
			glBindTexture(GL_TEXTURE_2D, mColourTextures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mColourTextures[i], 0);
			attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		if (mColourTextureCount == 1)
			glDrawBuffer(GL_FRAMEBUFFER);
		else
			glDrawBuffers(mColourTextureCount, attachments.data());
	}
	else {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	// Depth stencil
	if (hasDepth) {
		glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencil);
	}

	// Finish
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::FRAMEBUFFERCOLOUR:: Framebuffer is not complete!" << "\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferColour::bindColourTexture(int colourTextureIndex, int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, mColourTextures[colourTextureIndex]);
}

void FramebufferColour::updateDimensions(int index, int width, int height) {
	mWidth = width;
	mHeight = height;
	glBindTexture(GL_TEXTURE_2D, mColourTextures[index]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

const TEX& FramebufferColour::getColourTex(int i) {
	return mColourTextures[i];
}