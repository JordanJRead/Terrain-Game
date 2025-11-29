#ifndef CAMERA_CASCADE_H
#define CAMERA_CASCADE_H

#include "camerai.h"
#include "cameraplayer.h"

class CameraCascade : public CameraI {
public:
	CameraCascade(const CameraPlayer& playerCamera, float min, float max);

	glm::vec3 getPosition() const override { return mPosition; }
	bool isAABBVisible(const AABB& aabb) const override { return true; }
	glm::mat4 getViewMatrix() const override { return mViewMatrix; }
	glm::mat4 getProjectionMatrix() const override { return mProjectionMatrix; }

private:
	glm::vec3 mPosition;
	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;
};

#endif