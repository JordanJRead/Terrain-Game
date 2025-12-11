#include "camerai.h"
#include "cameraplayer.h"
#include <array>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"
#include "cameracascaded.h"

void CameraCascaded::updateCamera(const glm::vec3& dirToLight, const std::array<glm::vec3, 8>& frustumPoints, float min, float max, const AABB& sceneAABB) {
	// frustumPoints defines lower left, lower right, upper left, upper right, then repeat for far plane

	// Get world pos of frustum slice
	std::array<glm::vec3, 8> frustumSliceWorldPositions;
	glm::vec3 center{ 0, 0, 0 };
	for (size_t i{ 0 }; i < 4; ++i) {
		frustumSliceWorldPositions[i]     = frustumPoints[i] + min * (frustumPoints[i + 4] - frustumPoints[i]);
		frustumSliceWorldPositions[i + 4] = frustumPoints[i] + max * (frustumPoints[i + 4] - frustumPoints[i]);
		center += frustumSliceWorldPositions[i] + frustumSliceWorldPositions[i + 4];
	}
	center /= 8;

	// Create view matrix
	mPosition = center + dirToLight;
	mViewMatrix = glm::lookAt(mPosition, center, { 0, 1, 0 });
	glm::mat4 inverseViewMatrix{ glm::inverse(mViewMatrix) };

	// Get light space pos of frustum slice
	std::array<glm::vec3, 8> frustumSliceLightPositions;
	float minX{ std::numeric_limits<float>::max() };
	float maxX{ std::numeric_limits<float>::min() };
	float minY{ std::numeric_limits<float>::max() };
	float maxY{ std::numeric_limits<float>::min() };
	float minZ{ std::numeric_limits<float>::max() };
	float maxZ{ std::numeric_limits<float>::min() };
	for (size_t i{ 0 }; i < 8; ++i) {
		frustumSliceLightPositions[i] = mViewMatrix * glm::vec4{ frustumSliceWorldPositions[i], 1 };
		minX = fmin(frustumSliceLightPositions[i].x, minX);
		maxX = fmax(frustumSliceLightPositions[i].x, maxX);
		minY = fmin(frustumSliceLightPositions[i].y, minY);
		maxY = fmax(frustumSliceLightPositions[i].y, maxY);
		minZ = fmin(frustumSliceLightPositions[i].z, minZ);
		maxZ = fmax(frustumSliceLightPositions[i].z, maxZ);
	}

	// Get ortho positions in light space
	std::array<glm::vec3, 4> orthoFrontPlaneLightPositions;
	orthoFrontPlaneLightPositions[0] = glm::vec3{ minX, minY, maxZ };
	orthoFrontPlaneLightPositions[1] = glm::vec3{ maxX, minY, maxZ };
	orthoFrontPlaneLightPositions[2] = glm::vec3{ minX, maxY, maxZ };
	orthoFrontPlaneLightPositions[3] = glm::vec3{ maxX, maxY, maxZ };

	// Get ortho positions in world space
	std::array<glm::vec3, 4> orthoFrontPlaneWorldPositions;
	for (size_t i{ 0 }; i < 4; ++i) {
		orthoFrontPlaneWorldPositions[i] = inverseViewMatrix * glm::vec4{ orthoFrontPlaneLightPositions[i], 1 };
	}

	float t1{ sceneAABB.rayFarthestIntersect(orthoFrontPlaneWorldPositions[0], dirToLight) };
	float t2{ sceneAABB.rayFarthestIntersect(orthoFrontPlaneWorldPositions[1], dirToLight) };
	float t3{ sceneAABB.rayFarthestIntersect(orthoFrontPlaneWorldPositions[2], dirToLight) };
	float t4{ sceneAABB.rayFarthestIntersect(orthoFrontPlaneWorldPositions[3], dirToLight) };

	float maxT{ fmax(fmax(t1, t2), fmax(t3, t4)) };
	if (maxT < 0)
		maxT = 0;
	maxZ += maxT;

	mProjectionMatrix = glm::ortho(minX, maxX, minY, maxY, -maxZ, -minZ);
}