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
	void use() const { glBindRenderbuffer(GL_RENDERBUFFER, mID); }

	RBO(const RBO&) = delete;
	RBO& operator=(const RBO&) = delete;

	RBO(RBO&& other) noexcept {
		other.mIsOwner = false;
		this->mID = other.mID;
	}
	RBO& operator=(RBO&& other) noexcept {
		other.mIsOwner = false;
		if (mIsOwner) glDeleteRenderbuffers(1, &mID);
		this->mID = other.mID;
	}

private:
	unsigned int mID;
	bool mIsOwner{ true };
};

#endif