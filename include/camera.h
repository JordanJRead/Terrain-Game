#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"
#include <iostream>
#include <physicsobject.h>

class PlanePhysics;

class Camera {
public:
	Camera(int screenWidth, int screenHeight, const glm::vec3& position = { 0, 0, 0 }, float speed = 10, float sens = 0.005f);
	void mouseCallback(GLFWwindow* window, double xPos, double yPos, bool isCursorHidden);
	void move(GLFWwindow* window, float deltaTime, PlanePhysics& physicsPlane);
	void toggleFreecam();
	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
	glm::vec3 getPosition() const { return mPosition; }
	glm::vec3 getForward() const;
	float getNearPlaneDist() const { return mNear; }
	float getFarPlaneDist() const { return mFar; }
	float getXNear() const { return mXNear; }
	float getYNear() const { return mYNear; }

private:
	float mPitch{ 0 }; // In radians
	float mYaw{ 0 }; // In radians
	float mSens;
	float mAspectRatio;
	float mPrevX{ 0 };
	float mPrevY{ 0 };
	bool mIsFirstLook{ true };

	float mFOVYRad{ 90 * 3.1415926535897932384626433832795 / 180.0f };
	float mNear{ 0.1f };
	float mYNear{ mNear * glm::tan(mFOVYRad / 2.0) };
	float mXNear{ mYNear * mAspectRatio };
	float mFar{ 10000.0f };

	glm::vec3 mPosition;

	float mSpeed;
	bool mIsFreecam{ false };
	double mJumpCooldown{ 0 };

	PhysicsObject mPhysicsObject{};
};

#endif