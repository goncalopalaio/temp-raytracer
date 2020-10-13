#ifndef GLFW_FB_HEADER
#define GLFW_FB_HEADER

	#define FB_RGB(r, g, b) (((unsigned int)b) << 16) | (((unsigned int)g) << 8) | r

	int fb_start(const char* window_title, int window_width, int window_height);
	int fb_get_texture_width();
	int fb_get_texture_height();
	unsigned int* fb_update(int timestep);
	void fb_close();


#endif

