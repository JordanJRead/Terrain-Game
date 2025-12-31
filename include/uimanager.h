#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <array>
#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <string>
#include <GLFW/glfw3.h>
#include "imagecount.h"

class UIManager {
private:
	template <typename T>
	class UIElement {
		friend UIManager;

	public:
		UIElement(const T& data) : mData{ data }, mOldData{ data } {}
		const T& data() const { return mData; }
		bool hasChanged() const { return mHasChanged; }

	private:
		T mData;
		T mOldData;
		bool mHasChanged{ false };
		T* getDataPtr() { return &mData; }
		void update() {
			mHasChanged = mData != mOldData;
			mOldData = mData;
		}
	};

public:
	void render(double deltaTime, bool visible = true) {

		mDeltaTimeSum += deltaTime;
		++mFrameIndex;

		if (mFrameIndex == mMaxFrameIndex) {
			mDisplayDeltaTime = mDeltaTimeSum / mMaxFrameIndex;
			mDeltaTimeSum = 0;
			mFrameIndex = 0;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//mDayTime.mData += deltaTime * 0.01;
		if (visible) {

			// FPS
			ImGui::Begin("FPS");
			ImGui::LabelText(std::to_string(1 / mDisplayDeltaTime).c_str(), "");
			ImGui::End();

			// Artistic Parameters
			ImGui::Begin("Artistic Parameters");

			ImGui::DragFloat("Terrain scale", mTerrainScale.getDataPtr());
			mTerrainScale.update();

			ImGui::DragFloat("Grass dot cutoff", mGrassDotCutoff.getDataPtr(), 0.005, 0, 1);
			mGrassDotCutoff.update();

			ImGui::DragFloat("Snow dot cutoff", mSnowDotCutoff.getDataPtr(), 0.005, 0, 1);
			mSnowDotCutoff.update();

			ImGui::DragInt("Shell count", mShellCount.getDataPtr(), 0.1, 0, 256);
			mShellCount.update();

			ImGui::DragFloat("Shell max height", mShellMaxHeight.getDataPtr(), 0.001, 0, 10);
			mShellMaxHeight.update();

			ImGui::DragFloat("Grass noise scale", mGrassNoiseScale.getDataPtr(), 1, 1, 1000);
			mGrassNoiseScale.update();

			//ImGui::DragFloat("Snow noise scale", mSnowNoiseScale.getDataPtr(), 1, 1, 1000);
			//mSnowNoiseScale.update();

			ImGui::DragFloat("Shell max cutoff", mShellMaxCutoff.getDataPtr(), 0.01, 0, 1);
			mShellMaxCutoff.update();

			ImGui::DragFloat("Shell base cutoff", mShellBaseCutoff.getDataPtr(), 0.01, 0, 1);
			mShellBaseCutoff.update();

			ImGui::DragFloat("Snow height", mSnowHeight.getDataPtr(), 0.05);
			mSnowHeight.update();

			ImGui::DragFloat("Seafoam", mSeaFoam.getDataPtr(), 0.01, 0, 10);
			mSeaFoam.update();

			ImGui::DragFloat("Snow line noise scale", mSnowLineNoiseScale.getDataPtr(), 0.001, 0, 100);
			mSnowLineNoiseScale.update();

			ImGui::DragFloat("Snow line noise amplitude", mSnowLineNoiseAmplitude.getDataPtr(), 0.01, 0, 10);
			mSnowLineNoiseAmplitude.update();

			ImGui::DragFloat("Mountain snow cutoff", mMountainSnowCutoff.getDataPtr(), 0.01, 0, 10);
			mMountainSnowCutoff.update();

			ImGui::DragFloat("Snow line ease", mSnowLineEase.getDataPtr(), 0.01, 0, 10);
			mSnowLineEase.update();

			ImGui::DragFloat("Shell ambient occlusion", mShellAmbientOcclusion.getDataPtr(), 0.001, 0, 1);
			mShellAmbientOcclusion.update();

			ImGui::End();

			// Terrain Parameters
			ImGui::Begin("Terrain Parameters");

			ImGui::DragInt("Octave count", mTerrainOctaveCount.getDataPtr(), 0.1, 1, 30);
			mTerrainOctaveCount.update();

			ImGui::DragInt("Smooth octave count", mTerrainSmoothOctaveCount.getDataPtr(), 0.1, 1, 30);
			mTerrainSmoothOctaveCount.update();

			ImGui::DragFloat("Amplitude", mTerrainAmplitude.getDataPtr(), 0.7, 0, 500);
			mTerrainAmplitude.update();

			ImGui::DragFloat("Amplitude decay", mTerrainAmplitudeMultiplier.getDataPtr(), 0.0005, 0, 100);
			mTerrainAmplitudeMultiplier.update();

			ImGui::DragFloat("Spread factor", mTerrainSpreadFactor.getDataPtr(), 0.001, 0, 100);
			mTerrainSpreadFactor.update();

			ImGui::DragFloat("Mountain frequency", mMountainFrequency.getDataPtr(), 0.003, 0, 2);
			mMountainFrequency.update();

			ImGui::DragFloat("Mountain exponent", mMountainExponent.getDataPtr(), 0.01, 0.1, 30);
			mMountainExponent.update();

			ImGui::DragFloat("Anti flat factor", mAntiFlatFactor.getDataPtr(), 0.001, 0, 1);
			mAntiFlatFactor.update();

			ImGui::DragFloat("River frequency", mRiverFrequency.getDataPtr(), 0.001, 0, 2);
			mRiverFrequency.update();

			ImGui::DragFloat("River strength", mRiverStrength.getDataPtr(), 1, 0, 1000);
			mRiverStrength.update();

			ImGui::DragFloat("River exponent", mRiverExponent.getDataPtr(), 1, 0, 1000);
			mRiverExponent.update();

			ImGui::DragFloat("Water eating mountains", mWaterEatingMountain.getDataPtr(), 0.1, 0, 5);
			mWaterEatingMountain.update();

			ImGui::DragFloat("Lake frequency", mLakeFrequency.getDataPtr(), 0.001, 0, 2);
			mLakeFrequency.update();

			ImGui::DragFloat("Lake strength", mLakeStrength.getDataPtr(), 0.5, 0, 1000);
			mLakeStrength.update();

			ImGui::DragFloat("Lake exponent", mLakeExponent.getDataPtr(), 0.01, 0, 1000);
			mLakeExponent.update();

			ImGui::End();

			// Water Parameters
			ImGui::Begin("Water Parameters");

			ImGui::DragInt("Wave count", mWaterWaveCount.getDataPtr(), 0.1, 1, 100);
			mWaterWaveCount.update();

			ImGui::DragFloat("Initial amplitude", mWaterAmplitude.getDataPtr(), 0.005, 0.01, 1);
			mWaterAmplitude.update();

			ImGui::DragFloat("Amplitude multiplier", mWaterAmplitudeMultiplier.getDataPtr(), 0.001, 0, 1);
			mWaterAmplitudeMultiplier.update();

			ImGui::DragFloat("Initial frequency", mWaterFrequency.getDataPtr(), 0.01, 0, 5);
			mWaterFrequency.update();

			ImGui::DragFloat("Frequency multiplier", mWaterFrequencyMultiplier.getDataPtr(), 0.01, 0, 1.5);
			mWaterFrequencyMultiplier.update();

			ImGui::DragFloat("Initial speed", mWaterSpeed.getDataPtr(), 0.02, 0, 20);
			mWaterSpeed.update();

			ImGui::DragFloat("Speed multiplier", mWaterSpeedMultiplier.getDataPtr(), 0.007, 0, 2);
			mWaterSpeedMultiplier.update();

			ImGui::DragFloat("Shininess", mWaterShininess.getDataPtr());
			mWaterShininess.update();

			ImGui::End();


			// Atmosphere
			ImGui::Begin("Atmosphere");

			ImGui::DragFloat("Atmosphere height", mAtmosphereHeight.getDataPtr());
			mAtmosphereHeight.update();

			ImGui::DragFloat("Atmosphere width", mAtmosphereWidth.getDataPtr(), 1000);
			mAtmosphereWidth.update();
			
			ImGui::DragFloat("Rayleigh density falloff", mAtmosphereRayleighDensityFalloff.getDataPtr(), 0.001);
			mAtmosphereRayleighDensityFalloff.update();

			ImGui::DragFloat("Mie density falloff", mAtmosphereMieDensityFalloff.getDataPtr(), 0.001);
			mAtmosphereMieDensityFalloff.update();

			ImGui::DragFloat("Rayleigh density scale", mAtmosphereRayleighDensityScale.getDataPtr(), 0.001);
			mAtmosphereRayleighDensityScale.update();

			ImGui::DragFloat("Mie density scale", mAtmosphereMieDensityScale.getDataPtr(), 0.001);
			mAtmosphereMieDensityScale.update();

			ImGui::DragFloat3("Rayleigh scattering", (float*)mAtmosphereRayleighScattering.getDataPtr(), 1);
			mAtmosphereRayleighScattering.update();

			ImGui::DragFloat3("Mie scattering", (float*)mAtmosphereMieScattering.getDataPtr(), 1);
			mAtmosphereMieScattering.update();

			ImGui::DragFloat("Rayleigh G", mAtmosphereRayleighG.getDataPtr(), 0.001);
			mAtmosphereRayleighG.update();

			ImGui::DragFloat("Mie G", mAtmosphereMieG.getDataPtr(), 0.001);
			mAtmosphereMieG.update();

			ImGui::DragFloat("HDR scale", mHDRScale.getDataPtr(), 0.1);
			mHDRScale.update();

			ImGui::DragInt("Ray atmosphere steps", mRayAtmosphereStepCount.getDataPtr(), 0.1, 1, 100);
			mRayAtmosphereStepCount.update();

			ImGui::DragInt("Ray sun steps", mRaySunStepCount.getDataPtr(), 0.1, 1, 100);
			mRaySunStepCount.update();

			ImGui::End();


			// Colours
			ImGui::Begin("Colours");

			ImGui::Checkbox("Deferred Rendering", mIsDeferredRendering.getDataPtr());
			mIsDeferredRendering.update();

			ImGui::Checkbox("Show physics plane", mShowPhysicsPlane.getDataPtr());
			mShowPhysicsPlane.update();

			ImGui::ColorPicker3("Dirt", (float*)mDirtColour.getDataPtr());
			mDirtColour.update();

			ImGui::ColorPicker3("Mountain", (float*)mMountainColour.getDataPtr());
			mMountainColour.update();

			ImGui::ColorPicker3("Grass 1", (float*)mGrassColour1.getDataPtr());
			mGrassColour1.update();

			ImGui::ColorPicker3("Grass 2", (float*)mGrassColour2.getDataPtr());
			mGrassColour2.update();

			ImGui::ColorPicker3("Snow", (float*)mSnowColour.getDataPtr());
			mSnowColour.update();

			ImGui::ColorPicker3("Water", (float*)mWaterColour.getDataPtr());
			mWaterColour.update();

			ImGui::ColorPicker3("Sun", (float*)mSunColour.getDataPtr());
			mSunColour.update();

			ImGui::End();

			// Plane Chunking
			ImGui::Begin("Plane Chunking");

			ImGui::DragFloat("Span", mTerrainSpan.getDataPtr(), 1, 1, 100000);
			mTerrainSpan.update();

			ImGui::DragInt("Count", mChunkCount.getDataPtr(), 1, 1, 1000);
			mChunkCount.update();

			ImGui::DragFloat("Low quality plane vertices", mLowQualityVertexDensity.getDataPtr(), 0.01 , 0, 100);
			mLowQualityVertexDensity.update();

			ImGui::DragInt("Medium quality plane quality scale", mMediumQualityPlaneQualityScale.getDataPtr(), 1, 1, 100);
			mMediumQualityPlaneQualityScale.update();

			ImGui::DragInt("High quality plane quality scale", mHighQualityPlaneQualityScale.getDataPtr(), 1, 1, 100);
			mHighQualityPlaneQualityScale.update();

			ImGui::DragFloat("Near vertex LOD dist", mVertexLODDistanceNear.getDataPtr(), 1, 1, 1000);
			mVertexLODDistanceNear.update();

			ImGui::DragFloat("Far vertex LOD dist", mVertexLODDistanceFar.getDataPtr(), 1, 1, 1000);
			mVertexLODDistanceFar.update();

			ImGui::DragFloat("Shell LOD dist", mShellLODDistance.getDataPtr(), 1, 1, 1000);
			mShellLODDistance.update();

			ImGui::DragFloat("Water height", mWaterHeight.getDataPtr(), 0.1);
			mWaterHeight.update();

			ImGui::Checkbox("Frustum culling", mFrustumCulling.getDataPtr());
			mFrustumCulling.update();

			ImGui::End();

			// Day
			ImGui::Begin("Day");

			ImGui::DragFloat("Day time", mDayTime.getDataPtr(), 0.001, 0, 2);
			mDayTime.update();

			ImGui::End();

			// Terrain Images
			ImGui::Begin("Terrain Images");
			for (int i{ 0 }; i < ImageCount; ++i) {
				std::string indexString{ std::to_string(i + 1) };

				ImGui::DragFloat(("World size " + indexString).c_str(), mImageWorldSizes[i].getDataPtr(), 1, 1, 100000);
				mImageWorldSizes[i].update();

				if (i != ImageCount - 1 && mImageWorldSizes[i + 1].data() < mImageWorldSizes[i].data()) {
					mImageWorldSizes[i + 1].mData = mImageWorldSizes[i].data();
				}

				ImGui::InputInt(("Pixel quality " + indexString).c_str(), mImagePixelDimensions[i].getDataPtr(), 100, 1000);
				mImagePixelDimensions[i].update();
			}
			ImGui::End();

			// Shadow Parameters
			ImGui::Begin("Shadow Parameters");

			ImGui::DragFloat("Blur width", mShadowBlurWidth.getDataPtr(), 0.01, 0, 10);
			mShadowBlurWidth.update();

			ImGui::DragInt("Blur quality", mShadowBlurQuality.getDataPtr(), 0.03, 0, 9);
			mShadowBlurQuality.update();

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	int mFrameIndex{ 0 };
	int mMaxFrameIndex{ 60 };
	double mDeltaTimeSum{ 0 };
	double mDisplayDeltaTime{ 1 };

	// Terrain Images
	std::array<UIElement<float>, ImageCount> mImageWorldSizes{
		UIElement<float>{ 1 },
		UIElement<float>{ 4 },
		UIElement<float>{ 12 },
		UIElement<float>{ 64 },
		UIElement<float>{ 300 }
	};

	std::array<UIElement<int>, ImageCount> mImagePixelDimensions{
		UIElement<int>{ 2500 },
		UIElement<int>{ 2500 },
		UIElement<int>{ 2500 },
		UIElement<int>{ 5000 },
		UIElement<int>{ 5000 }
	};

	std::array<float, ImageCount> getImageSizes() const {
		std::array<float, ImageCount> output;
		for (int i{ 0 }; i < ImageCount; ++i) {
			output[i] = mImageWorldSizes[i].mData;
		}
		return output;
	}

	// Day
	UIElement<float> mDayTime{ 0.75 };

	// Colours
	UIElement<bool>      mIsDeferredRendering{ true };
	UIElement<bool>      mShowPhysicsPlane{ false };
	UIElement<glm::vec3> mDirtColour    { {40 / 255.0, 21 / 255.0, 10  / 255.0} };
	UIElement<glm::vec3> mMountainColour{ {34 / 255.0, 34 / 255.0, 34 / 255.0} };
	UIElement<glm::vec3> mGrassColour1  { {0   / 255.0, 56 / 255.0, 0   / 255.0} };
	UIElement<glm::vec3> mGrassColour2{ {15 / 255.0, 56 / 255.0, 0 / 255.0} };
	UIElement<glm::vec3> mSnowColour    { {255 / 255.0, 255 / 255.0, 255 / 255.0} };
	UIElement<glm::vec3> mWaterColour   { {0   / 255.0, 1.7  / 255.0, 56 / 255.0} };
	UIElement<glm::vec3> mSunColour     { {255 / 255.0, 255 / 255.0, 255 / 255.0} };

	// Terrain Parameters
	UIElement<int>    mTerrainOctaveCount        { 15 };
	UIElement<int>    mTerrainSmoothOctaveCount  { 6 };
	UIElement<float> mTerrainAmplitude	         { 250 };
	UIElement<float> mTerrainAmplitudeMultiplier{ 0.4 };
	UIElement<float> mTerrainSpreadFactor       { 2 };
	UIElement<float> mMountainFrequency         { 0.2 };
	UIElement<float> mMountainExponent          { 4 };
	UIElement<float> mAntiFlatFactor            { 0.04 };
	UIElement<float> mRiverFrequency              { 0.05 };
	UIElement<float> mRiverStrength               { 20 };
	UIElement<float> mRiverExponent{ 32 };
	UIElement<float> mWaterEatingMountain{ 30 };
	UIElement<float> mLakeFrequency{ 0.06 };
	UIElement<float> mLakeStrength{ 40 };
	UIElement<float> mLakeExponent{ 100 }; // 100 for quintic, 5 not

	// Plane Chunking
	UIElement<int>    mChunkCount{ 19 };
	UIElement<float>  mTerrainSpan{ 5000 };
	UIElement<float>    mLowQualityVertexDensity{ 0.15 };
	UIElement<int>    mMediumQualityPlaneQualityScale{ 4 }; // TODO another plane LOD?
	UIElement<int>    mHighQualityPlaneQualityScale{ 25 };
	UIElement<float> mVertexLODDistanceNear{ 256 };
	UIElement<float> mVertexLODDistanceFar{ 1000 };
	UIElement<float> mShellLODDistance{ 60 };
	UIElement<float> mWaterHeight{ -1.5 };
	UIElement<bool> mFrustumCulling{ true };

	// Water Parameters
	UIElement<int>   mWaterWaveCount{ 24 };
	UIElement<float> mWaterAmplitude{ 0.07 };
	UIElement<float> mWaterAmplitudeMultiplier{ 0.82 };
	UIElement<float> mWaterFrequency{ 1 };
	UIElement<float> mWaterFrequencyMultiplier{ 1.13 };
	UIElement<float> mWaterSpeed{ 2 };
	UIElement<float> mWaterSpeedMultiplier{ 1.07 };
	UIElement<float> mWaterShininess{ 200 };

	// Artistic Parameters
	UIElement<float> mTerrainScale{ 58 };
	UIElement<float> mGrassDotCutoff{ 0.6 };
	UIElement<float> mSnowDotCutoff{ 0.3 };
	UIElement<int>   mShellCount{ 30 };
	UIElement<float> mShellMaxHeight{ 0.117 };
	UIElement<float> mGrassNoiseScale{ 100 };
	UIElement<float> mSnowNoiseScale{ 1 };
	UIElement<float> mShellMaxCutoff{ 1 };
	UIElement<float> mShellBaseCutoff{ 0.2 };
	UIElement<float> mSnowHeight{ 65 };
	UIElement<float> mSeaFoam{ 0.4 };
	UIElement<float> mSnowLineNoiseScale{ 0.3 };
	UIElement<float> mSnowLineNoiseAmplitude{ 2.75 };
	UIElement<float> mMountainSnowCutoff{ 0.9 };
	UIElement<float> mSnowLineEase{ 4.5 };
	UIElement<float> mShellAmbientOcclusion{ 0.2 };

	// Atmosphere Parameters
	UIElement<float> mAtmosphereHeight{ 3498 };
	UIElement<float> mAtmosphereWidth{ 136100 };
	UIElement<float> mAtmosphereRayleighDensityFalloff{ 1 };
	UIElement<float> mAtmosphereMieDensityFalloff{ 5 };
	UIElement<float> mAtmosphereRayleighDensityScale{ 0.004 };
	UIElement<float> mAtmosphereMieDensityScale{ 0.06 };
	UIElement<glm::vec3> mAtmosphereRayleighScattering{ {5, 106, 594} };
	UIElement<glm::vec3> mAtmosphereMieScattering{ {2, 2, 2 } };
	UIElement<float> mAtmosphereRayleighG{ 0 };
	UIElement<float> mAtmosphereMieG{ 0.957 };
	UIElement<float> mHDRScale{ 15 };
	UIElement<int> mRayAtmosphereStepCount{ 10 };
	UIElement<int> mRaySunStepCount{ 8 };

	// Shadow Parameters
	UIElement<float> mShadowBlurWidth{ 0.5 };
	UIElement<int> mShadowBlurQuality{ 7 };
};

#endif