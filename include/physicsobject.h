#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include "glm/glm.hpp"
#include "physics.h"

class PhysicsObject {
public:
	bool isGrounded() { return mIsGrounded; }
	void incrementGravityVelocity(double value) { mGravityVelocity += value; }

	glm::vec3 move(glm::vec3 position, const glm::vec3& displacement, const PlanePhysics& physicsPlane, float deltaTime);

	float mGroundedCheckDist{ 0.01 };
	float mGravity{ 9.81 };
	bool mIsGrounded{ false };
	double mGravityVelocity{ 0 };
	glm::vec3 mCapsuleScales{ 0.3, 1, 0.3 };
	int mMaxRecursionDepth{ 10 };

private:
	static bool floatEqual(float x, float y, float tol = 0.001) {
		return x - y < tol;
	}
};

#endif