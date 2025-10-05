#ifndef ARTISTIC_PARAMS_BUFFER_H
#define ARTISTIC_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "uimanager.h"

class ParamsBufferArtistic {
public:
	ParamsBufferArtistic(const UIManager& uiManager) {
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 10 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, mBUF);
		updateGPU(uiManager, true);
	}
	void updateGPU(const UIManager& uiManager, bool force);
	void forceShaderShellCount(int newShellCount) {
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		int offset{ 4 * sizeof(float) };
		int size = sizeof(int);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &newShellCount);
	}

	void fixShellCount(const UIManager& uiManager);

private:
	BUF mBUF;
};

#endif