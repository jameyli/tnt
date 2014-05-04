PRGNAME = tntlib_unit_tests
OUTFILE = $(PRGNAME)
EXEFILE = $(OUTFILE)

#这个里面定义所有的变量和定义,你自己看吧
include ../../Makefile.macros

all_sources := $(shell find ../ \( -path "../map_old" -o -path "../tools" \) -prune -o -name "*.cpp" -print)
ignore_sources := %/main.cpp
CPP_FILE := $(filter-out $(ignore_sources), $(all_sources))


INC_ALL += -I./ \
           -I../\
		   ${INC_GTEST} \
		   ${INC_GMOCK}

LIB_OWN = ../libtnt.a \
		  ${LIB_GTEST} \
		  ${LIB_GMOCK}


#如果你想改变Mafile的规则，请在这个地方加入你自己的定义.

#这个定义其他所有的规则,还是你自己看吧
include ../../Makefile.rule

