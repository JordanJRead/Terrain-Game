#ifndef TERRAIN_PARAMS_BUFFER_H
#define TERRAIN_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "shaderguipair.h"

struct TerrainParamsData {
	int octaveCount;
	float initialAmplitude;
	float amplitudeDecay;
	float spreadFactor;
	float mountainFrequency;
	float mountainExponent;
	float antiFlatFactor;
	float dipScale;
	float dipStrength;
};

class TerrainParamsBuffer {
public:
	TerrainParamsBuffer(const TerrainParamsData& data)
		: mOctaveCount{ data.octaveCount }
		, mInitialAmplitude{ data.initialAmplitude }
		, mAmplitudeDecay{ data.amplitudeDecay }
		, mSpreadFactor{ data.spreadFactor }
		, mMountainFrequency{ data.mountainFrequency }
		, mMountainExponent{ data.mountainExponent }
		, mAntiFlatFactor{ data.antiFlatFactor }
		, mDipScale{ data.dipScale }
		, mDipStrength{ data.dipStrength }
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, mBUF);
		updateGPU(true);
	}
	void renderUI();
	bool updateGPU(bool force);
	int getOctaveCount() const { return mOctaveCount.mShader; }
	float getInitialAmplitude() const { return mInitialAmplitude.mShader; }
	float getAmplitudeDecay() const { return mAmplitudeDecay.mShader; }
	float getSpreadFactor() const { return mSpreadFactor.mShader; }
	float getMountainFrequency() const { return mMountainFrequency.mShader; }
	float getMountainExponent() const { return mMountainExponent.mShader; }
	float getAntiFlatFactor() const { return mAntiFlatFactor.mShader; }
	float getDipScale() const { return mDipScale.mShader; }
	float getDipStrength() const { return mDipStrength.mShader; }

private:
	BUF mBUF;

	ShaderGUIPair<int> mOctaveCount;
	ShaderGUIPair<float> mInitialAmplitude;
	ShaderGUIPair<float> mAmplitudeDecay;
	ShaderGUIPair<float> mSpreadFactor;
	ShaderGUIPair<float> mMountainFrequency;
	ShaderGUIPair<float> mMountainExponent;
	ShaderGUIPair<float> mAntiFlatFactor;
	ShaderGUIPair<float> mDipScale;
	ShaderGUIPair<float> mDipStrength;
};

#endif