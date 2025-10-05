#include "paramsbufferwater.h"
#include "glad/glad.h"
#include "imgui.h"
#include "uimanager.h"

void ParamsBufferWater::updateGPU(const UIManager& uiManager, bool force) {
	bool hasChanged{ false };
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };

	int size = sizeof(int);
	if (uiManager.mWaterWaveCount.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mWaterWaveCount.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mWaterAmplitude.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mWaterAmplitude.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mWaterAmplitudeMultiplier.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mWaterAmplitudeMultiplier.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mWaterFrequency.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mWaterFrequency.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mWaterFrequencyMultiplier.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mWaterFrequencyMultiplier.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mWaterSpeed.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mWaterSpeed.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mWaterSpeedMultiplier.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mWaterSpeedMultiplier.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mWaterShininess.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mWaterShininess.data());
	}
	offset += size;
}