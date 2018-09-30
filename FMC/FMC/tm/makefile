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
DEV_DIR=$(DEV_MAIN_DIR)/tm
PKG_DIR=$(PKG_MAIN_DIR)/tm
INST_DIR=$(INST_MAIN_DIR)/tm
#------------------------------------------------------------------------------
PKG_SRC_DIR=$(PKG_DIR)/src
PKG_INC_DIR=$(PKG_DIR)/include
PKG_TESTS_SRC_DIR=$(PKG_DIR)/tests
#------------------------------------------------------------------------------
DEV_LIB_DIR=$(DEV_MAIN_DIR)/lib
DEV_BIN_DIR=$(DEV_MAIN_DIR)/bin
DEV_SBIN_DIR=$(DEV_MAIN_DIR)/sbin
DEV_TESTS_DIR=$(DEV_MAIN_DIR)/tests
#------------------------------------------------------------------------------
CFLAGS=-Wall -O2 -g -std=gnu99 -Iinclude -I../include -I../utils/include -I$(DIM_INC) -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC
CXXFLAGS=-Wall -O  -Iinclude -I../include -I../utils/include -I$(DIM_INC) -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC
LDLIBS=-L$(DIM_LIB) -ldim -lpthread -L$(DEV_LIB_DIR) -lFMCutils-$(LU_MREV)
#------------------------------------------------------------------------------
.PHONY: all Servers Clients Tests clean package install
vpath %.c src tests
vpath %.o src tests
vpath %.C src tests ../utils/src
vpath %.h include ../sensors/include ../include ../utils/include
vpath %.IC include ../utils/include
vpath %.H ../include
VER=../include/fmcVersion.h
SERVERS:=$(DEV_SBIN_DIR)/tmSrv
CLIENTS:=$(patsubst src/%.C,$(DEV_BIN_DIR)/%,$(wildcard src/*.C))
TESTS_C:=$(patsubst tests/%.c,$(DEV_TESTS_DIR)/%,$(wildcard tests/*.c))
TESTS_CC:=$(patsubst tests/%.C,$(DEV_TESTS_DIR)/%,$(wildcard tests/*.C))
#------------------------------------------------------------------------------
all: $(SERVERS) $(CLIENTS) $(TESTS_C) $(TESTS_CC)
Servers: $(SERVERS)
Clients: $(CLIENTS)
Tests: $(TESTS_C) $(TESTS_CC)
#------------------------------------------------------------------------------
$(SERVERS): $(DEV_SBIN_DIR)/%: %.c tmUtils.c tmSrv.h tmUtils.h $(VER)
	$(LINK.c) $< src/tmUtils.c $(LOADLIBES) $(LDLIBS) -o $@
#$(CLIENTS): $(DEV_BIN_DIR)/%: %.C $(VER)
$(CLIENTS): $(DEV_BIN_DIR)/%: %.C ../utils/src/FmcUtils.C $(VER)
	$(LINK.cc) $(filter-out %.IC %.h %.H, $^) $(LOADLIBES) $(LDLIBS) -o $@
#$(TESTS_C): $(DEV_TESTS_DIR)/%: %.c
#	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -lrt -lm -o $@
$(TESTS_C): $(DEV_TESTS_DIR)/%: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@
$(TESTS_CC): $(DEV_TESTS_DIR)/%: %.C
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@
$(CLIENTS)/tmLl: FmcUtils.IC FmcUtils.C \
	fmcMsgUtils.h fmcSrvNameUtils.h fmcDimUtils.h \
	fmcPar.h fmcVersion.h fmcGroffMacro.h
$(CLIENTS)/tmLs: FmcUtils.IC FmcUtils.C \
	fmcMsgUtils.h fmcSrvNameUtils.h fmcDimUtils.h \
	fmcPar.h fmcVersion.h fmcGroffMacro.h
#------------------------------------------------------------------------------
clean:
	rm -f src/*.o tests/*.o
	rm -f $(SERVERS) $(CLIENTS) $(TESTS_C) $(TESTS_CC)
#------------------------------------------------------------------------------
package:
	if [ ! -d $(PKG_DIR) ] ; then mkdir -p $(PKG_DIR) ; fi
	if [ ! -d $(PKG_SRC_DIR) ] ; then mkdir $(PKG_SRC_DIR) ; fi
	if [ ! -d $(PKG_INC_DIR) ] ; then mkdir $(PKG_INC_DIR) ; fi
	if [ ! -d $(PKG_TESTS_SRC_DIR) ] ; then mkdir $(PKG_TESTS_SRC_DIR) ; fi
	cp -p makefile $(PKG_DIR)
	cp -p include/*.h $(PKG_INC_DIR)
	cp -p src/*.c $(PKG_SRC_DIR)
	cp -p src/*.C $(PKG_SRC_DIR)
	cp -p tests/*.c $(PKG_TESTS_SRC_DIR)
	cp -p tests/*.C $(PKG_TESTS_SRC_DIR)
#------------------------------------------------------------------------------
install:
	if [ ! -d $(INST_DIR) ] ; then mkdir -p $(INST_DIR) ; fi
	cp -dpR * $(INST_DIR)
#------------------------------------------------------------------------------
