#ifndef GPU_PLANE_H
#define GPU_PLANE_H

#include "glad/glad.h"
#include "vertexarray.h"
#include "glm/glm.hpp"

class GPUPlane {
public:
	GPUPlane(int verticesPerEdge);
	void rebuild(int vertexLengthCount);
	void useVertexArray() { mVertexArray.use(); };
	int getIndexCount() const { return mIndexCount; }
	int getVerticesPerEdge() const { return mVerticesPerEdge; }
	float getStepSize() const { return 1.0 / (mVerticesPerEdge - 1); }

private:
	VertexArray mVertexArray;
	int mVerticesPerEdge;
	int mIndexCount;
};

#endif