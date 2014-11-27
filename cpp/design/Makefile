PROG=$(patsubst %.cpp, %, $(wildcard *.cpp))

all:$(PROG)

%:%.cpp
	g++ -std=c++11 -g -o $@ $< -pthread

.PHONY:clean
clean:
	@rm -f $(PROGS)
