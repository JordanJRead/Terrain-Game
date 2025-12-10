#ifndef TEX_H
#define TEX_H

#include <glad/glad.h>

/// <summary>
/// Wrapper class for automatically managing OpenGL object memory
/// </summary>
class TEX {
public:
	TEX() { glGenTextures(1, &mID); }
	~TEX() { if (mIsOwner) glDeleteTextures(1, &mID); }
	operator unsigned int() const { return mID; }
	void use(int target) const { glBindTexture(target, mID); }

	TEX(const TEX&) = delete;
	TEX& operator=(const TEX&) = delete;

	TEX(TEX&& other) noexcept {
		other.mIsOwner = false;
		this->mID = other.mID;
	}
	TEX& operator=(TEX&& other) noexcept {
		other.mIsOwner = false;
		if (mIsOwner)
			glDeleteTextures(1, &mID);
		mID = other.mID;
	}

private:
	unsigned int mID;
	bool mIsOwner{ true };
};

#endif