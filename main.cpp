#include <stdio.h>
#include "g_types.h"
#include "minifb/include/minifb.h"

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char const *argv[]) {
	
	printf("Hi\n");

	unsigned int buffer[WIDTH * HEIGHT];

	int noise, carry, seed = 0xbeef;

	if (!mfb_open("Raytracer", WIDTH, HEIGHT))
		return 0;

	for (;;)
	{
		int i, state;

		for (i = 0; i < WIDTH * HEIGHT; ++i)
		{
			noise = seed;
			noise >>= 3;
			noise ^= seed;
			carry = noise & 1;
			noise >>= 1;
			seed >>= 1;
			seed |= (carry << 30);
			noise &= 0xFF;
			buffer[i] = MFB_RGB(noise, noise, noise); 
		}

		state = mfb_update(buffer);

		if (state < 0)
			break;
	}

	mfb_close();


	return 0;
}