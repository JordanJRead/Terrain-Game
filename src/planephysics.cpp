#include "planephysics.h"
#include "planeunit.h"
#include "terrainrenderer.h"

PlanePhysics::PlanePhysics(int verticesPerEdge, const glm::vec3& worldPos, float width, const TerrainRenderer& terrainRenderer)
	: PlaneI{ verticesPerEdge }
	, mWidth{ width }
{
	constexpr int floatsPerVertex{ 3 };

	PlaneUnit basePlane{ verticesPerEdge };
	mIndices = basePlane.getIndices();
	mIndexCount = basePlane.getIndexCount();
	int vertexCount{ mVerticesPerEdge * mVerticesPerEdge };

	mVertexData.reserve(vertexCount * floatsPerVertex);

	const std::vector<float>& baseVertexData{ basePlane.getVertexData() };
	glm::vec3 flatStepWorldPos{ getClosestWorldVertexPos(worldPos) };

	for (int baseVertexI{ 0 }; baseVertexI < baseVertexData.size() - 1; baseVertexI += 2) {
		float xValue{ baseVertexData[baseVertexI] };
		float zValue{ baseVertexData[baseVertexI + 1] };
		xValue *= mWidth;
		xValue += flatStepWorldPos.x;
		zValue *= mWidth;
		zValue += flatStepWorldPos.z;
		float yValue = terrainRenderer.getHeightAtPoint({ xValue, zValue });
		mVertexData.push_back(xValue);
		mVertexData.push_back(yValue);
		mVertexData.push_back(zValue);
	}
}

glm::vec3 PlanePhysics::getClosestWorldVertexPos(const glm::vec3 pos) {
	float stepSize{ getStepSize() };
	glm::vec3 stepSizesAway = pos / stepSize;
	stepSizesAway = glm::vec3{ (int)stepSizesAway.x, (int)stepSizesAway.y, (int)stepSizesAway.z };
	return stepSizesAway * stepSize;
}