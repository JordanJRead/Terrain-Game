#include "physics.h"
#include "glm/glm.hpp"
#include <vector>
#include "planephysics.h"

//Physics::CollisionData Physics::getCollisionData(const glm::vec3& spherePosition, const glm::vec3& displacement, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) {
//
//}

glm::vec3 Physics::move(const glm::vec3& capsulePos, const glm::vec3& capsuleScales, const glm::vec3& displacement, const PlanePhysics& physicsPlane, int maxRecursionDepth) {
	const std::vector<float>& vertexData{ physicsPlane.getVertexData() };
	const std::vector<unsigned int>& vertexIndices{ physicsPlane.getIndices() };

	glm::vec3 ePosition{ capsulePos / capsuleScales };
	glm::vec3 eDisplacement{ displacement / capsuleScales };

	//std::vector<float> eVertexData;
	//eVertexData.reserve(vertexData.size());

	//for (int vertexDataI{ 0 }; vertexDataI < vertexData.size(); ++vertexDataI) {
	//	eVertexData.push_back(vertexData[vertexDataI] / capsuleScales[vertexDataI % 3]); // compress into ellipse space
	//}

	//for (int recursionI{ 0 }; recursionI < maxRecursionDepth; ++recursionI) {
	//	float smallestT{ 2 };
	//	glm::vec3 closestCollisionPosition{ 0, 0, 0 };

	//	// For each triangle
	//	for (int vertexIndexI{ 0 }; vertexIndexI < physicsPlane.getIndexCount(); vertexIndexI += 3) {
	//		float index0{ vertexIndices[vertexIndexI + 0] };
	//		float index1{ vertexIndices[vertexIndexI + 1] };
	//		float index2{ vertexIndices[vertexIndexI + 2] };

	//		glm::vec3 p0{ eVertexData[index0 * 3 + 0], eVertexData[index0 * 3 + 1], eVertexData[index0 * 3 + 2] };
	//		glm::vec3 p1{ eVertexData[index1 * 3 + 0], eVertexData[index1 * 3 + 1], eVertexData[index1 * 3 + 2] };
	//		glm::vec3 p2{ eVertexData[index2 * 3 + 0], eVertexData[index2 * 3 + 1], eVertexData[index2 * 3 + 2] };

	//		CollisionData collisionData{ getCollisionData(ePosition, eDisplacement, p0, p1, p2) };
	//		if (collisionData.t < smallestT) {
	//			smallestT = collisionData.t;
	//			closestCollisionPosition = collisionData.position;
	//		}
	//	}

	//	// Exit early
	//	if (smallestT == 2) // Default impossible value for t
	//		break;

	//	glm::vec3 eDesiredDestination{ ePosition + displacement };

	//	// Collision response
	//	ePosition += eDisplacement * smallestT;
	//	glm::vec3 eSlidingPlanePoint{ closestCollisionPosition };
	//	glm::vec3 eSlidingPlaneNormal{ glm::normalize(ePosition - closestCollisionPosition) };
	//	float eDesiredDestinationDistanceFromSlidingPlane{ distanceFromPlane(eDesiredDestination, eSlidingPlanePoint, eSlidingPlaneNormal) };
	//	glm::vec3 eNewDestination{ eDesiredDestination + eDesiredDestinationDistanceFromSlidingPlane * eSlidingPlaneNormal };

	//	eDisplacement = eNewDestination - ePosition;
	//	ePosition -= 0.01f * eSlidingPlaneNormal; 
	//}

	return (ePosition + eDisplacement) * capsuleScales;
}