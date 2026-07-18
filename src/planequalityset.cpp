#include "planequalityset.h"
#include <cassert>
#include <algorithm>
#include <cmath>
#include "imgui/imgui.h"
#include <string>

PlaneQualitySet::PlaneQualitySet(float highQualityVertexDensity, std::span<const float> scales, std::span<const float> lods, float chunkWidth)
	: mHighQualityVertexDensityUI{ highQualityVertexDensity }
	, mScalesUI{ scales.begin(), scales.end() }
	, mLODs{ lods.begin(), lods.end() }
{
	assert(mScalesUI.size() == mLODs.size());
	if (highQualityVertexDensity < sMinHighQualityVertexDensity) {
		highQualityVertexDensity = sMinHighQualityVertexDensity;
	}
	for (size_t i{ 0 }; i < mScalesUI.size() + 1; ++i) {
		mPlanes.emplace_back(2);
	}
	updateFromUIValues(chunkWidth);
}

void PlaneQualitySet::updateFromUIValues(float chunkWidth) {
	int highQualityVerticesPerEdge = (int)(mHighQualityVertexDensityUI * chunkWidth);
	if (highQualityVerticesPerEdge < 2)
		highQualityVerticesPerEdge = 2;
	if (highQualityVerticesPerEdge != mPlanes[0].getVerticesPerEdge()) {
		mPlanes[0].rebuild(highQualityVerticesPerEdge);
	}

	for (size_t planeI{ 1 }; planeI < mPlanes.size(); ++planeI) {
		size_t scaleI{ planeI - 1 };

		float properScale = mScalesUI[scaleI];// 1.0 / std::round(1 / mScalesUI[scaleI]);
		int verticesPerEdge = (int)((highQualityVerticesPerEdge - 1) * properScale + 1); // TODO?
		if (verticesPerEdge < 2)
			verticesPerEdge = 2;
		if (verticesPerEdge != mPlanes[planeI].getVerticesPerEdge()) {
			mPlanes[planeI].rebuild(verticesPerEdge);
		}
	}
}

void PlaneQualitySet::renderUI(float chunkWidth) {
	ImGui::PushID(this);

	if (ImGui::Button("Add")) {
		mPlanes.emplace_back(mPlanes.back().getVerticesPerEdge());
		mLODs.emplace_back(mLODs.back());
		mScalesUI.emplace_back(mScalesUI.back());
	}
	if (mPlanes.size() > 1 && ImGui::Button("Remove")) {
		mPlanes.pop_back();
		mLODs.pop_back();
		mScalesUI.pop_back();
	}

	ImGui::DragFloat("High quality vertex density", &mHighQualityVertexDensityUI, 1, sMinHighQualityVertexDensity);

	for (size_t planeI{ 1 }; planeI < mPlanes.size(); ++planeI) {
		// LOD
		size_t lodI{ planeI - 1 };
		std::string lodLabel{ "LOD " };
		lodLabel += std::to_string(lodI + 1);
		ImGui::DragFloat(lodLabel.c_str(), &mLODs[lodI], 10, 0);
		if (lodI > 0 && mLODs[lodI] < mLODs[lodI - 1]) {
			mLODs[lodI] = mLODs[lodI - 1];
		}

		// Scale
		size_t scaleI{ planeI - 1 };
		std::string scaleLabel{ "Scale " };
		scaleLabel += std::to_string(scaleI + 2);

		ImGui::DragFloat(scaleLabel.c_str(), &mScalesUI[planeI - 1], 0.05f, 0.01f, 1);
		if (scaleI > 0 && mScalesUI[scaleI] > mScalesUI[scaleI - 1]) {
			mScalesUI[scaleI] = mScalesUI[scaleI - 1];
		}
	}
	updateFromUIValues(chunkWidth);

	ImGui::PopID();
}

const PlaneGPU& PlaneQualitySet::getPlaneAtIndex(size_t index) const {
	assert(index < mPlanes.size());
	return mPlanes[index];
}

size_t PlaneQualitySet::getPlaneIndexAtDistance(float distance) const {
	if (mLODs.empty() || distance < mLODs[0]) {
		return 0;
	}

	for (size_t planeI{ 1 }; planeI < mPlanes.size(); ++planeI) {
		size_t lodI{ planeI - 1 };
		if (distance > mLODs[lodI]) {
			return planeI;
		}
	}
	return mPlanes.size() - 1;
}
