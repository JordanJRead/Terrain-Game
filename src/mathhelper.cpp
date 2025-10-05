#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <array>
#include <optional>
#include "uimanager.h"
#include "mathhelper.h"

bool MathHelper::floatEqual(float x, float y, float tol) {
	return x - y < tol;
}

double MathHelper::signedDistFromPlane(const glm::vec3 pos, const glm::vec3 planeNormal, const glm::vec3& planePoint) {
	return glm::dot(planeNormal, (pos - planePoint));
}

double MathHelper::areaOfTriangle(const std::array<glm::vec3, 3>& trianglePoints) {
	return 0.5 * glm::length(glm::cross(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0]));
}

bool MathHelper::isPointInTriangle(const glm::vec3& point,
	const glm::vec3& pa, const glm::vec3& pb, const glm::vec3& pc)
{
	glm::vec3 e10 = pb - pa;
	glm::vec3 e20 = pc - pa;
	float a = glm::dot(e10, e10);
	float b = glm::dot(e10, e20);
	float c = glm::dot(e20, e20);
	float ac_bb = (a * c) - (b * b);
	glm::vec3 vp(point.x - pa.x, point.y - pa.y, point.z - pa.z);
	float d = glm::dot(vp, e10);
	float e = glm::dot(vp, e20);
	float x = (d * c) - (e * b);
	float y = (e * a) - (d * b);
	float z = x + y - ac_bb;
	return ((((int)z) & ~(((int)x) | ((int)y))) & 0x80000000);
}

std::optional<double> MathHelper::getSmallestQuadratic(double a, double b, double c) {
	double disc{ b * b - 4 * a * c };
	if (disc < 0) {
		return std::nullopt;
	}
	double x1 = (-b + std::sqrt(disc)) / (2 * a);
	double x2 = (-b - std::sqrt(disc)) / (2 * a);
	return std::min(x1, x2);
}

glm::vec3 MathHelper::getClosestWorldStepPosition(const glm::vec3 pos, float stepSize) {
	glm::vec3 stepSizesAway = pos / stepSize;
	stepSizesAway = glm::vec3{ (int)stepSizesAway.x, (int)stepSizesAway.y, (int)stepSizesAway.z };
	return stepSizesAway * stepSize;
}


glm::vec3 MathHelper::getDirToSun(const UIManager& uiManager) {
	float theta{ uiManager.mDayTime.data() * glm::pi<float>() };
	return glm::vec3{ glm::cos(theta), glm::sin(theta), 0 };
}

int MathHelper::getClosestInt(float x) {
	return int(round(x) + 0.1 * (x < 0 ? -1 : 1));
}

int MathHelper::max(int x, int y) {
	return x > y ? x : y;
}

unsigned int MathHelper::rand(unsigned int n) {
	unsigned int state = n * 747796405u + 2891336453u;
	unsigned int word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	word = (word >> 22u) ^ word;
	return word;
}

float MathHelper::randToFloat(unsigned int n) {
	return float(n) / 4294967296.0;
}

unsigned int MathHelper::labelPoint(int x, int y) {
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

glm::vec2 MathHelper::randUnitVector(float randNum) {
	float theta = 2 * 3.14159 * randNum;
	return glm::normalize(glm::vec2(cos(theta), sin(theta)));
}

glm::vec2 MathHelper::quinticInterpolation(glm::vec2 t) {
	return t * t * t * (t * (t * glm::vec2(6) - glm::vec2(15)) + glm::vec2(10));
}

glm::vec2 MathHelper::quinticDerivative(glm::vec2 t) {
	return glm::vec2(30) * t * t * (t * (t - glm::vec2(2)) + glm::vec2(1));
}

float MathHelper::perlin(const glm::vec2& pos, int reroll) {
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

#endif