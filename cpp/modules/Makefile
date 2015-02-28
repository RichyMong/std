all:module

module:$(wildcard *.cpp)
	g++ -std=c++14 -g -o $@ $? -pthread -ljsoncpp

.PHONY:clean
clean:
	@rm -f $(PROGS)
