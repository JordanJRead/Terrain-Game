#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <array>
#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <string>
#include <GLFW/glfw3.h>

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
	void render(double deltaTime) {
		double currTime{ glfwGetTime() };
		double currTimeFrac{ currTime - (long)currTime };
		if (currTimeFrac < mPrevTimeFrac) {
			mDisplayDeltaTime = deltaTime;
		}
		mPrevTimeFrac = currTimeFrac;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// FPS
		ImGui::Begin("FPS");
		ImGui::LabelText(std::to_string(1 / mDisplayDeltaTime).c_str(), "");
		ImGui::End();

		// Artistic Parameters
		ImGui::Begin("Artistic Parameters");

		ImGui::DragFloat("Terrain scale", mTerrainScale.getDataPtr());
		mTerrainScale.update();

		ImGui::DragFloat("View distance", mViewDistance.getDataPtr(), 1, 0, 1000);
		mViewDistance.update();

		ImGui::DragFloat("Fog encroachment", mFogEncroachment.getDataPtr(), 1, 0, int(mViewDistance.data()));
		mFogEncroachment.update();

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

		ImGui::DragFloat("Snow height", mSnowHeight.getDataPtr());
		mSnowHeight.update();

		ImGui::DragFloat("Seafoam", mSeaFoam.getDataPtr(), 0.01, 0, 10);
		mSeaFoam.update();

		ImGui::DragFloat("Snow line noise scale", mSnowLineNoiseScale.getDataPtr(), 0.001, 0, 100);
		mSnowLineNoiseScale.update();

		ImGui::DragFloat("Snow line noise amplitude", mSnowLineNoiseAmplitude.getDataPtr(), 0.01, 0, 10);
		mSnowLineNoiseAmplitude.update();

		ImGui::DragFloat("Mountain snow cutoff", mMountainSnowCutoff.getDataPtr(), 0.01, 0, 10);
		mMountainSnowCutoff.update();

		ImGui::End();

		// Terrain Parameters
		ImGui::Begin("Terrain Parameters");

		ImGui::DragInt("Octave count", mTerrainOctaveCount.getDataPtr(), 0.1, 1, 30);
		mTerrainOctaveCount.update();

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

		// Colours
		ImGui::Begin("Colours");

		ImGui::ColorPicker3("Dirt", (float*)mDirtColour.getDataPtr());
		mDirtColour.update();

		ImGui::ColorPicker3("Mountain", (float*)mMountainColour.getDataPtr());
		mMountainColour.update();

		ImGui::ColorPicker3("Grass", (float*)mGrassColour.getDataPtr());
		mGrassColour.update();

		ImGui::ColorPicker3("Snow", (float*)mSnowColour.getDataPtr());
		mSnowColour.update();

		ImGui::ColorPicker3("Water", (float*)mWaterColour.getDataPtr());
		mWaterColour.update();

		ImGui::ColorPicker3("Sun", (float*)mSunColour.getDataPtr());
		mSunColour.update();

		ImGui::End();

		// Plane Chunking
		ImGui::Begin("Plane Chunking");

		ImGui::DragFloat("Width", mChunkWidth.getDataPtr(), 1, 1, 100);
		mChunkWidth.update();

		ImGui::DragInt("Count", mChunkCount.getDataPtr(), 1, 1, 100);
		mChunkCount.update();

		ImGui::DragInt("Low quality plane vertices", mLowQualityPlaneVertices.getDataPtr(), 1, 2, 1000);
		mLowQualityPlaneVertices.update();

		ImGui::DragInt("High quality plane quality scale", mHighQualityPlaneQualityScale.getDataPtr(), 1, 2, 1000);
		mHighQualityPlaneQualityScale.update();

		ImGui::DragFloat("Vertex LOD dist", mVertexLODDistance.getDataPtr(), 1, 1, 1000);
		mVertexLODDistance.update();

		ImGui::DragFloat("Shell LOD dist", mShellLODDistance.getDataPtr(), 1, 1, 1000);
		mShellLODDistance.update();

		ImGui::DragFloat("Water height", mWaterHeight.getDataPtr(), 0.1);
		mWaterHeight.update();

		ImGui::End();

		// Day
		ImGui::Begin("Day");

		ImGui::DragFloat("Day time", mDayTime.getDataPtr(), 0.001, 0.1, 0.9);
		mDayTime.update();

		ImGui::End();

		// Terrain Images
		ImGui::Begin("Terrain Images");
		for (int i{ 0 }; i < 4; ++i) {
			std::string indexString{ std::to_string(i + 1) };

			ImGui::DragFloat(("World size " + indexString).c_str(), mImageWorldSizes[i].getDataPtr(), 1, 1, 100000);
			mImageWorldSizes[i].update();

			ImGui::InputInt(("Pixel quality " + indexString).c_str(), mImagePixelDimensions[i].getDataPtr(), 100, 1000);
			mImagePixelDimensions[i].update();
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	double mDisplayDeltaTime{ 1 };
	double mPrevTimeFrac{ 0.9 };

	// Terrain Images
	std::array<UIElement<float>, 4> mImageWorldSizes{
		UIElement<float>{ 1 },
		UIElement<float>{ 4 },
		UIElement<float>{ 12 },
		UIElement<float>{ 36 }
	};

	std::array<UIElement<int>, 4> mImagePixelDimensions{
		UIElement<int>{ 2500 },
		UIElement<int>{ 2500 },
		UIElement<int>{ 2500 },
		UIElement<int>{ 5000 }
	};

	// Day
	UIElement<float> mDayTime{ 0.5 };

	// Colours
	UIElement<glm::vec3> mDirtColour    { {40 / 255.0, 21 / 255.0, 10  / 255.0} };
	UIElement<glm::vec3> mMountainColour{ {34 / 255.0, 34 / 255.0, 34 / 255.0} };
	UIElement<glm::vec3> mGrassColour   { {0   / 255.0, 56 / 255.0, 0   / 255.0} };
	UIElement<glm::vec3> mSnowColour    { {255 / 255.0, 255 / 255.0, 255 / 255.0} };
	UIElement<glm::vec3> mWaterColour   { {0   / 255.0, 1.7  / 255.0, 56 / 255.0} };
	UIElement<glm::vec3> mSunColour     { {255 / 255.0, 255 / 255.0, 255 / 255.0} };

	// Terrain Parameters
	UIElement<int>    mTerrainOctaveCount        { 15 };
	UIElement<float> mTerrainAmplitude	         { 80 };
	UIElement<float> mTerrainAmplitudeMultiplier{ 0.4 };
	UIElement<float> mTerrainSpreadFactor       { 2 };
	UIElement<float> mMountainFrequency         { 0.2 };
	UIElement<float> mMountainExponent          { 4 };
	UIElement<float> mAntiFlatFactor            { 0.04 };
	UIElement<float> mRiverFrequency              { 0.05 };
	UIElement<float> mRiverStrength               { 20 };
	UIElement<float> mRiverExponent{ 32 };

	// Plane Chunking
	UIElement<float> mChunkWidth{ 16 };
	UIElement<int>    mChunkCount{ 40 };
	UIElement<int>    mLowQualityPlaneVertices{ 20 };
	UIElement<int>    mHighQualityPlaneQualityScale{ 6 };
	UIElement<float> mVertexLODDistance{ 110 };
	UIElement<float> mShellLODDistance{ 60 };
	UIElement<float> mWaterHeight{ -1.5 };

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
	UIElement<float> mTerrainScale{ 22 };
	UIElement<float> mViewDistance{ 328 };
	UIElement<float> mFogEncroachment{ 68 };
	UIElement<float> mGrassDotCutoff{ 0.875 };
	UIElement<float> mSnowDotCutoff{ 0.3 };
	UIElement<int>   mShellCount{ 30 };
	UIElement<float> mShellMaxHeight{ 0.117 };
	UIElement<float> mGrassNoiseScale{ 100 };
	UIElement<float> mSnowNoiseScale{ 1 };
	UIElement<float> mShellMaxCutoff{ 1 };
	UIElement<float> mShellBaseCutoff{ 0.2 };
	UIElement<float> mSnowHeight{ 28 };
	UIElement<float> mSeaFoam{ 0.4 };
	UIElement<float> mSnowLineNoiseScale{ 0.3 };
	UIElement<float> mSnowLineNoiseAmplitude{ 2.75 };
	UIElement<float> mMountainSnowCutoff{ 0.9 };
};

#endif