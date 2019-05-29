src = src
bin = bin
build = build
CFLAGS = -std=c++11 -g -w

objects := $(build)/glad.o $(build)/main.o $(build)/create_window.o\
$(build)/shader.o

target := $(bin)/main

$(target) : $(objects)
	g++ $(objects) -o $@ -lglfw -lm $(CFLAGS)

# glad
$(build)/glad.o : $(src)/glad.c
	g++ $< -o $@ $(CFLAGS) -c

# main
$(build)/main.o : $(src)/main.cpp
	g++ $< -o $@ $(CFLAGS) -c

# create_window
$(build)/create_window.o : $(src)/create_window/CreateWindow.cpp $(src)/create_window/CreateWindow.hpp
	g++ $< -o $@ $(CFLAGS) -c

# shader
$(build)/shader.o : $(src)/shader.cpp $(src)/shader.h
	g++ $< -o $@ $(CFLAGS) -c

clean:
	@rm -rf bin/
	@rm -rf build/
	mkdir build/
	mkdir bin/

run:
	bin/main