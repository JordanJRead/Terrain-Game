#ifndef WATER_PARAMS_BUFFER_H
#define WATER_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "glm/glm.hpp"

class UIManager;

class ParamsBufferWater {
public:
	ParamsBufferWater(const UIManager& uiManager) {
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 7 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, mBUF);
		updateGPU(uiManager, true);
	}
	void updateGPU(const UIManager& uiManager, bool force);

private:
	BUF mBUF;
};

#endif