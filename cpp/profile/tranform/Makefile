PROGS=$(patsubst %.cpp, %, $(wildcard *.cpp))

all:$(PROGS)

%:%.cpp
	g++ -fdiagnostics-color=always -std=c++14 -O2 -g -o $@ $^ -pthread

.PHONY:clean
clean:
	@rm -f $(PROGS)
