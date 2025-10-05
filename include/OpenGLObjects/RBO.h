#ifndef RBO_H
#define RBO_H

#include <glad/glad.h>

/// <summary>
/// Wrapper class for automatically managing OpenGL object memory
/// </summary>
class RBO {
public:
	RBO() { glGenRenderbuffers(1, &mID); }
	~RBO() { glDeleteRenderbuffers(1, &mID); }
	operator unsigned int() const { return mID; }
	void use() const { glBindRenderbuffer(GL_RENDERBUFFER, mID); }

	RBO(const RBO&) = delete;
	RBO(RBO&&) = delete;
	RBO& operator=(const RBO&) = delete;
	RBO& operator=(RBO&&) = delete;

private:
	unsigned int mID;
};

#endif