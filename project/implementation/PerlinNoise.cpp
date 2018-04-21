#include "PerlinNoise.h"

int PerlinNoise::_p[] = {};

PerlinNoise::PerlinNoise()
{
	for (int i = 0; i < 512; i++) {
		_p[i] = _permutation[i % 256];
	}
}

int PerlinNoise::inc(int num)
{
	num++;
	return num;
}

float PerlinNoise::fade(float t)
{
	// 6t^5 - 15t^4 + 10t^3, as defined by Perlin
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(float a, float b, float x)
{
	return a + x * (b - a);
}

// calculate the dot product of a randomly selected gradient vector and the 8 location vectors
float PerlinNoise::grad(int hash, float x, float y, float z)
{
	switch (hash & 0xF)
	{
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		case 0x4: return  x + z;
		case 0x5: return -x + z;
		case 0x6: return  x - z;
		case 0x7: return -x - z;
		case 0x8: return  y + z;
		case 0x9: return -y + z;
		case 0xA: return  y - z;
		case 0xB: return -y - z;
		case 0xC: return  y + x;
		case 0xD: return -y + z;
		case 0xE: return  y - x;
		case 0xF: return -y - z;
		default: return 0;
	}
}

float PerlinNoise::perlin(float x, float y, float z)
{
	// Calculate the "unit cube" that the point asked will be located in
	// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
	// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
	int xi = (int)x & 255;
	int yi = (int)y & 255;
	int zi = (int)z & 255;
	float xf = x - (int)x;
	float yf = y - (int)y;
	float zf = z - (int)z;

	float u = fade(xf);
	float v = fade(yf);
	float w = fade(zf);

	// Hash Function to get a unique value for every coordinate input
	// hashes all 8 unit cube coordinates surrounding the input coordinate
	int aaa, aba, aab, abb, baa, bba, bab, bbb;
	aaa = _p[_p[_p[xi] + yi] + zi];
	aba = _p[_p[_p[xi] + inc(yi)] + zi];
	aab = _p[_p[_p[xi] + yi] + inc(zi)];
	abb = _p[_p[_p[xi] + inc(yi)] + inc(zi)];
	baa = _p[_p[_p[inc(xi)] + yi] + zi];
	bba = _p[_p[_p[inc(xi)] + inc(yi)] + zi];
	bab = _p[_p[_p[inc(xi)] + yi] + inc(zi)];
	bbb = _p[_p[_p[inc(xi)] + inc(yi)] + inc(zi)];

	// The gradient function calculates the dot product between a pseudorandom
	// gradient vector and the vector from the input coordinate to the 8
	// surrounding points in its unit cube.
	// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
	// values we made earlier.
	float x1, x2, y1, y2;
	x1 = lerp(grad(aaa, xf, yf, zf), grad(baa, xf - 1, yf, zf), u);
	x2 = lerp(grad(aba, xf - 1, yf, zf), grad(bba, xf - 1, yf - 1, zf), u);

	y1 = lerp(x1, x2, v);

	x1 = lerp(grad(aab, xf, yf, zf - 1), grad(bab, xf - 1, yf, zf - 1), u);
	x2 = lerp(grad(abb, xf, yf - 1, zf - 1), grad(bbb, xf - 1, yf - 1, zf - 1), u);

	y2 = lerp(x1, x2, v);

	return (lerp(y1, y2, w) + 1) / 2;
}

float PerlinNoise::generateHeight(float x, float y, float z)
{
	float total = 0;
	float frequency = 4;
	float amplitude = 70;
	float maxValue = 0;
	float persistence = 2;

	int octaves = 3;

	for (int i = 0; i < octaves; i++)
	{
		total = total + perlin(x * frequency, y * frequency, z * frequency) * amplitude;

		maxValue = maxValue + amplitude;

		amplitude *= persistence;
		frequency *= 2;
	}

	return total / maxValue;
}
