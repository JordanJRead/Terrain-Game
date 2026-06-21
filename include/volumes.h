#ifndef VOLUMES_H
#define VOLUMES_H

#include "glm/glm.hpp"

struct ParticipatingMedia {
	glm::vec3 colour;
	float phaseG;
};

struct AtmosphereMedia {
	ParticipatingMedia media;
	float density;
	float falloff;
};

#endif