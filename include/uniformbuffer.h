#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include "glad/glad.h"
#include "OpenGLObjects/BUF.h"
#include "uimanager.h"
#include "glm/glm.hpp"
#include "cameraplayer.h"
#include <iostream>
#include <array>
#include "imagecount.h"
#include "shadowmapper.h"
#include "mathhelper.h"

namespace BufferTypes {
	
	struct TerrainParams {
		TerrainParams() : octaveCount{ -1 } {}
		TerrainParams(const UIManager& uiManager)
			: octaveCount        { uiManager.mTerrainOctaveCount.data() }
			, smoothOctaveCount  { uiManager.mTerrainSmoothOctaveCount.data() }
			, initialAmplitude   { uiManager.mTerrainAmplitude.data() }
			, amplitudeDecay     { uiManager.mTerrainAmplitudeMultiplier.data() }
			, spreadFactor       { uiManager.mTerrainSpreadFactor.data() }
			, mountainFrequency  { uiManager.mMountainFrequency.data() }
			, mountainExponent   { uiManager.mMountainExponent.data() }
			, antiFlatFactor     { uiManager.mAntiFlatFactor.data() }
			, riverScale         { uiManager.mRiverFrequency.data() }
			, riverStrength      { uiManager.mRiverStrength.data() }
			, riverExponent      { uiManager.mRiverExponent.data() }
			, waterEatingMountain{ uiManager.mWaterEatingMountain.data() }
			, lakeScale          { uiManager.mLakeFrequency.data() }
			, lakeStrength       { uiManager.mLakeStrength.data() }
			, lakeExponent       { uiManager.mLakeExponent.data() }
		{}
		bool operator==(const TerrainParams&) const = default;

		int   octaveCount;
		int   smoothOctaveCount{};
		float initialAmplitude{};
		float amplitudeDecay{};
		float spreadFactor{};
		float mountainFrequency{};
		float mountainExponent{};
		float antiFlatFactor{};
		float riverScale{};
		float riverStrength{};
		float riverExponent{};
		float waterEatingMountain{};
		float lakeScale{};
		float lakeStrength{};
		float lakeExponent{};
	};

	struct ArtisticParams {
		ArtisticParams() {}
		ArtisticParams(const UIManager& uiManager)
			: terrainScale{ uiManager.mTerrainScale.data() }
			, maxViewDistance{ uiManager.mTerrainSpan.data() * 0.5f * 0.95f }
			, fogEncroach{ maxViewDistance * 0.1f }
			, grassDotCutoff{ uiManager.mGrassDotCutoff.data() }
			, snowDotCutoff{ uiManager.mSnowDotCutoff.data() }
			, shellMaxHeight{ uiManager.mShellMaxHeight.data() }
			, grassNoiseScale{ uiManager.mGrassNoiseScale.data() }
			, snowNoiseScale{ uiManager.mSnowNoiseScale.data() }
			, shellMaxCutoff{ uiManager.mShellMaxCutoff.data() }
			, shellBaseCutoff{ uiManager.mShellBaseCutoff.data() }
			, snowHeight{ uiManager.mSnowHeight.data() }
			, seafoamStrength{ uiManager.mSeaFoam.data() }
			, snowLineNoiseScale{ uiManager.mSnowLineNoiseScale.data() }
			, snowLineNoiseAmplitude{ uiManager.mSnowLineNoiseAmplitude.data() }
			, mountainSnowCutoff{ uiManager.mMountainSnowCutoff.data() }
			, snowLineEase{ uiManager.mSnowLineEase.data() }
			, shellAmbientOcclusion{ uiManager.mShellAmbientOcclusion.data() }
		{}
		bool operator==(const ArtisticParams&) const = default;

		float terrainScale{};
		float maxViewDistance{};
		float fogEncroach{};
		float grassDotCutoff{};
		float snowDotCutoff{};
		float shellMaxHeight{};
		float grassNoiseScale{};
		float snowNoiseScale{};
		float shellMaxCutoff{};
		float shellBaseCutoff{};
		float snowHeight{};
		float seafoamStrength{};
		float snowLineNoiseScale{};
		float snowLineNoiseAmplitude{};
		float mountainSnowCutoff{};
		float snowLineEase{};
		float shellAmbientOcclusion{};
	};

	struct WaterParams {
		WaterParams() : waveCount{ -1 } {}
		WaterParams(const UIManager& uiManager)
			: waveCount{ uiManager.mWaterWaveCount.data() }
			, initialAmplitude{ uiManager.mWaterAmplitude.data() }
			, amplitudeMult{ uiManager.mWaterAmplitudeMultiplier.data() }
			, initialFreq{ uiManager.mWaterFrequency.data() }
			, freqMult{ uiManager.mWaterFrequencyMultiplier.data() }
			, initialSpeed{ uiManager.mWaterSpeed.data() }
			, speedMult{ uiManager.mWaterSpeedMultiplier.data() }
			, specExp{ uiManager.mWaterShininess.data() }
			, height{ uiManager.mWaterHeight.data() }
		{
		}
		bool operator==(const WaterParams&) const = default;

		int waveCount;
		float initialAmplitude{};
		float amplitudeMult{};
		float initialFreq{};
		float freqMult{};
		float initialSpeed{};
		float speedMult{};
		float specExp{};
		float height{};
	};

	struct ColourParams {
		ColourParams() : dirtColour{ -1, -1, -1, -1 } {}
		ColourParams(const UIManager& uiManager)
			: dirtColour{ uiManager.mDirtColour.data(), 1 }
			, mountainColour{ uiManager.mMountainColour.data(), 1 }
			, grassColour1{ uiManager.mGrassColour1.data(), 1 }
			, grassColour2{ uiManager.mGrassColour2.data(), 1 }
			, snowColour{ uiManager.mSnowColour.data(), 1 }
			, waterColour{ uiManager.mWaterColour.data(), 1 }
			, sunColour{ uiManager.mSunColour.data() * uiManager.mHDRScale.data(), 1}
		{
		}
		bool operator==(const ColourParams&) const = default;

		glm::vec4 dirtColour;
		glm::vec4 mountainColour{};
		glm::vec4 grassColour1{};
		glm::vec4 grassColour2{};
		glm::vec4 snowColour{};
		glm::vec4 waterColour{};
		glm::vec4 sunColour{};
	};

	struct PerFrameInfo {
		PerFrameInfo() : time{ -1 } {}
		PerFrameInfo(const CameraI& camera, glm::vec3 _dirToSun, float _time)
			: viewMatrix{ camera.getViewMatrix() }
			, projectionMatrix{ camera.getProjectionMatrix() }
			, cameraPos{ camera.getPosition(), 1 }
			, dirToSun{ _dirToSun, 0 }
			, time{ _time }
			, fovX{ camera.getFOVX() }
			, fovY{ camera.getFOVY() }
			, yaw{ camera.getYaw() }
			, pitch{ camera.getPitch() }
			, cameraNear{ camera.getNearPlaneDist() }
			, cameraFar{ camera.getFarPlaneDist() }
		{ }
		bool operator==(const PerFrameInfo&) const = default;

		glm::mat4 viewMatrix{};
		glm::mat4 projectionMatrix{};
		glm::vec4 cameraPos{};
		glm::vec4 dirToSun{};
		float time;
		float fovX{};
		float fovY{};
		float yaw{};
		float pitch{};
		float cameraNear{};
		float cameraFar{};
	};

	struct TerrainImagesInfo {
		TerrainImagesInfo() {}
		TerrainImagesInfo(const std::array<float, ImageCount>& _imageScales, const std::array<glm::vec2, ImageCount>& _imagePositions)
			: imageScales{ _imageScales }
			, imagePositions{ _imagePositions } { }
		bool operator==(const TerrainImagesInfo&) const = default;

		std::array<glm::vec2, ImageCount> imagePositions{};
		std::array<float, ImageCount> imageScales{};
	};

	struct AtmosphereInfo {
		AtmosphereInfo() : maxRadius{ -1 } {}
		AtmosphereInfo(const UIManager& uiManager)
			: maxRadius{ (4 * uiManager.mAtmosphereHeight.data() * uiManager.mAtmosphereHeight.data() + uiManager.mAtmosphereWidth.data() * uiManager.mAtmosphereWidth.data()) / (8 * uiManager.mAtmosphereHeight.data())}
			, minRadius{ maxRadius - uiManager.mAtmosphereHeight.data() }
			, centerY{ uiManager.mAtmosphereHeight.data() - maxRadius }
			, rayleighDensityFalloff{ uiManager.mAtmosphereRayleighDensityFalloff.data() }
			, mieDensityFalloff{ uiManager.mAtmosphereMieDensityFalloff.data() }
			, rayleighDensityScale{ uiManager.mAtmosphereRayleighDensityScale.data() * 0.0001F }
			, mieDensityScale{ uiManager.mAtmosphereMieDensityScale.data() * 0.0001F }
			, rayleighScattering{ uiManager.mAtmosphereRayleighScattering.data(), 1}
			, mieScattering{ uiManager.mAtmosphereMieScattering.data(), 1 }
			, rayleighG{ uiManager.mAtmosphereRayleighG.data() }
			, mieG{ uiManager.mAtmosphereMieG.data() }
			, rayAtmosphereStepCount{ uiManager.mRayAtmosphereStepCount.data() }
			, raySunStepCount{ uiManager.mRaySunStepCount.data() }
			, brightness{ uiManager.mAtmosphereBrightness.data() }
		{
		}
		bool operator==(const AtmosphereInfo&) const = default;

		glm::vec4 rayleighScattering{};
		glm::vec4 mieScattering{};
		float maxRadius;
		float minRadius{};
		float centerY{};
		float rayleighDensityFalloff{};
		float mieDensityFalloff{};
		float rayleighDensityScale{};
		float mieDensityScale{};
		float rayleighG{};
		float mieG{};
		int rayAtmosphereStepCount{};
		int raySunStepCount{};
		float brightness{};
	};

	struct ShadowInfo {
		ShadowInfo() {}
		ShadowInfo(const ShadowMapper<CascadeCount>& shadowMapper, const UIManager& uiManager)
			: blurWidth{ uiManager.mShadowBlurWidth.data() }
			, blurQuality{ uiManager.mShadowBlurQuality.data() }
			, exposure{ uiManager.mExposure.data() }
			, minBias{ uiManager.mMinShadowBias.data() }
			, maxBias{ uiManager.mMaxShadowBias.data() }
		{
			if (blurQuality % 2 == 0)
				blurQuality += 1;
			for (int i{ 0 }; i < CascadeCount; ++i) {
				viewMatrices[i] = shadowMapper.getCamera(i).getViewMatrix();
				projectionMatrices[i] = shadowMapper.getCamera(i).getProjectionMatrix();
			}
			splits = shadowMapper.getSplits();
			for (int i{ 0 }; i < CascadeCount; ++i) {
				widths[i] = shadowMapper.getWorldWidth(i);
			}
			
			// Grid sum
			for (int x{ 0 }; x < blurQuality; ++x) {
				for (int y{ 0 }; y < blurQuality; ++y) {
					blurGridSum += blurQuality - MathHelper::taxicabDist(glm::ivec2(x, y), glm::ivec2(blurQuality / 2, blurQuality / 2));
				}
			}
		}
		bool operator==(const ShadowInfo&) const = default;

		std::array<glm::mat4, CascadeCount> viewMatrices{};
		std::array<glm::mat4, CascadeCount> projectionMatrices{};
		std::array<float, CascadeCount - 1> splits{};
		std::array<float, CascadeCount> widths{};
		float blurWidth{};
		int blurQuality{}; // odd
		float blurGridSum{};
		float exposure{};
		float minBias{};
		float maxBias{};
	};
}

template <typename T>
class UniformBuffer {
public:
	UniformBuffer(int bindingIndex, bool isSSBO = false) : mIsSSBO{ isSSBO } {
		glBindBuffer(mIsSSBO ? GL_SHADER_STORAGE_BUFFER : GL_UNIFORM_BUFFER, mBUF);
		glBufferData(mIsSSBO ? GL_SHADER_STORAGE_BUFFER : GL_UNIFORM_BUFFER, sizeof(T), 0, GL_STATIC_DRAW);
		glBindBufferBase(mIsSSBO ? GL_SHADER_STORAGE_BUFFER : GL_UNIFORM_BUFFER, bindingIndex, mBUF);
	}

	// Returns whether the data was changed between calls
	bool updateGPU(const T& data) {
		if (data != mPrevData) {
			auto x{ sizeof(T) };
			glBindBuffer(mIsSSBO ? GL_SHADER_STORAGE_BUFFER : GL_UNIFORM_BUFFER, mBUF);
			glBufferData(mIsSSBO ? GL_SHADER_STORAGE_BUFFER : GL_UNIFORM_BUFFER, sizeof(T), &data, GL_STATIC_DRAW);
			mPrevData = data;
			return true;
		}
		return false;
	}

private:
	BUF mBUF;
	T mPrevData;
	bool mIsSSBO;
};

#endif