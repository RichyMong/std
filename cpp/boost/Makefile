PROGS=$(patsubst %.cpp, %, $(wildcard *.cpp))

all:$(PROGS)

%:%.cpp
	g++ -DBOOST_LOG_DYN_LINK -std=c++14 -O0 -g -o $@ $< -pthread -lboost_system -lboost_log -lboost_thread

.PHONY:clean
clean:
	@rm -f $(PROGS)
