CXXFLAGS=-std=c++0x -g -O0 -Wall

SRC_FILES=$(wildcard *.cpp)

all: release

release:$(SRC_FILES)
	g++ $(CXXFLAGS) -o $@ $^ -lpthread -lcrypto

clean:
	@rm -vf release
	@rm -vf *.o
