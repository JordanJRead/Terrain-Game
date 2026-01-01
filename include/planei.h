#ifndef PLANE_I
#define PLANE_I

class PlaneI {
public:
	int getIndexCount() const { return mIndexCount; }
	int getVerticesPerEdge() const { return mVerticesPerEdge; }

protected:
	PlaneI(int verticesPerEdge) : mVerticesPerEdge{ verticesPerEdge } {}
	int mIndexCount{ 0 };
	int mVerticesPerEdge;
};

#endif