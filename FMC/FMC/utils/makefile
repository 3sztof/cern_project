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
ifndef LU_REV
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
ifndef LU_MREV
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
#------------------------------------------------------------------------------
# Do not change the variables here defined. 
# To change installation directories modify ../Makefile instead.
DEV_DIR=$(DEV_MAIN_DIR)/utils
PKG_DIR=$(PKG_MAIN_DIR)/utils
INST_DIR=$(INST_MAIN_DIR)/utils
#------------------------------------------------------------------------------
PKG_SRC_DIR=$(PKG_DIR)/src
PKG_INC_DIR=$(PKG_DIR)/include
PKG_MAN_DIR=$(PKG_DIR)/man
PKG_TESTS_SRC_DIR=$(PKG_DIR)/tests
PKG_OBJ_DIR=$(PKG_DIR)/obj
#------------------------------------------------------------------------------
DEV_OBJ_DIR=$(DEV_DIR)/obj
DEV_LIB_DIR=$(DEV_MAIN_DIR)/lib
DEV_TESTS_DIR=$(DEV_MAIN_DIR)/tests
#------------------------------------------------------------------------------
CFLAGS=-Wall -O2 -std=gnu99 -Iinclude -I../include -I$(DIM_INC) -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC
LDLIBS=-L$(DIM_LIB) -ldim -L$(DEV_LIB_DIR) -lFMCutils-$(LU_MREV) -lpthread
#------------------------------------------------------------------------------
.PHONY: all clean package install
vpath %.c src tests
vpath %.o $(DEV_OBJ_DIR) tests
vpath %.h include
VER=../include/fmcVersion.h
OBJS:=$(patsubst src/%.c,$(DEV_OBJ_DIR)/%.o,$(wildcard src/*Utils.c))
LIBRARIES:=$(DEV_LIB_DIR)/libFMCutils-$(LU_REV).so
TESTS:=$(patsubst tests/%.c,$(DEV_TESTS_DIR)/%,$(wildcard tests/*.c))
#------------------------------------------------------------------------------
# dependencies
all: $(TESTS)
$(TESTS): $(LIBRARIES)
$(LIBRARIES): $(OBJS)
$(DEV_OBJ_DIR):
	mkdir -p $(DEV_OBJ_DIR)
#------------------------------------------------------------------------------
$(OBJS): $(DEV_OBJ_DIR)/%.o: %.c %.h $(VER) $(DEV_OBJ_DIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<
$(LIBRARIES): $(DEV_LIB_DIR)/%.so: $(OBJS)
	gcc -shared -Wl,-soname,libFMCutils-$(LU_MREV).so -o $@ -lc $^
	cd $(DEV_LIB_DIR) ; if [ -f libFMCutils-$(LU_MREV).so ] ; then \
        rm libFMCutils-$(LU_MREV).so ; fi ; ln -sf libFMCutils-$(LU_REV).so \
        libFMCutils-$(LU_MREV).so
#------------------------------------------------------------------------------
$(TESTS): $(DEV_TESTS_DIR)/%: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@
#------------------------------------------------------------------------------
clean:
	rm -f $(DEV_OBJ_DIR)/*.o tests/*.o
	rm -f $(LIBRARIES) $(TESTS)
	rm -f $(DEV_LIB_DIR)/libFMCutils-*.so
#------------------------------------------------------------------------------
package:
	if [ ! -d $(PKG_DIR) ] ; then mkdir -p $(PKG_DIR) ; fi
	if [ ! -d $(PKG_INC_DIR) ] ; then mkdir $(PKG_INC_DIR) ; fi
	if [ ! -d $(PKG_SRC_DIR) ] ; then mkdir $(PKG_SRC_DIR) ; fi
	if [ ! -d $(PKG_MAN_DIR) ] ; then mkdir $(PKG_MAN_DIR) ; fi
	if [ ! -d $(PKG_OBJ_DIR) ] ; then mkdir $(PKG_OBJ_DIR) ; fi
	if [ ! -d $(PKG_TESTS_SRC_DIR) ] ; then mkdir $(PKG_TESTS_SRC_DIR) ; fi
	cp -p makefile $(PKG_DIR)
	cp -p src/*.c $(PKG_SRC_DIR)
	cp -p src/*.C $(PKG_SRC_DIR)
	cp -p src/*.cpp $(PKG_SRC_DIR)
	cp -p include/*.h $(PKG_INC_DIR)
	cp -p include/*.IC $(PKG_INC_DIR)
	cp -p man/*.1 man/*.3 $(PKG_MAN_DIR)
	cp -p tests/*.c $(PKG_TESTS_SRC_DIR)
	cp -p tests/oneliners $(PKG_TESTS_SRC_DIR)
	cp -p obj/* $(PKG_OBJ_DIR)
#------------------------------------------------------------------------------
install:
	if [ ! -d $(INST_DIR) ] ; then mkdir -p $(INST_DIR) ; fi
	cp -dpR * $(INST_DIR)
#------------------------------------------------------------------------------
