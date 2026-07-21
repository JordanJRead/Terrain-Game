//bool isPointInAtmosphere(vec3 pos) {
//	vec3 atmosphereCenter = getAtmosphereCenter();
//	return length(pos - atmosphereCenter) <= atmosphereInfo.maxRadius;
//}

vec3 getAtmosphereCenter() {
	return vec3(perFrameInfo.cameraPos.x, atmosphereInfo.centerY, perFrameInfo.cameraPos.z);
}

vec2 rayAtmosphereIntersection(vec3 pos, vec3 dir) {
	vec3 atmosphereCenter = getAtmosphereCenter();
	float a = dot(dir, dir);
	float b = 2 * (dot(dir, (pos - atmosphereCenter)));
	float c = dot((pos - atmosphereCenter), (pos - atmosphereCenter)) - pow(atmosphereInfo.maxRadius, 2);
	float disc = b * b - 4 * a * c;
	if (disc > 0) {
		vec2 ts = vec2((-b + sqrt(disc)) / (2 * a), (-b - sqrt(disc)) / (2 * a));
		ts = vec2(min(ts.x, ts.y), max(ts.x, ts.y));
		if (ts.x < 0)
			ts.x = 0;
		return ts;
	}
	return vec2(-1, -1);
}

float rayleighDensityAtPoint(vec3 pos) {
	vec3 atmosphereCenter = getAtmosphereCenter();
	float distFromCenter = length(atmosphereCenter - pos);
	if (distFromCenter < atmosphereInfo.minRadius) {
		return atmosphereInfo.rayleighDensity;
	}
	if (distFromCenter > atmosphereInfo.maxRadius) {
		return 0;
	}
	float norm = (distFromCenter - atmosphereInfo.minRadius) / (atmosphereInfo.maxRadius - atmosphereInfo.minRadius);
	return exp(-atmosphereInfo.rayleighDensityFalloff * norm) * (1 - norm) * atmosphereInfo.rayleighDensity;
}

float mieDensityAtPoint(vec3 pos) {
	vec3 atmosphereCenter = getAtmosphereCenter();
	float distFromCenter = length(atmosphereCenter - pos);
	if (distFromCenter < atmosphereInfo.minRadius) {
		return atmosphereInfo.mieDensity;
	}
	if (distFromCenter > atmosphereInfo.maxRadius) {
		return 0;
	}
	float norm = (distFromCenter - atmosphereInfo.minRadius) / (atmosphereInfo.maxRadius - atmosphereInfo.minRadius);
	return exp(-atmosphereInfo.mieDensityFalloff * norm) * (1 - norm) * atmosphereInfo.mieDensity;
}

float opticalDepth(vec3 pos, vec3 dir, bool isRayleigh) {
	vec2 ts = rayAtmosphereIntersection(pos, dir);
	if (ts.x == -1) {
		return 0;
	}

	int stepCount = atmosphereInfo.raySunStepCount;
	float totalDistance = ts.y - ts.x;
	float dx = totalDistance / stepCount;
	vec3 samplePos = pos;

	float depth = 0;
	for (int i = 0; i < stepCount; ++i) {
		float density = isRayleigh ? rayleighDensityAtPoint(samplePos) : mieDensityAtPoint(samplePos);
		depth += density * dx;
		samplePos += dir * dx;
	}
	return depth;
}