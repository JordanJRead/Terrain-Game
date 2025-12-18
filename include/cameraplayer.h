#ifndef CAMERA_PLAYER_H
#define CAMERA_PLAYER_H

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"
#include <iostream>
#include <physicsobject.h>
#include <array>
#include "aabb.h"
#include "camerai.h"

class PlanePhysics;

struct OBB {
	OBB(const std::array<glm::vec3, 4>& orderedCorners)
		: mAxes{ { orderedCorners[1] - orderedCorners[0], orderedCorners[2] - orderedCorners[0], orderedCorners[3] - orderedCorners[0] } }
		, mCenter{ orderedCorners[0] + 0.5f * (mAxes[0] + mAxes[1] + mAxes[2]) }
		, mExtents{ glm::length(mAxes[0]), glm::length(mAxes[1]), glm::length(mAxes[2]) }
	{
		mAxes[0] /= mExtents.x;
		mAxes[1] /= mExtents.y;
		mAxes[2] /= mExtents.z;
		mExtents *= 0.5;
	}

	std::array<glm::vec3, 3> mAxes;
	glm::vec3 mExtents;
	glm::vec3 mCenter;
};

class CameraPlayer : public CameraI {
public:
	CameraPlayer(int screenWidth, int screenHeight, const glm::vec3& position = { 0, 0, 0 }, float speed = 10, float sens = 0.005f);

	glm::vec3 getPosition() const override { return mPosition; }
	bool isAABBVisible(const AABB& aabb) const override;
	glm::mat4 getViewMatrix() const override;
	glm::mat4 getProjectionMatrix() const override;

	void mouseCallback(GLFWwindow* window, double xPos, double yPos, bool isCursorHidden);
	void move(GLFWwindow* window, float deltaTime, PlanePhysics& physicsPlane);
	void toggleFreecam();
	glm::vec3 getForward() const;
	float getNearPlaneDist() const override { return mNear; }
	float getFarPlaneDist() const override { return mFar; }
	float getXNear() const { return mXNear; }
	float getYNear() const { return mYNear; }
	float getFOVX() const override { return 2 * atan(tan(mFOVYRad / 2) * mAspectRatio); }
	float getFOVY() const override { return mFOVYRad; }
	float getYaw() const override { return mYaw; }
	float getPitch() const override { return mPitch; }
	std::array<glm::vec3, 8> getFrustumPointsCameraSpace() const;

private:
	static bool doesOBBOverlapFrustumAlongAxis(const OBB& obb, const glm::vec3& axis, float xNear, float yNear, float near, float far);
	float mPitch{ 0 }; // In radians
	float mYaw{ 0 }; // In radians
	float mSens;
	float mAspectRatio;
	float mPrevX{ 0 };
	float mPrevY{ 0 };
	bool mIsFirstLook{ true };

	float mFOVYRad{ 90 * 3.1415926535897932384626433832795 / 180.0f };
	float mNear{ 0.1f };
	float mYNear{ mNear * glm::tan(mFOVYRad / 2.0f) };
	float mXNear{ mYNear * mAspectRatio };
	float mFar{ 4000.0f };

	glm::vec3 mPosition;

	float mSpeed;
	bool mIsFreecam{ false };
	double mJumpCooldown{ 0 };

	PhysicsObject mPhysicsObject{};
};

#endif