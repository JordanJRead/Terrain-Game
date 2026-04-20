#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>

/// <summary>
/// Wrapper class for automatically managing OpenGL object memory
/// </summary>
class VAO {
public:
	VAO() { glGenVertexArrays(1, &mID); }
	~VAO() { if (mIsOwner) glDeleteVertexArrays(1, &mID); }
	operator unsigned int() const { return mID; }
	void bind() const { glBindVertexArray(mID); }

	VAO(const VAO&) = delete;
	VAO(VAO&& other) noexcept {
		mID = other.mID;
		other.mIsOwner = false;
	}
	VAO& operator=(const VAO&) = delete;
	VAO& operator=(VAO&&) = delete;

private:
	unsigned int mID;
	bool mIsOwner{ true };
};

#endif