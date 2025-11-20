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
#include "camera.h"
#include <iostream>
#include "cubemap.h"
#include "cubevertices.h"
#include "uimanager.h"
#include "mathhelper.h"
#include "framebuffer.h"
#include "uniformbuffer.h"
#include "deferredrenderer.h"
#include "imagecount.h"

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
				//"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_2_Left+X.png",
				//"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_3_Right-X.png",
				//"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_4_Up+Y.png",
				//"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_5_Down-Y.png",
				//"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_0_Front+Z.png",
				//"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_1_Back-Z.png"
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
		mMinTerrainHeight = -1000;// getMinHeight(uiManager);
		mMaxTerrainHeight = 1000;// getMaxHeight(uiManager);

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

	void render(const Camera& camera, float time, const UIManager& uiManager, const Framebuffer<1>& targetFramebuffer) {
		bool hasTerrainChanged{ mTerrainParams.updateGPU({uiManager}) };
		if (hasTerrainChanged) {
			//mMinTerrainHeight = getMinHeight(uiManager);
			//mMaxTerrainHeight = getMaxHeight(uiManager);
		}
		glm::vec3 dirToSun{ MathHelper::getDirToSun(uiManager) };
		mArtisticParams.updateGPU(uiManager);
		mWaterParams.updateGPU(uiManager);
		mColourParams.updateGPU(uiManager);
		mPerFrameInfo.updateGPU({ camera, dirToSun, time });
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

		glm::vec3 cameraForward{ camera.getForward() };

		if (uiManager.mIsDeferredRendering.data()) {
			mDeferredRenderer.useFramebuffer();
			glClearColor(0, 0, 0, -3);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else {
			targetFramebuffer.use();
		}
		// For each chunk
		int verticesDrawn{ 0 };
		int visibleChunks{ 0 };
		for (int x{ -chunkCount / 2 }; x <= chunkCount / 2; ++x) {
			for (int z{ -chunkCount / 2 }; z <= chunkCount / 2; ++z) {

				glm::vec3 chunkPos{ MathHelper::getClosestWorldStepPosition(camera.getPosition(), chunkWidth) - glm::vec3(x * chunkWidth, 0, z * chunkWidth)};

				// Frustum culling
				std::array<float, 2> xVals{ chunkPos.x - chunkWidth / 2.0, chunkPos.x + chunkWidth / 2.0 };
				std::array<float, 2> yVals{ mMinTerrainHeight, mMaxTerrainHeight};
				std::array<float, 2> zVals{ chunkPos.z - chunkWidth / 2.0, chunkPos.z + chunkWidth / 2.0 };

				bool isVisible{ false };
				if (uiManager.mFrustumCulling.data())
					isVisible = isAABBInFrustum(camera, { {chunkPos.x - chunkWidth / 2.0, mMinTerrainHeight, chunkPos.z - chunkWidth / 2.0}, {chunkPos.x + chunkWidth / 2.0, mMaxTerrainHeight, chunkPos.z + chunkWidth / 2.0} });
				else {
					isVisible = true;
				}

				if (isVisible) {
					visibleChunks += 1;
					float chunkDist{ glm::length(chunkPos - camera.getPosition()) };
					bool highQuality{ chunkDist < uiManager.mVertexLODDistanceNear.data() };
					bool mediumQuality{ chunkDist > uiManager.mVertexLODDistanceNear.data() && chunkDist < uiManager.mVertexLODDistanceFar.data() };
					PlaneGPU& currPlane{ highQuality ? mHighQualityPlane : (mediumQuality ? mMediumQualityPlane : mLowQualityPlane) };

					//mTerrainShader.setVector3("planePos", { chunkPos.x, 0, chunkPos.z });
					//mTerrainShader.setFloat("planeWorldWidth", chunkWidth);

					currPlane.useVertexArray();

					// LOD shell count
					int newShellCount{ uiManager.mShellCount.data() };
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

					// Draw terrain
					if (uiManager.mIsDeferredRendering.data()) {
						glDisable(GL_BLEND);
						mDeferredRenderer.useShaderTerrainGeometryPass();
						mDeferredRenderer.setTerrainPlaneInfo({ chunkPos.x, 0, chunkPos.z }, chunkWidth);
					}
					else {
						mTerrainShader.use();
						mTerrainShader.setVector3("planePos", { chunkPos.x, 0, chunkPos.z });
						mTerrainShader.setFloat("planeWorldWidth", chunkWidth);
					}
					verticesDrawn += (newShellCount + 1) * currPlane.getIndexCount();
					glDrawElementsInstanced(GL_TRIANGLES, currPlane.getIndexCount(), GL_UNSIGNED_INT, 0, newShellCount + 1); // Draw each shell plus the base terrain

					// Draw water
					if (uiManager.mIsDeferredRendering.data()) {
						glDisable(GL_BLEND);
						mDeferredRenderer.useShaderWaterGeometryPass();
						mDeferredRenderer.setWaterPlaneInfo({ chunkPos.x, uiManager.mWaterHeight.data(), chunkPos.z }, chunkWidth);
					}
					else {
						mWaterShader.use();
						mWaterShader.setVector3("planePos", { chunkPos.x, uiManager.mWaterHeight.data(), chunkPos.z });
						mWaterShader.setFloat("planeWorldWidth", chunkWidth);
					}
					mReallyLowQualityPlane.useVertexArray();
					glDrawElements(GL_TRIANGLES, mReallyLowQualityPlane.getIndexCount(), GL_UNSIGNED_INT, 0);

					mArtisticParams.updateGPU({ uiManager });
				}
			}
		}
		if (uiManager.mIsDeferredRendering.data())
			mDeferredRenderer.doDeferredShading(targetFramebuffer, mScreenQuad);
		//std::cout << verticesDrawn << "\n";
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
	// The chunk collection consists of a square of chunkCount * chunkCount chunks, each having a width of chunkWidth

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

	Shader mTerrainImageShader;
	Shader mTerrainShader;
	Shader mWaterShader;
	Shader mSkyboxShader;
	Cubemap mDaySkybox;
	Cubemap mNightSkybox;
	CubeVertices mCubeVertices;
	DeferredRenderer mDeferredRenderer;
	//glm::vec3 mDirToLight{ -0.008373, 0.089878, 0.995917 };

	PlaneGPU mLowQualityPlane;
	PlaneGPU mMediumQualityPlane;
	PlaneGPU mHighQualityPlane;
	PlaneGPU mReallyLowQualityPlane;

	VertexArray mScreenQuad;

	float getMaxHeight(const UIManager& uiManager) {
		float mountain = 1;
		mountain = pow(mountain, uiManager.mMountainExponent.data());

		mountain = mountain * (1 - uiManager.mAntiFlatFactor.data()) + uiManager.mAntiFlatFactor.data();

		// Rivers
		float river = 0;

		river *= 2;
		river -= 1;
		river = abs(river);
		river = 1 - river;

		river = pow(river, uiManager.mRiverExponent.data());

		river *= uiManager.mRiverStrength.data();
		river *= (mountain * uiManager.mWaterEatingMountain.data() + 1);

		// Lakes
		float lake = 0;

		lake = MathHelper::extreme(lake);

		lake = pow(lake, uiManager.mLakeExponent.data());

		lake *= uiManager.mLakeStrength.data();
		lake *= (mountain * uiManager.mWaterEatingMountain.data() + 1);

		float terrainInfo = 0;

		float amplitude = uiManager.mTerrainAmplitude.data();
		float spread = 1;

		for (int i = 0; i < uiManager.mTerrainOctaveCount.data(); ++i) {
			float perlinData = 1;

			terrainInfo += amplitude * perlinData;
			amplitude *= uiManager.mTerrainAmplitudeMultiplier.data();
			spread *= uiManager.mTerrainSpreadFactor.data();
		}

		terrainInfo *= mountain;

		terrainInfo -= river;

		terrainInfo -= lake;

		return terrainInfo;
	}

	float getMinHeight(const UIManager& uiManager) {
		float mountain = 0;
		mountain = pow(mountain, uiManager.mMountainExponent.data());

		mountain = mountain * (1 - uiManager.mAntiFlatFactor.data()) + uiManager.mAntiFlatFactor.data();

		// Rivers
		float river = 1;

		river *= 2;
		river -= 1;
		river = abs(river);
		river = 1 - river;

		river = pow(river, uiManager.mRiverExponent.data());

		river *= uiManager.mRiverStrength.data();
		river *= (mountain * uiManager.mWaterEatingMountain.data() + 1);

		// Lakes
		float lake = 1;

		lake = MathHelper::extreme(lake);

		lake = pow(lake, uiManager.mLakeExponent.data());

		lake *= uiManager.mLakeStrength.data();
		lake *= (mountain * uiManager.mWaterEatingMountain.data() + 1);

		float terrainInfo = 0;

		float amplitude = uiManager.mTerrainAmplitude.data();
		float spread = 1;

		for (int i = 0; i < uiManager.mTerrainOctaveCount.data(); ++i) {
			float perlinData = 0;

			terrainInfo += amplitude * perlinData;
			amplitude *= uiManager.mTerrainAmplitudeMultiplier.data();
			spread *= uiManager.mTerrainSpreadFactor.data();
		}

		terrainInfo *= mountain;

		terrainInfo -= river;

		terrainInfo -= lake;

		return terrainInfo;
	}

	struct AABB {
		glm::vec3 mMin;
		glm::vec3 mMax;
	};

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

	static bool doesOBBOverlapFrustumAlongAxis(const OBB& obb, const glm::vec3& axis, float xNear, float yNear, float near, float far) {
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
	static bool isAABBInFrustum(const Camera& camera, const AABB& aabb) {
		float xNear = camera.getXNear();
		float yNear = camera.getYNear();
		float near = -camera.getNearPlaneDist();
		float far = -camera.getFarPlaneDist();

		std::array<glm::vec3, 4> obbCorners{ {
				aabb.mMin,
			   {aabb.mMax.x, aabb.mMin.y, aabb.mMin.z},
			   {aabb.mMin.x, aabb.mMax.y, aabb.mMin.z},
			   {aabb.mMin.x, aabb.mMin.y, aabb.mMax.z}
			} };

		for (size_t i{ 0 }; i < obbCorners.size(); ++i) {
			obbCorners[i] = camera.getViewMatrix() * glm::vec4{ obbCorners[i], 1 };
		}

		OBB obb{ obbCorners };
		 
		std::array<glm::vec3, 5> frustumNormals{ {
			{ 0.0, 0.0, 1 },
			{ 0.0, -near, yNear },
			{ 0.0, near, yNear },
			{ -near, 0.0f, xNear },
			{ near, 0.0f, xNear }
		} };

		for (const glm::vec3& frustumNormal : frustumNormals) {
			if (!doesOBBOverlapFrustumAlongAxis(obb, frustumNormal, xNear, yNear, near, far)) {
				return false;
			}
		}

		for (const glm::vec3& obbAxis : obb.mAxes) {
			if (!doesOBBOverlapFrustumAlongAxis(obb, obbAxis, xNear, yNear, near, far)) {
				return false;
			}
		}

		std::array<glm::vec3, 18> edgeCrosses;

		for (size_t obbAxisIndex{ 0 }; obbAxisIndex < 3; ++obbAxisIndex) {
			edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 1, 0, 0 });
			edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 0, 1, 0 });

			edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 0, 1, near });
			edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 0, 1, near });
			edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 0, 1, near });
			edgeCrosses[obbAxisIndex + 0] = glm::cross(obb.mAxes[obbAxisIndex], { 0, yNear, near });
		}

		for (const glm::vec3& edgeCross : edgeCrosses) {
			if (!doesOBBOverlapFrustumAlongAxis(obb, edgeCross, xNear, yNear, near, far)) {
				return false;
			}
		}

		return true;
	}
};

#endif