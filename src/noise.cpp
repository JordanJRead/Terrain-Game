#include "noise.h"
#include "stbimage/stb_image.h"
#include <iostream>

Noise::Noise(const std::string& filePath) {
	int c;
	unsigned char* data{ stbi_load(filePath.c_str(), &mWidth, &mHeight, &c, 3)};
	if (!data) {
		std::cerr << "ERROR OPENING NOISE IMAGE " << filePath << "\n";
	}

	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	int externalFormat;
	switch (c) {
	case 1:
		externalFormat = GL_RED;
		break;
	case 3:
		externalFormat = GL_RGB;
		break;
	case 4:
		externalFormat = GL_RGBA;
		break;
	default:
		externalFormat = GL_RED;
		std::cerr << "INVALID FORMAT IN NOISE IMAGE " << filePath << "\n";
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWidth, mHeight, 0, externalFormat, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int Noise::getWidth() const {
	return mWidth;
}

int Noise::getHeight() const {
	return mHeight;
}

void Noise::use(int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}