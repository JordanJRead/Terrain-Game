#ifndef MY_MATH_HELPER_H
#define MY_MATH_HELPER_H

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <array>
#include <optional>
#include "uimanager.h"

namespace MathHelper {
	int taxicabDist(glm::ivec2 p1, glm::ivec2 p2);
	bool floatEqual(float x, float y, float tol = 0.001);

	float signedDistFromPlane(const glm::vec3 pos, const glm::vec3 planeNormal, const glm::vec3& planePoint);
	double areaOfTriangle(const std::array<glm::vec3, 3>& trianglePoints);
	bool isPointInTriangle(const glm::vec3& point, const glm::vec3& pa, const glm::vec3& pb, const glm::vec3& pc);

	std::optional<double> getSmallestQuadratic(double a, double b, double c);

	template <size_t N>
	inline std::pair<size_t, double> getMinOfArray(const std::array<double, N>& arr) {
		double min{ arr[0] };
		size_t minI{ 0 };
		for (int i{ 1 }; i < N; ++i) {
			if (arr[i] < min) {
				min = arr[i];
				minI = i;
			}
		}
		return { minI, min };
	}

	glm::vec3 getClosestWorldStepPosition(const glm::vec3 pos, float stepSize);

	glm::vec3 getDirToSun(const UIManager& uiManager);

	int getClosestInt(float x);

	int max(int x, int y);

	unsigned int rand(unsigned int n);

	float randToFloat(unsigned int n);

	unsigned int labelPoint(int x, int y);

	glm::vec2 randUnitVector(float randNum);

	glm::vec2 quinticInterpolation(glm::vec2 t);

	glm::vec2 quinticDerivative(glm::vec2 t);

	float extreme(float x);

	float perlin(const glm::vec2& pos, int reroll);
}

#endif