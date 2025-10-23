#include "paramsbufferartistic.h"
#include "glad/glad.h"
#include "uimanager.h"


void ParamsBufferArtistic::fixShellCount(const UIManager& uiManager) {
	forceShaderShellCount(uiManager.mShellCount.data());
}

template <typename T>
void updateShaderVariable(const UIManager::UIElement<T>& element, bool force, int& offset) {
	int size = sizeof(float);
	if (element.hasChanged() || force) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &element.data());
	}
	offset += size;
}

void ParamsBufferArtistic::updateGPU(const UIManager& uiManager, bool force) {
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };
	updateShaderVariable(uiManager.mTerrainScale, force, offset);
	updateShaderVariable(uiManager.mViewDistance, force, offset);
	updateShaderVariable(uiManager.mFogEncroachment, force, offset);
	updateShaderVariable(uiManager.mGrassDotCutoff, force, offset);
	updateShaderVariable(uiManager.mSnowDotCutoff, force, offset);
	updateShaderVariable(uiManager.mShellCount, force, offset);
	updateShaderVariable(uiManager.mShellMaxHeight, force, offset);
	updateShaderVariable(uiManager.mGrassNoiseScale, force, offset);
	updateShaderVariable(uiManager.mSnowNoiseScale, force, offset);
	updateShaderVariable(uiManager.mShellMaxCutoff, force, offset);
	updateShaderVariable(uiManager.mShellBaseCutoff, force, offset);
	updateShaderVariable(uiManager.mSnowHeight, force, offset);
	updateShaderVariable(uiManager.mSeaFoam, force, offset);
	updateShaderVariable(uiManager.mSnowLineNoiseScale, force, offset);
	updateShaderVariable(uiManager.mSnowLineNoiseAmplitude, force, offset);
	updateShaderVariable(uiManager.mMountainSnowCutoff, force, offset);
	updateShaderVariable(uiManager.mSnowLineEase, force, offset);
	updateShaderVariable(uiManager.mShellAmbientOcclusion, force, offset);
}