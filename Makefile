# Flags
CFLAGS = -pedantic -O2
LIBS :=

ifeq ($(OS),Windows_NT)
BIN := $(BIN).exe
	LIBS := -lglfw3 -lopengl32 -lm -lGLU32 -lGLEW32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LIBS := -lglfw3 -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm -lGLEW -L/usr/local/lib
		CFLAGS += -I/usr/local/include
	else
		LIBS := -lglfw -lGL -lm -lGLU -lGLEW
	endif
endif

all: generate main

generate: clean
ifeq ($(OS),Windows_NT)
	@mkdir bin 2> nul || exit 0
else
	@mkdir -p bin	
endif

clean:
	@rm -rf bin

main: generate
	$(CC) $(CFLAGS) -o bin/main toy.c $(LIBS)