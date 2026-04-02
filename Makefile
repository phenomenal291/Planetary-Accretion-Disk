CXX = g++
CXXFLAGS = -std=c++17 -Wall
INCLUDES = -I include
LIBS = -lraylib -lm -lpthread -ldl -lrt -lX11

SRC = $(wildcard src/*.cpp)
HEADERS = $(wildcard include/*.h)

all: main

main: $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o main src/main.cpp $(LIBS)

test: src/test.cpp include/test.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o test src/test.cpp $(LIBS)

clean:
	rm -f test main
