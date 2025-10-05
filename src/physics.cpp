#include "physics.h"
#include "glm/glm.hpp"
#include <vector>
#include "planephysics.h"
#include <optional>
#include <cmath>
#include <array>
#include <algorithm>
#include <iostream>
#include "mathhelper.h"

// https://www.peroxide.dk/papers/collision/collision.pdf
Physics::CollisionData Physics::getCollisionData(const glm::vec3& spherePosition, const glm::vec3& displacement, const std::array<glm::vec3, 3>& trianglePoints) {
	glm::vec3 planeNormal{ glm::cross(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0]) };
	planeNormal = glm::normalize(planeNormal);
	double normalDotDisplacement{ glm::dot(planeNormal, displacement) };
	double distFromPlane{ MathHelper::signedDistFromPlane(spherePosition, planeNormal, trianglePoints[0])};

	// Moving in wrong direction
	if (glm::dot(planeNormal, glm::normalize(displacement)) > 0) {
		//std::cerr << "1: " << 2 << "\n";
		return Physics::CollisionData{ 2, glm::vec3 {0, 0, 0} }; // No collision
	}

	float t0; // Time when sphere is on front side of plane
	float t1; // Time when sphere is on back  side of plane

	if (normalDotDisplacement == 0) { // Moving parallel to plane
		if (std::abs(distFromPlane) <= 1) { // Always colliding
			t0 = 0;
			t1 = 1;
		}
		else {
			//std::cerr << "2: " << 2 << "\n";
			return Physics::CollisionData{ 2, glm::vec3 {0, 0, 0} }; // No collision
		}
	}
	else {
		t0 = (1 - distFromPlane) / (normalDotDisplacement);
		t1 = (-1 - distFromPlane) / (normalDotDisplacement);
	}

	if ((t0 < 0 && t1 < 0) || (t0 > 1 && t1 > 1)) {
	//if ((t0 < 0 || t0 > 1) && (t1 < 0 || t1 > 1)) {
		//std::cerr << "3: " << 2 << "\n";
		return Physics::CollisionData{ 2, glm::vec3 {0, 0, 0} }; // No collision
	}

	t0 = fmax(t0, 0);
	t1 = fmin(t0, 1);

	// Collision happens inside triangle
	glm::vec3 planeIntersectionPoint{ spherePosition + displacement * t0 - planeNormal };
	if (MathHelper::isPointInTriangle(planeIntersectionPoint, trianglePoints[0], trianglePoints[1], trianglePoints[2])) {
		//std::cerr << "4: " << t0 << " " << planeNormal.x << " " << planeNormal.y << " " << planeNormal.z << " " << "\n";
		return { t0, planeIntersectionPoint };
	}

	// Collision against each vertex
	std::array<double, 3> vertexTValues;

	for (int vertexI{ 0 }; vertexI < 3; ++vertexI) {
		double a{ glm::dot(displacement, displacement) };
		double b{ 2 * glm::dot(displacement, (spherePosition - trianglePoints[vertexI])) };
		double c{ glm::dot(trianglePoints[vertexI] - spherePosition, trianglePoints[vertexI] - spherePosition) - 1 };
		std::optional<double> collisionT{ MathHelper::getSmallestQuadratic(a, b, c) };
		if (collisionT && 0 <= collisionT.value() && collisionT.value() <= 1)
			vertexTValues[vertexI] = collisionT.value();
		else
			vertexTValues[vertexI] = 2;
	}

	// Collision against each edge
	std::array<double, 3> edgeTValues;
	std::array<glm::vec3, 3> edgeCollisionPositions;

	for (int edgeI{ 0 }; edgeI < 3; ++edgeI) {
		const glm::vec3& start{ trianglePoints[(edgeI + 0) % 3] };
		const glm::vec3& end  { trianglePoints[(edgeI + 1) % 3] };

		glm::vec3 edge{ end - start };
		glm::vec3 spherePosToVertex{ start - spherePosition };

		double a = glm::dot(edge, edge) * -glm::dot(displacement, displacement) + glm::dot(edge, displacement) * glm::dot(edge, displacement);
		double b = glm::dot(edge, edge) * 2 * (glm::dot(displacement, spherePosToVertex)) - 2 * (glm::dot(edge, displacement) * glm::dot(edge, spherePosToVertex));
		double c = glm::dot(edge, edge) * (1 - glm::dot(spherePosToVertex, spherePosToVertex)) + glm::dot(edge, spherePosToVertex) * glm::dot(edge, spherePosToVertex);

		std::optional<double> x1 = MathHelper::getSmallestQuadratic(a, b, c);

		if (!x1 || x1 < 0 || x1 > 1) {
			edgeTValues[edgeI] = 2;
			continue;
		}

		double f0{ (glm::dot(edge, displacement) * x1.value() - glm::dot(edge, spherePosToVertex)) / glm::dot(edge, edge)};
		if (f0 < 0 || f0 > 1)
			edgeTValues[edgeI] = 2;
		else {
			edgeTValues[edgeI] = x1.value();
			edgeCollisionPositions[edgeI] = start + (float)f0 * edge;
		}
	}

	// Find smallest out of edges and vertices
	std::pair<size_t, double> minVertex{ MathHelper::getMinOfArray(vertexTValues) };
	std::pair<size_t, double> minEdge{ MathHelper::getMinOfArray(edgeTValues) };

	if (minVertex.second < minEdge.second) {
		//std::cerr << "5: " << minVertex.second << "\n";
		return { minVertex.second, trianglePoints[minVertex.first] };
	}
	else {
		//std::cerr << "6: " << minEdge.second << "\n";
		return { minEdge.second, edgeCollisionPositions[minEdge.first] };
	}
}
 
glm::vec3 Physics::move(const glm::vec3& capsulePos, const glm::vec3& capsuleScales, const glm::vec3& displacement, const PlanePhysics& physicsPlane, int maxRecursionDepth) {
	if (glm::length(displacement) == 0)
		return capsulePos;

	const std::vector<float>& vertexData{ physicsPlane.getVertexData() };
	const std::vector<unsigned int>& vertexIndices{ physicsPlane.getIndices() };

	glm::vec3 ePosition{ capsulePos / capsuleScales };
	glm::vec3 eDisplacement{ displacement / capsuleScales };

	std::vector<float> eVertexData;
	eVertexData.reserve(vertexData.size());

	for (int vertexDataI{ 0 }; vertexDataI < vertexData.size(); ++vertexDataI) {
		eVertexData.push_back(vertexData[vertexDataI] / capsuleScales[vertexDataI % 3]); // compress into ellipse space
	}

	for (int recursionI{ 0 }; recursionI < maxRecursionDepth; ++recursionI) {
		double smallestT{ 2 };
		glm::vec3 closestCollisionPosition{ 0, 0, 0 }; // collision when colliding or collision position?

		// For each triangle
		for (int vertexIndexI{ 0 }; vertexIndexI < physicsPlane.getIndexCount(); vertexIndexI += 3) {
			unsigned int index0{ vertexIndices[vertexIndexI + 0] };
			unsigned int index1{ vertexIndices[vertexIndexI + 1] };
			unsigned int index2{ vertexIndices[vertexIndexI + 2] };

			glm::vec3 p0{ eVertexData[index0 * 3 + 0], eVertexData[index0 * 3 + 1], eVertexData[index0 * 3 + 2] };
			glm::vec3 p1{ eVertexData[index1 * 3 + 0], eVertexData[index1 * 3 + 1], eVertexData[index1 * 3 + 2] };
			glm::vec3 p2{ eVertexData[index2 * 3 + 0], eVertexData[index2 * 3 + 1], eVertexData[index2 * 3 + 2] };

			CollisionData collisionData{ getCollisionData(ePosition, eDisplacement, {p0, p1, p2}) };
			if (collisionData.t < smallestT) {
				smallestT = collisionData.t;
				closestCollisionPosition = collisionData.planeIntersectionPoint; // can we just calculate this with t and eDisplacement?
			}
		}

		// Exit early
		if (smallestT == 2) // Default impossible value for t
			break;

		glm::vec3 eDesiredDestination{ ePosition + eDisplacement };

		// Collision response
		ePosition += eDisplacement * (float)smallestT;

		glm::vec3 eSlidingPlanePoint{ closestCollisionPosition };
		glm::vec3 eSlidingPlaneNormal{ glm::normalize(ePosition - closestCollisionPosition) };

		double distCutOff = 1 - std::abs(MathHelper::signedDistFromPlane(eDesiredDestination, eSlidingPlaneNormal, eSlidingPlanePoint));

		float offset = 0.00001f;

		glm::vec3 eNewDesiredDestination{ eDesiredDestination + eSlidingPlaneNormal * (float)distCutOff + offset * eSlidingPlaneNormal };
		ePosition += offset * eSlidingPlaneNormal;
		eDisplacement = eNewDesiredDestination - ePosition;

		if (glm::length(eDisplacement) > 10) {
			int x = 1;
		}
	}

	return (ePosition + eDisplacement) * capsuleScales;
}