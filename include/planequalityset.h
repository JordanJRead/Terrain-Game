#ifndef GPU_PLANE_QUALITY_SET
#define GPU_PLANE_QUALITY_SET

#include <vector>
#include "planegpu.h"
#include <span>

/// <summary>
/// A set of GPU planes which have varying vertex counts and have common multiples of vertex distances
/// </summary>
class PlaneQualitySet {
public:
	PlaneQualitySet(float highQualityVertexDensity, std::span<const float> scales, std::span<const float> lods, float chunkWidth);

	void renderUI(float chunkWidth);

	const PlaneGPU& getHighestQualityPlane() const { return mPlanes[0]; }
	const PlaneGPU& getLowestQualityPlane() const { return mPlanes[mPlanes.size() - 1]; }
	const PlaneGPU& getPlaneAtIndex(size_t index) const;
	size_t getPlaneIndexAtDistance(float distance) const;
	size_t getPlaneCount() const { return mPlanes.size(); }

private:
	std::vector<PlaneGPU> mPlanes;
	std::vector<float> mLODs;

	float mHighQualityVertexDensityUI;
	std::vector<float> mScalesUI;

	static constexpr float sMinHighQualityVertexDensity{ 1.0f };

private:
	void updateFromUIValues(float chunkWidth);
};

#endif