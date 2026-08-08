#ifndef COMMON_BUFFER_TYPES_H
#define COMMON_BUFFER_TYPES_H

#include "glad/glad.h"
#include "OpenGLObjects/BUF.h"
#include "glm/glm.hpp"
#include "cameraplayer.h"
#include <iostream>
#include <array>
#include "constants.h"
#include "shadowmapper.h"
#include "mathhelper.h"
#include <cassert>
#include <cmath>

namespace CommonBufferTypes {

	struct TerrainParams {
		int   octaveCount{};
		int   smoothOctaveCount{};
		float initialAmplitude{};
		float amplitudeDecay{};
		float spreadFactor{};
		float mountainFrequency{};
		float mountainExponent{};
		float antiFlatFactor{};
		float riverScale{};
		float riverStrength{};
		float riverExponent{};
		float waterEatingMountain{};
		float lakeScale{};
		float lakeStrength{};
		float lakeExponent{};

		bool operator==(const TerrainParams&) const = default;

		static TerrainParams getDefaultValue() {
			return TerrainParams {
				.octaveCount = 15,
				.smoothOctaveCount = 6,
				.initialAmplitude = 250,
				.amplitudeDecay = 0.4f,
				.spreadFactor = 2,
				.mountainFrequency = 0.2f,
				.mountainExponent = 4,
				.antiFlatFactor = 0.04f,
				.riverScale = 0.05f,
				.riverStrength = 20,
				.riverExponent = 32,
				.waterEatingMountain = 30,
				.lakeScale = 0.06f,
				.lakeStrength = 40,
				.lakeExponent = 100
			};
		}
	};

	struct ScreenSpaceReflectionParams {
		float maxWorldDistance{};
		int stepCount{};
		int binarySearchSteps{};
		bool fadeOutTowardsCamera{};
		bool fadeOutDepth{};
		bool enforceDepthCheck{};
		bool fadeOutDistance{};

		bool operator==(const ScreenSpaceReflectionParams&) const = default;

		static ScreenSpaceReflectionParams getDefaultValue() {
			return ScreenSpaceReflectionParams{
				.maxWorldDistance = 1100,
				.stepCount = 20,
				.binarySearchSteps = 4,
				.fadeOutTowardsCamera = true,
				.fadeOutDepth = true,
				.fadeOutDistance = true,
			};
		}
	};

	struct DebugData {
		bool hasData;
		glm::vec3 worldStart;
		glm::vec3 worldEnd;
		int maxPointCount;
		int currentPointCount;
		float tCurrent;
		float tVisible;
		std::array<glm::vec4, 1000> pointLine;
		std::array<glm::vec4, 1000> pointScene;
		std::array<glm::vec2, 1000> pointUV;

		bool operator==(const DebugData&) const = default;

		static DebugData getDefaultValue() {
			DebugData value;
			value.hasData = false;
			value.worldStart = {};
			value.worldEnd = {};
			value.maxPointCount = 1000;
			value.currentPointCount = 0;
			return value;
		}
	};

	struct ArtisticParams {
		float terrainScale{};
		float maxViewDistance{};
		float fogEncroach{};
		float grassDotCutoff{};
		float snowDotCutoff{};
		float shellMaxHeight{};
		float grassNoiseScale{};
		float snowNoiseScale{};
		float shellMaxCutoff{};
		float shellBaseCutoff{};
		float snowHeight{};
		float seafoamStrength{};
		float snowLineNoiseScale{};
		float snowLineNoiseAmplitude{};
		float mountainSnowCutoff{};
		float snowLineEase{};
		float shellAmbientOcclusion{};

		static ArtisticParams getDefaultValue() {
			return ArtisticParams {
				.terrainScale = 58,
				.maxViewDistance = 1,
				.fogEncroach = 1,
				.grassDotCutoff = 0.6f,
				.snowDotCutoff = 0.3f,
				.shellMaxHeight = 0.117f,
				.grassNoiseScale = 100,
				.snowNoiseScale = 1,
				.shellMaxCutoff = 1,
				.shellBaseCutoff = 0.2f,
				.snowHeight = 65,
				.seafoamStrength = 0.4f,
				.snowLineNoiseScale = 0.3f,
				.snowLineNoiseAmplitude = 2.75f,
				.mountainSnowCutoff = 0.9f,
				.snowLineEase = 4.5f,
				.shellAmbientOcclusion = 0.2f
			};
		}

		bool operator==(const ArtisticParams&) const = default;
	};

	struct WaterParams {
		int waveCount{};
		float initialAmplitude{};
		float amplitudeMult{};
		float initialFreq{};
		float freqMult{};
		float initialSpeed{};
		float speedMult{};
		float specExp{};
		float height{};

		bool operator==(const WaterParams&) const = default;

		static WaterParams getDefaultValue() {
			return WaterParams{
				.waveCount = 24,
				.initialAmplitude = 0.01f,
				.amplitudeMult = 0.82f,
				.initialFreq = 1,
				.freqMult = 1.13f,
				.initialSpeed = 2,
				.speedMult = 1.07f,
				.specExp = 200,
				.height = -1.5f
			};
		}
	};

	struct ColourParams {
		glm::vec3 dirtColour{};
		glm::vec3 mountainColour{};
		glm::vec3 grassColour1{};
		glm::vec3 grassColour2{};
		glm::vec3 snowColour{};
		glm::vec3 waterColour{};
		glm::vec3 sunColour{};
		float sunBrightness{};
		glm::vec3 moonColour{};
		float moonBrightness{};
		glm::vec3 starColour{};
		float starBrightness{};

		bool operator==(const ColourParams&) const = default;

		static ColourParams getDefaultValue() {
			return ColourParams{
				.dirtColour = {40 / 255.0f, 21 / 255.0f, 10 / 255.0f},
				.mountainColour = {34 / 255.0f, 34 / 255.0f, 34 / 255.0f},
				.grassColour1 = {0 / 255.0f, 56 / 255.0f, 0 / 255.0f},
				.grassColour2 = {15 / 255.0f, 56 / 255.0f, 0 / 255.0f},
				.snowColour = {255 / 255.0f, 255 / 255.0f, 255 / 255.0f},
				.waterColour = {0 / 255.0f, 1.7 / 255.0f, 56 / 255.0f},
				.sunColour = {255 / 255.0f, 255 / 255.0f, 255 / 255.0f},
				.sunBrightness = 25.0f,
				.moonColour = {255 / 255.0f, 255 / 255.0f, 255 / 255.0f},
				.moonBrightness = 0.1f,
				.starColour     = {255 / 255.0f, 255 / 255.0f, 255 / 255.0f},
				.starBrightness = 2
			};
		}
	};

	struct PerFrameInfo {
		glm::mat4 viewMatrix{};
		glm::mat4 projectionMatrix{};
		glm::mat4 starRotationMatrix{};
		glm::vec3 cameraPos{};
		glm::vec3 dirToSun{};
		float time{};
		float fovX{};
		float fovY{};
		float yaw{};
		float pitch{};
		float cameraNear{};
		float cameraFar{};
		float tanHalfFOVX{};
		float tanHalfFOVY{};
		float dayTime{};
		float nightStrength{};

		void fromData(const CameraI& camera, glm::vec3 _dirToSun, float time, float dayTime) {
			viewMatrix = camera.getViewMatrix();
			projectionMatrix = camera.getProjectionMatrix();
			starRotationMatrix = glm::rotate(glm::mat4(1.0f), -MathHelper::PI * dayTime, { 0, 0, 1 });
			cameraPos = camera.getPosition();
			dirToSun = _dirToSun;
			this->time = time;
			fovX = camera.getFOVX();
			fovY = camera.getFOVY();
			yaw = camera.getYaw();
			pitch = camera.getPitch();
			cameraNear = camera.getNearPlaneDist();
			cameraFar = camera.getFarPlaneDist();
			tanHalfFOVX = glm::tan(camera.getFOVX() / 2);
			tanHalfFOVY = glm::tan(camera.getFOVY() / 2);
			this->dayTime = dayTime;
			nightStrength = MathHelper::starBrightnessAtTime(dayTime);
		}

		bool operator==(const PerFrameInfo&) const = default;
	};

	struct TerrainImagesInfo {
		std::array<glm::vec2, ImageCount> imagePositions{};
		std::array<float, ImageCount> imageScales{};

		bool operator==(const TerrainImagesInfo&) const = default;
	};

	struct AtmosphereInfo {
		glm::vec3 rayleighScattering{};
		glm::vec3 mieScattering{};
		float maxRadius{};
		float minRadius{};
		float centerY{};
		float rayleighDensityFalloff{};
		float mieDensityFalloff{};
		float rayleighDensity{};
		float mieDensity{};
		float rayleighG{};
		float mieG{};
		int rayAtmosphereStepCount{};
		int raySunStepCount{};
		float brightness{};
		float ditherStrength{};
		float sunSizeDeg{};

		float getWidth() {
			float height = maxRadius - minRadius;
			return std::sqrt(8 * maxRadius * height - 4 * height * height);
		}

		float getHeight() {
			return maxRadius - minRadius;
		}

		void updateSphere(float width, float height) {
			maxRadius = MathHelper::getRadiusFromChordWidthAndHeight(width, height);
			minRadius = maxRadius - height;
			centerY = height - maxRadius;
		}

		bool operator==(const AtmosphereInfo&) const = default;

		static AtmosphereInfo getDefaultValue() {
			float height{ 72144 };
			float width{ 2151231 };
			float maxRadius = MathHelper::getRadiusFromChordWidthAndHeight(width, height);
			return AtmosphereInfo{
				.rayleighScattering = { 3, 28, 66 },
				.mieScattering = { 1, 1, 1 },
				.maxRadius = maxRadius,
				.minRadius = maxRadius - height,
				.centerY = height - maxRadius,
				.rayleighDensityFalloff = 1,
				.mieDensityFalloff = 5,
				.rayleighDensity = 0.004f * 0.0001F,
				.mieDensity = 0,// 0.06f * 0.0001F,
				.rayleighG = 0,
				.mieG = 0.957f,
				.rayAtmosphereStepCount = 30,
				.raySunStepCount = 3,
				.brightness = 25,
				.ditherStrength = 1.8f,
				.sunSizeDeg = 2.65f
			};
		}
	};

	struct ShadowInfo {
		void computeValues(const ShadowMapper<CascadeCount>& shadowMapperSun, const ShadowMapper<CascadeCount>& shadowMapperMoon) {
			assert(blurQuality % 2 == 1);
			if (blurQuality % 2 == 0)
				blurQuality += 1;

			for (int i{ 0 }; i < CascadeCount; ++i) {
				viewMatricesSun[i] = shadowMapperSun.getCamera(i).getViewMatrix();
				projectionMatricesSun[i] = shadowMapperSun.getCamera(i).getProjectionMatrix();
			}

			for (int i{ 0 }; i < CascadeCount; ++i) {
				viewMatricesMoon[i] = shadowMapperMoon.getCamera(i).getViewMatrix();
				projectionMatricesMoon[i] = shadowMapperMoon.getCamera(i).getProjectionMatrix();
			}

			splits = shadowMapperSun.getSplits();

			for (int i{ 0 }; i < CascadeCount; ++i) {
				widthsSun[i] = shadowMapperSun.getWorldWidth(i);
			}

			for (int i{ 0 }; i < CascadeCount; ++i) {
				widthsMoon[i] = shadowMapperMoon.getWorldWidth(i);
			}

			// Grid sum
			blurGridSum = 0;
			for (int x{ 0 }; x < blurQuality; ++x) {
				for (int y{ 0 }; y < blurQuality; ++y) {
					blurGridSum += blurQuality - MathHelper::taxicabDist(glm::ivec2(x, y), glm::ivec2(blurQuality / 2, blurQuality / 2));
				}
			}
		}

		bool operator==(const ShadowInfo&) const = default;

		std::array<glm::mat4, CascadeCount> viewMatricesSun{};
		std::array<glm::mat4, CascadeCount> projectionMatricesSun{};
		std::array<glm::mat4, CascadeCount> viewMatricesMoon{};
		std::array<glm::mat4, CascadeCount> projectionMatricesMoon{};
		std::array<float, CascadeCount - 1> splits{};
		std::array<float, CascadeCount> widthsSun{};
		std::array<float, CascadeCount> widthsMoon{};
		float blurWidth{};
		int blurQuality{}; // odd
		float blurGridSum{};
		float exposure{};
		float minBias{};
		float maxBias{};

		static ShadowInfo getDefaultValue() {
			ShadowInfo value;
			value.blurWidth = 0.5f;
			value.blurQuality = 7;
			value.exposure = 0.1f;
			value.minBias = 2.7f;
			value.maxBias = 7.5f;
			return value;
		}
	};
}

#endif