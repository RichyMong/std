include ../Makefile.def

# LDFLAGS += -L../shared -Wl,-rpath=../shared/
LDLIBS += -lpthread

$(eval $(call make-app,server,$(wildcard *.cpp)))
