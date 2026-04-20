#ifndef RBO_H
#define RBO_H

#include <glad/glad.h>

/// <summary>
/// Wrapper class for automatically managing OpenGL object memory
/// </summary>
class RBO {
public:
	RBO() { glGenRenderbuffers(1, &mID); }
	~RBO() { if (mIsOwner) glDeleteRenderbuffers(1, &mID); }
	operator unsigned int() const { return mID; }

	RBO(const RBO&) = delete;
	RBO(RBO&& other) noexcept {
		mID = other.mID;
		other.mIsOwner = false;
	}
	RBO& operator=(const RBO&) = delete;
	RBO& operator=(RBO&&) = delete;

private:
	unsigned int mID;
	bool mIsOwner{ true };
};

#endif