PRGNAME = libtnt
OUTFILE = $(PRGNAME).a
LIBFILE = $(OUTFILE)

#这个里面定义所有的变量和定义,你自己看吧
include ../Makefile.macros

# ALL_CPP_FILES += $(wildcard *.cpp )
IGNORE_DIRS := conf test unit_test detail

ALL_DIRS = $(shell ls -l --time-style=long-iso | grep ^d | awk '{print $$8}')
SUB_DIRS := $(filter-out ${IGNORE_DIRS}, $(ALL_DIRS))

ALL_CPP_FILES += $(wildcard *.cpp )
ALL_CPP_FILES += $(foreach sub_dir, ${SUB_DIRS}, $(wildcard ${sub_dir}/*.cpp))

CPP_FILE := ${ALL_CPP_FILES}

INC_ALL += -I./


#如果你想改变Mafile的规则，请在这个地方加入你自己的定义.

#这个定义其他所有的规则,还是你自己看吧
include ../Makefile.rule

