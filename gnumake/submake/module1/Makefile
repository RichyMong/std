include ../Makefile.def

LDFLAGS += -L../shared -Wl,-rpath=../shared/
LDLIBS += -lmodule

$(eval $(call make-app,module1,$(wildcard *.c)))
