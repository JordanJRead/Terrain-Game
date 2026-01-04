#ifndef SHADOW_MAPPER_H
#define SHADOW_MAPPER_H

#include "cameracascaded.h"
#include "cameraplayer.h"
#include "aabb.h"
#include "uimanager.h"
#include <array>
#include <vector>
#include "framebufferdepth.h"
#include "constants.h"
#include "shaders/shaderchunk.h"

template <int CascadeCount>
class ShadowMapper {
public:
	ShadowMapper(const UIManager& uiManager) {
		for (size_t i{ 0 }; i < CascadeCount; ++i) {
			mFramebuffers.emplace_back(2048 * 2, 2048 * 2);
		}

		for (size_t i{ 0 }; i < CascadeCount - 1; ++i) {
			mSplits[i] = uiManager.mCascadeSplits[i].data();
		}
	}

	void updateCameras(const glm::vec3& dirToLight, const CameraPlayer& playerCamera, const AABB& sceneAABB, const UIManager& uiManager) {
		for (size_t i{ 0 }; i < CascadeCount - 1; ++i) {
			mSplits[i] = uiManager.mCascadeSplits[i].data();
		}

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
			camera.updateCamera(dirToLight, frustumPoints, min, max, sceneAABB);
		}
	}

	const CameraCascaded& getCamera(size_t i) const {
		return mCameras[i];
	}

	const std::array<glm::vec3, 8>& getOrthoWorldPositions(size_t i) const { return mCameras[i].getOrthoWorldPositions(); }

	const std::array<float, CascadeCount - 1>& getSplits() const {
		return mSplits;
	}

	float getWorldWidth(size_t i) const {
		return mCameras[i].getWidth();
	}

	const FramebufferDepth& getFramebuffer(size_t i) const {
		return mFramebuffers[i];
	}

	void bindDepthTexture(size_t i, int unit) {
		mFramebuffers[i].getDepthTexture().use(GL_TEXTURE_2D, unit);
	}

	ShaderChunk mTerrainDepthShader{ "assets/shaders/terraindepth.vert", "assets/shaders/terraindepth.frag" };
	ShaderChunk mWaterDepthShader{ "assets/shaders/waterdepth.vert", "assets/shaders/waterdepth.frag" };

private:
	std::array<CameraCascaded, CascadeCount> mCameras;
	std::vector<FramebufferDepth> mFramebuffers;
	std::array<float, CascadeCount - 1> mSplits;
};

#endif