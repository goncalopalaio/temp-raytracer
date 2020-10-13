CC=g++
LIBS :=
# 
CFLAGS=-c -Wall -I/usr/local/include
GLFW_FB_INCLUDE_FILE := "glfw_fb/glfw_fb.h"
GLFW_FB_SOURCE_DIR := "glfw_fb/src"
GLFW_FB_SOURCES := $(shell find $(GLFW_FB_SOURCE_DIR) -name "*.cpp")
GLFW_FB_OBJECTS := $(patsubst %.c, %.o, $(GLFW_FB_SOURCES))

LIBS := -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm -lGLEW -L/usr/local/lib


all: raytracer

raytracer: main.o glfwfb.o $(GLFW_FB_OBJECTS)
	$(CC) *.o -o raytracer $(LIBS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

glfwfb.o: $(GLFW_FB_SOURCES) 
	echo "building glfw_fb"
	$(CC) $(CFLAGS) $(GLFW_FB_SOURCES) $(LIBS)

clean:
	rm *o raytracer