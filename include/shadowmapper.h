#ifndef SHADOW_MAPPER_H
#define SHADOW_MAPPER_H

#include "cameracascaded.h"
#include "cameraplayer.h"
#include "aabb.h"
#include "uimanager.h"
#include <array>
#include <vector>
#include "framebuffer.h"
#include "shader.h"

template <int CascadeCount>
class ShadowMapper {
public:
	ShadowMapper(const std::array<float, CascadeCount - 1>& splits) : mSplits{ splits }
	{
		for (size_t i{ 0 }; i < CascadeCount; ++i) {
			mFramebuffers.emplace_back(2048, 2048, 0);
		}
	}
	void updateCameras(const glm::vec3& dirToLight, const CameraPlayer& playerCamera, const AABB& sceneAABB, const UIManager& uiManager) {

		// Get world space frustum points
		std::array<glm::vec3, 8> frustumPointsCameraSpace{ playerCamera.getFrustumPointsCameraSpace() };
		std::array<glm::vec3, 8> frustumPoints;
		glm::mat4 inverseViewMatrix{ glm::inverse(playerCamera.getViewMatrix()) };
		for (size_t i{ 0 }; i < 8; ++i) {
			frustumPoints[i] = inverseViewMatrix * glm::vec4{ frustumPointsCameraSpace[i], 1 };
		}

		for (size_t i{ 0 }; i < CascadeCount; ++i) {
			CameraCascaded& camera{ mCameras[i] };
			float min;
			float max;
			if (i == 0)
				min = 0;
			else
				min = mSplits[i - 1];

			if (i == CascadeCount - 1)
				max = 1;
			else
				max = mSplits[i];
			camera.updateCamera(dirToLight, frustumPoints, min, max, sceneAABB, inverseViewMatrix);
		}
	}

	const CameraCascaded& getCamera(int i) const {
		return mCameras[i];
	}

	const Framebuffer<0>& getFramebuffer(int i) const {
		return mFramebuffers[i];
	}

	Shader terrainDepthShader{ "assets/shaders/terraindepth.vert", "assets/shaders/terraindepth.frag" };
	Shader waterDepthShader{ "assets/shaders/waterdepth.vert", "assets/shaders/waterdepth.frag" };

private:
	std::array<CameraCascaded, CascadeCount> mCameras;
	std::vector<Framebuffer<0>> mFramebuffers;
	std::array<float, CascadeCount - 1> mSplits;
};

#endif