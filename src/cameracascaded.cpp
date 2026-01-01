#include "camerai.h"
#include "cameraplayer.h"
#include <array>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"
#include "cameracascaded.h"
#include <iostream>

bool CameraCascaded::isAABBVisible(const AABB& aabb) const {
	return true;
}

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
	std::array<glm::vec3, 8> orthoCornersLightPositions;
	orthoCornersLightPositions[0] = glm::vec3{ minX, minY, maxZ };
	orthoCornersLightPositions[1] = glm::vec3{ maxX, minY, maxZ };
	orthoCornersLightPositions[2] = glm::vec3{ minX, maxY, maxZ };
	orthoCornersLightPositions[3] = glm::vec3{ maxX, maxY, maxZ };

	orthoCornersLightPositions[4] = glm::vec3{ minX, minY, minZ };
	orthoCornersLightPositions[5] = glm::vec3{ maxX, minY, minZ };
	orthoCornersLightPositions[6] = glm::vec3{ minX, maxY, minZ };
	orthoCornersLightPositions[7] = glm::vec3{ maxX, maxY, minZ };

	// Get ortho positions in world space
	std::array<glm::vec3, 8> orthoPointsWorldPositions;
	for (size_t i{ 0 }; i < 8; ++i) {
		orthoPointsWorldPositions[i] = inverseViewMatrix * glm::vec4{ orthoCornersLightPositions[i], 1 };
	}
	
	mAABB = AABB{ orthoPointsWorldPositions }; // doesn't have extended front plane values btw

	float t1{ sceneAABB.rayFarthestIntersect(orthoPointsWorldPositions[0], dirToLight) };
	float t2{ sceneAABB.rayFarthestIntersect(orthoPointsWorldPositions[1], dirToLight) };
	float t3{ sceneAABB.rayFarthestIntersect(orthoPointsWorldPositions[2], dirToLight) };
	float t4{ sceneAABB.rayFarthestIntersect(orthoPointsWorldPositions[3], dirToLight) };

	float maxT{ fmax(fmax(t1, t2), fmax(t3, t4)) };
	if (maxT < 0)
		maxT = 0;
	static float offset{ 10.0f };
	maxZ += offset;

	mWidth = (maxX - minX + maxY - minY) / 2;
	mProjectionMatrix = glm::ortho(minX, maxX, minY, maxY, -maxZ, -minZ);
}