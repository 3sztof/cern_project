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
DEV_DIR=$(DEV_MAIN_DIR)/cm
PKG_DIR=$(PKG_MAIN_DIR)/cm
INST_DIR=$(INST_MAIN_DIR)/cm
#------------------------------------------------------------------------------
PKG_SRC_DIR=$(PKG_DIR)/src
PKG_INC_DIR=$(PKG_DIR)/include
#------------------------------------------------------------------------------
DEV_LIB_DIR=$(DEV_MAIN_DIR)/lib
DEV_BIN_DIR=$(DEV_MAIN_DIR)/bin
DEV_SBIN_DIR=$(DEV_MAIN_DIR)/sbin
#------------------------------------------------------------------------------
CFLAGS=-Wall -O -std=gnu99 -Iinclude -I../include -I../utils/include -I$(DIM_INC) -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC
CXXFLAGS=-Wall -O -Iinclude -I../include -I../utils/include -I$(DIM_INC) -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC
LDLIBS=-L$(DIM_LIB) -ldim -lpthread -L$(DEV_LIB_DIR) -lFMCutils-$(LU_MREV)
#------------------------------------------------------------------------------
.PHONY: package all clean install
vpath %.c src
vpath %.o src 
vpath %.C src ../utils/src
vpath %.h include ../include ../utils/include
vpath %.H include ../include
vpath %.IC include ../utils/include
VER=../include/fmcVersion.h
SERVERS:=$(DEV_SBIN_DIR)/cmSrv
CLIENTS:=$(DEV_BIN_DIR)/cmGet $(DEV_BIN_DIR)/cmSet $(DEV_BIN_DIR)/cmUnset
all: $(SERVERS) $(CLIENTS)
#------------------------------------------------------------------------------
$(SERVERS): $(DEV_SBIN_DIR)/%: %.C $(VER)
	$(LINK.cc) $(filter-out %.IC %.h %.H, $^) $(LOADLIBES) $(LDLIBS) -o $@
$(CLIENTS): $(DEV_BIN_DIR)/%: %.C $(VER)
	$(LINK.cc) $(filter-out %.IC %.h %.H, $^) $(LOADLIBES) $(LDLIBS) -o $@
# fmcDate.h prerequisite omitted since it is remade at each compilation
$(SERVERS): cmSrv.H \
            CmSrvDimErrorHandler.IC CmSrvDimErrorHandler.C \
            CmSrvGetFileRpc.IC CmSrvGetFileRpc.C \
            CmSrvSetFileRpc.IC CmSrvSetFileRpc.C \
            CmSrvUnsetFileRpc.IC CmSrvUnsetFileRpc.C \
            CmAcl.IC CmAcl.C \
            FmcUtils.IC FmcUtils.C \
            fmcDimUtils.h fmcMsgUtils.h fmcSrvNameUtils.h \
            fmcPar.h fmcMacro.h fmcMacro.H fmcVersion.h fmcGroffMacro.h
$(CLIENTS): cmSrv.H \
            CmAcl.IC CmAcl.C \
            FmcUtils.IC FmcUtils.C \
            fmcDimUtils.h \
            fmcPar.h fmcVersion.h fmcGroffMacro.h 
#------------------------------------------------------------------------------
clean:
	rm -f src/*.o
	rm -f $(SERVERS) $(CLIENTS)
#------------------------------------------------------------------------------
package:
	if [ -d $(PKG_DIR) ] ; then rm -Rf $(PKG_DIR) ; fi
	if [ ! -d $(PKG_DIR) ] ; then mkdir -p $(PKG_DIR) ; fi
	if [ ! -d $(PKG_SRC_DIR) ] ; then mkdir $(PKG_SRC_DIR) ; fi
	if [ ! -d $(PKG_INC_DIR) ] ; then mkdir $(PKG_INC_DIR) ; fi
	cp -p include/*.H include/*.IC $(PKG_INC_DIR)
	cp -p makefile $(PKG_DIR)
	cp -p src/*.C $(PKG_SRC_DIR)
#------------------------------------------------------------------------------
install:
	if [ ! -d $(INST_DIR) ] ; then mkdir -p $(INST_DIR) ; fi
	cp -dpR * $(INST_DIR)
#------------------------------------------------------------------------------
