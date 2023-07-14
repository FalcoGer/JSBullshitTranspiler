# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC=clang
CXX=clang++
CFLAGS=-std=c++23 -O2 -g -I. -Wall -Wextra -Wpedantic
LDFLAGS=-lfmt

DEPS = transpiler.hpp
OBJ = main.o transpiler.o
TARGET = main

# Compile cpp files with their header dependencies with CXX into object files

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	rm $(TARGET) $(OBJ)
