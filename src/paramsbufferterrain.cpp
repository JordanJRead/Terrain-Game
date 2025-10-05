#include "paramsbufferterrain.h"
#include "glad/glad.h"
#include "imgui.h"
#include "uimanager.h"

bool ParamsBufferTerrain::updateGPU(const UIManager& uiManager, bool force) {
	bool hasChanged{ false };
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };

	int size{ sizeof(int) };
	if (uiManager.mTerrainOctaveCount.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mTerrainOctaveCount.data());
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mTerrainAmplitude.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mTerrainAmplitude.data());
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mTerrainAmplitudeMultiplier.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mTerrainAmplitudeMultiplier.data());
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mTerrainSpreadFactor.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mTerrainSpreadFactor.data());
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mMountainFrequency.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mMountainFrequency.data());
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mMountainExponent.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mMountainExponent.data());
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mAntiFlatFactor.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mAntiFlatFactor.data());
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mDipFrequency.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mDipFrequency.data());
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mDipStrength.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mDipStrength.data());
		hasChanged = true;
	}
	offset += size;

	return hasChanged;
}