#include <stdio.h>
#include "glfw_fb/glfw_fb.h"

#define WIDTH 800
#define HEIGHT 600

unsigned int buffer[WIDTH * HEIGHT];

int fb_get_texture_width() {
	return WIDTH;
}

int fb_get_texture_height() {
	return HEIGHT;
}

unsigned int* fb_update(int timestep) {

	for (int i = 0; i < WIDTH; ++i) {
		for (int j = 0; j < HEIGHT; ++j) {
			if (i < 10) {
				buffer[i + j * WIDTH] = FB_RGB(200, 1, 1);	
			} else if (i % ((50 + timestep % 200)) <= 10) {
				buffer[i + j * WIDTH] = FB_RGB(0, 255, 0);	
			} else {
				buffer[i + j * WIDTH] = FB_RGB(1, 1, 255);	
			}
		}
	}

	return buffer;
}

int main(int argc, char const *argv[]) {
	
	printf("Hi\n");

	int error = fb_start("Window title");
	if (error) {
		printf("error, exiting\n");
		return 1;
	}
	fb_close();


	return 0;
}