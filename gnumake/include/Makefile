.PHONY : all
all:

include Makefile.def

include module1/Makefile
include module2/Makefile

all : $(all_apps)

.PHONY : clean
clean: $(cleans)
	@rm -fr $(OUT_DIR)
