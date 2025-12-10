#ifndef FBO_H
#define FBO_H

#include <glad/glad.h>

/// <summary>
/// Wrapper class for automatically managing OpenGL object memory
/// </summary>
class FBO {
public:
	FBO() { glGenFramebuffers(1, &mID); }
	~FBO() { if (mIsOwner) glDeleteFramebuffers(1, &mID); }
	operator unsigned int() const { return mID; }
	void use() const { glBindFramebuffer(GL_FRAMEBUFFER, mID); }

	FBO(const FBO&) = delete;
	FBO(FBO&& other) noexcept {
		other.mIsOwner = false;
		this->mID = other.mID;
	}
	FBO& operator=(const FBO&) = delete;
	FBO& operator=(FBO&& other) noexcept  {
		other.mIsOwner = false;
		if (mIsOwner) glDeleteFramebuffers(1, &mID);
		this->mID = other.mID;
	}

private:
	unsigned int mID;
	bool mIsOwner{ true };
};

#endif