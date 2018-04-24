#include "PerlinNoise2D.h"
#define _USE_MATH_DEFINES
#include <math.h>

int PerlinNoise2D::_p[] = {};

PerlinNoise2D::PerlinNoise2D()
{
	for (int i = 0; i < 512; i++) {
		_p[i] = _permutation[i % 256];
	}
}

int PerlinNoise2D::inc(int num)
{
	num++;
	return num;
}

float PerlinNoise2D::fade(float t)
{
	// 6t^5 - 15t^4 + 10t^3, as defined by Perlin
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise2D::lerp(float a, float b, float x)
{
	return a + x * (b - a);
}

float PerlinNoise2D::cosineInterpolate(float a, float b, float mu)
{
	float theta = mu * M_PI;
	float f = (1.0f - cos(theta)) * 0.5f;
	return a * (1.0f - f) + b + f;
}

// calculate the dot product of a randomly selected gradient vector and the 4 location vectors
float PerlinNoise2D::grad(int hash, float x, float z)
{
	switch (hash & 0x7)
	{
		case 0x0: return  x + z;
		case 0x1: return -x + z;
		case 0x2: return  x - z;
		case 0x3: return -x - z;
		case 0x4: return  z + x;
		case 0x5: return -z + x;
		case 0x6: return  z - x;
		case 0x7: return -z - x;
		default: return 0;
	}
}

float PerlinNoise2D::perlin(float x, float z)
{
	// Calculate the "unit cube" that the point asked will be located in
	// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
	// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
	int xi = (int)x & 255;
	int zi = (int)z & 255;
	float xf = x - (int)x;
	float zf = z - (int)z;

	float u = fade(xf);
	float v = fade(zf);

	// Hash Function to get a unique value for every coordinate input
	// hashes all 4 unit cube coordinates surrounding the input coordinate
	int aa, ab, ba, bb;

	aa = _p[_p[xi] + zi];
	ab = _p[_p[xi] + inc(zi)];
	ba = _p[_p[inc(xi)] + zi];
	bb = _p[_p[inc(xi)] + inc(zi)];

	// The gradient function calculates the dot product between a pseudorandom
	// gradient vector and the vector from the input coordinate to the 4
	// surrounding points in its unit cube.
	// This is all then lerped together as a sort of weighted average based on the faded (u,v)
	// values we made earlier.
	float x1, x2, y1, y2;
	x1 = lerp(grad(aa, xf, zf), grad(ab, xf - 1, zf), u);
	x2 = lerp(grad(ba, xf - 1, zf), grad(bb, xf - 1, zf), u);

	y1 = lerp(x1, x2, v);

	return (y1 + 1) / 2;
}

float PerlinNoise2D::generateHeight(float x, float z)
{
	float total = 0;
	float frequency = 4;
	float amplitude = 1;
	float maxValue = 0;
	float persistence = 2;

	int octaves = 1;

	for (int i = 0; i < octaves; i++)
	{
		total = total + perlin(x * frequency, z * frequency) * amplitude;

		maxValue = maxValue + amplitude;

		amplitude *= persistence;
		frequency *= 2;
	}

	return (total / maxValue);
}
