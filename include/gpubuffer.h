#ifndef GPU_BUFFER
#define GPU_BUFFER

#include <span>
#include "OpenGLObjects/BUF.h"

class GPUBuffer {
public:
	void initialize(int bindingIndex) {
		mBuffer.bind(GL_SHADER_STORAGE_BUFFER);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, mBuffer);
	}

	void setData(std::span<float> data) {
		mBuffer.bind(GL_SHADER_STORAGE_BUFFER);
		int newSize = data.size_bytes();
		if (newSize <= mMaxBytes) {
			glBufferData(GL_SHADER_STORAGE_BUFFER, mMaxBytes, 0, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, newSize, data.data());
		}
		else {
			mMaxBytes = newSize;
			glBufferData(GL_SHADER_STORAGE_BUFFER, mMaxBytes, data.data(), GL_DYNAMIC_DRAW);
		}
	}

private:
	BUF mBuffer;
	int mMaxBytes{ 1 };
};

#endif