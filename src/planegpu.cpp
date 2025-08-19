#include "vertexarray.h"
#include "glm/glm.hpp"
#include "planegpu.h"
#include "planeunit.h"
#include <iostream>

PlaneGPU::PlaneGPU(int verticesPerEdge)
	: PlaneI{ verticesPerEdge }
{
	PlaneUnit plane{ verticesPerEdge };
	mIndexCount = plane.getIndexCount();
	mVertexArray.create(plane.getVertexData(), plane.getIndices(), std::vector<int>{ 2 });
}

PlaneGPU::PlaneGPU(const PlanePhysics& physicsPlane)
	: PlaneI{ physicsPlane.getVerticesPerEdge() }
{
	mIndexCount = physicsPlane.getIndexCount();
	mIndexCount = physicsPlane.getIndexCount();
	mVertexArray.create(physicsPlane.getVertexData(), physicsPlane.getIndices(), std::vector<int>{ 3 });
}

void PlaneGPU::rebuild(int verticesPerEdge) {
	PlaneUnit plane{ verticesPerEdge };
	mIndexCount = plane.getIndexCount();
	mVertexArray.updateVertices(plane.getVertexData(), plane.getIndices());
}

glm::vec3 PlaneGPU::getClosestWorldVertexPos(const glm::vec3 pos, float scale) {
	float stepSize{ getStepSize(scale) };
	glm::vec3 stepSizesAway = pos / stepSize;
	stepSizesAway = glm::vec3{ (int)stepSizesAway.x, (int)stepSizesAway.y, (int)stepSizesAway.z };
	return stepSizesAway * stepSize;
}