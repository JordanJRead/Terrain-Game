#ifndef NOISE_H
#define NOISE_H

#include <string>
#include "OpenGLObjects/TEX.h"

class Noise {
public:
	Noise(const std::string& filePath);
	int getWidth() const;
	int getHeight() const;
	void use(int unit) const;

private:
	TEX mTexture;
	int mWidth;
	int mHeight;
};

#endif