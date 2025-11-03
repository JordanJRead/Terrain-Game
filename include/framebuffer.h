#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "OpenGLObjects/FBO.h"
#include "OpenGLObjects/TEX.h"
#include "OpenGLObjects/RBO.h"
#include <array>

template <int ColourTextureCount>
class Framebuffer {
public:
	Framebuffer(int width, int height, int glInternalFormat) {
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

		unsigned int attachments[ColourTextureCount];
		for (int i{ 0 }; i < ColourTextureCount; ++i) {
			glBindTexture(GL_TEXTURE_2D, mColourTextures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mColourTextures[i], 0);
			attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		}

		glDrawBuffers(3, attachments);

		glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencil);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << "\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void use() const {
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	}
	void bindColourTexture(int colourTextureIndex, int unit) const {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, mColourTextures[colourTextureIndex]);
	}

private:
	FBO mFBO;
	std::array<TEX, ColourTextureCount> mColourTextures;
	RBO mDepthStencil;
};

#endif