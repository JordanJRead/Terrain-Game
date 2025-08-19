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
#include "artisticparamsbuffer.h"
#include "terrainparamsbuffer.h"
#include "waterparamsbuffer.h"
#include "colourbuffer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "camera.h"
#include <iostream>
#include "cubemap.h"
#include "cubevertices.h"

constexpr int ImageCount{ 4 };
//template <int ImageCount>

class TerrainRenderer {
public:
	TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos,
		int chunkWidth, int chunkCount, const ArtisticParamsData& artistParams, const TerrainParamsData& terrainParams, const WaterParamsData& waterParams, const ColourBufferData& colours,
		std::array<int, ImageCount> imagePixelDims, std::array<float, ImageCount> imageWorldSizes, std::array<glm::vec2, ImageCount> imageWorldPositions,
		int lowQualityPlaneVerticesPerEdge, int highQualityPlaneVerticesPerEdgeScale, float vertexQualityDropoffDistance, float shellQualityDropoffDistance, float waterHeight, float dayTime)
		: mChunkWidth{ chunkWidth }
		, mChunkCount{ chunkCount }
		
		, mArtisticParams{ artistParams }
		, mTerrainParams{ terrainParams }
		, mWaterParams{ waterParams }
		, mColours{ colours }

		, mLowQualityPlaneVerticesPerEdge{ lowQualityPlaneVerticesPerEdge }
		, mHighQualityPlaneVerticesPerEdgeScale{ highQualityPlaneVerticesPerEdgeScale }

		, mLowQualityPlane{ mLowQualityPlaneVerticesPerEdge }
		, mHighQualityPlane{ mHighQualityPlaneVerticesPerEdgeScale }

		, mTerrainImageShader{ "assets/shaders/terrainimage.vert", "assets/shaders/terrainimage.frag" }
		, mTerrainShader{ "assets/shaders/terrain.vert", "assets/shaders/terrain.frag" }
		, mWaterShader{ "assets/shaders/water.vert", "assets/shaders/water.frag" }
		, mSkyboxShader{ "assets/shaders/skybox.vert", "assets/shaders/skybox.frag" }

		, mImageWorldPositions{ imageWorldPositions }
		, mImagePixelDims{ imagePixelDims }
		, mImageWorldSizes{ imageWorldSizes }

		, mVertexQualityDropoffDistance{ vertexQualityDropoffDistance }
		, mShellQualityDropoffDistance{ shellQualityDropoffDistance }

		, mImages{ {
			{mImagePixelDims[0], mImageWorldSizes[0], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 0)},
			{mImagePixelDims[1], mImageWorldSizes[1], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 1)},
			{mImagePixelDims[2], mImageWorldSizes[2], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 2)},
			{mImagePixelDims[3], mImageWorldSizes[3], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 3)}
		} }

		, mWaterHeight{ waterHeight }

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
		, mDayTime{ dayTime }
	{
		std::vector<float> vertexData{
		-1, -1,
		 1, -1,
		-1,  1,
		 1,  1
		};

		std::vector<unsigned int> indices{
			0, 1, 2, 1, 2, 3
		};

		std::vector<int> attribs{
			2
		};

		mScreenQuad.create(vertexData, indices, attribs);

		// Set shader uniforms
		glm::vec3 dirToSun{ getDirToSun() };
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
			mTerrainShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);
			mTerrainShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);

			mWaterShader.use();
			mWaterShader.setInt("images[" + indexString + "]", i);
			mWaterShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);
			mWaterShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);

			mImages[i].updateTexture(mScreenQuad, mTerrainImageShader);
		}

		mSkyboxShader.use();
		mSkyboxShader.setInt("skybox", 7);
		mSkyboxShader.setVector3("dirToLight", dirToSun);
	}

	void render(const Camera& camera, double displayDeltaTime, float time) {
		bool hasTerrainChanged{ mTerrainParams.updateGPU(false) };
		mArtisticParams.updateGPU(false);
		mWaterParams.updateGPU(false);
		mColours.updateGPU(false);
		mTerrainShader.use();

		// Update plane types
		if (mLowQualityPlaneVerticesPerEdge != mLowQualityPlane.getVerticesPerEdge()) {
			mLowQualityPlane.rebuild(mLowQualityPlaneVerticesPerEdge);
		}

		int highQualityVerticesPerEdge{ mHighQualityPlaneVerticesPerEdgeScale * (mLowQualityPlaneVerticesPerEdge - 1) + 1 }; // We want the distance between vertices to be multiples of each other, so we do this
		if (highQualityVerticesPerEdge != mHighQualityPlane.getVerticesPerEdge()) {
			mHighQualityPlane.rebuild(highQualityVerticesPerEdge);
		}

		// Update images
		for (int i{ 0 }; i < ImageCount; ++i) {

			// Move images along with the player
			glm::vec2 scaledCameraPos{ glm::vec2(camera.getPosition().x, camera.getPosition().z) / mArtisticParams.getTerrainScale() };
			double cameraDistFromImageCenter{ glm::length(scaledCameraPos - mImageWorldPositions[i]) };
			if (cameraDistFromImageCenter * 2 > 0.2 * mImageWorldSizes[i]) { // If near edge of image, update image
				glm::vec3 pixelPosition{ getClosestWorldPixelPos(camera.getPosition() / mArtisticParams.getTerrainScale(), i)};
				mImageWorldPositions[i] = glm::vec2(pixelPosition.x, pixelPosition.z);
			}

			// Recalculate image checks
			std::string indexString{ std::to_string(i) };
			bool hasImageChanged{ false };

			// GUI
			if (mImages[i].getWorldSize() != mImageWorldSizes[i]) {
				mImages[i].setWorldSize(mImageWorldSizes[i]);

				mTerrainShader.use();
				mTerrainShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);

				mWaterShader.use();
				mWaterShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);

				hasImageChanged = true;
			}

			// GUI
			if (mImages[i].getPixelDim() != mImagePixelDims[i]) {
				mImages[i].updatePixelDim(mImagePixelDims[i]);
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

		glm::vec3 dirToSun{ getDirToSun() };
		// Render skybox
		mSkyboxShader.use();
		mSkyboxShader.setMatrix4("view", camera.getViewMatrix());
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
		mTerrainShader.setFloat("waterHeight", mWaterHeight);
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

		float maxChunkHeight{ getMaxHeight() };

		float minChunkHeight{ getMinHeight() };
		glm::vec3 cameraForward{ camera.getForward() };
		// For each chunk
 		for (int x{ -mChunkCount / 2 }; x <= mChunkCount / 2; ++x) {
			for (int z{ -mChunkCount / 2 }; z <= mChunkCount / 2; ++z) {

				mTerrainShader.use();
				//glm::vec3 chunkPos{ getClosestWorldVertexPos(camera.getPosition()) - glm::vec3(x * mChunkWidth, 0, z * mChunkWidth) };
				glm::vec3 chunkPos{ mLowQualityPlane.getClosestWorldVertexPos(camera.getPosition(), mChunkWidth) - glm::vec3(x * mChunkWidth, 0, z * mChunkWidth) };

				// Frustum culling
				std::array<float, 2> xVals{ chunkPos.x - mChunkWidth / 2.0, chunkPos.x + mChunkWidth / 2.0 };
				std::array<float, 2> yVals{ minChunkHeight - mChunkWidth / 2.0, maxChunkHeight + mChunkWidth / 2.0 };
				std::array<float, 2> zVals{ chunkPos.z - mChunkWidth / 2.0, chunkPos.z + mChunkWidth / 2.0 };

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
					bool highQuality{ !(chunkDist > mVertexQualityDropoffDistance) };
					PlaneGPU& currPlane{ highQuality ? mHighQualityPlane : mLowQualityPlane };

					mTerrainShader.setVector3("planePos", { chunkPos.x, 0, chunkPos.z });
					mTerrainShader.setFloat("planeWorldWidth", mChunkWidth);

					currPlane.useVertexArray();

					// LOD shell count
					int shellCount{ mArtisticParams.getShellCount() };
					if (chunkDist > mShellQualityDropoffDistance * 4) {
						shellCount = mArtisticParams.getShellCount() > 3 ? 3 : mArtisticParams.getShellCount();
						mArtisticParams.forceShaderShellCount(shellCount);
					}
					if (chunkDist > mShellQualityDropoffDistance * 2) {
						shellCount = mArtisticParams.getShellCount() > 7 ? 7 : mArtisticParams.getShellCount();
						mArtisticParams.forceShaderShellCount(shellCount);
					}
					else if (chunkDist > mShellQualityDropoffDistance) {
						shellCount = mArtisticParams.getShellCount() > 10 ? 10 : mArtisticParams.getShellCount();
						mArtisticParams.forceShaderShellCount(shellCount);
					}

					// Draw terrain
					// glInstanceID is 1 greater than the shellIndex (base terrain is -1 shell index, first shell is 0 shell index)
					glDrawElementsInstanced(GL_TRIANGLES, currPlane.getIndexCount(), GL_UNSIGNED_INT, 0, shellCount + 1); // Draw each shell plus the base terrain
					// I could do each of the plane qualities in one instanced call, but for some reason it is slightly slower

					// Draw water
					mWaterShader.use();
					mWaterShader.setVector3("planePos", { chunkPos.x, mWaterHeight, chunkPos.z });
					mWaterShader.setFloat("planeWorldWidth", mChunkWidth);
					glDrawElements(GL_TRIANGLES, currPlane.getIndexCount(), GL_UNSIGNED_INT, 0); // Draw each shell plus the base terrain

					mArtisticParams.fixShellCount();
				}
			}
		}

		if (mIsUIVisible)
			renderUI(displayDeltaTime);
	}

	glm::vec3 getClosestWorldPixelPos(const glm::vec3 pos, int imageIndex) {
		float stepSize{ mImageWorldSizes[imageIndex] / mImagePixelDims[imageIndex] * mArtisticParams.getTerrainScale() };
		glm::vec3 stepSizesAway = pos / stepSize;
		stepSizesAway = glm::vec3{ (int)stepSizesAway.x, (int)stepSizesAway.y, (int)stepSizesAway.z };
		return stepSizesAway * stepSize;
	}

	void toggleUI() {
		mIsUIVisible = !mIsUIVisible;
	}

	static float quintic(float x) {
		return x < 0.5 ? (16 * x * x * x * x * x) : 1 - pow(-2 * x + 2, 5.0) / 2.0;
	}

	float getHeightAtPoint(const glm::vec2& worldPos) const {
		glm::vec2 pos = worldPos / mArtisticParams.getTerrainScale();
		float mountain = perlin(pos * mTerrainParams.getMountainFrequency(), 0);

		mountain = pow(mountain, mTerrainParams.getMountainExponent());

		mountain = mountain * (1 - mTerrainParams.getAntiFlatFactor()) + mTerrainParams.getAntiFlatFactor();

		float offset = perlin(pos * mTerrainParams.getDipScale(), 1);

		offset = quintic(offset);

		offset *= mTerrainParams.getDipStrength();

		float terrainHeight = 0;

		float amplitude = mTerrainParams.getInitialAmplitude();
		float spread = 1;

		for (int i = 0; i < mTerrainParams.getOctaveCount(); ++i) {
			glm::vec2 samplePos = pos * spread;
			float perlinData = perlin(samplePos, 0);

			terrainHeight += amplitude * perlinData;

			amplitude *= mTerrainParams.getAmplitudeDecay();
			spread *= mTerrainParams.getSpreadFactor();
		}

		float finalOutput = 0;
		finalOutput = terrainHeight * mountain;

		finalOutput += offset;
		return finalOutput;
	}

private:
	// The chunk collection consists of a square of chunkCount * chunkCount chunks, each having a width of chunkWidth

	// The chunks will go from high to low quality, while the far chunks will all be low quality?
	int mChunkWidth;
	int mChunkCount;

	ArtisticParamsBuffer mArtisticParams;
	TerrainParamsBuffer mTerrainParams;
	WaterParamsBuffer mWaterParams;
	ColourBuffer mColours;
	bool mIsUIVisible{ true };

	std::array<int, ImageCount> mImagePixelDims;
	std::array<float, ImageCount> mImageWorldSizes;
	std::array<glm::vec2, ImageCount> mImageWorldPositions;
	std::array<TerrainImageGenerator, ImageCount> mImages;

	int mLowQualityPlaneVerticesPerEdge;
	int mHighQualityPlaneVerticesPerEdgeScale;
	float mWaterHeight;

	Shader mTerrainImageShader;
	Shader mTerrainShader;
	Shader mWaterShader;
	Shader mSkyboxShader;
	Cubemap mDaySkybox;
	Cubemap mNightSkybox;
	CubeVertices mCubeVertices;
	float mDayTime;
	//glm::vec3 mDirToLight{ -0.008373, 0.089878, 0.995917 };

	PlaneGPU mLowQualityPlane;
	PlaneGPU mHighQualityPlane;

	float mVertexQualityDropoffDistance;
	float mShellQualityDropoffDistance;

	VertexArray mScreenQuad;

	void renderUI(double displayDeltaTime) {

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("FPS");
		ImGui::LabelText(std::to_string(1 / displayDeltaTime).c_str(), "");
		ImGui::End();

		mArtisticParams.renderUI();
		mTerrainParams.renderUI();
		mWaterParams.renderUI();
		mColours.renderUI();

		ImGui::Begin("Plane Chunking");
		ImGui::DragInt("Width", &mChunkWidth, 1, 1, 100);
		ImGui::DragInt("Count", &mChunkCount, 1, 1, 100);
		ImGui::DragInt("Low quality plane vertices", &mLowQualityPlaneVerticesPerEdge, 1, 2, 1000);
		ImGui::DragInt("High quality plane quality scale", &mHighQualityPlaneVerticesPerEdgeScale, 1, 2, 1000);
		ImGui::DragFloat("Vertex LOD dist", &mVertexQualityDropoffDistance, 1, 1, 1000);
		ImGui::DragFloat("Shell LOD dist", &mShellQualityDropoffDistance, 1, 1, 1000);
		ImGui::DragFloat("Water height", &mWaterHeight, 0.1);
		ImGui::End();

		ImGui::Begin("Day");
		ImGui::DragFloat("Day time", &mDayTime, 0.001, 0.1, 0.9);
		ImGui::End();

		ImGui::Begin("Terrain Images");
		for (int i{ 0 }; i < ImageCount; ++i) {
			std::string indexString{ std::to_string(i + 1) };
			ImGui::DragFloat(("World size " + indexString).c_str(), &mImageWorldSizes[i], 1, 1, 100000);
			ImGui::InputInt(("Pixel quality " + indexString).c_str(), &mImagePixelDims[i], 100, 1000);
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	float getMaxHeight() {
		glm::vec3 mountain = { 1, 0, 0 };

		mountain.x *= mountain.x;
		mountain.x *= mountain.x;
		mountain.x = mountain.x * 0.95 + 0.05;

		glm::vec3 offset = { 1, 0, 0 };

		offset.x = offset.x < 0.5 ? (16 * offset.x * offset.x * offset.x * offset.x * offset.x) : 1 - pow(-2 * offset.x + 2, 5.0) / 2.0;

		offset *= 20;

		glm::vec3 terrainInfo = { 0, 0, 0 };

		float amplitude = mTerrainParams.getInitialAmplitude();

		for (int i{ 0 }; i < mTerrainParams.getOctaveCount(); ++i) {
			glm::vec3 perlinData = { 1, 0, 0 };

			terrainInfo.x += amplitude * perlinData.x;

			amplitude *= mTerrainParams.getAmplitudeDecay();
		}

		glm::vec3 finalOutput = { 0, 0, 0 };
		finalOutput.x = terrainInfo.x * mountain.x;

		finalOutput.x += offset.x;
		return finalOutput.x;
	}

	float getMinHeight() {
		glm::vec3 mountain = { 0, 0, 0 };

		mountain.x *= mountain.x;
		mountain.x *= mountain.x;
		mountain.x = mountain.x * 0.95 + 0.05;

		glm::vec3 offset = { 0, 0, 0 };

		offset.x = offset.x < 0.5 ? (16 * offset.x * offset.x * offset.x * offset.x * offset.x) : 1 - pow(-2 * offset.x + 2, 5.0) / 2.0;

		offset *= 20;

		glm::vec3 terrainInfo = { 0, 0, 0 };

		float amplitude = mTerrainParams.getInitialAmplitude();

		for (int i{ 0 }; i < mTerrainParams.getOctaveCount(); ++i) {
			glm::vec3 perlinData = { 0, 0, 0 };

			terrainInfo.x += amplitude * perlinData.x;

			amplitude *= mTerrainParams.getAmplitudeDecay();
		}

		glm::vec3 finalOutput = { 0, 0, 0 };
		finalOutput.x = terrainInfo.x * mountain.x;

		finalOutput.x += offset.x;
		return finalOutput.x;
	}

	glm::vec3 getDirToSun() {
		float theta{ mDayTime * glm::pi<float>() };
		return glm::vec3{ glm::cos(theta), glm::sin(theta), 0 };
	}
	static int getClosestInt(float x) {
		return int(round(x) + 0.1 * (x < 0 ? -1 : 1));
	}

	static int max(int x, int y) {
		return x > y ? x : y;
	}

	static unsigned int rand(unsigned int n) {
		unsigned int state = n * 747796405u + 2891336453u;
		unsigned int word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		word = (word >> 22u) ^ word;
		return word;
	}

	static float randToFloat(unsigned int n) {
		return float(n) / 4294967296.0;
	}

	static unsigned int labelPoint(int x, int y) {
		if (x == 0 && y == 0)
			return 0;

		int n = max(abs(x), abs(y));
		int width = 2 * n + 1;
		int startingIndex = (width - 2) * (width - 2);

		if (n == y) { // top row
			return startingIndex + x + n;
		}
		if (n == -y) { // bottom row
			return startingIndex + width + x + n;
		}
		if (n == x) { // right col
			return startingIndex + width * 2 + y - 1 + n;
		}
		if (n == -x) { // right col
			return startingIndex + width * 2 + width - 2 + y - 1 + n;
		}
		return 0;
	}

	static glm::vec2 randUnitVector(float randNum) {
		float theta = 2 * 3.14159 * randNum;
		return glm::normalize(glm::vec2(cos(theta), sin(theta)));
	}

	static glm::vec2 quinticInterpolation(glm::vec2 t) {
		return t * t * t * (t * (t * glm::vec2(6) - glm::vec2(15)) + glm::vec2(10));
	}

	static glm::vec2 quinticDerivative(glm::vec2 t) {
		return glm::vec2(30) * t * t * (t * (t - glm::vec2(2)) + glm::vec2(1));
	}

	static float perlin(const glm::vec2& pos, int reroll) {
		int x0 = getClosestInt(floor(pos.x));
		int x1 = getClosestInt(ceil(pos.x));
		int y0 = getClosestInt(floor(pos.y));
		int y1 = getClosestInt(ceil(pos.y));

		glm::vec2 p00 = glm::vec2(x0, y0);

		glm::vec2 relPoint = pos - p00;

		unsigned int rui00 = rand(labelPoint(x0, y0));
		unsigned int rui10 = rand(labelPoint(x1, y0));
		unsigned int rui01 = rand(labelPoint(x0, y1));
		unsigned int rui11 = rand(labelPoint(x1, y1));

		for (int i = 0; i < reroll; ++i) {
			rui00 = rand(rui00);
			rui10 = rand(rui10);
			rui01 = rand(rui01);
			rui11 = rand(rui11);
		}

		float r00 = randToFloat(rui00);
		float r10 = randToFloat(rui10);
		float r01 = randToFloat(rui01);
		float r11 = randToFloat(rui11);

		glm::vec2 g00 = randUnitVector(r00);
		glm::vec2 g10 = randUnitVector(r10);
		glm::vec2 g01 = randUnitVector(r01);
		glm::vec2 g11 = randUnitVector(r11);

		glm::vec2 v00 = relPoint;
		glm::vec2 v11 = relPoint - glm::vec2(1, 1);
		glm::vec2 v10 = relPoint - glm::vec2(1, 0);
		glm::vec2 v01 = relPoint - glm::vec2(0, 1);

		float d00 = glm::dot(v00, g00);
		float d10 = glm::dot(v10, g10);
		float d01 = glm::dot(v01, g01);
		float d11 = glm::dot(v11, g11);

		// From https://iquilezles.org/articles/gradientnoise/ and Acerola's github
		glm::vec2 u = quinticInterpolation(relPoint);
		glm::vec2 du = quinticDerivative(relPoint);
		float noise = d00 + u.x * (d10 - d00) + u.y * (d01 - d00) + u.x * u.y * (d00 - d10 - d01 + d11);
		noise = noise / 1.414 + 0.5;
		return noise;
	}
};

#endif