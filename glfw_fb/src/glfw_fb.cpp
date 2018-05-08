#include <stdio.h>
#include <GLFW/glfw3.h>
#include "../../gl_crap.h"

// callbacks

void fb_close_callback(GLFWwindow* window) {
    printf("close_callback");
}

void fb_size_callback(GLFWwindow * window, int width, int height) {
    printf("size_callback");
}

void fb_cursorpos_callback(GLFWwindow * window, double mx, double my) {}

void fb_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    printf("key_callback");
    glfwSetWindowShouldClose(window, 1);
}

void fb_mousebutton_callback(GLFWwindow * window, int button, int action, int mods) {}

void fb_char_callback(GLFWwindow * window, unsigned int key) {
    printf("char_callback");
}

void fb_error_callback(int error, const char* description) {
    printf("%s\n", description);
}

// opengl setup

GLuint build_shader() {

    char str_points_vert_shader[] =
        "attribute vec3 position;"
        "attribute vec3 color;"
        "varying vec3 v_color;"
        "uniform mat4 u_view_matrix;"
        "uniform mat4 u_projection_matrix;"
        "void main(){"
        "v_color = color;"
        "gl_Position =  u_projection_matrix * u_view_matrix * vec4(position, 1.0);"
        "}";

    char str_points_frag_shader[] =
        "varying vec3 v_color;"
        "void main() {"
        "gl_FragColor = vec4(v_color, 1.0);"
        "}";

    return compile_shader_program(str_points_vert_shader,
    													  str_points_frag_shader,
    													  "position", "color");
}

int fb_open() {
	int window_w = 600;
	int window_h = 600;
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

    printf("fb_open shader\n");
    GLuint program = build_shader();

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

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