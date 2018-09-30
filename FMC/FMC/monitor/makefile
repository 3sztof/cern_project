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
ifndef LS_MREV
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
#------------------------------------------------------------------------------
# Do not change the variables here defined. 
# To change installation directories modify ../Makefile instead.
DEV_DIR=$(DEV_MAIN_DIR)/monitor
PKG_DIR=$(PKG_MAIN_DIR)/monitor
INST_DIR=$(INST_MAIN_DIR)/monitor
#------------------------------------------------------------------------------
PKG_SRC_DIR=$(PKG_DIR)/src
PKG_INC_DIR=$(PKG_DIR)/include
PKG_SH_DIR=$(PKG_DIR)/sh
#------------------------------------------------------------------------------
DEV_LIB_DIR=$(DEV_MAIN_DIR)/lib
DEV_BIN_DIR=$(DEV_MAIN_DIR)/bin
DEV_SBIN_DIR=$(DEV_MAIN_DIR)/sbin
#------------------------------------------------------------------------------
CFLAGS=-Wall -O -g -std=gnu99 -Iinclude -I../include -I../utils/include -I$(DIM_INC) -I../sensors/include -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC
CXXFLAGS=-Wall -O -g -Iinclude -I../include -I$(DIM_INC) -I../utils/include -I../sensors/include -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC -lcurses -lmenu -lform
LDLIBS_S=-L$(DIM_LIB) -ldim -lpthread -L$(DEV_LIB_DIR) -lFMCutils-$(LU_MREV) -lFMCsensors-$(LS_MREV)
LDLIBS_C=-L$(DIM_LIB) -ldim -lpthread -L$(DEV_LIB_DIR) -lFMCutils-$(LU_MREV)
#------------------------------------------------------------------------------
.PHONY: all clean package install
vpath %.c src
vpath %.C src ../utils/src
vpath %.o src
vpath %.h include ../sensors/include ../include ../utils/include
vpath %.IC include ../utils/include
vpath %.H ../include
VER=../include/fmcVersion.h
SERVERS=$(patsubst src/%.c,$(DEV_SBIN_DIR)/%,$(wildcard src/*Srv.c))
CLIENTS=$(patsubst src/%.C,$(DEV_BIN_DIR)/%,$(wildcard src/*Viewer*.C) $(wildcard src/*Reset*.C) $(wildcard src/*Monitor*.C) $(wildcard src/*SetProperties*.C))
#------------------------------------------------------------------------------
all: $(SERVERS) $(CLIENTS)
#------------------------------------------------------------------------------
$(SERVERS): $(DEV_SBIN_DIR)/%: %.c $(VER)
	$(LINK.c) $(filter-out %.h, $^) $(LOADLIBES) $(LDLIBS_S) -o $@
$(CLIENTS): $(DEV_BIN_DIR)/%: %.C $(VER)
	$(LINK.cc) $(filter-out %.IC %.h %.H, $^) $(LOADLIBES) $(LDLIBS_C) -o $@
$(SERVERS): fmcMsgUtils.h fmcSrvNameUtils.h fmcDimUtils.h \
            fmcPar.h fmcVersion.h fmcGroffMacro.h
$(CLIENTS): FmcUtils.IC FmcUtils.C \
            fmcMsgUtils.h fmcSrvNameUtils.h fmcDimUtils.h \
            fmcPar.h fmcVersion.h fmcGroffMacro.h
$(DEV_BIN_DIR)/psMonitor: PsMoniDimErrorHandler.IC PsMoniDimErrorHandler.C \
                          PsMoniSrvSeeker.IC PsMoniSrvSeeker.C \
                          PsMoniSvcSeeker.IC PsMoniSvcSeeker.C \
                          PsMoniSvcWatcher.IC PsMoniSvcWatcher.C \
                          fmcMacro.h fmcMacro.H
$(DEV_SBIN_DIR)/osSrv: osSrv.h getOsInfo.h
$(DEV_SBIN_DIR)/cpuinfoSrv: cpuinfoSrv.h getCpuinfoFromProc.h
$(DEV_BIN_DIR)/fmcVersionViewer: fmcCmpUtils.h
$(DEV_BIN_DIR)/osViewer: osSrv.h
$(DEV_BIN_DIR)/cpuinfoViewer: cpuinfoSrv.h
$(DEV_BIN_DIR)/psViewer: psSrv.h
#------------------------------------------------------------------------------
clean:
	rm -f src/*.o
	rm -f $(SERVERS)
	rm -f $(CLIENTS)
#------------------------------------------------------------------------------
package:
	if [ ! -d $(PKG_DIR) ] ; then mkdir -p $(PKG_DIR) ; fi
	if [ ! -d $(PKG_SRC_DIR) ] ; then mkdir $(PKG_SRC_DIR) ; fi
	if [ ! -d $(PKG_INC_DIR) ] ; then mkdir $(PKG_INC_DIR) ; fi
	if [ ! -d $(PKG_SH_DIR) ] ; then mkdir $(PKG_SH_DIR) ; fi
	cp -p makefile $(PKG_DIR)
	cp -p src/*.c $(PKG_SRC_DIR)
	cp -p src/*.C $(PKG_SRC_DIR)
	cp -p include/*.h $(PKG_INC_DIR)
	cp -p include/*.IC $(PKG_INC_DIR)
#------------------------------------------------------------------------------
install:
	if [ ! -d $(INST_DIR) ] ; then mkdir -p $(INST_DIR) ; fi
	cp -dpR * $(INST_DIR)
#------------------------------------------------------------------------------
