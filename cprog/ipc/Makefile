PROGS=$(patsubst %.c, %, $(wildcard *.c))

all:$(PROGS)

%:%.c
	gcc -O0 -g -o $@ $< -pthread -fdiagnostics-color=auto

.PHONY:clean
clean:
	@rm -f $(PROGS)
