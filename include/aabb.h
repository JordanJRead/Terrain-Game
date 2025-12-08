#ifndef AABB_H
#define AABB_H

#include "glm/glm.hpp"

struct AABB {
	glm::vec3 mMin;
	glm::vec3 mMax;

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
			fixedRayDir.x = 0.0001;
		if (fixedRayDir.y == 0)
			fixedRayDir.y = 0.0001;
		if (fixedRayDir.z == 0)
			fixedRayDir.z = 0.0001;
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

	bool isPointInside(const glm::vec3& point) {
		return point.x > mMin.x && point.x < mMax.x
			&& point.y > mMin.y && point.y < mMax.y
			&& point.z > mMin.z && point.z < mMax.z;
	}
};

#endif