#ifndef HEADER_ATMOSPHERE_GLSL
#define HEADER_ATMOSPHERE_GLSL

#include "_headershadows.glsl"
#include "_headeruniformbuffers.glsl"
#include "_headervolumetrics.glsl"

float phase(float cosTheta, float g) {
	return 1 / (4 * PI) * (1 - g * g) / pow(1 + g * g - 2 * g * cosTheta, 3 / 2);
}

vec3 lightReceived(vec3 rayPos, vec3 rayDir, bool isSky, vec3 worldPosOfVisibleObject, vec3 albedo, vec3 normal = vec3(0), bool doShadows = true, bool water = false) {
	vec3 colourOfObject = albedo;
	if (water) // temporary, will rework deferred renderer soon
		return colourOfObject;
	if (!isSky) {
		float sunShadow = isPointInSunShadow(worldPosOfVisibleObject, normal, true);
		float moonShadow = isPointInMoonShadow(worldPosOfVisibleObject, normal, true);

		vec3 sunColourHittingHere = (1 - sunShadow) * colours.sunColour * colours.sunBrightness * exp(-(opticalDepth(worldPosOfVisibleObject, perFrameInfo.dirToSun, true) * atmosphereInfo.rayleighScattering + opticalDepth(worldPosOfVisibleObject, perFrameInfo.dirToSun, false) * atmosphereInfo.mieScattering));
		vec3 moonColourHittingHere = (1 - moonShadow) * colours.moonColour * colours.moonBrightness * exp(-(opticalDepth(worldPosOfVisibleObject, -perFrameInfo.dirToSun, true) * atmosphereInfo.rayleighScattering + opticalDepth(worldPosOfVisibleObject, -perFrameInfo.dirToSun, false) * atmosphereInfo.mieScattering));

		float sunDot = dot(normal, perFrameInfo.dirToSun);
		float moonDot = dot(normal, -perFrameInfo.dirToSun);
		sunDot = max(sunDot, 0);
		moonDot = max(moonDot, 0);

		float ambientDot = mix((moonDot + 1) / 2, (sunDot + 1) / 2, perFrameInfo.nightStrength);

		vec3 dayAmbient = vec3(1);
		vec3 nightAmbient = vec3(0.02);
		vec3 ambient = perFrameInfo.nightStrength * nightAmbient + (1 - perFrameInfo.nightStrength) * dayAmbient;

		colourOfObject = albedo * (sunDot * sunColourHittingHere + moonDot * moonColourHittingHere + ambientDot * ambient);
	}

	vec2 ts = rayAtmosphereIntersection(rayPos, rayDir);
	if (ts.x < 0 && ts.y < 0)
		return colourOfObject;
	if (!isSky)
		ts.y = min(ts.y, length(rayPos - worldPosOfVisibleObject));
	if (ts.x > ts.y)
		return colourOfObject;

	vec3 a = rayPos + rayDir * ts.x;
	vec3 b = rayPos + rayDir * ts.y;

	int stepCount = atmosphereInfo.rayAtmosphereStepCount;
	float totalDistance = ts.y - ts.x;
	float dx = totalDistance / stepCount;

	vec2 noiseSamplePos = gl_FragCoord.xy / textureSize(blueNoise, 0).xy;
	vec3 samplePos = a + rayDir * dx - texture(blueNoise, noiseSamplePos).r * atmosphereInfo.ditherStrength;

	float currentOpticalDepthRayleigh = 0;
	float currentOpticalDepthMie = 0;
	vec3 inScatteredLight = vec3(0, 0, 0);

	for (int n = 0; n < stepCount; ++n) {
		bool inSunShadow = doShadows ? (isPointInSunShadow(samplePos, normal) > 0.5) : false;
		bool inMoonShadow = doShadows ? (isPointInMoonShadow(samplePos, normal) > 0.5) : false;

		float rayleighDensity = rayleighDensityAtPoint(samplePos);
		float mieDensity = mieDensityAtPoint(samplePos);

		currentOpticalDepthRayleigh += rayleighDensity * dx;
		currentOpticalDepthMie += mieDensity * dx;

		// todo dont calculate if in shadow
		float toSunRayleighOpticalDepth = inSunShadow ? 0 : opticalDepth(samplePos, perFrameInfo.dirToSun, true);
		float toMoonRayleighOpticalDepth = inSunShadow ? 0 : opticalDepth(samplePos, -perFrameInfo.dirToSun, true);
		float toSunMieOpticalDepth = inMoonShadow ? 0 : opticalDepth(samplePos, perFrameInfo.dirToSun, false);
		float toMoonMieOpticalDepth = inMoonShadow ? 0 : opticalDepth(samplePos, -perFrameInfo.dirToSun, false);

		float cosThetaSun = dot(rayDir, perFrameInfo.dirToSun);
		float cosThetaMoon = dot(rayDir, -perFrameInfo.dirToSun);

		// In-scattering from sun
		vec3 inScatteredFromSunRayleigh = (inSunShadow ? vec3(0) : colours.sunColour * colours.sunBrightness * rayleighDensity * phase(cosThetaSun, atmosphereInfo.rayleighG) * atmosphereInfo.rayleighScattering * exp(-(currentOpticalDepthRayleigh + toSunRayleighOpticalDepth) * atmosphereInfo.rayleighScattering));
		vec3 inScatteredFromMoonRayleigh = (inMoonShadow ? vec3(0) : colours.moonColour * colours.moonBrightness * rayleighDensity * phase(cosThetaMoon, atmosphereInfo.rayleighG) * atmosphereInfo.rayleighScattering * exp(-(currentOpticalDepthRayleigh + toMoonRayleighOpticalDepth) * atmosphereInfo.rayleighScattering));
		vec3 inScatteredFromSunMie = (inSunShadow ? vec3(0) : colours.sunColour * colours.sunBrightness * mieDensity * phase(cosThetaSun, atmosphereInfo.mieG) * atmosphereInfo.mieScattering * exp(-(currentOpticalDepthMie + toSunMieOpticalDepth) * atmosphereInfo.mieScattering));
		vec3 inScatteredFromMoonMie = (inMoonShadow ? vec3(0) : colours.moonColour * colours.moonBrightness * mieDensity * phase(cosThetaMoon, atmosphereInfo.mieG) * atmosphereInfo.mieScattering * exp(-(currentOpticalDepthMie + toMoonMieOpticalDepth) * atmosphereInfo.mieScattering));

		inScatteredLight += (inScatteredFromSunRayleigh + inScatteredFromMoonRayleigh + inScatteredFromSunMie + inScatteredFromMoonMie) * atmosphereInfo.brightness * dx;

		samplePos += rayDir * dx;
	}

	return inScatteredLight + colourOfObject * exp(-(currentOpticalDepthRayleigh * atmosphereInfo.rayleighScattering + currentOpticalDepthMie * atmosphereInfo.mieScattering));
}

int getSplitIndex(vec3 dir) {
	return int((dir.y + 1) / 2 / (1.0 / STARYSPLITCOUNT));
}

bool isStarVisibleInSplit(vec3 dir, int i) {
	ivec2 indexInfo = starData.indexData[i];
	for (int starI = indexInfo.x; starI < indexInfo.x + indexInfo.y; ++starI) {
		vec4 star = starData.stars[starI];
		vec3 starDir = star.xyz;
		float starSize = star.w;

		if (dot(dir, starDir) > cos(radians(starSize))) {
			return true;
		}
	}
	return false;
}

vec3 getStarColour(vec3 dir) {
	float theta = -perFrameInfo.dayTime * PI;
	vec3 rotatedDir = mat3(perFrameInfo.starRotationMatrix) * dir;

	if (dot(dir, perFrameInfo.dirToSun) > cos(radians(atmosphereInfo.sunSizeDeg))) {
		return colours.sunColour * colours.sunBrightness;
	}

	if (dot(dir, -perFrameInfo.dirToSun) > cos(radians(atmosphereInfo.sunSizeDeg))) {
		return colours.moonColour * colours.moonBrightness;
	}

	int ySplitIndex = clamp(getSplitIndex(rotatedDir), 0, STARYSPLITCOUNT - 1);

	if (isStarVisibleInSplit(rotatedDir, ySplitIndex))
		return colours.starColour * colours.starBrightness * perFrameInfo.nightStrength;

	if (ySplitIndex != 0)
		ySplitIndex -= 1;

	if (isStarVisibleInSplit(rotatedDir, ySplitIndex))
		return colours.starColour * colours.starBrightness * perFrameInfo.nightStrength;

	ySplitIndex++;
	if (ySplitIndex != STARYSPLITCOUNT - 1)
		ySplitIndex += 1;

	if (isStarVisibleInSplit(rotatedDir, ySplitIndex))
		return colours.starColour * colours.starBrightness * perFrameInfo.nightStrength;

	return vec3(0);
}

#endif
