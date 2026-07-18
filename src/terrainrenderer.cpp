#include "terrainrenderer.h"
#include "mathhelper.h"
#include "framebufferi.h"
#include "camerai.h"
#include "aabb.h"
#include "heightfunction.h"
#include "imgui/imgui.h"
#include <algorithm>

TerrainRenderer::TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos)
	: mScreenQuad{ VertexArray::createScreenVertexArray() }

	, mShaderTerrainImage{ "assets/shaders/terrainimage.vert", "assets/shaders/terrainimage.frag" }
	, mShaderTerrainForward{ "assets/shaders/terrain.vert", "assets/shaders/terrain.frag" }
	, mShaderWaterForward{ "assets/shaders/water.vert", "assets/shaders/water.frag" }
	, mSkyboxShader{ "assets/shaders/skybox.vert", "assets/shaders/skybox.frag" }

	, mTerrainImageSet{ ImageCount, cameraPos, mArtisticParams.mValue.terrainScale, mScreenQuad, mShaderTerrainImage }

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
	, mShadowMapperSun{ { 0.02f, 0.1f, 0.3f } }
	, mShadowMapperMoon{ { 0.02f, 0.1f, 0.3f } }
{}

void TerrainRenderer::bindTerrainImage(int i, int unit) const {
	mTerrainImageSet.getImage(i).bindImage(unit);
}

const DeferredRenderer& TerrainRenderer::getDeferredRenderer() const {
	return mDeferredRenderer;
}

const ShadowMapper<CascadeCount>& TerrainRenderer::getShadowMapperSun() const {
	return mShadowMapperSun;
}

const ShadowMapper<CascadeCount>& TerrainRenderer::getShadowMapperMoon() const {
	return mShadowMapperMoon;
}

void TerrainRenderer::updateAndRenderUI(const glm::vec3& cameraPos, bool renderUI) {
	bool hasTerrainChanged{ false };
	if (renderUI) {
		ImGui::Begin("Terrain Parameters");

			ImGui::DragInt("Octave count", &mTerrainParams.mValue.octaveCount, 0.1f, 1, 30);

			ImGui::DragInt("Smooth octave count", &mTerrainParams.mValue.smoothOctaveCount, 0.1f, 1, 30);

			ImGui::DragFloat("Amplitude", &mTerrainParams.mValue.initialAmplitude, 0.7f, 0, 500);

			ImGui::DragFloat("Amplitude decay", &mTerrainParams.mValue.amplitudeDecay, 0.0005f, 0, 100);

			ImGui::DragFloat("Spread factor", &mTerrainParams.mValue.spreadFactor, 0.001f, 0, 100);

			ImGui::DragFloat("Mountain frequency", &mTerrainParams.mValue.mountainFrequency, 0.003f, 0, 2);

			ImGui::DragFloat("Mountain exponent", &mTerrainParams.mValue.mountainExponent, 0.01f, 0.1f, 30);

			ImGui::DragFloat("Anti flat factor", &mTerrainParams.mValue.antiFlatFactor, 0.001f, 0, 1);

			ImGui::DragFloat("River frequency", &mTerrainParams.mValue.riverScale, 0.001f, 0, 2);

			ImGui::DragFloat("River strength", &mTerrainParams.mValue.riverStrength, 1, 0, 1000);

			ImGui::DragFloat("River exponent", &mTerrainParams.mValue.riverExponent, 1, 0, 1000);

			ImGui::DragFloat("Water eating mountains", &mTerrainParams.mValue.waterEatingMountain, 0.1f, 0, 5);

			ImGui::DragFloat("Lake frequency", &mTerrainParams.mValue.lakeScale, 0.001f, 0, 2);

			ImGui::DragFloat("Lake strength", &mTerrainParams.mValue.lakeStrength, 0.5f, 0, 1000);

			ImGui::DragFloat("Lake exponent", &mTerrainParams.mValue.lakeExponent, 0.01f, 0, 1000);

		ImGui::End();
		hasTerrainChanged = mTerrainParams.updateGPU();
		if (hasTerrainChanged) {
			mMinTerrainHeight = HeightFunction::getHeightWithPerlin(mTerrainParams.mValue, HeightFunction::gMinPerlinValues);
			mMaxTerrainHeight = HeightFunction::getHeightWithPerlin(mTerrainParams.mValue, HeightFunction::gMaxPerlinValues);
		}

		ImGui::Begin("Water Parameters");

			ImGui::DragInt("Wave count", &mWaterParams.mValue.waveCount, 0.1f, 1, 100);

			ImGui::DragFloat("Initial amplitude", &mWaterParams.mValue.initialAmplitude, 0.005f, 0.01f, 1);

			ImGui::DragFloat("Amplitude multiplier", &mWaterParams.mValue.amplitudeMult, 0.001f, 0, 1);

			ImGui::DragFloat("Initial frequency", &mWaterParams.mValue.initialFreq, 0.01f, 0, 5);

			ImGui::DragFloat("Frequency multiplier", &mWaterParams.mValue.freqMult, 0.01f, 0, 1.5f);

			ImGui::DragFloat("Initial speed", &mWaterParams.mValue.initialSpeed, 0.02f, 0, 20);

			ImGui::DragFloat("Speed multiplier", &mWaterParams.mValue.speedMult, 0.007f, 0, 2);

			ImGui::DragFloat("Shininess", &mWaterParams.mValue.specExp);

			ImGui::DragFloat("Height", &mWaterParams.mValue.height);

		ImGui::End();
		mWaterParams.updateGPU();

		ImGui::Begin("Atmosphere");

			float atmosphereHeightUI{ mAtmosphereInfo.mValue.getHeight() };
			ImGui::DragFloat("Atmosphere height", &atmosphereHeightUI);

			float atmosphereWidthUI{ mAtmosphereInfo.mValue.getWidth() };
			ImGui::DragFloat("Atmosphere width", &atmosphereWidthUI, 1000);

			mAtmosphereInfo.mValue.updateSphere(atmosphereWidthUI, atmosphereHeightUI);

			ImGui::DragFloat("Rayleigh density falloff", &mAtmosphereInfo.mValue.rayleighDensityFalloff, 0.001f);

			ImGui::DragFloat("Mie density falloff", &mAtmosphereInfo.mValue.mieDensityFalloff, 0.001f);

			float uiRayleighDensity = mAtmosphereInfo.mValue.rayleighDensity / 0.0001F;
			ImGui::DragFloat("Rayleigh density scale", &uiRayleighDensity, 0.001f);
			mAtmosphereInfo.mValue.rayleighDensity = uiRayleighDensity * 0.0001F;

			float uiMieDensity = mAtmosphereInfo.mValue.mieDensity / 0.0001F;
			ImGui::DragFloat("Mie density scale", &uiMieDensity, 0.001f);
			mAtmosphereInfo.mValue.mieDensity = uiMieDensity * 0.0001F;

			ImGui::DragFloat3("Rayleigh scattering", (float*)&mAtmosphereInfo.mValue.rayleighScattering, 1);

			ImGui::DragFloat3("Mie scattering", (float*)&mAtmosphereInfo.mValue.mieScattering, 1);

			ImGui::DragFloat("Rayleigh G", &mAtmosphereInfo.mValue.rayleighG, 0.001f);

			ImGui::DragFloat("Mie G", &mAtmosphereInfo.mValue.mieG, 0.001f);

			ImGui::DragInt("Ray atmosphere steps", &mAtmosphereInfo.mValue.rayAtmosphereStepCount, 0.1f, 1, 100);

			ImGui::DragInt("Ray sun steps", &mAtmosphereInfo.mValue.raySunStepCount, 0.1f, 1, 100);

			ImGui::DragFloat("Atmosphere brightness", &mAtmosphereInfo.mValue.brightness, 0.05f);

			ImGui::DragFloat("Atmopshere dither strength", &mAtmosphereInfo.mValue.ditherStrength, 0.1f);

			ImGui::DragFloat("Sun size", &mAtmosphereInfo.mValue.sunSizeDeg, 0.1f);

		ImGui::End();
		mAtmosphereInfo.updateGPU();

		ImGui::Begin("Colours");

			ImGui::Checkbox("Deferred Rendering", &mDoDeferredRendering);

			ImGui::ColorPicker3("Dirt", (float*)&mColourParams.mValue.dirtColour);

			ImGui::ColorPicker3("Mountain", (float*)&mColourParams.mValue.mountainColour);

			ImGui::ColorPicker3("Grass 1", (float*)&mColourParams.mValue.grassColour1);

			ImGui::ColorPicker3("Grass 2", (float*)&mColourParams.mValue.grassColour2);

			ImGui::ColorPicker3("Snow", (float*)&mColourParams.mValue.snowColour);

			ImGui::ColorPicker3("Water", (float*)&mColourParams.mValue.waterColour);

			ImGui::ColorPicker3("Sun", (float*)&mColourParams.mValue.sunColour);

			ImGui::DragFloat("Sun brightness", &mColourParams.mValue.sunBrightness, 0.01f);

			ImGui::ColorPicker3("Moon", (float*)&mColourParams.mValue.moonColour);

			ImGui::DragFloat("Moon brightness", &mColourParams.mValue.moonBrightness, 0.01f);

			ImGui::ColorPicker3("Star", (float*)&mColourParams.mValue.starColour);

			ImGui::DragFloat("Star brightness", &mColourParams.mValue.starBrightness, 0.01f);

		ImGui::End();
		mColourParams.updateGPU();

		ImGui::Begin("Star Parameters");

			StarParameters starParams{ mStarManager.getPrevParametersCopy() };

			ImGui::DragFloat("Star size min", &starParams.minSize, 0.001f);

			ImGui::DragFloat("Star size max", &starParams.maxSize, 0.001f);

			ImGui::DragInt("Star count", &starParams.count);

		ImGui::End();
		mStarManager.update(starParams);

		ImGui::Begin("Chunks");

			mChunkManager.renderUI();

		ImGui::End();

		ImGui::Begin("Artistic Parameters");

			ImGui::DragFloat("Terrain scale", &mArtisticParams.mValue.terrainScale);

			ImGui::DragFloat("Grass dot cutoff", &mArtisticParams.mValue.grassDotCutoff, 0.005f, 0, 1);

			ImGui::DragFloat("Snow dot cutoff", &mArtisticParams.mValue.snowDotCutoff, 0.005f, 0, 1);

			ImGui::DragInt("Shell count", &mShellCount, 0.1f, 0, 256);

			ImGui::DragFloat("Shell max height", &mArtisticParams.mValue.shellMaxHeight, 0.001f, 0, 10);

			ImGui::DragFloat("Grass noise scale", &mArtisticParams.mValue.grassNoiseScale, 1, 1, 1000);

			ImGui::DragFloat("Shell max cutoff", &mArtisticParams.mValue.shellMaxCutoff, 0.01f, 0, 1);

			ImGui::DragFloat("Shell base cutoff", &mArtisticParams.mValue.shellBaseCutoff, 0.01f, 0, 1);

			ImGui::DragFloat("Snow height", &mArtisticParams.mValue.snowHeight, 0.05f);

			ImGui::DragFloat("Seafoam", &mArtisticParams.mValue.seafoamStrength, 0.01f, 0, 10);

			ImGui::DragFloat("Snow line noise scale", &mArtisticParams.mValue.snowLineNoiseScale, 0.001f, 0, 100);

			ImGui::DragFloat("Snow line noise amplitude", &mArtisticParams.mValue.snowLineNoiseAmplitude, 0.01f, 0, 10);

			ImGui::DragFloat("Mountain snow cutoff", &mArtisticParams.mValue.mountainSnowCutoff, 0.01f, 0, 10);

			ImGui::DragFloat("Snow line ease", &mArtisticParams.mValue.snowLineEase, 0.01f, 0, 10);

			ImGui::DragFloat("Shell ambient occlusion", &mArtisticParams.mValue.shellAmbientOcclusion, 0.001f, 0, 1);

		ImGui::End();
		mArtisticParams.mValue.maxViewDistance = mChunkManager.getTerrainSpan() * 0.5f * 0.95f;
		mArtisticParams.mValue.fogEncroach = mArtisticParams.mValue.maxViewDistance * 0.1f;
		mArtisticParams.updateGPU();

		ImGui::Begin("Frustum Culling");

			ImGui::Checkbox("Frustum culling", &mDoFrustumCulling);

		ImGui::End();

		ImGui::Begin("Time");
			ImGui::DragFloat("Day time", &mDayTime, 0.001f);
			if (mDayTime < 0) {
				mDayTime += 2;
			}
			else if (mDayTime > 2) {
				mDayTime -= 2;
			}
		ImGui::End();


		ImGui::Begin("Shadow Parameters");

			ImGui::DragFloat("Blur width", &mShadowInfo.mValue.blurWidth, 0.01f, 0, 10);

			ImGui::DragInt("Blur quality", &mShadowInfo.mValue.blurQuality, 0.03f, 0, 9);

			//int currCamera;
			//ImGui::InputInt("Curr camera", &currCamera, 1);

			ImGui::DragFloat("Exposure", &mShadowInfo.mValue.exposure, 0.001f);

			ImGui::DragFloat("Min bias", &mShadowInfo.mValue.minBias, 0.1f);

			ImGui::DragFloat("Max bias", &mShadowInfo.mValue.maxBias, 0.1f);

			if (mShadowInfo.mValue.minBias > mShadowInfo.mValue.maxBias)
				mShadowInfo.mValue.minBias = mShadowInfo.mValue.maxBias;

			mShadowMapperSun.renderSplitsUI();
			mShadowMapperMoon.setSplits(mShadowMapperSun.getSplits());
			mShadowInfo.mValue.computeValues(mShadowMapperSun, mShadowMapperMoon);

		ImGui::End();
		mShadowInfo.updateGPU();
	}

	std::array<float, ImageCount> imageWorldSizes;
	std::array<int, ImageCount> imagePixelDimensions;

	if (renderUI)
		ImGui::Begin("Terrain Images");

	mTerrainImageSet.renderUIAndUpdate(hasTerrainChanged, cameraPos, mArtisticParams.mValue.terrainScale, mScreenQuad, mShaderTerrainImage, renderUI);

	if (renderUI)
		ImGui::End();
}

class ScopedDebugGroup {
public:
	ScopedDebugGroup(const char* message) {
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, message);
	}
	~ScopedDebugGroup() {
		glPopDebugGroup();
	}
};

void TerrainRenderer::render(const CameraPlayer& camera, float time, const FramebufferColour& targetFramebuffer) {
	glm::vec3 dirToSun{ MathHelper::getDirToSun(mDayTime) };

	std::ranges::copy(mTerrainImageSet.getImagePositions(), mTerrainImagesInfo.mValue.imagePositions.begin());
	std::ranges::copy(mTerrainImageSet.getImageWorldWidths(), mTerrainImagesInfo.mValue.imageScales.begin());
	mTerrainImagesInfo.updateGPU();

	// Render skybox
	if (!mDoDeferredRendering) {
		mPerFrameInfo.mValue.fromData(camera, dirToSun, time, mDayTime);
		mPerFrameInfo.updateGPU();
		mSkyboxShader.setRenderData(mDaySkybox);
		mSkyboxShader.render(targetFramebuffer, mCubeVertices.getVertexArray());
	}

	for (int i{ 0 }; i < ImageCount; ++i) {
		mTerrainImageSet.getImage(i).bindImage(i);
	}

	if (mDoDeferredRendering) {
		mDeferredRenderer.mFramebuffer.use();
		glClearColor(0, 0, 0, -3);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mShadowMapperSun.updateCameras(dirToSun, camera, getSceneWorldAABB(camera.getPosition()));
		mShadowMapperMoon.updateCameras(-dirToSun, camera, getSceneWorldAABB(camera.getPosition()));
		mShadowInfo.mValue.computeValues(mShadowMapperSun, mShadowMapperMoon);
		mShadowInfo.updateGPU();

		{
			ScopedDebugGroup d{ "Shadow Pass "};
			for (size_t i{ 0 }; i < CascadeCount; ++i) {
				bool isDay = mDayTime < 1;

				const CameraI& depthCameraSun{ mShadowMapperSun.getCamera(i) };
				const FramebufferI& depthFramebufferSun{ mShadowMapperSun.getFramebuffer(i) };
				depthFramebufferSun.use();
				glClearColor(0, 0, 0, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				renderTerrain(depthFramebufferSun, depthCameraSun, camera.getPosition(), mShadowMapperSun.mTerrainDepthShader, mShadowMapperSun.mWaterDepthShader, dirToSun, time, true, !isDay);

				const CameraI& depthCameraMoon{ mShadowMapperMoon.getCamera(i) };
				const FramebufferI& depthFramebufferMoon{ mShadowMapperMoon.getFramebuffer(i) };
				depthFramebufferMoon.use();
				glClearColor(0, 0, 0, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				renderTerrain(depthFramebufferMoon, depthCameraMoon, camera.getPosition(), mShadowMapperMoon.mTerrainDepthShader, mShadowMapperMoon.mWaterDepthShader, dirToSun, time, true, isDay);
			}
		}

		//const CameraI* pCamera{ &camera };
		//const CameraI* pCamera0{ &mShadowMapperSun.getCamera(0) };
		//const CameraI* pCamera1{ &mShadowMapperSun.getCamera(1) };
		//const CameraI* pCamera2{ &mShadowMapperSun.getCamera(2) };
		//const CameraI* currCamera{ uiManager.mCurrCamera.data() == -1 ? pCamera : (uiManager.mCurrCamera.data() == 0 ? pCamera0 : (uiManager.mCurrCamera.data() == 1 ? pCamera1 : (pCamera2))) };
		const CameraI* currCamera{ &camera };

		glEnable(GL_CULL_FACE);
		{
			ScopedDebugGroup d{ "Geometry Pass" };
			renderTerrain(mDeferredRenderer.mFramebuffer, *currCamera, camera.getPosition(), mDeferredRenderer.mShaderTerrainDeferred, mDeferredRenderer.mShaderWaterDeferred, dirToSun, time);
		}
		glDisable(GL_CULL_FACE);

		mPerFrameInfo.mValue.fromData(camera, dirToSun, time, mDayTime);
		mPerFrameInfo.updateGPU();
		{
			ScopedDebugGroup d{ "Deferred Pass" };
			mDeferredRenderer.doDeferredShading(targetFramebuffer, *this, mScreenQuad);
		}
	}
	else {
		mShadowInfo.mValue.computeValues(mShadowMapperSun, mShadowMapperMoon);
		mShadowInfo.updateGPU();
		renderTerrain(targetFramebuffer, camera, camera.getPosition(), mShaderTerrainForward, mShaderWaterForward, dirToSun, time);

		// Shadow map ortho volume debugging (messy)
		//if (mColourParams.mValue.sunBrightness < 10) // TODO?
		//	mShadowMapperSun.updateCameras(dirToSun, camera, getSceneWorldAABB(camera.getPosition()));
		VertexArray orthoVertexArray;

		std::vector<unsigned int> indices{ 0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7, 2, 3, 6, 3, 6, 7, 0, 1, 4, 1, 4, 5, 0, 2, 4, 2, 4, 6, 1, 3, 5, 3, 5, 7 };
		std::vector<int> layout{ 3 };
		for (int i{ 0 }; i < CascadeCount; ++i) {
			const std::array<glm::vec3, 8>& orthoPoints{ mShadowMapperSun.getOrthoWorldPositions(i) };
			std::vector<float> vertexData;
			for (const glm::vec3& orthoPoint : orthoPoints) {
				vertexData.push_back(orthoPoint.x);
				vertexData.push_back(orthoPoint.y);
				vertexData.push_back(orthoPoint.z);
			}
			orthoVertexArray.create(vertexData, indices, layout);
			mPerFrameInfo.mValue.fromData(camera, dirToSun, time, mDayTime);
			mPerFrameInfo.updateGPU();
			glm::vec3 colour = { 0, 0, 0 };
			colour[i] = 1;
			mShaderOrtho.setColour(colour);
			mShaderOrtho.render(targetFramebuffer, orthoVertexArray);
		}
	}
}

void TerrainRenderer::renderTerrain(const FramebufferI& targetFramebuffer, const CameraI& camera, const glm::vec3& playerCameraPosition, ShaderChunk& terrainShader, ShaderChunk& waterShader, const glm::vec3& dirToSun, float time, bool depthPass, bool forceLowQuality) {
	mPerFrameInfo.mValue.fromData(camera, dirToSun, time, mDayTime);
	mPerFrameInfo.updateGPU();

	mChunkManager.populateBuffers(camera, depthPass, forceLowQuality, mDoFrustumCulling, depthPass);

	for (size_t i{ 0 }; i < mTerrainImageSet.getImageCount(); ++i) {
		mTerrainImageSet.getImage(i).bindImage(i);
	}

	// Draw water
	{
		ScopedDebugGroup d{ "Water" };
		int i{ 0 };
		while (auto optionalVAOAndInstanceCount = mChunkManager.flushSomeWater()) {
			std::string debug{ "Quality Index " };
			debug += std::to_string(i);
			debug += " (low is higher quality)";
			ScopedDebugGroup d{ debug.c_str() };
			int instanceCount = optionalVAOAndInstanceCount.value().second;
			if (instanceCount == 0) {
				continue;
			}
			waterShader.setRenderData(*this, depthPass ? 10000 : mChunkManager.getChunkWidth(), instanceCount, mDaySkybox);
			waterShader.render(targetFramebuffer, optionalVAOAndInstanceCount.value().first);
			i++;
		}
	}

	// Draw terrain
	glDisable(GL_BLEND);
	{
		ScopedDebugGroup d{ "Terrain" };
		int i{ 0 };
		while (auto optionalVAOAndInstanceCount = mChunkManager.flushSomeTerrain()) {
			std::string debug{ "Quality Index " };
			debug += std::to_string(i);
			debug += " (low is higher quality)";
			ScopedDebugGroup d{ debug.c_str() };
			int instanceCount = optionalVAOAndInstanceCount.value().second;
			if (instanceCount == 0) {
				continue;
			}
			terrainShader.setRenderData(*this, mChunkManager.getChunkWidth(), instanceCount, mDaySkybox);
			terrainShader.render(targetFramebuffer, optionalVAOAndInstanceCount.value().first);
			i++;
		}
	}
}

AABB TerrainRenderer::getSceneWorldAABB(const glm::vec3& playerCameraPos) const {
	glm::vec3 minPosition{ -mChunkManager.getTerrainSpan() / 2, mMinTerrainHeight, -mChunkManager.getTerrainSpan() / 2 };
	glm::vec3 maxPosition{ mChunkManager.getTerrainSpan() / 2, mMaxTerrainHeight,  mChunkManager.getTerrainSpan() / 2 };
	minPosition += playerCameraPos;
	maxPosition += playerCameraPos;
	return AABB{ minPosition * 1.1f, maxPosition * 1.1f };
}