PRGNAME = libtnt
OUTFILE = $(PRGNAME).a
LIBFILE = $(OUTFILE)

IGNORE_DIRS := conf test unit_test detail

ALL_DIRS = $(shell ls -l | grep ^d | awk '{print $$9}')
SUB_DIRS := $(filter-out ${IGNORE_DIRS}, $(ALL_DIRS))
ALL_CPP_FILES += $(wildcard *.cpp )
ALL_CPP_FILES += $(foreach sub_dir, ${SUB_DIRS}, $(wildcard ${sub_dir}/*.cpp))
CPP_FILE := ${ALL_CPP_FILES}

O_C_FILE   = $(patsubst %.c, %.o, $(C_FLIE))
O_CPP_FILE = $(patsubst %.cpp, %.o, $(CPP_FILE))
O_FILE  = $(O_C_FILE) $(O_CPP_FILE) ${O_PROTO_CC_FILE}

LDFLAGS := -lz -lbz2 -lpthread -ldl
CPPFLAGS := -Werror -Wall -MMD -ggdb -pipe -D_GNU_SOURCE

.PHONY: clean

$(LIBFILE): $(O_FILE)
	$(AR) $(ARFLAGS) $(LIBFILE) $(O_FILE)

%.o : %.c
	$(COMPILE.cc) $< -o $@ ;
	@echo " $(COMPILE.cc) $< -o $@" >> $(patsubst %.o,%.d,$@)

%.o : %.cpp
	$(COMPILE.cc) $< -o $@ ;
	@echo " $(COMPILE.cc) $< -o $@" >> $(patsubst %.o,%.d,$@)

clean:
	rm -f $(OUTFILE) $(O_FILE) *.d

