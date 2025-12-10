#ifndef CAMERA_I_H
#define CAMERA_I_H

#include "glm/glm.hpp"
#include "aabb.h"

class CameraI {
public:
	virtual glm::vec3 getPosition() const = 0;
	virtual bool isAABBVisible(const AABB& aabb) const = 0;
	virtual glm::mat4 getViewMatrix() const = 0;
	virtual glm::mat4 getProjectionMatrix() const = 0;
	virtual float getFOVX() const = 0;
	virtual float getFOVY() const = 0;
	virtual float getYaw() const = 0;
	virtual float getPitch() const = 0;
};

#endif