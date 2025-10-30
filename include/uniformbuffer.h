//#ifndef UNIFORM_BUFFER_H
//#define UNIFORM_BUFFER_H
//
//#include "glad/glad.h"
//#include "OpenGLObjects/BUF.h"
//
//template <typename T>
//class UniformBuffer {
//public:
//	UniformBuffer(int bindingIndex) {
//		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
//		glBufferData(GL_UNIFORM_BUFFER, sizeof(T), 0, GL_STATIC_DRAW);
//		glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, mBUF);
//	}
//	updateGPU(const T& data) {
//		if (data != mPrevData) {
//			glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
//			glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &data, GL_STATIC_DRAW);
//			mPrevData = data
//		}
//	}
//
//private:
//	BUF mBUF;
//	T mPrevData;
//};
//
//#endif