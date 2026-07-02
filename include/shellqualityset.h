#ifndef SHELL_QUALITY_SET_H
#define SHELL_QUALITY_SET_H

#include <vector>
#include "imgui/imgui.h"
#include <string>
#include <span>
#include <cassert>
#include <cmath>

class ShellQualitySet {
public:
	ShellQualitySet(int baseShellCount, std::span<const float> scales, std::span<const float> lods) : mBaseShellCount{baseShellCount}, mScales{ scales.begin(), scales.end() }, mLODs{ lods.begin(), lods.end() }
	{
		assert(mScales.size() == mLODs.size());
		assert(mBaseShellCount >= 0);
	}

	void renderUI() {
		ImGui::PushID(this);

		if (ImGui::Button("Add")) {
			mLODs.emplace_back(mLODs.back());
			mScales.emplace_back(mScales.back());
		}
		if (!mScales.empty() && ImGui::Button("Remove")) {
			mLODs.pop_back();
			mScales.pop_back();
		}

		ImGui::DragInt("Base shell count", &mBaseShellCount, 1, 0);

		for (size_t i{ 0 }; i < mScales.size(); ++i) {
			// LOD
			std::string lodLabel{ "LOD " };
			lodLabel += std::to_string(i + 1);
			ImGui::DragFloat(lodLabel.c_str(), &mLODs[i], 10, 0);
			if (i > 0 && mLODs[i] < mLODs[i - 1]) {
				mLODs[i] = mLODs[i - 1];
			}

			// Scale
			std::string scaleLabel{ "Scale " };
			scaleLabel += std::to_string(i + 1);
			ImGui::DragFloat(scaleLabel.c_str(), &mScales[i], 0.1f, 0.1f);
			if (i > 0 && mScales[i] < mScales[i - 1]) {
				mScales[i] = mScales[i - 1];
			}
		}

		ImGui::PopID();
	}

	int getShellCountAtDistance(float distance) const {
		if (mLODs.empty() || distance < mLODs[0]) {
			return mBaseShellCount;
		}

		for (size_t i{ 0 }; i < mLODs.size(); ++i) {
			if (distance > mLODs[i]) {
				return (int)std::round(mScales[i] * mBaseShellCount);
			}
		}
		return (int)std::round(mScales.back() * mBaseShellCount);
	}

	int getHighQualityShellCount() const { return mBaseShellCount; }

private:
	int mBaseShellCount;
	std::vector<float> mScales;
	std::vector<float> mLODs;
};

#endif