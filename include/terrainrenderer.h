#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#include "shader.h"
#include "glm/glm.hpp"
#include "vertexarray.h"
#include "terrainimagegenerator.h"
#include "planegpu.h"
#include <array>
#include <string>
#include <string_view>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "cameraplayer.h"
#include <iostream>
#include "cubemap.h"
#include "cubevertices.h"
#include "uimanager.h"
#include "mathhelper.h"
#include "framebuffer.h"
#include "uniformbuffer.h"
#include "deferredrenderer.h"
#include "imagecount.h"
#include "camerai.h"
#include "aabb.h"
#include "shadowmapper.h"

class TerrainRenderer {
public:
	TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos, const UIManager& uiManager)
		: mLowQualityPlane{ 2 }
		, mMediumQualityPlane{ 2 }
		, mHighQualityPlane{ 2 } // ? temporary i think?
		, mReallyLowQualityPlane{ 2 }

		, mTerrainImageShader{ "assets/shaders/terrainimage.vert", "assets/shaders/terrainimage.frag" }
		, mTerrainShader{ "assets/shaders/terrain.vert", "assets/shaders/terrain.frag" }
		, mWaterShader{ "assets/shaders/water.vert", "assets/shaders/water.frag" }
		, mSkyboxShader{ "assets/shaders/skybox.vert", "assets/shaders/skybox.frag" }

		, mImages{ {
			{uiManager.mImagePixelDimensions[0].data(), uiManager.mImageWorldSizes[0].data(), screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 0, uiManager)},
			{uiManager.mImagePixelDimensions[1].data(), uiManager.mImageWorldSizes[1].data(), screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 1, uiManager)},
			{uiManager.mImagePixelDimensions[2].data(), uiManager.mImageWorldSizes[2].data(), screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 2, uiManager)},
			{uiManager.mImagePixelDimensions[3].data(), uiManager.mImageWorldSizes[3].data(), screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 3, uiManager)},
			{uiManager.mImagePixelDimensions[4].data(), uiManager.mImageWorldSizes[4].data(), screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 4, uiManager)}
		} }

		, mDaySkybox{ {
				"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_2_Left+X.png",
				"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_3_Right-X.png",
				"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_4_Up+Y.png",
				"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_5_Down-Y.png",
				"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_0_Front+Z.png",
				"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_1_Back-Z.png"
			} }
		, mNightSkybox{ {
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_2_Left+X.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_3_Right-X.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_4_Up+Y.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_5_Down-Y.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_0_Front+Z.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_1_Back-Z.png"
			} }
		, mDeferredRenderer{ screenWidth, screenHeight }
	{
		mMinTerrainHeight = getHeightWithPerlin(uiManager, mMinPerlinValues);
		mMaxTerrainHeight = getHeightWithPerlin(uiManager, mMaxPerlinValues);

		//mMinPerlin = getMinHeightPerlinValues(uiManager);
		//mMaxPerlin = getMaxHeightPerlinValues(uiManager);

		std::vector<float> vertexData{
		-1, -1,
		 1, -1,
		-1,  1,
		 1,  1
		};

		std::vector<unsigned int> indices{
			0, 1, 2, 2, 1, 3
		};

		std::vector<int> attribs{
			2
		};

		mScreenQuad.create(vertexData, indices, attribs);

		// Set shader uniforms
		mWaterShader.use();
		mWaterShader.setInt("skybox", 8);
		mTerrainShader.use();
		mTerrainShader.setInt("skybox", 8);
		for (int i{ 0 }; i < ImageCount; ++i) {
			std::string indexString{ std::to_string(i) };

			mTerrainShader.use();
			mTerrainShader.setInt("images[" + indexString + "]", i);
			mWaterShader.use();
			mWaterShader.setInt("images[" + indexString + "]", i);

			mImages[i].updateTexture(mScreenQuad, mTerrainImageShader);
		}

		mSkyboxShader.use();
		mSkyboxShader.setInt("skybox", 8);
	}

	void render(const CameraPlayer& camera, float time, const UIManager& uiManager, const Framebuffer<1>& targetFramebuffer) {
		bool hasTerrainChanged{ mTerrainParams.updateGPU({uiManager}) };
		if (hasTerrainChanged) {
			mMinTerrainHeight = getHeightWithPerlin(uiManager, mMinPerlinValues);
			mMaxTerrainHeight = getHeightWithPerlin(uiManager, mMaxPerlinValues);
		}
		glm::vec3 dirToSun{ MathHelper::getDirToSun(uiManager) };
		mArtisticParams.updateGPU(uiManager);
		mWaterParams.updateGPU(uiManager);
		mColourParams.updateGPU(uiManager);
		mAtmosphereInfo.updateGPU(uiManager);
		mTerrainShader.use();

		int chunkCount{ uiManager.mChunkCount.data() };
		float chunkWidth{ uiManager.mTerrainSpan.data() / chunkCount };
		// Update plane types
		int lowQualityPlaneVerticesPerEdge{ (int)(chunkWidth * uiManager.mLowQualityVertexDensity.data()) };
		if (lowQualityPlaneVerticesPerEdge < 2)
			lowQualityPlaneVerticesPerEdge = 2;
		if (lowQualityPlaneVerticesPerEdge != mLowQualityPlane.getVerticesPerEdge()) {
			mLowQualityPlane.rebuild(lowQualityPlaneVerticesPerEdge);
		}
		int highQualityVerticesPerEdge{ uiManager.mHighQualityPlaneQualityScale.data() * (lowQualityPlaneVerticesPerEdge - 1) + 1 }; // We want the distance between vertices to be multiples of each other, so we do this
		if (highQualityVerticesPerEdge != mHighQualityPlane.getVerticesPerEdge()) {
			mHighQualityPlane.rebuild(highQualityVerticesPerEdge);
		}

		int mediumQualityVerticesPerEdge{ uiManager.mMediumQualityPlaneQualityScale.data() * (lowQualityPlaneVerticesPerEdge - 1) + 1 }; // We want the distance between vertices to be multiples of each other, so we do this`	
		if (mediumQualityVerticesPerEdge != mMediumQualityPlane.getVerticesPerEdge()) {
			mMediumQualityPlane.rebuild(mediumQualityVerticesPerEdge);
		}

		// Update images
		for (int i{ 0 }; i < ImageCount; ++i) {

			// Move images along with the player
			glm::vec2 scaledCameraPos{ glm::vec2(camera.getPosition().x, camera.getPosition().z) / uiManager.mTerrainScale.data() };
			double cameraDistFromImageCenter{ glm::length(scaledCameraPos - mImageWorldPositions[i]) };
			if (cameraDistFromImageCenter * 2 > 0.2 * uiManager.mImageWorldSizes[i].data()) { // If near edge of image, update image
				glm::vec3 pixelPosition{ getClosestWorldPixelPos(camera.getPosition() / uiManager.mTerrainScale.data(), i, uiManager) };
				mImageWorldPositions[i] = glm::vec2(pixelPosition.x, pixelPosition.z);
			}

			// Recalculate image checks
			std::string indexString{ std::to_string(i) };
			bool hasImageChanged{ false };

			// GUI
			if (uiManager.mImageWorldSizes[i].hasChanged()) {
				mImages[i].setWorldSize(uiManager.mImageWorldSizes[i].data());
				hasImageChanged = true;
			}

			// GUI
			if (uiManager.mImagePixelDimensions[i].hasChanged()) {
				mImages[i].updatePixelDim(uiManager.mImagePixelDimensions[i].data());
				hasImageChanged = true;
			}

			// The above position calculation
			if (mImages[i].getWorldPos() != mImageWorldPositions[i]) { // Updated automatically
				mImages[i].setWorldPos(mImageWorldPositions[i]);
				hasImageChanged = true;
			}

			if (hasImageChanged || hasTerrainChanged) {
				mImages[i].updateTexture(mScreenQuad, mTerrainImageShader); // binds another shader
			}
		}
		mTerrainImagesInfo.updateGPU({ uiManager.getImageSizes(), mImageWorldPositions });

		// Render skybox
		if (!uiManager.mIsDeferredRendering.data()) {
			targetFramebuffer.use();
			mSkyboxShader.use();
			mDaySkybox.bindTexture(8);
			mCubeVertices.useVertexArray();
			glDisable(GL_DEPTH_TEST);
			glDrawElements(GL_TRIANGLES, mCubeVertices.getIndexCount(), GL_UNSIGNED_INT, 0);
			glEnable(GL_DEPTH_TEST);
		}

		for (int i{ 0 }; i < ImageCount; ++i) {
			mImages[i].bindImage(i);
		}

		if (uiManager.mIsDeferredRendering.data()) {
			mDeferredRenderer.mFramebuffer.use();
			glClearColor(0, 0, 0, -3);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderTerrain(camera, camera.getPosition(), mDeferredRenderer.mShaderTerrainGeometry, mDeferredRenderer.mShaderWaterGeometry, uiManager, dirToSun, time);

			mShadowMapper.updateCameras(dirToSun, camera, getSceneWorldAABB(camera.getPosition(), uiManager), uiManager);
			for (size_t i{ 0 }; i < 3; ++i) {
				const CameraI& depthCamera{ mShadowMapper.getCamera(i) };
				const Framebuffer<0>& depthFramebuffer{ mShadowMapper.getFramebuffer(i) };
				depthFramebuffer.use();
				glClearColor(0, 0, 0, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				renderTerrain(depthCamera, camera.getPosition(), mShadowMapper.mTerrainDepthShader, mShadowMapper.mWaterDepthShader, uiManager, dirToSun, time, true);
			}

			mPerFrameInfo.updateGPU({ camera, dirToSun, time });
			mDeferredRenderer.doDeferredShading(targetFramebuffer, mScreenQuad);
		}
		else {
			targetFramebuffer.use();
			renderTerrain(camera, camera.getPosition(), mTerrainShader, mWaterShader, uiManager, dirToSun, time);
		}
	}

	void renderTerrain(const CameraI& camera, const glm::vec3& chunkCameraCenterPosition, const Shader& terrainShader, const Shader& waterShader, const UIManager& uiManager, const glm::vec3& dirToSun, float time, bool depthPass = false) {
		mPerFrameInfo.updateGPU({ camera, dirToSun, time });
		int chunkCount{ uiManager.mChunkCount.data() };
		float chunkWidth{ uiManager.mTerrainSpan.data() / chunkCount };

		for (int x{ -chunkCount / 2 }; x <= chunkCount / 2; ++x) {
			for (int z{ -chunkCount / 2 }; z <= chunkCount / 2; ++z) {
				
 				glm::vec3 chunkPos{ MathHelper::getClosestWorldStepPosition(chunkCameraCenterPosition, chunkWidth) + glm::vec3(x * chunkWidth, 0, z * chunkWidth) };

				// Frustum culling
				std::array<float, 2> xVals{ chunkPos.x - chunkWidth / 2.0, chunkPos.x + chunkWidth / 2.0 };
				std::array<float, 2> yVals{ mMinTerrainHeight, mMaxTerrainHeight };
				std::array<float, 2> zVals{ chunkPos.z - chunkWidth / 2.0, chunkPos.z + chunkWidth / 2.0 };

				bool isVisible{ false };
				if (uiManager.mFrustumCulling.data())
					isVisible = camera.isAABBVisible({ {chunkPos.x - chunkWidth / 2.0, mMinTerrainHeight, chunkPos.z - chunkWidth / 2.0}, {chunkPos.x + chunkWidth / 2.0, mMaxTerrainHeight, chunkPos.z + chunkWidth / 2.0} });
				else {
					isVisible = true;
				}

				if (isVisible) {
					float chunkDist{ glm::length(chunkPos - camera.getPosition()) };
					bool highQuality{ chunkDist < uiManager.mVertexLODDistanceNear.data() };
					bool mediumQuality{ chunkDist > uiManager.mVertexLODDistanceNear.data() && chunkDist < uiManager.mVertexLODDistanceFar.data() };
					PlaneGPU& currPlane{ highQuality ? mHighQualityPlane : (mediumQuality ? mMediumQualityPlane : mLowQualityPlane) };

					currPlane.useVertexArray();

					// LOD shell count
					int newShellCount{ uiManager.mShellCount.data() };
					if (!depthPass) {
						int oldShellCount{ uiManager.mShellCount.data() };
						float shellLODDistance{ uiManager.mShellLODDistance.data() };
						if (chunkDist > shellLODDistance * 4) {
							newShellCount = oldShellCount > 3 ? 3 : oldShellCount;
							mArtisticParams.updateGPU({ uiManager, newShellCount });
						}
						if (chunkDist > shellLODDistance * 2) {
							newShellCount = oldShellCount > 7 ? 7 : oldShellCount;
							mArtisticParams.updateGPU({ uiManager, newShellCount });
						}
						else if (chunkDist > shellLODDistance) {
							newShellCount = oldShellCount > 10 ? 10 : oldShellCount;
							mArtisticParams.updateGPU({ uiManager, newShellCount });
						}
					}
					else {
						newShellCount = 0;
						mArtisticParams.updateGPU({ uiManager, newShellCount });
					}

					// Draw terrain
					glDisable(GL_BLEND);
					terrainShader.use();
					terrainShader.setVector3("planePos", { chunkPos.x, 0, chunkPos.z });
					terrainShader.setFloat("planeWorldWidth", chunkWidth);
					glDrawElementsInstanced(GL_TRIANGLES, currPlane.getIndexCount(), GL_UNSIGNED_INT, 0, newShellCount + 1); // Draw each shell plus the base terrain

					// Draw water
					waterShader.use();
					waterShader.setVector3("planePos", { chunkPos.x, uiManager.mWaterHeight.data(), chunkPos.z });
					waterShader.setFloat("planeWorldWidth", chunkWidth);

					mReallyLowQualityPlane.useVertexArray();
					glDrawElements(GL_TRIANGLES, mReallyLowQualityPlane.getIndexCount(), GL_UNSIGNED_INT, 0);

					mArtisticParams.updateGPU({ uiManager });
				}
			}
		}
	}

	AABB getSceneWorldAABB(const glm::vec3& playerCameraPos, const UIManager& uiManager) const {
		glm::vec3 minPosition{ -uiManager.mTerrainSpan.data() / 2, mMinTerrainHeight, -uiManager.mTerrainSpan.data() / 2 };
		glm::vec3 maxPosition{  uiManager.mTerrainSpan.data() / 2, mMaxTerrainHeight,  uiManager.mTerrainSpan.data() / 2 };
		minPosition += playerCameraPos;
		maxPosition += playerCameraPos;
		return AABB{ minPosition * 1.1f, maxPosition * 1.1f};
	}

	glm::vec3 getClosestWorldPixelPos(const glm::vec3 pos, int imageIndex, const UIManager& uiManager) {
		return MathHelper::getClosestWorldStepPosition(pos, uiManager.mImageWorldSizes[imageIndex].data() / uiManager.mImagePixelDimensions[imageIndex].data() * uiManager.mTerrainScale.data());
	}

	float getHeightAtPoint(const glm::vec2& worldPos, const UIManager& uiManager) const {
		glm::vec2 pos = worldPos / uiManager.mTerrainScale.data();
		float mountain = MathHelper::perlin(pos * uiManager.mMountainFrequency.data(), 0);
		mountain = pow(mountain, uiManager.mMountainExponent.data());

		mountain = mountain * (1 - uiManager.mAntiFlatFactor.data()) + uiManager.mAntiFlatFactor.data();

		// Rivers
		float river = MathHelper::perlin(pos * uiManager.mRiverFrequency.data(), 1);

		river *= 2;
		river -= 1;
		river = abs(river);
		river = 1 - river;

		river = pow(river, uiManager.mRiverExponent.data());

		river *= uiManager.mRiverStrength.data();
		river *= (mountain * uiManager.mWaterEatingMountain.data() + 1);

		// Lakes
		float lake = MathHelper::perlin(pos * uiManager.mLakeFrequency.data(), 1);
		
		lake = MathHelper::extreme(lake);

		lake = pow(lake, uiManager.mLakeExponent.data());

		lake *= uiManager.mLakeStrength.data();
		lake *= (mountain * uiManager.mWaterEatingMountain.data() + 1);

		float terrainInfo = 0;

		float amplitude = uiManager.mTerrainAmplitude.data();
		float spread = 1;

		for (int i = 0; i < uiManager.mTerrainOctaveCount.data(); ++i) {
			glm::vec2 samplePos = pos * spread;
			float perlinData = MathHelper::perlin(samplePos, 0);

			terrainInfo += amplitude * perlinData;
			amplitude *= uiManager.mTerrainAmplitudeMultiplier.data();
			spread *= uiManager.mTerrainSpreadFactor.data();
		}

		terrainInfo *= mountain;

		terrainInfo -= river;

		terrainInfo -= lake;

		return terrainInfo;
	}

private:
	UniformBuffer<BufferTypes::TerrainParams> mTerrainParams{ 0 };
	UniformBuffer<BufferTypes::ArtisticParams> mArtisticParams{ 1 };
	UniformBuffer<BufferTypes::WaterParams> mWaterParams{ 2 };
	UniformBuffer<BufferTypes::ColourParams> mColourParams{ 3 };
	UniformBuffer<BufferTypes::PerFrameInfo> mPerFrameInfo{ 4 };
	UniformBuffer<BufferTypes::TerrainImagesInfo> mTerrainImagesInfo{ 5, true };
	UniformBuffer<BufferTypes::AtmosphereInfo> mAtmosphereInfo{ 6 };
	std::array<glm::vec2, ImageCount> mImageWorldPositions;
	std::array<TerrainImageGenerator, ImageCount> mImages;
	float mMinTerrainHeight;
	float mMaxTerrainHeight;
	std::array<float, 4> mMinPerlinValues{ {1, 0, 1, 0} };
	std::array<float, 4> mMaxPerlinValues{ {1, 0, 0, 1} };

	Shader mTerrainImageShader;
	Shader mTerrainShader;
	Shader mWaterShader;
	Shader mSkyboxShader;
	Cubemap mDaySkybox;
	Cubemap mNightSkybox;
	CubeVertices mCubeVertices;
	DeferredRenderer mDeferredRenderer;
	ShadowMapper<3> mShadowMapper{ {0.1, 0.5} };

	PlaneGPU mLowQualityPlane;
	PlaneGPU mMediumQualityPlane;
	PlaneGPU mHighQualityPlane;
	PlaneGPU mReallyLowQualityPlane;

	VertexArray mScreenQuad;

	std::array<float, 4> getMaxHeightPerlinValues(const UIManager& uiManager) {
		std::array<float, 4> maxPerlinValues{ 1, 0, 0, 1 };
		float maxHeight{ getHeightWithPerlin(uiManager, maxPerlinValues) };
		for (int i1{ 0 }; i1 <= 100; ++i1) {
			for (int i2{ 0 }; i2 <= 100; ++i2) {
				for (int i3{ 0 }; i3 <= 100; ++i3) {
					for (int i4{ 0 }; i4 <= 100; ++i4) {
						std::array<float, 4> testPerlinValues{ {i1 / 100, i2 / 100, i3 / 100, i4 / 100} };
						float height{ getHeightWithPerlin(uiManager, testPerlinValues) };
						if (height > maxHeight) {
							maxPerlinValues = testPerlinValues;
							maxHeight = height;
						}
					}
				}
			}
		}
		return maxPerlinValues;
	}

	std::array<float, 4> getMinHeightPerlinValues(const UIManager& uiManager) {
		std::array<float, 4> minPerlinValues{ 0, 1, 1, 0 };
		float minHeight{ getHeightWithPerlin(uiManager, minPerlinValues) };
		for (int i1{ 0 }; i1 <= 100; ++i1) {
			for (int i2{ 0 }; i2 <= 100; ++i2) {
				for (int i3{ 0 }; i3 <= 100; ++i3) {
					for (int i4{ 0 }; i4 <= 100; ++i4) {
						std::array<float, 4> testPerlinValues{ {i1 / 100, i2 / 100, i3 / 100, i4 / 100} };
						float height{ getHeightWithPerlin(uiManager, testPerlinValues) };
						if (height < minHeight) {
							minPerlinValues = testPerlinValues;
							minHeight = height;
						}
					}
				}
			}
		}
		return minPerlinValues;
	}

	float getHeightWithPerlin(const UIManager& uiManager, const std::array<float, 4>& perlinValues) {
		float mountain = perlinValues[0];
		mountain = pow(mountain, uiManager.mMountainExponent.data());

		mountain = mountain * (1 - uiManager.mAntiFlatFactor.data()) + uiManager.mAntiFlatFactor.data();

		// Rivers
		float river = perlinValues[1];

		river *= 2;
		river -= 1;
		river = abs(river);
		river = 1 - river;

		river = pow(river, uiManager.mRiverExponent.data());

		river *= uiManager.mRiverStrength.data();
		river *= (mountain * uiManager.mWaterEatingMountain.data() + 1);

		// Lakes
		float lake = perlinValues[2];

		lake = MathHelper::extreme(lake);

		lake = pow(lake, uiManager.mLakeExponent.data());

		lake *= uiManager.mLakeStrength.data();
		lake *= (mountain * uiManager.mWaterEatingMountain.data() + 1);

		float terrainInfo = 0;

		float amplitude = uiManager.mTerrainAmplitude.data();
		float spread = 1;

		for (int i = 0; i < uiManager.mTerrainOctaveCount.data(); ++i) {
			float perlinData = perlinValues[3];

			terrainInfo += amplitude * perlinData;
			amplitude *= uiManager.mTerrainAmplitudeMultiplier.data();
			spread *= uiManager.mTerrainSpreadFactor.data();
		}

		terrainInfo *= mountain;

		terrainInfo -= river;

		terrainInfo -= lake;

		return terrainInfo;
	}
};

#endif