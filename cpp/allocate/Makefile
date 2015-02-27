all:meng

meng:$(wildcard *.cpp)
	g++ -std=c++14 -O0 -g -o $@ $?

.PHONY:clean
clean:
	@rm -f meng
