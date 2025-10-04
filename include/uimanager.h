#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <array>
#include "glm/glm.hpp"

class UIManager {
private:
	template <typename T>
	class UIElement {
		friend UIManager;

	public:
		UIElement(const T& data) : mData{ data } {}

		const T& getData() { return mData; }
		void update() {
			mHasChanged = mData != mOldData;
			mOldData = mData;
		}

	private:
		T mData;
		T mOldData;
		bool mHasChanged{ false };
		T* getDataPtr() { return &mData; }
	};

public:
	// Terrain Images
	std::array<UIElement<double>, 4> mImageWorldSizes{
		UIElement<double>{ 1 },
		UIElement<double>{ 4 },
		UIElement<double>{ 12 },
		UIElement<double>{ 36 }
	};

	std::array<UIElement<int>, 4> mImagePixelDimensions{
		UIElement<int>{ 2500 },
		UIElement<int>{ 2500 },
		UIElement<int>{ 2500 },
		UIElement<int>{ 2500 }
	};

	// Day
	UIElement<double> mDayTime{ 0.5 };

	// Colours
	UIElement<glm::vec3> mDirtColour    { {110 / 255.0, 82  / 255.0, 59  / 255.0} };
	UIElement<glm::vec3> mMountainColour{ {102 / 255.0, 102 / 255.0, 102 / 255.0} };
	UIElement<glm::vec3> mGrassColour   { {0   / 255.0, 128 / 255.0, 0   / 255.0} };
	UIElement<glm::vec3> mSnowColour    { {255 / 255.0, 255 / 255.0, 255 / 255.0} };
	UIElement<glm::vec3> mWaterColour   { {0   / 255.0, 26  / 255.0, 128 / 255.0} };
	UIElement<glm::vec3> mSunColour     { {255 / 255.0, 255 / 255.0, 255 / 255.0} };

	// Terrain Parameters
	UIElement<int> mOctaveCount         { 15 };
	UIElement<double> mAmplitude	    { 80 };
	UIElement<double> mAmplitudeDecay   { 0.4 };
	UIElement<double> mSpreadFactor     { 2 };
	UIElement<double> mMountainFrequency{ 0.2 };
	UIElement<double> mMountainExponent { 4 };
	UIElement<double> mAntiFlatFactor   { 0.04 };
	UIElement<double> mDipFrequency     { 0.08 };
	UIElement<double> mDipStrength      { 20 };
};

#endif