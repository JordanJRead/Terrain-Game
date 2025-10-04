#ifndef PHYSICS_H
#define PHYSICS_H

#include "glm/glm.hpp"
#include <array>

class PlanePhysics;

namespace Physics {
	struct CollisionData {
		double t;
		glm::vec3 planeIntersectionPoint;
	};

	glm::vec3 move(const glm::vec3& capsulePos, const glm::vec3& capsuleScales, const glm::vec3& displacement, const PlanePhysics& physicsPlane, int maxRecursionDepth = 10);
	CollisionData getCollisionData(const glm::vec3& spherePosition, const glm::vec3& displacement, const std::array<glm::vec3, 3>& trianglePoints);
}

#endif