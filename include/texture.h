#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "OpenGLObjects/TEX.h"
#include <string>

template <int Target>
class Texture {
public:
	Texture(const std::string& name, bool nearestFiltering)
		: mName{ name }
	{
		glBindTexture(Target, mTex);
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, nearestFiltering ? GL_NEAREST : GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, nearestFiltering ? GL_NEAREST : GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (Target == GL_TEXTURE_CUBE_MAP)
			glTexParameteri(Target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void setData(int width, int height, int internalFormat, int externalFormat, void* data) {
		mTex.use(Target);
		if (Target == GL_TEXTURE_CUBE_MAP)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + mTimesDataSet, 0, internalFormat, width, height, 0, externalFormat, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(Target, 0, internalFormat, width, height, 0, externalFormat, GL_UNSIGNED_BYTE, data);
	}

	void attachToFBOColour(const FBO& fbo, int i) {
		fbo.use();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, Target, mTex, 0);
	}

private:
	TEX mTex;
	std::string mName;
	int mTimesDataSet{ 0 };
};

#endif