#ifndef PLANE_I
#define PLANE_I

#include <cassert>

class PlaneI {
public:
	int getIndexCount() const { return mIndexCount; }
	int getVerticesPerEdge() const { return mVerticesPerEdge; }

protected:
	PlaneI(int verticesPerEdge) : mVerticesPerEdge{ verticesPerEdge } {
		assert(verticesPerEdge >= 2);
	}
	int mIndexCount{ 0 };
	int mVerticesPerEdge;
};

#endif