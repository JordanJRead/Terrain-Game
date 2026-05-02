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
	void bind(int target, int unit) const {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(target, mID);
	}

	TEX(const TEX&) = delete;
	TEX(TEX&& other) noexcept {
		mID = other.mID;
		other.mIsOwner = false;
	}
	TEX& operator=(const TEX&) = delete;
	TEX& operator=(TEX&&) = delete;

private:
	unsigned int mID;
	bool mIsOwner{ true };
};

#endif