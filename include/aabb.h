#ifndef AABB_H
#define AABB_H

#include "glm/glm.hpp"
#include <array>
#include <numeric>

struct AABB {
	glm::vec3 mMin{};
	glm::vec3 mMax{};

	AABB(const glm::vec3& min, const glm::vec3& max) : mMin{ min }, mMax{ max } {}
	AABB() {};

	template <int N>
	AABB(const std::array<glm::vec3, N>& points) {
		mMin = glm::vec3{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
		mMax = glm::vec3{ std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };

		for (int i{ 0 }; i < N; ++i) {
			mMin = componentMin(mMin, points[i]);
			mMax = componentMax(mMax, points[i]);
		}
	}

	static glm::vec3 componentMin(const glm::vec3& v1, const glm::vec3& v2) {
		return { fmin(v1.x, v2.x), fmin(v1.y, v2.y), fmin(v1.z, v2.z) };
	}
	static glm::vec3 componentMax(const glm::vec3& v1, const glm::vec3& v2) {
		return { fmax(v1.x, v2.x), fmax(v1.y, v2.y), fmax(v1.z, v2.z) };
	}

	/// <summary>
	/// Returns the largest t-value satisfying the parametric ray equation, or -1 if there is none.
	/// -1 can either mean no intersection or a backwards intersection.
	/// </summary>
	float rayFarthestIntersect(const glm::vec3& rayPos, const glm::vec3& rayDir) const {
		glm::vec3 fixedRayDir{ rayDir };
		if (fixedRayDir.x == 0)
			fixedRayDir.x = 0.0001f;
		if (fixedRayDir.y == 0)
			fixedRayDir.y = 0.0001f;
		if (fixedRayDir.z == 0)
			fixedRayDir.z = 0.0001f;
		glm::vec3 tMin = (mMin - rayPos) / rayDir;
		glm::vec3 tMax = (mMax - rayPos) / rayDir;
		glm::vec3 t1 = componentMin(tMin, tMax);
		glm::vec3 t2 = componentMax(tMin, tMax);
		float tNear = fmax(fmax(t1.x, t1.y), t1.z);
		float tFar = fmin(fmin(t2.x, t2.y), t2.z);
		if (tFar < tNear)
			return -1;
		return tFar;
	}

	std::array<glm::vec3, 8> getCorners() const {
		std::array<glm::vec3, 8> corners;
		corners[0] = { mMin.x, mMin.y, mMin.z };
		corners[1] = { mMax.x, mMin.y, mMin.z };
		corners[2] = { mMin.x, mMax.y, mMin.z };
		corners[3] = { mMax.x, mMax.y, mMin.z };
		corners[4] = { mMin.x, mMin.y, mMax.z };
		corners[5] = { mMax.x, mMin.y, mMax.z };
		corners[6] = { mMin.x, mMax.y, mMax.z };
		corners[7] = { mMax.x, mMax.y, mMax.z };
		return corners;
	};

	bool isPointInside(const glm::vec3& point) {
		return point.x > mMin.x && point.x < mMax.x
			&& point.y > mMin.y && point.y < mMax.y
			&& point.z > mMin.z && point.z < mMax.z;
	}

	bool intersects(const AABB& other) const {
		return !(mMin.x >= other.mMax.x || mMax.x <= other.mMin.x || mMin.y >= other.mMax.y || mMax.y <= other.mMin.y || mMin.z >= other.mMax.z || mMax.z <= other.mMin.z);
	}
};

#endif