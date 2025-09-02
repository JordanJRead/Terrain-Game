#ifndef PHYSICS_H
#define PHYSICS_H

#include "glm/glm.hpp"
class PlanePhysics;

namespace Physics {
	struct CollisionData {
		float t;
		glm::vec3 position;
	};

	glm::vec3 move(const glm::vec3& capsulePos, const glm::vec3& capsuleScales, const glm::vec3& displacement, const PlanePhysics& physicsPlane, int maxRecursionDepth = 10);
	CollisionData getCollisionData(const glm::vec3& spherePosition, const glm::vec3& displacement, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);
}

#endif