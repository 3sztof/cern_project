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
DEV_DIR=$(DEV_MAIN_DIR)/logger
PKG_DIR=$(PKG_MAIN_DIR)/logger
INST_DIR=$(INST_MAIN_DIR)/logger
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
#CXXFLAGS=-Wall -Wextra -Weffc++ -g -O2 -I$(DIM_INC) -DMIPSEL -Iinclude -I../include -I../utils/include -DPROTOCOL=1 -Dunix -Dlinux -fPIC
CXXFLAGS=-Wall -g -O2 -I$(DIM_INC) -DMIPSEL -Iinclude -I../include -I../utils/include -DPROTOCOL=1 -Dunix -Dlinux -fPIC
CFLAGS=-Wall -g -O2 -std=gnu99 -I$(DIM_INC) -Iinclude -I../include -I../utils/include -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC
LDLIBS=-L$(DIM_LIB) -ldim -lpthread -L$(DEV_LIB_DIR) -lFMCutils-$(LU_MREV) -lm
#------------------------------------------------------------------------------
.PHONY: all Servers Clients Tests clean package install 
vpath %.c src tests
vpath %.C src ../utils/src
vpath %.IC include ../utils/include
vpath %.H include
vpath %.h include ../include ../utils/include
VER=../include/fmcVersion.h
SERVERS:=$(DEV_SBIN_DIR)/logSrv
CLIENTS:=$(DEV_BIN_DIR)/logViewer $(DEV_BIN_DIR)/logSetProperties $(DEV_BIN_DIR)/logGetProperties $(DEV_BIN_DIR)/mPrint
TESTS:=$(DEV_TESTS_DIR)/sendMsg $(DEV_TESTS_DIR)/sendFryMsg
#------------------------------------------------------------------------------
all: $(SERVERS) $(CLIENTS) $(TESTS)
Servers: $(SERVERS)
Clients: $(CLIENTS)
Tests:   $(TESTS)
#------------------------------------------------------------------------------
$(SERVERS): $(DEV_SBIN_DIR)/%: %.c $(VER)
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@
$(CLIENTS): $(DEV_BIN_DIR)/%: %.C $(VER)
	$(LINK.cc) $(filter-out %.IC %.h %.H, $^) $(LOADLIBES) $(LDLIBS) -o $@
$(TESTS): $(DEV_TESTS_DIR)/%: %.c
	$(LINK.c) $^ $(LOADLIBES) -o $@
# fmcDate.h prerequisite omitted since it is remade at each compilation
$(CLIENTS): severityPatterns.h \
            FmcUtils.IC FmcUtils.C \
            fmcMsgUtils.h fmcCmpUtils.h fmcSrvNameUtils.h fmcDimUtils.h \
            fmcVersion.h fmcGroffMacro.h \
            logSrv.h
$(DEV_BIN_DIR)/logViewer: LogDimErrorHandler.IC LogDimErrorHandler.C \
                          LogUtils.IC LogUtils.C \
                          LogSettingsWatcher.IC LogSettingsWatcher.C \
                          LogSrvSeeker.IC LogSrvSeeker.C \
                          LogWriter.IC LogWriter.C \
                          logSrv.h
#------------------------------------------------------------------------------
clean:
	rm -f src/*.o tests/*.o
	rm -f $(SERVERS) $(CLIENTS) $(TESTS)
#------------------------------------------------------------------------------
package:
	if [ ! -d $(PKG_DIR) ] ; then mkdir -p $(PKG_DIR) ; fi
	if [ ! -d $(PKG_SRC_DIR) ] ; then mkdir $(PKG_SRC_DIR) ; fi
	if [ ! -d $(PKG_INC_DIR) ] ; then mkdir $(PKG_INC_DIR) ; fi
	if [ ! -d $(PKG_TESTS_SRC_DIR) ] ; then mkdir $(PKG_TESTS_SRC_DIR) ; fi
	cp -p makefile $(PKG_DIR)
	cp -p include/*.h include/*.H include/*.IC $(PKG_INC_DIR)
	cp -p src/logSrv.c src/logViewer.C src/logSetProperties.C \
          src/logGetProperties.C src/mPrint.C src/Log*.C $(PKG_SRC_DIR)
	cp -p tests/*.c $(PKG_TESTS_SRC_DIR)
#------------------------------------------------------------------------------
install:
	if [ ! -d $(INST_DIR) ] ; then mkdir -p $(INST_DIR) ; fi
	cp -dpR * $(INST_DIR)
#------------------------------------------------------------------------------
