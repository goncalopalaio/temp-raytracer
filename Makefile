CC=g++
LIBS :=
# 
CFLAGS=-c -Wall
MINIFB_INCLUDE_FILE := "minifb/include/MiniFB.h"
MINIFB_SOURCE_DIR := "minifb/src/macosx/"
MINIFB_SOURCES := $(shell find $(MINIFB_SOURCE_DIR) -name "*.m")
MINIFB_OBJECTS := $(patsubst %.c, %.o, $(MINIFB_SOURCES))

LIBS := -framework Cocoa -L/usr/local/lib


all: raytracer

raytracer: main.o $(MINIFB_OBJECTS)
	$(CC) *.o -o raytracer $(LIBS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

minifb.o: $(MINIFB_SOURCES)
	cp $(MINIFB_INCLUDE_FILE) $(MINIFB_SOURCE_DIR)
	$(CC) $(CFLAGS) $(MINIFB_SOURCES)

clean:
	rm *o raytracer