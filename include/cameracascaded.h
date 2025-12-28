#ifndef CAMERA_CASCADE_H
#define CAMERA_CASCADE_H

#include "camerai.h"
#include "cameraplayer.h"
#include <array>
#include "glm/glm.hpp"
#include "uimanager.h"

class CameraCascaded : public CameraI {
public:
	void updateCamera(const glm::vec3& dirToLight, const std::array<glm::vec3, 8>& frustumPoints, float min, float max, const AABB& sceneAABB);

	glm::vec3 getPosition() const override { return mPosition; }
	bool isAABBVisible(const AABB& aabb) const override { return true; }
	glm::mat4 getViewMatrix() const override { return mViewMatrix; }
	glm::mat4 getProjectionMatrix() const override { return mProjectionMatrix; }

	float getFOVX() const override { return 0; }
	float getFOVY() const override { return 0; }
	float getYaw() const override { return 0; }
	float getPitch() const override { return 0; }
	float getWidth() const { return mWidth; }

private:
	glm::vec3 mPosition;
	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;
	float mWidth;
};

#endif