#include <stdio.h>
#include "g_types.h"
#include "glfw_fb/glfw_fb.h"

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char const *argv[]) {
	
	printf("Hi\n");

	unsigned int buffer[WIDTH * HEIGHT];

	int res = fb_open();
	fb_update(buffer);
	fb_close();


	return 0;
}