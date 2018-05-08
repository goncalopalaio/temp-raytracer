#include <stdio.h>
#include <GLFW/glfw3.h>
#include "../../gl_crap.h"

//#define FB_ARGB(a, r, g, b) (((unsigned int)a) << 24) | (((unsigned int)r) << 16) | (((unsigned int)g) << 8) | b
#define FB_RGB(r, g, b) (((unsigned int)b) << 16) | (((unsigned int)g) << 8) | r

// callbacks

void fb_close_callback(GLFWwindow* window) {
    printf("close_callback\n");
}

void fb_size_callback(GLFWwindow * window, int width, int height) {
    printf("size_callback\n");
}

void fb_cursorpos_callback(GLFWwindow * window, double mx, double my) {}

void fb_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    printf("key_callback\n");
    glfwSetWindowShouldClose(window, 1);
}

void fb_mousebutton_callback(GLFWwindow * window, int button, int action, int mods) {}

void fb_char_callback(GLFWwindow * window, unsigned int key) {
    printf("char_callback\n");
}

void fb_error_callback(int error, const char* description) {
    printf("%s\n", description);
}

int fb_open() {
	int window_w = 1000;
	int window_h = 300;
	const char* window_name = {"Hello"};
	printf("fb_open\n");

	if (!glfwInit()) {
		printf("error in glfwInit\n");
		return 1;
	}

	printf("fb_open init\n");

	GLFWwindow* window = glfwCreateWindow(window_w, window_h, window_name, NULL, NULL);
	
	if (!window) {
		printf("error creating window\n");		
		return 1;
	}

	printf("fb_open window\n");

	glfwSetWindowCloseCallback(window, fb_close_callback);
    glfwSetWindowSizeCallback(window, fb_size_callback);
    glfwSetKeyCallback(window, fb_key_callback);
    glfwSetMouseButtonCallback(window, fb_mousebutton_callback);
    glfwSetCharCallback(window, fb_char_callback);
    glfwSetCursorPosCallback(window, fb_cursorpos_callback);
    glfwSetErrorCallback(fb_error_callback);

    printf("fb_open context\n");
    glfwMakeContextCurrent(window);

    printf("fb_open drawing\n");

  	// setup matrices
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, window_w, 0, window_h, -1, 1);
    glMatrixMode(GL_MODELVIEW);

	// setup texture
	int tex_w = window_w * 2;
	int tex_h = window_h * 2;
    unsigned int texDat[tex_w * tex_h];

    GLuint tex;
    glGenTextures(1, &tex);
    
    printf("using texture with %d by %d\n", tex_w, tex_h);
    // runtime
    int timestep = 0;


    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// generate texture
		for (int i = 0; i < tex_w; ++i) {
			for (int j = 0; j < tex_h; ++j) {
				if (i < 10) {
					texDat[i + j * tex_w] = FB_RGB(200, 1, 1);	
				} else if (i % ((50 + timestep % 200)) <= 50) {
					texDat[i + j * tex_w] = FB_RGB(0, 255, 0);	
				} else {
					texDat[i + j * tex_w] = FB_RGB(1, 1, 255);	
				}
			}
		}


		// upload texture 
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texDat);
		glBindTexture(GL_TEXTURE_2D, 0);



		int h = window_h;
		int w = window_w;

		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, tex);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(10, 10);
		glTexCoord2i(0, 1); glVertex2i(10, h - 10);
		glTexCoord2i(1, 1); glVertex2i(w - 10, h - 10);
		glTexCoord2i(1, 0); glVertex2i(w - 10, 10);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFlush(); //don't need this with GLUT_DOUBLE and glutSwapBuffers


		++timestep;
		glfwSwapBuffers(window);
        glfwPollEvents();
	}

	glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(window);

	return 0;
}

void fb_update(unsigned int *buffer) {
	printf("fb_update\n");
}

void fb_close() {
	printf("fb_close\n");
}