PROGS=$(patsubst %.cpp, %, $(wildcard *.cpp))

all:$(PROGS)

%:%.cpp
	g++ -std=c++14 -g -o $@ $? -lleveldb

.PHONY:clean
clean:
	@rm -f $(PROGS)
