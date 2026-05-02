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

	FBO(const FBO&) = delete;
	FBO(FBO&& other) noexcept {
		mID = other.mID;
		other.mIsOwner = false;
	}
	FBO& operator=(const FBO&) = delete;
	FBO& operator=(FBO&&) = delete;

private:
	unsigned int mID;
	bool mIsOwner{ true };
};

#endif