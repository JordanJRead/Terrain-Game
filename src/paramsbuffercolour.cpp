#include "paramsbuffercolour.h"
#include "glad/glad.h"
#include "uimanager.h"

void ParamsBufferColour::updateGPU(const UIManager& uiManager, bool force) {
	bool hasChanged{ false };
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };

	int size = 4 * sizeof(float);
	if (uiManager.mDirtColour.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mDirtColour.data());
	}
	offset += size;

	size = 4 * sizeof(float);
	if (uiManager.mMountainColour.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mMountainColour.data());
	}
	offset += size;

	size = 4 * sizeof(float);
	if (uiManager.mGrassColour.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mGrassColour.data());
	}
	offset += size;

	size = 4 * sizeof(float);
	if (uiManager.mSnowColour.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mSnowColour.data());
	}
	offset += size;

	size = 4 * sizeof(float);
	if (uiManager.mWaterColour.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mWaterColour.data());
	}
	offset += size;

	size = 4 * sizeof(float);
	if (uiManager.mSunColour.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mSunColour.data());
	}
	offset += size;
}