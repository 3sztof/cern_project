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
ifndef LS_REV
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
ifndef LS_MREV
$(error Please, do not try to run this makefile! Do "cd .. \; make" instead!)
endif
#------------------------------------------------------------------------------
# Do not change the variables here defined. 
# To change installation directories modify ../Makefile instead.
DEV_DIR=$(DEV_MAIN_DIR)/sensors
PKG_DIR=$(PKG_MAIN_DIR)/sensors
INST_DIR=$(INST_MAIN_DIR)/sensors
#------------------------------------------------------------------------------
PKG_SRC_DIR=$(PKG_DIR)/src
PKG_INC_DIR=$(PKG_DIR)/include
PKG_OBJ_DIR=$(PKG_DIR)/obj
#------------------------------------------------------------------------------
DEV_LIB_DIR=$(DEV_MAIN_DIR)/lib
DEV_OBJ_DIR=$(DEV_DIR)/obj
#------------------------------------------------------------------------------
CFLAGS=-Wall -fPIC -O -std=gnu99 -Iinclude -I../utils/include -I../include -I$(DIM_INC) -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux 
LDLIBS=-L$(DEV_LIB_DIR) -lFMCsensors-$(LS_MREV) -lFMCutils-$(LU_MREV) -L$(DIM_LIB) -ldim -lpthread
#------------------------------------------------------------------------------
.PHONY: all clean package install
vpath %.c src
vpath %.o src
LIBRARIES:=$(DEV_LIB_DIR)/libFMCsensors-$(LS_REV).so
OBJS:=$(patsubst src/%.c,$(DEV_OBJ_DIR)/%.o,$(wildcard src/*.c))
#------------------------------------------------------------------------------
#all: $(OBJS) $(LIBRARIES)
#all:
#	$(MAKE) $(OBJS)
#	$(MAKE) $(LIBRARIES)
# dependencies
all: $(LIBRARIES)
$(LIBRARIES): $(OBJS)
#------------------------------------------------------------------------------
$(OBJS): $(DEV_OBJ_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $@
$(LIBRARIES): $(DEV_LIB_DIR)/%.so: $(OBJS)
	gcc -shared -Wl,-soname,libFMCsensors-$(LS_MREV).so -o $@ -lc $^
	cd $(DEV_LIB_DIR) ; if [ -f libFMCsensors-$(LS_MREV).so ] ; then \
        rm libFMCsensors-$(LS_MREV).so ; fi ; ln -sf \
        libFMCsensors-$(LS_REV).so libFMCsensors-$(LS_MREV).so
#------------------------------------------------------------------------------
clean:
	rm -f $(OBJS) $(LIBRARIES)
	rm -f $(DEV_OBJ_DIR)/*.o
	rm -f $(DEV_LIB_DIR)/libFMCsensors-*.so
#------------------------------------------------------------------------------
package:
	if [ ! -d $(PKG_DIR) ] ; then mkdir -p $(PKG_DIR) ; fi
	if [ ! -d $(PKG_SRC_DIR) ] ; then mkdir $(PKG_SRC_DIR) ; fi
	if [ ! -d $(PKG_INC_DIR) ] ; then mkdir $(PKG_INC_DIR) ; fi
	if [ ! -d $(PKG_OBJ_DIR) ] ; then mkdir $(PKG_OBJ_DIR) ; fi
	cp -p makefile $(PKG_DIR)
	cp -p include/*.h $(PKG_INC_DIR)
	cp -p src/*.c $(PKG_SRC_DIR)
	cp -p obj/* $(PKG_OBJ_DIR)
#------------------------------------------------------------------------------
install:
	if [ ! -d $(INST_DIR) ] ; then mkdir -p $(INST_DIR) ; fi
	cp -dpR * $(INST_DIR)
#------------------------------------------------------------------------------
