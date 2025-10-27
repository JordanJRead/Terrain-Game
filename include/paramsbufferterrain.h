#ifndef TERRAIN_PARAMS_BUFFER_H
#define TERRAIN_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"

class UIManager;

class ParamsBufferTerrain {
public:
	ParamsBufferTerrain(const UIManager& uiManager) {
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(int) + 13 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, mBUF);
		updateGPU(uiManager, true);
	}
	bool updateGPU(const UIManager& uiManager, bool force);

private:
	BUF mBUF;
};

#endif