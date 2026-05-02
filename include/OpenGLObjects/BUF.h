#ifndef BUF_H
#define BUF_H

#include <glad/glad.h>

/// <summary>
/// Wrapper class for automatically managing OpenGL object memory
/// </summary>
class BUF {
public:
	BUF() { glGenBuffers(1, &mID); }
	~BUF() { if (mIsOwner) glDeleteBuffers(1, &mID); }
	operator unsigned int() const { return mID; }
	void bind(int target) const { glBindBuffer(target, mID); }

	BUF(const BUF&) = delete;
	BUF(BUF&& other) noexcept {
		mID = other.mID;
		other.mIsOwner = false;
	}
	BUF& operator=(const BUF&) = delete;
	BUF& operator=(BUF&&) = delete;

private:
	unsigned int mID;
	bool mIsOwner{ true };
};

#endif