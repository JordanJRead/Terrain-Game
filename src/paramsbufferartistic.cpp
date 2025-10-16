#include "paramsbufferartistic.h"
#include "glad/glad.h"
#include "uimanager.h"


void ParamsBufferArtistic::fixShellCount(const UIManager& uiManager) {
	forceShaderShellCount(uiManager.mShellCount.data());
}

void ParamsBufferArtistic::updateGPU(const UIManager& uiManager, bool force) {
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };

	int size{ sizeof(float) };
	if (uiManager.mTerrainScale.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mTerrainScale.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mViewDistance.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mViewDistance.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mFogEncroachment.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mFogEncroachment.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mGrassDotCutoff.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mGrassDotCutoff.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mSnowDotCutoff.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mSnowDotCutoff.data());
	}
	offset += size;

	size = sizeof(int);
	if (uiManager.mShellCount.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mShellCount.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mShellMaxHeight.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mShellMaxHeight.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mGrassNoiseScale.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mGrassNoiseScale.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mSnowNoiseScale.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mSnowNoiseScale.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mShellMaxCutoff.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mShellMaxCutoff.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mShellBaseCutoff.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mShellBaseCutoff.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mSnowHeight.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mSnowHeight.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mSeaFoam.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mSeaFoam.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mSnowLineNoiseScale.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mSnowLineNoiseScale.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mSnowLineNoiseAmplitude.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mSnowLineNoiseAmplitude.data());
	}
	offset += size;

	size = sizeof(float);
	if (uiManager.mMountainSnowCutoff.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &uiManager.mMountainSnowCutoff.data());
	}
	offset += size;
}