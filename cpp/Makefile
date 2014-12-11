PROGS=$(patsubst %.cpp, %, $(wildcard *.cpp))

all:$(PROGS)

%:%.cpp
	g++ -std=c++11 -g -o $@ $< -pthread

.PHONY:clean
clean:
	@rm -f $(PROGS)
