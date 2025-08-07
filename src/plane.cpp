#include "vertexarray.h"
#include "glm/glm.hpp"
#include "plane.h"
#include "random.h"
#include <iostream>
#include <cassert>

Plane::Plane(int verticesPerEdge)
	: mVerticesPerEdge{ verticesPerEdge }
	, mFloatsPerVertex{ 2 }
	, mWidth{ 1 }
{
	int vertexCount{ mVerticesPerEdge * mVerticesPerEdge };

	mVertices.reserve(vertexCount * mFloatsPerVertex);

	double stepSize{ 1.0 / (mVerticesPerEdge - 1) };
	double currentXPos{ -0.5 };
	double currentZPos{ -0.5 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		mVertices.push_back(currentXPos);
		mVertices.push_back(currentZPos);

		if ((vertexI + 1) % mVerticesPerEdge == 0) {
			currentXPos = -0.5;
			currentZPos += stepSize;
		}
		else {
			currentXPos += stepSize;
		}
	}

	int numOfQuads{ (mVerticesPerEdge - 1) * (mVerticesPerEdge - 1) };
	int numOfTriangles{ numOfQuads * 2 };
	mIndexCount = numOfTriangles * 3;
	mIndices.reserve(mIndexCount);

	int quadsFound{ 0 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		// Ignore vertices on right or top edge
		if ((vertexI + 1) % mVerticesPerEdge == 0 || vertexI >= vertexCount - mVerticesPerEdge)
			continue;

		mIndices.push_back(vertexI);
		mIndices.push_back(vertexI + 1);
		mIndices.push_back(vertexI + mVerticesPerEdge);

		mIndices.push_back(vertexI + 1);
		mIndices.push_back(vertexI + mVerticesPerEdge);
		mIndices.push_back(vertexI + 1 + mVerticesPerEdge);

		++quadsFound;
	}
}

Plane::Plane(const Plane& basePlane, const glm::vec3 worldPos, float scale, std::function<float(const glm::vec2&)> heightFunction)
	: mVerticesPerEdge{ basePlane.getVerticesPerEdge() }
	, mIndexCount{ basePlane.getIndexCount() }
	, mFloatsPerVertex{ 3 }
	, mIndices{ basePlane.getIndices() }
	, mWidth{ scale }
{
	assert(basePlane.getFloatsPerVertex() == 2);

	int vertexCount{ mVerticesPerEdge * mVerticesPerEdge };

	mVertices.reserve(vertexCount * mFloatsPerVertex);

	const std::vector<float>& baseVertexData{ basePlane.getVertices() };
	glm::vec3 flatStepWorldPos{ getClosestWorldVertexPos(worldPos) };

	for (int baseVertexI{ 0 }; baseVertexI < baseVertexData.size() - 1; baseVertexI += 2) {
		float xValue{ baseVertexData[baseVertexI] };
		float zValue{ baseVertexData[baseVertexI + 1] };
		xValue *= scale;
		xValue += flatStepWorldPos.x;
		zValue *= scale;
		zValue += flatStepWorldPos.z;
		float yValue = heightFunction({ xValue, zValue });
		mVertices.push_back(xValue);
		mVertices.push_back(yValue);
		mVertices.push_back(zValue);
	}
}