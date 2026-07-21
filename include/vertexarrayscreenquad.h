#ifndef VERTEX_ARRAY_SCREEN_QUAD_H
#define VERTEX_ARRAY_SCREEN_QUAD_H

#include "vertexarray.h"

class VertexArrayScreenQuad {
public:
	VertexArrayScreenQuad() : mVertexArray{ VertexArray::createScreenVertexArray() } {

	}
	const VertexArray& getVertexArray() const { return mVertexArray; }

private:
	VertexArray mVertexArray;
};

#endif