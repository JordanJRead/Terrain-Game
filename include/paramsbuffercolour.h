#ifndef COLOUR_BUFFER_H
#define COLOUR_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "glm/glm.hpp"

class UIManager;

class ParamsBufferColour {
public:
	ParamsBufferColour(const UIManager& uiManager) {
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, 6 * 4 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 3, mBUF);
		updateGPU(uiManager, true);
	}
	void updateGPU(const UIManager& uiManager, bool force);

private:
	BUF mBUF;
};

#endif