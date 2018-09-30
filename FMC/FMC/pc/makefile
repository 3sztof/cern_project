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
DEV_DIR=$(DEV_MAIN_DIR)/pc
PKG_DIR=$(PKG_MAIN_DIR)/pc
INST_DIR=$(INST_MAIN_DIR)/pc
#------------------------------------------------------------------------------
PKG_SRC_DIR=$(PKG_DIR)/src
PKG_INC_DIR=$(PKG_DIR)/include
#------------------------------------------------------------------------------
DEV_LIB_DIR=$(DEV_MAIN_DIR)/lib
DEV_BIN_DIR=$(DEV_MAIN_DIR)/bin
DEV_SBIN_DIR=$(DEV_MAIN_DIR)/sbin
#------------------------------------------------------------------------------
CXXFLAGS=-Wall -O -g -I$(DIM_INC) -Iinclude -I../include -I../utils/include -I../include -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC
LDLIBS=-L$(DIM_LIB) -ldim -lpthread -L$(DEV_LIB_DIR) -lFMCutils-$(LU_MREV)
#------------------------------------------------------------------------------
.PHONY: package all clean install
vpath %.o src
vpath %.c src
vpath %.C src ../utils/src
vpath %.h include ../include ../utils/include
vpath %.H include ../include
vpath %.IC include ../utils/include
VER=../include/fmcVersion.h
SERVERS=$(DEV_SBIN_DIR)/pcSrv
CLIENTS=$(DEV_BIN_DIR)/pcAdd $(DEV_BIN_DIR)/pcRm $(DEV_BIN_DIR)/pcLs \
        $(DEV_BIN_DIR)/pcLl $(DEV_BIN_DIR)/pcLss
all: $(SERVERS) $(CLIENTS)
#------------------------------------------------------------------------------
$(SERVERS): $(DEV_SBIN_DIR)/%: %.C $(VER)
	$(LINK.cc) $(filter-out %.IC %.h %.H, $^) $(LOADLIBES) $(LDLIBS) -o $@
$(CLIENTS): $(DEV_BIN_DIR)/%: %.C $(VER)
	$(LINK.cc) $(filter-out %.IC %.h %.H, $^) $(LOADLIBES) $(LDLIBS) -o $@
# fmcDate.h prerequisite omitted since it is remade at each compilation */
$(SERVERS): pcSrv.H \
            PcCluster.IC PcCluster.C PcProcess.IC PcProcess.C \
            PcNode.IC PcNode.C PcAcl.IC PcAcl.C PcAddCmd.IC PcAddCmd.C \
            PcRmCmd.IC PcRmCmd.C PcLsSvc.IC PcLsSvc.C PcLlSvc.IC PcLlSvc.C \
            PcLssSvc.IC PcLssSvc.C PcTmSeeker.IC PcTmSeeker.C \
            PcTmWatcher.IC PcTmWatcher.C \
            PcDimErrorHandler.IC PcDimErrorHandler.C \
            FmcUtils.IC FmcUtils.C \
            fmcDimUtils.h fmcMsgUtils.h fmcSrvNameUtils.h \
            fmcPar.h fmcMacro.h fmcMacro.H fmcVersion.h fmcGroffMacro.h
$(CLIENTS): pcSrv.H \
            FmcUtils.IC FmcUtils.C \
            fmcDimUtils.h \
            fmcPar.h fmcVersion.h fmcGroffMacro.h
#------------------------------------------------------------------------------
clean:
	rm -f src/*.o
	rm -f $(SERVERS) $(CLIENTS)
#------------------------------------------------------------------------------
package:
	if [ ! -d $(PKG_DIR) ] ; then mkdir -p $(PKG_DIR) ; fi
	if [ ! -d $(PKG_SRC_DIR) ] ; then mkdir $(PKG_SRC_DIR) ; fi
	if [ ! -d $(PKG_INC_DIR) ] ; then mkdir $(PKG_INC_DIR) ; fi
	if [ ! -d $(PKG_TESTS_SRC_DIR) ] ; then mkdir $(PKG_TESTS_SRC_DIR) ; fi
	cp -p makefile $(PKG_DIR)
	cp -p include/*.H include/*.IC $(PKG_INC_DIR)
	cp -p src/*.C $(PKG_SRC_DIR)
#------------------------------------------------------------------------------
install:
	if [ ! -d $(INST_DIR) ] ; then mkdir -p $(INST_DIR) ; fi
	cp -dpR * $(INST_DIR)
#------------------------------------------------------------------------------
