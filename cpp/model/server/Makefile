CXXFLAGS=-std=c++0x -g -O0 -Wall

SRC_FILES=$(wildcard *.cpp)

APP=server
all: $(APP)

$(APP):$(SRC_FILES)
	g++ $(CXXFLAGS) -o $@ $^ -lpthread -lcrypto

clean:
	@rm -vf $(APP)
	@rm -vf *.o
