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
#include "paramsbufferartistic.h"
#include "paramsbufferterrain.h"
#include "paramsbufferwater.h"
#include "paramsbuffercolour.h"
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

constexpr int ImageCount{ 4 };
//template <int ImageCount>

class TerrainRenderer {
public:
	TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos, std::array<glm::vec2, ImageCount> imageWorldPositions, const UIManager& uiManager)
		
		: mArtisticParams{ uiManager }
		, mTerrainParams{ uiManager }
		, mWaterParams{ uiManager }
		, mColours{ uiManager }


		, mLowQualityPlane{ uiManager.mLowQualityPlaneVertices.data() }
		, mHighQualityPlane{ uiManager.mHighQualityPlaneQualityScale.data() }

		, mTerrainImageShader{ "assets/shaders/terrainimage.vert", "assets/shaders/terrainimage.frag" }
		, mTerrainShader{ "assets/shaders/terrain.vert", "assets/shaders/terrain.frag" }
		, mWaterShader{ "assets/shaders/water.vert", "assets/shaders/water.frag" }
		, mSkyboxShader{ "assets/shaders/skybox.vert", "assets/shaders/skybox.frag" }

		, mImageWorldPositions{ imageWorldPositions }

		, mImages{ {
			{uiManager.mImagePixelDimensions[0].data(), uiManager.mImageWorldSizes[0].data(), screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 0, uiManager)},
			{uiManager.mImagePixelDimensions[1].data(), uiManager.mImageWorldSizes[1].data(), screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 1, uiManager)},
			{uiManager.mImagePixelDimensions[2].data(), uiManager.mImageWorldSizes[2].data(), screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 2, uiManager)},
			{uiManager.mImagePixelDimensions[3].data(), uiManager.mImageWorldSizes[3].data(), screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 3, uiManager)}
		} }

		, mDaySkybox{ {
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_2_Left+X.png",
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_3_Right-X.png",
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_4_Up+Y.png",
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_5_Down-Y.png",
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_0_Front+Z.png",
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_1_Back-Z.png"
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_2_Left+X.png",
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_3_Right-X.png",
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_4_Up+Y.png",
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_5_Down-Y.png",
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_0_Front+Z.png",
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_1_Back-Z.png"
			} }
		, mNightSkybox{ {
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_2_Left+X.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_3_Right-X.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_4_Up+Y.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_5_Down-Y.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_0_Front+Z.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_1_Back-Z.png"
			} }
	{
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
		glm::vec3 dirToSun{ MathHelper::getDirToSun(uiManager) };
		mWaterShader.use();
		mWaterShader.setInt("skybox", 7);
		mWaterShader.setVector3("dirToLight", dirToSun);
		mTerrainShader.use();
		mTerrainShader.setInt("skybox", 7);
		mTerrainShader.setInt("imageCount", ImageCount);
		mTerrainShader.setVector3("dirToLight", dirToSun);
		for (int i{ 0 }; i < ImageCount; ++i) {
			std::string indexString{ std::to_string(i) };

			mTerrainShader.use();
			mTerrainShader.setInt("images[" + indexString + "]", i);
			mTerrainShader.setFloat("imageScales[" + indexString + "]", uiManager.mImageWorldSizes[i].data());
			mTerrainShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);
			// TODO 2 of these?
			mWaterShader.use();
			mWaterShader.setInt("images[" + indexString + "]", i);
			mWaterShader.setFloat("imageScales[" + indexString + "]", uiManager.mImageWorldSizes[i].data());
			mWaterShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);

			mImages[i].updateTexture(mScreenQuad, mTerrainImageShader);
		}

		mSkyboxShader.use();
		mSkyboxShader.setInt("skybox", 7);
		mSkyboxShader.setVector3("dirToLight", dirToSun);
	}

	void render(const Camera& camera, float time, const UIManager& uiManager, const Framebuffer& framebuffer) {
		bool hasTerrainChanged{ mTerrainParams.updateGPU(uiManager, false) };
		mArtisticParams.updateGPU(uiManager, false);
		mWaterParams.updateGPU(uiManager, false);
		mColours.updateGPU(uiManager, false);
		mTerrainShader.use();

		// Update plane types
		if (uiManager.mLowQualityPlaneVertices.hasChanged()) {
			mLowQualityPlane.rebuild(uiManager.mLowQualityPlaneVertices.data());
		}

		int highQualityVerticesPerEdge{ uiManager.mHighQualityPlaneQualityScale.data() * (uiManager.mLowQualityPlaneVertices.data() - 1) + 1}; // We want the distance between vertices to be multiples of each other, so we do this
		if (highQualityVerticesPerEdge != mHighQualityPlane.getVerticesPerEdge()) {
			mHighQualityPlane.rebuild(highQualityVerticesPerEdge);
		}

		// Update images
		for (int i{ 0 }; i < ImageCount; ++i) {

			// Move images along with the player
			glm::vec2 scaledCameraPos{ glm::vec2(camera.getPosition().x, camera.getPosition().z) / uiManager.mTerrainScale.data() };
			double cameraDistFromImageCenter{ glm::length(scaledCameraPos - mImageWorldPositions[i]) };
			if (cameraDistFromImageCenter * 2 > 0.2 * uiManager.mImageWorldSizes[i].data()) { // If near edge of image, update image
				glm::vec3 pixelPosition{ getClosestWorldPixelPos(camera.getPosition() / uiManager.mTerrainScale.data(), i, uiManager)};
				mImageWorldPositions[i] = glm::vec2(pixelPosition.x, pixelPosition.z);
			}

			// Recalculate image checks
			std::string indexString{ std::to_string(i) };
			bool hasImageChanged{ false };

			// GUI
			if (uiManager.mImageWorldSizes[i].hasChanged()) {
				mImages[i].setWorldSize(uiManager.mImageWorldSizes[i].data());

				mTerrainShader.use();
				mTerrainShader.setFloat("imageScales[" + indexString + "]", uiManager.mImageWorldSizes[i].data());

				mWaterShader.use();
				mWaterShader.setFloat("imageScales[" + indexString + "]", uiManager.mImageWorldSizes[i].data());

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

				mTerrainShader.use();
				mTerrainShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);

				mWaterShader.use();
				mWaterShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);

				hasImageChanged = true;
			}

			if (hasImageChanged || hasTerrainChanged) {
				mImages[i].updateTexture(mScreenQuad, mTerrainImageShader); // binds another shader
				mTerrainShader.use();
			}
		}

		glm::vec3 dirToSun{ MathHelper::getDirToSun(uiManager) };

		framebuffer.bind();
		// Render skybox
		mSkyboxShader.use();
		mSkyboxShader.setMatrix4("view", camera.getViewMatrix()); // TODO make these just buffers
		mSkyboxShader.setMatrix4("proj", camera.getProjectionMatrix());
		mSkyboxShader.setVector3("dirToLight", dirToSun);
		mDaySkybox.bindTexture(7);
		mCubeVertices.useVertexArray();
		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, mCubeVertices.getIndexCount(), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);

		mTerrainShader.use();
		mTerrainShader.setMatrix4("view", camera.getViewMatrix());
		mTerrainShader.setMatrix4("proj", camera.getProjectionMatrix());
		mTerrainShader.setVector3("cameraPos", camera.getPosition());
		mTerrainShader.setFloat("waterHeight", uiManager.mWaterHeight.data());
		mTerrainShader.setVector3("dirToLight", dirToSun);
		mWaterShader.use();
		mWaterShader.setMatrix4("view", camera.getViewMatrix());
		mWaterShader.setMatrix4("proj", camera.getProjectionMatrix());
		mWaterShader.setVector3("cameraPos", camera.getPosition());
		mWaterShader.setFloat("time", time);
		mWaterShader.setVector3("dirToLight", dirToSun);
		for (int i{ 0 }; i < mImages.size(); ++i) {
			mImages[i].bindImage(i);
		}

		float maxChunkHeight{ getMaxHeight(uiManager) };

		float minChunkHeight{ getMinHeight(uiManager) };
		glm::vec3 cameraForward{ camera.getForward() };
		// For each chunk
		int chunkCount{ uiManager.mChunkCount.data() };
		float chunkWidth{ uiManager.mChunkWidth.data() };
 		for (int x{ -chunkCount / 2 }; x <= chunkCount / 2; ++x) {
			for (int z{ -chunkCount / 2 }; z <= chunkCount / 2; ++z) {

				mTerrainShader.use();
				//glm::vec3 chunkPos{ getClosestWorldVertexPos(camera.getPosition()) - glm::vec3(x * mChunkWidth, 0, z * mChunkWidth) };
				glm::vec3 chunkPos{ mLowQualityPlane.getClosestWorldVertexPos(camera.getPosition(), chunkWidth) - glm::vec3(x * chunkWidth, 0, z * chunkWidth) };

				// Frustum culling
				std::array<float, 2> xVals{ chunkPos.x - chunkWidth / 2.0, chunkPos.x + chunkWidth / 2.0 };
				std::array<float, 2> yVals{ minChunkHeight - chunkWidth / 2.0, maxChunkHeight + chunkWidth / 2.0 };
				std::array<float, 2> zVals{ chunkPos.z - chunkWidth / 2.0, chunkPos.z + chunkWidth / 2.0 };

				bool isVisible{ false };
				for (float x : xVals) {
					for (float y : yVals) {
						for (float z : zVals) {
							glm::vec3 corner{ x, y, z };
							glm::vec3 viewDir{ glm::normalize(corner - camera.getPosition()) };
							if (glm::dot(viewDir, cameraForward) > 0) {
								isVisible = true;
								break;
							}
						}
						if (isVisible)
							break;
					}
					if (isVisible)
						break;
				}

				if (isVisible) {
					float chunkDist{ glm::length(chunkPos - camera.getPosition()) };
					bool highQuality{ !(chunkDist > uiManager.mVertexLODDistance.data()) };
					PlaneGPU& currPlane{ highQuality ? mHighQualityPlane : mLowQualityPlane };

					mTerrainShader.setVector3("planePos", { chunkPos.x, 0, chunkPos.z });
					mTerrainShader.setFloat("planeWorldWidth", chunkWidth);

					currPlane.useVertexArray();

					// LOD shell count
					int newShellCount{ uiManager.mShellCount.data() };
					int oldShellCount{ uiManager.mShellCount.data() };
					float shellLODDistance{ uiManager.mShellLODDistance.data() };
					if (chunkDist > shellLODDistance * 4) {
						newShellCount = oldShellCount > 3 ? 3 : oldShellCount;
						mArtisticParams.forceShaderShellCount(newShellCount);
					}
					if (chunkDist > shellLODDistance * 2) {
						newShellCount = oldShellCount > 7 ? 7 : oldShellCount;
						mArtisticParams.forceShaderShellCount(newShellCount);
					}
					else if (chunkDist > shellLODDistance) {
						newShellCount = oldShellCount > 10 ? 10 : oldShellCount;
						mArtisticParams.forceShaderShellCount(newShellCount);
					}

					// Draw terrain
					// glInstanceID is 1 greater than the shellIndex (base terrain is -1 shell index, first shell is 0 shell index)
					for (int i{ 0 }; i <= newShellCount; ++i) {
						mTerrainShader.setInt("instanceID", i);
						glDrawElements(GL_TRIANGLES, currPlane.getIndexCount(), GL_UNSIGNED_INT, 0); // Draw each shell plus the base terrain
					}
					// I could do each of the plane qualities in one instanced call, but for some reason it is slightly slower

					// Draw water
					mWaterShader.use();
					mWaterShader.setVector3("planePos", { chunkPos.x, uiManager.mWaterHeight.data(), chunkPos.z});
					mWaterShader.setFloat("planeWorldWidth", chunkWidth);
					glDrawElements(GL_TRIANGLES, currPlane.getIndexCount(), GL_UNSIGNED_INT, 0); // Draw each shell plus the base terrain

					mArtisticParams.fixShellCount(uiManager);
				}
			}
		}
	}

	glm::vec3 getClosestWorldPixelPos(const glm::vec3 pos, int imageIndex, const UIManager& uiManager) {
		return MathHelper::getClosestWorldStepPosition(pos, uiManager.mImageWorldSizes[imageIndex].data() / uiManager.mImagePixelDimensions[imageIndex].data() * uiManager.mTerrainScale.data());
	}

	static float quintic(float x) {
		return x < 0.5 ? (16 * x * x * x * x * x) : 1 - pow(-2 * x + 2, 5.0) / 2.0;
	}

	float getHeightAtPoint(const glm::vec2& worldPos, const UIManager& uiManager) const {

		glm::vec2 pos = worldPos / uiManager.mTerrainScale.data();
		float mountain = MathHelper::perlin(pos * uiManager.mMountainFrequency.data(), 0);

		mountain = pow(mountain, uiManager.mMountainExponent.data());

		mountain = mountain * (1 - uiManager.mAntiFlatFactor.data()) + uiManager.mAntiFlatFactor.data();

		// Rivers
		float river = MathHelper::perlin(pos * uiManager.mRiverFrequency.data() , 1);

		river *= 2;
		river -= 1;
		river = abs(river);
		river = 1 - river;

		river = pow(river, uiManager.mRiverExponent.data());

		river *= uiManager.mRiverStrength.data();
		river *= (mountain * 5 + 1);

		float terrainHeight = 0;

		float amplitude = uiManager.mTerrainAmplitude.data();
		float spread = 1;

		for (int i = 0; i < uiManager.mTerrainOctaveCount.data(); ++i) {
			glm::vec2 samplePos = pos * spread;
			float perlinData = MathHelper::perlin(samplePos, 0);

			terrainHeight += amplitude * perlinData;

			amplitude *= uiManager.mTerrainAmplitudeMultiplier.data();
			spread *= uiManager.mTerrainSpreadFactor.data();
		}

		float finalOutput = 0;
		finalOutput = terrainHeight * mountain;

		finalOutput -= river;
		return finalOutput;
	}

private:
	// The chunk collection consists of a square of chunkCount * chunkCount chunks, each having a width of chunkWidth

	ParamsBufferArtistic mArtisticParams;
	ParamsBufferTerrain mTerrainParams;
	ParamsBufferWater mWaterParams;
	ParamsBufferColour mColours;
	std::array<glm::vec2, ImageCount> mImageWorldPositions;
	std::array<TerrainImageGenerator, ImageCount> mImages;

	Shader mTerrainImageShader;
	Shader mTerrainShader;
	Shader mWaterShader;
	Shader mSkyboxShader;
	Cubemap mDaySkybox;
	Cubemap mNightSkybox;
	CubeVertices mCubeVertices;
	//glm::vec3 mDirToLight{ -0.008373, 0.089878, 0.995917 };

	PlaneGPU mLowQualityPlane;
	PlaneGPU mHighQualityPlane;

	VertexArray mScreenQuad;

	float getMaxHeight(const UIManager& uiManager) {
		glm::vec3 mountain = { 1, 0, 0 };

		mountain.x *= mountain.x;
		mountain.x *= mountain.x;
		mountain.x = mountain.x * 0.95 + 0.05;

		glm::vec3 offset = { 1, 0, 0 };

		offset.x = offset.x < 0.5 ? (16 * offset.x * offset.x * offset.x * offset.x * offset.x) : 1 - pow(-2 * offset.x + 2, 5.0) / 2.0;

		offset *= 20;

		glm::vec3 terrainInfo = { 0, 0, 0 };

		float amplitude = uiManager.mTerrainAmplitude.data();

		for (int i{ 0 }; i < uiManager.mTerrainOctaveCount.data(); ++i) {
			glm::vec3 perlinData = { 1, 0, 0 };

			terrainInfo.x += amplitude * perlinData.x;

			amplitude *= uiManager.mTerrainAmplitudeMultiplier.data();
		}

		glm::vec3 finalOutput = { 0, 0, 0 };
		finalOutput.x = terrainInfo.x * mountain.x;

		finalOutput.x += offset.x;
		return finalOutput.x;
	}

	float getMinHeight(const UIManager& uiManager) {
		glm::vec3 mountain = { 0, 0, 0 };

		mountain.x *= mountain.x;
		mountain.x *= mountain.x;
		mountain.x = mountain.x * 0.95 + 0.05;

		glm::vec3 offset = { 0, 0, 0 };

		offset.x = offset.x < 0.5 ? (16 * offset.x * offset.x * offset.x * offset.x * offset.x) : 1 - pow(-2 * offset.x + 2, 5.0) / 2.0;

		offset *= 20;

		glm::vec3 terrainInfo = { 0, 0, 0 };

		float amplitude = uiManager.mTerrainAmplitude.data();

		for (int i{ 0 }; i < uiManager.mTerrainOctaveCount.data(); ++i) {
			glm::vec3 perlinData = { 0, 0, 0 };

			terrainInfo.x += amplitude * perlinData.x;

			amplitude *= uiManager.mTerrainAmplitudeMultiplier.data();
		}

		glm::vec3 finalOutput = { 0, 0, 0 };
		finalOutput.x = terrainInfo.x * mountain.x;

		finalOutput.x += offset.x;
		return finalOutput.x;
	}
};

#endif