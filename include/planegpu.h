#ifndef PLANE_GPU_H
#define PLANE_GPU_H

#include "glad/glad.h"
#include "vertexarray.h"
#include "glm/glm.hpp"
#include "planei.h"

#include "planephysics.h"

class PlaneGPU : public PlaneI {
public:
	PlaneGPU(int verticesPerEdge);
	PlaneGPU(const PlanePhysics& physicsPlane);
	void rebuild(int verticesPerEdge);
	void useVertexArray() { mVertexArray.use(); }
	float getStepSize(float scale) const { return (1.0f / (mVerticesPerEdge - 1)) * scale; }
	glm::vec3 getClosestWorldVertexPos(const glm::vec3 pos, float scale);
	const VertexArray& getVertexArray() const { return mVertexArray; }

private:
	VertexArray mVertexArray;
	bool isPhysics{ false };
};

#endif