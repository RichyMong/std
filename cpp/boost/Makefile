PROGS=$(patsubst %.cpp, %, $(wildcard *.cpp))

all:$(PROGS)

%:%.cpp
	g++ -std=c++14 -O0 -g -o $@ $< -pthread -lboost_system

.PHONY:clean
clean:
	@rm -f $(PROGS)
