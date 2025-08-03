#include "vertexarray.h"
#include "glm/glm.hpp"
#include "gpuplane.h"
#include "plane.h"
#include <iostream>

GPUPlane::GPUPlane(int verticesPerEdge)
	: mVerticesPerEdge{ verticesPerEdge }
{
	Plane plane{ verticesPerEdge };
	mIndexCount = plane.getIndexCount();
	mVertexArray.create(plane.getVertices(), plane.getIndices(), std::vector<int>{Plane::mAttribs});
}

void GPUPlane::rebuild(int verticesPerEdge) {
	Plane plane{ verticesPerEdge };
	mIndexCount = plane.getIndexCount();
	mVertexArray.updateVertices(plane.getVertices(), plane.getIndices());
}