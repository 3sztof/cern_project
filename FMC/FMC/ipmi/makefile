#------------------------------------------------------------------------------
# This makefile must be run by ../Makefile, which sets DIM_INC, DIM_INC,
# INST_MAIN_DIR PKG_MAIN_DIR DEV_MAIN_DIR and ARCH
ifndef DIM_INC
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
ifndef DIM_LIB
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
ifndef INST_MAIN_DIR
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
ifndef PKG_MAIN_DIR
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
ifndef DEV_MAIN_DIR
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
ifndef ARCH
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
ifndef LU_MREV
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
#------------------------------------------------------------------------------
# Do not change the variables here defined. 
# To change installation directories modify ../Makefile instead.
DEV_DIR=$(DEV_MAIN_DIR)/ipmi
PKG_DIR=$(PKG_MAIN_DIR)/ipmi
INST_DIR=$(INST_MAIN_DIR)/ipmi
#------------------------------------------------------------------------------
PKG_SRC_DIR=$(PKG_DIR)/src
PKG_INC_DIR=$(PKG_DIR)/include
#------------------------------------------------------------------------------
DEV_LIB_DIR=$(DEV_MAIN_DIR)/lib
DEV_BIN_DIR=$(DEV_MAIN_DIR)/bin
DEV_SBIN_DIR=$(DEV_MAIN_DIR)/sbin
#------------------------------------------------------------------------------
CFLAGS=-Wall -g -O -std=gnu99 -Iinclude -I../include -I../utils/include -I$(DIM_INC) -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC
CXXFLAGS=-Wall -O  -I$(DIM_INC) -Iinclude -I../include -I../utils/include -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC
LDLIBS=-L$(DIM_LIB) -ldim -lpthread -L$(DEV_LIB_DIR) -lFMCutils-$(LU_MREV)
#------------------------------------------------------------------------------
.PHONY: package all clean install
vpath %.c src
vpath %.o src 
vpath %.C src ../utils/src
vpath %.h include ../include ../utils/include
vpath %.IC include ../utils/include
VER=../include/fmcVersion.h
SERVERS=$(patsubst src/%.c,$(DEV_SBIN_DIR)/%,$(wildcard src/*Srv.c))
CLIENTS=$(patsubst src/%.C,$(DEV_BIN_DIR)/%,$(wildcard src/*.C))
all: $(SERVERS) $(CLIENTS)
#------------------------------------------------------------------------------
$(SERVERS): $(DEV_SBIN_DIR)/%: %.c
	$(LINK.c) $(filter-out %.h, $^) $(LOADLIBES) $(LDLIBS) -o $@
# fmcDate.h prerequisite omitted since it is remade at each compilation
$(CLIENTS): $(DEV_BIN_DIR)/%: %.C
	$(LINK.cc) $(filter-out %.IC %.h %.H, $^) $(LOADLIBES) $(LDLIBS) -o $@
$(SERVERS): ipmiSrv.h \
            ipmiUtils.c ipmiUtils.h \
            fmcSrvNameUtils.h fmcMsgUtils.h fmcThreadUtils.h \
            fmcMacro.h fmcPar.h fmcVersion.h fmcGroffMacro.h
$(CLIENTS): ipmiSrv.h \
            ipmiUtils.h \
            FmcUtils.IC FmcUtils.C \
            fmcSrvNameUtils.h fmcMsgUtils.h fmcThreadUtils.h \
            fmcMacro.h fmcPar.h fmcVersion.h fmcGroffMacro.h
#------------------------------------------------------------------------------
clean:
	rm -f src/*.o
	rm -f $(SERVERS) $(CLIENTS)
#------------------------------------------------------------------------------
package:
	if [ ! -d $(PKG_DIR) ] ; then mkdir -p $(PKG_DIR) ; fi
	if [ ! -d $(PKG_SRC_DIR) ] ; then mkdir $(PKG_SRC_DIR) ; fi
	if [ ! -d $(PKG_INC_DIR) ] ; then mkdir $(PKG_INC_DIR) ; fi
	cp -p makefile $(PKG_DIR)
	cp -p include/*.h $(PKG_INC_DIR)
	cp -p src/*.c src/*.C $(PKG_SRC_DIR)
#------------------------------------------------------------------------------
install:
	if [ ! -d $(INST_DIR) ] ; then mkdir -p $(INST_DIR) ; fi
	cp -dpR * $(INST_DIR)
#------------------------------------------------------------------------------
