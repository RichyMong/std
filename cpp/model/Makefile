# make the dependent directories first
subdirs = shared module1 module2

.PHONY : all
all : $(subdirs)

cleans =

define make-sub

.PHONY:$1
$1:
	@cd $1 && $(MAKE)

$(eval clean_sub := $(join clean_, $(1)))
.PHONY : $(clean_sub)
$(clean_sub) :
	@cd $1 && $(MAKE) clean

cleans += $(clean_sub)
endef

$(foreach dir, $(subdirs), $(eval $(call make-sub, $(dir))))

.PHONY : clean
clean : $(cleans)
