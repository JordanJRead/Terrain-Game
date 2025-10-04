#include "physicsobject.h"

glm::vec3 PhysicsObject::move(glm::vec3 position, const glm::vec3& displacement, const PlanePhysics& physicsPlane, float deltaTime) {
	position = Physics::move(position, mCapsuleScales, displacement, physicsPlane, mMaxRecursionDepth);

	// In air
	if (!mIsGrounded) {
		glm::vec3 oldPosition{ position };
		float gravDisplacement{ (float)mGravityVelocity * deltaTime };
		mGravityVelocity -= mGravity * deltaTime;
		position = Physics::move(position, mCapsuleScales, glm::vec3{ 0, gravDisplacement, 0 }, physicsPlane);

		// Check if hit ground
		if (!floatEqual(-glm::length(oldPosition - position), gravDisplacement)) {
			mIsGrounded = true;
			mGravityVelocity = 0;
		}
	}

	// On ground
	else {
		glm::vec3 newPosition{ Physics::move(position, mCapsuleScales, glm::vec3{0, -mGroundedCheckDist, 0 }, physicsPlane) };
		float distanceFallen{ glm::length(position - newPosition) };
		if (floatEqual(distanceFallen, mGroundedCheckDist)) {
			mIsGrounded = false;
		}
	}

	return position;
}