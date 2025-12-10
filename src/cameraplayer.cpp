#include "cameraplayer.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"
#include "physics.h"
#include "planephysics.h"

CameraPlayer::CameraPlayer(int screenWidth, int screenHeight, const glm::vec3& position, float speed, float sens)
	: mPosition{ position }
	, mSpeed{ speed } 
	, mSens{ sens }
, mAspectRatio{ static_cast<float>(screenWidth) / screenHeight }
{
	mYaw = 3.1415926535897932384626433832795 / 2.0f;
}

void CameraPlayer::mouseCallback(GLFWwindow* window, double xPos, double yPos, bool isCursorHidden) {
	static float pi{ 3.1415926535897932384626433832795 };

	if (!isCursorHidden) {
		mIsFirstLook = true;
		return;
	}
	if (mIsFirstLook) {
		mIsFirstLook = false;
		mPrevX = xPos;
		mPrevY = yPos;
		return;
	}
	float dx = xPos - mPrevX;
	float dy = yPos - mPrevY;

	mYaw -= dx * mSens;
	mPitch -= dy * mSens;

	if (mPitch >= pi / 2.0f)
		mPitch = pi / 2.0f - 0.001f;

	if (mPitch < -pi / 2.0f)
		mPitch = -pi / 2.0f + 0.001f;

	mPrevX = xPos;
	mPrevY = yPos;
}

glm::mat4 CameraPlayer::getViewMatrix() const {
	glm::vec3 forward{ getForward() };
	return glm::lookAt(mPosition, mPosition + forward, { 0, 1, 0 });
}

glm::mat4 CameraPlayer::getProjectionMatrix() const {
	return glm::perspective(mFOVYRad, mAspectRatio, mNear, mFar);
} 

glm::vec3 CameraPlayer::getForward() const {
	return {
		 glm::cos(mYaw) * glm::cos(mPitch),
		 glm::sin(mPitch),
		-glm::sin(mYaw) * glm::cos(mPitch)
	};
}

void CameraPlayer::toggleFreecam() {
	mIsFreecam = !mIsFreecam;
}

void CameraPlayer::move(GLFWwindow* window, float deltaTime, PlanePhysics& physicsPlane) {
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		mSpeed += 100 * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		mSpeed -= 100 * deltaTime;
	}

	if (mSpeed <= 0) {
		mSpeed = 1;
	}

	glm::vec3 move{ 0, 0, 0 };
	glm::vec3 forward{ getForward() };

	glm::vec3 flatForward{ forward.x, 0, forward.z };
	flatForward = glm::normalize(flatForward);

	glm::vec3 left{ forward.x, 0, forward.z };
	left = glm::normalize(left);
	left = glm::rotate(glm::identity<glm::mat4>(), glm::radians(90.0f), glm::vec3{0, 1, 0}) * glm::vec4{left.x, left.y, left.z, 1};

	if (mJumpCooldown > 0)
		mJumpCooldown -= deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (mIsFreecam)
			move += forward;
		else
			move += flatForward;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (mIsFreecam)
			move -= forward;
		else
			move -= flatForward;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		move += left;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		move -= left;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (mIsFreecam)
			move += glm::vec3{ 0, 1, 0 };
		else if (mJumpCooldown <= 0 && mPhysicsObject.isGrounded()) {
			mPhysicsObject.incrementGravityVelocity(4);
			mJumpCooldown = 0.2;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		if (mIsFreecam)
			move -= glm::vec3{ 0, 1, 0 };
	}
 	if (glm::length(move) != 0)
		move = glm::normalize(move);
	const glm::vec3 displacement{ move * mSpeed * deltaTime };

	if (!mIsFreecam)
		mPosition = mPhysicsObject.move(mPosition, displacement, physicsPlane, deltaTime);
	else
		mPosition += displacement;
}

bool CameraPlayer::doesOBBOverlapFrustumAlongAxis(const OBB& obb, const glm::vec3& axis, float xNear, float yNear, float near, float far) {
	float MoX = fabsf(axis.x);
	float MoY = fabsf(axis.y);
	float MoZ = axis.z;

	constexpr float epsilon = 1e-4;
	if (MoX < epsilon && MoY < epsilon && fabsf(MoZ) < epsilon) return true;

	float MoC = glm::dot(axis, obb.mCenter);

	float obb_radius = 0.0f;
	for (size_t i = 0; i < 3; i++) {
		obb_radius += fabsf(glm::dot(axis, obb.mAxes[i])) * obb.mExtents[i];
	}

	float obb_min = MoC - obb_radius;
	float obb_max = MoC + obb_radius;

	// Frustum projection
	float p = xNear * MoX + yNear * MoY;
	float tau_0 = near * MoZ - p;
	float tau_1 = near * MoZ + p;
	if (tau_0 < 0.0f) {
		tau_0 *= far / near;
	}
	if (tau_1 > 0.0f) {
		tau_1 *= far / near;
	}

	if (obb_min > tau_1 || obb_max < tau_0) {
		return false;
	}
}

// https://bruop.github.io/improved_frustum_culling/
bool CameraPlayer::isAABBVisible(const AABB& aabb) const {

	std::array<glm::vec3, 4> obbCorners{ {
			aabb.mMin,
		   {aabb.mMax.x, aabb.mMin.y, aabb.mMin.z},
		   {aabb.mMin.x, aabb.mMax.y, aabb.mMin.z},
		   {aabb.mMin.x, aabb.mMin.y, aabb.mMax.z}
		} };

	for (size_t i{ 0 }; i < obbCorners.size(); ++i) {
		obbCorners[i] = getViewMatrix() * glm::vec4{ obbCorners[i], 1 };
	}

	OBB obb{ obbCorners };

	std::array<glm::vec3, 5> frustumNormals{ {
		{ 0.0, 0.0, 1 },
		{ 0.0, mNear, mYNear },
		{ 0.0, -mNear, mYNear },
		{ mNear, 0.0f, mXNear },
		{ -mNear, 0.0f, mXNear }
	} };

	for (const glm::vec3& frustumNormal : frustumNormals) {
		if (!doesOBBOverlapFrustumAlongAxis(obb, frustumNormal, mXNear, mYNear, -mNear, -mFar)) {
			return false;
		}
	}

	for (const glm::vec3& obbAxis : obb.mAxes) {
		if (!doesOBBOverlapFrustumAlongAxis(obb, obbAxis, mXNear, mYNear, -mNear, -mFar)) {
			return false;
		}
	}

	std::array<glm::vec3, 18> edgeCrosses;

	for (size_t obbAxisIndex{ 0 }; obbAxisIndex < 3; ++obbAxisIndex) {
		edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 1, 0, 0 });
		edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 0, 1, 0 });

		edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 0, 1, -mNear });
		edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 0, 1, -mNear });
		edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 0, 1, -mNear });
		edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 0, mYNear, -mNear });
	}

	for (const glm::vec3& edgeCross : edgeCrosses) {
		if (!doesOBBOverlapFrustumAlongAxis(obb, edgeCross, mXNear, mYNear, -mNear, -mFar)) {
			return false;
		}
	}

	return true;
}

std::array<glm::vec3, 8> CameraPlayer::getFrustumPointsCameraSpace() const {
	std::array<glm::vec3, 8> frustumPoints;
	frustumPoints[0] = { -mXNear, -mYNear, -mNear };
	frustumPoints[1] = { mXNear, -mYNear, -mNear };
	frustumPoints[2] = { -mXNear,  mYNear, -mNear };
	frustumPoints[3] = { mXNear,  mYNear, -mNear };

	float xFar{ mFar * glm::tan(getFOVX() / 2.0f) };
	float yFar{ mFar * glm::tan(getFOVY() / 2.0f) };

	frustumPoints[4] = { -xFar, -yFar, -mFar };
	frustumPoints[5] = { xFar, -yFar, -mFar };
	frustumPoints[6] = { -xFar,  yFar, -mFar };
	frustumPoints[7] = { xFar,  yFar, -mFar };

	return frustumPoints;
}