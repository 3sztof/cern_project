# ---------------------------------------------------------------------------------------
REV=$(shell cat fmcVersion)
REL=$(shell ./make_scripts/get_release.sh)
RH_REL=$(shell ./make_scripts/get_distribution.sh)
OLDREL=$(shell echo $(REL)-1|bc -l)
ARCH:=$(shell uname -m)
CPU_N=$(shell ./make_scripts/cpu_count.sh)
# ---------------------------------------------------------------------------------------
# DIM_INC is the directory in which dim header files can be found.
# DIM_LIB is the directory in which dim library files can be found.
# INST_MAIN_DIR is the directory in which FMC package will be installed.
# To change installation directories modify only the 3 lines below:
ifndef DIM_INC
DIM_INC=/usr/local/include/dim
endif
ifndef DIM_LIB
  ifeq ($(ARCH), x86_64)
DIM_LIB=/usr/local/lib64
  else
DIM_LIB=/usr/local/lib
  endif
endif
ifndef INST_MAIN_DIR
INST_MAIN_DIR=/opt/FMC-$(REV)
endif
# ---------------------------------------------------------------------------------------
# check existence of directories DIM_INC and DIM_LIB
dimIncFound:=$(strip $(wildcard $(DIM_INC)))
ifeq ($(dimIncFound),)
  $(error Directory $(DIM_INC) not found! Please set correctly the \
    environment variable DIM_INC before running make!)
endif
dimIncFound:=$(strip $(wildcard $(DIM_INC)/dis.h))
ifeq ($(dimIncFound),)
  $(error Include file "dis.h" not found in directory DIM_INC="$(DIM_INC)"! \
    Please set correctly the environment variable DIM_INC before running make!)
endif
dimLibFound:=$(strip $(wildcard $(DIM_LIB)))
ifeq ($(dimLibFound),)
  $(error Directory "$(DIM_LIB)" not found! Please set correctly the \
    environment variable DIM_LIB before running make!)
endif
dimLibFound:=$(strip $(wildcard $(DIM_LIB)/libdim.so))
ifeq ($(dimLibFound),)
  $(error Library "libdim.so" not found in directory DIM_LIB="$(DIM_LIB)"! \
    Please set correctly the environment variable DIM_LIB before running make!)
endif
# ---------------------------------------------------------------------------------------
# libFMCutils version
LU_REV=6.0
LU_MREV=6
# libFMCsensors version
LS_REV=8.0
LS_MREV=8
# ---------------------------------------------------------------------------------------
PKG_MAIN_NAME=FMC-$(REV)-$(REL).$(RH_REL).$(ARCH)
PKG_ROOT_DIR=$(HOME)/releases/FMC/$(REV)
PKG_MAIN_DIR=$(PKG_ROOT_DIR)/$(PKG_MAIN_NAME)
TARBALL_DIR=$(HOME)/releases/FMC/$(REV)
TARBALL_NAME=$(PKG_MAIN_NAME).tar.gz
SPECFILE_NAME=$(PKG_MAIN_NAME).spec
RPM_DIR=$(HOME)/rpmbuild
RPM_NAME=$(PKG_MAIN_NAME).rpm
SRPM_NAME=FMC-$(REV)-$(REL).$(RH_REL).src.rpm
RPM_LOG=rpmbuild_$(PKG_MAIN_NAME).log
DEV_MAIN_DIR:=$(shell pwd)
# ---------------------------------------------------------------------------------------
# copy_to_cern
LOC_SRPMS_4:=$(RPM_DIR)/SRPMS/FMC-$(REV)-$(REL).SLC4.src.rpm
LOC_RPMS_4_32:=$(RPM_DIR)/RPMS/i386/FMC-$(REV)-$(REL).SLC4.i386.rpm
LOC_RPMS_4_32_A:=$(RPM_DIR)/RPMS/i386/FMC*-$(REV)-$(REL).SLC4.i386.rpm
LOC_RPMS_4_64:=$(RPM_DIR)/RPMS/x86_64/FMC-$(REV)-$(REL).SLC4.x86_64.rpm
LOC_RPMS_4_64_A:=$(RPM_DIR)/RPMS/x86_64/FMC*-$(REV)-$(REL).SLC4.x86_64.rpm
#
LOC_SRPMS_5=$(RPM_DIR)/SRPMS/FMC-$(REV)-$(REL).SLC5.src.rpm
LOC_RPMS_5_32=$(RPM_DIR)/RPMS/i386/FMC-$(REV)-$(REL).SLC5.i386.rpm
LOC_RPMS_5_32_A=$(RPM_DIR)/RPMS/i386/FMC*-$(REV)-$(REL).SLC5.i386.rpm
LOC_RPMS_5_64=$(RPM_DIR)/RPMS/x86_64/FMC-$(REV)-$(REL).SLC5.x86_64.rpm
LOC_RPMS_5_64_A=$(RPM_DIR)/RPMS/x86_64/FMC*-$(REV)-$(REL).SLC5.x86_64.rpm
#
CERN_RPMS_4_32=galli@lxplus.cern.ch:/afs/cern.ch/lhcb/project/web/online/online-rpm-repo/slc4X/i386/RPMS/
CERN_RPMS_4_64=galli@lxplus.cern.ch:/afs/cern.ch/lhcb/project/web/online/online-rpm-repo/slc4X/x86_64/RPMS/
CERN_SRPMS_4=galli@lxplus.cern.ch:/afs/cern.ch/lhcb/project/web/online/online-rpm-repo/slc4X/SRPMS/
#
CERN_RPMS_5_32=galli@lxplus.cern.ch:/afs/cern.ch/lhcb/project/web/online/online-rpm-repo/slc5X/i386/RPMS/
CERN_RPMS_5_64=galli@lxplus.cern.ch:/afs/cern.ch/lhcb/project/web/online/online-rpm-repo/slc5X/x86_64/RPMS/
CERN_SRPMS_5=galli@lxplus.cern.ch:/afs/cern.ch/lhcb/project/web/online/online-rpm-repo/slc5X/SRPMS/
# ---------------------------------------------------------------------------------------
export CPU_N INST_MAIN_DIR DIM_INC DIM_LIB DEV_MAIN_DIR PKG_MAIN_DIR ARCH LU_REV LS_REV LU_MREV LS_MREV
# ---------------------------------------------------------------------------------------
$(warning CPU N=$(CPU_N))
$(warning Revision=$(REV))
$(warning Release=$(REL))
$(warning RedhatRelease=$(RH_REL))
$(warning Architecture=$(ARCH))
$(warning DIM_INC=$(DIM_INC))
$(warning DIM_LIB=$(DIM_LIB))
$(warning DEV_MAIN_DIR=$(DEV_MAIN_DIR))
$(warning PKG_ROOT_DIR=$(PKG_ROOT_DIR))
$(warning PKG_MAIN_NAME=$(PKG_MAIN_NAME))
$(warning INST_MAIN_DIR=$(INST_MAIN_DIR))
$(warning libFMCutils main revision=$(LU_MREV))
$(warning libFMCutils revision=$(LU_REV))
$(warning libFMCsensors main revision=$(LS_MREV))
$(warning libFMCsensors revision=$(LS_REV))

# ---------------------------------------------------------------------------------------
TOOL_DIR_LS:=make_scripts
LIB_DIR_LS:=utils sensors
APP_DIR_LS:=logger tm pc ipmi cm monitor
DOCS:=SETUP.pdf SETUP.txt CONFIG.pdf CONFIG.txt CHANGELOG.spec LICENSE.GPL AUTHORS 
.PHONY: all man cpman clean tarball package install $(APP_DIR_LS) $(LIB_DIR_LS) $(TOOL_DIR_LS) Pre Post
# ---------------------------------------------------------------------------------------
#  
# This forces to build libs before apps (serialization)
all: Pre $(TOOL_DIR_LS) $(LIB_DIR_LS) $(APP_DIR_LS) Post

$(TOOL_DIR_LS):
	$(MAKE) -C $@

# This allow to gmake libs in parallel with "-j jobs" option
$(LIB_DIR_LS):
	$(MAKE) -C $@

# This allow to gmake apps in parallel with "-j jobs" option
$(APP_DIR_LS):
	$(MAKE) -C $@

Pre:
	./make_scripts/make_fmcVersion_h.sh $(REV)
	./make_scripts/make_fmcDate_h.sh

Post:
	cp -p scripts/test_trunc_proc_PID_environ.sh sbin/
	cp -p scripts/*Viewer bin/
	cp -p scripts/oneliners tests/
	cp -p scripts/test_* tests/
	./make_scripts/make_FILES_versions.sh
# ---------------------------------------------------------------------------------------
clean:
	@for d in $(TOOL_DIR_LS) $(LIB_DIR_LS) $(APP_DIR_LS);\
	do\
	  $(MAKE) -C $$d clean;\
	done
# ---------------------------------------------------------------------------------------
man:
	./make_scripts/make_man.sh
	./make_scripts/make_htmlMan.sh

cpman:
	rsync -avzu htmlMan root@lhcbweb2:/var/www/html/FMC/
	ssh root@lhcbweb2 chown -R apache:apache /var/www/html/FMC/
#	if [ -d /var/www/html ] ; then cp -dpR htmlMan /var/www/html/FMC/ ;\
#	chown -R apache:apache /var/www/html/FMC/ ; fi
#	rm -Rf /opt/FMC/man
#	cp -dpR man /opt/FMC/
# ---------------------------------------------------------------------------------------
package:
	# ..............................................................................
	./make_scripts/make_rpm_spec.sh $(REV) $(REL) $(RH_REL) $(ARCH)
	# ..............................................................................
	if [ -d $(PKG_MAIN_DIR) ] ; then rm -Rf $(PKG_MAIN_DIR) ; fi
	if [ ! -d $(PKG_MAIN_DIR) ] ; then mkdir -p $(PKG_MAIN_DIR) ; fi
	if [ ! -d $(PKG_MAIN_DIR)/include ] ; then mkdir -p $(PKG_MAIN_DIR)/include ; fi
	if [ ! -d $(PKG_MAIN_DIR)/skel ] ; then mkdir -p $(PKG_MAIN_DIR)/skel ; fi
	if [ ! -d $(PKG_MAIN_DIR)/scripts ] ; then mkdir -p $(PKG_MAIN_DIR)/scripts ; fi
	if [ ! -d $(PKG_MAIN_DIR)/make_scripts ] ; \
          then mkdir -p $(PKG_MAIN_DIR)/make_scripts ; fi
	# ..............................................................................
	find . -name etc_\* ! -name \*\,v -exec cp -p '{}' $(PKG_MAIN_DIR)/skel \;
	cp -p Makefile $(PKG_MAIN_DIR)
	cp -p make_scripts/*.sh $(PKG_MAIN_DIR)/make_scripts
	cp -p make_scripts/make_ref* $(PKG_MAIN_DIR)/make_scripts
	cp -p make_scripts/makefile $(PKG_MAIN_DIR)/make_scripts
	cp -p *.spec* $(PKG_MAIN_DIR)
	cp -p scripts/man2html.map $(PKG_MAIN_DIR)/scripts
	cp -p scripts/oneliners $(PKG_MAIN_DIR)/scripts
	cp -p scripts/*.sh $(PKG_MAIN_DIR)/scripts
	cp -p scripts/*.py $(PKG_MAIN_DIR)/scripts
	cp -p scripts/*Viewer $(PKG_MAIN_DIR)/scripts
	cp -p fmcVersion $(PKG_MAIN_DIR)
	cp -p release $(PKG_MAIN_DIR)
	cp -p $(DOCS) $(PKG_MAIN_DIR)
	cp -p FILES.versions $(PKG_MAIN_DIR)
	cp -p include/*.h include/*.H $(PKG_MAIN_DIR)/include
	# ..............................................................................
	cp -dpR bin $(PKG_MAIN_DIR)
	cp -dpR sbin $(PKG_MAIN_DIR)
	cp -dpR lib $(PKG_MAIN_DIR)
	cp -dpR tests $(PKG_MAIN_DIR)
	cp -dpR man $(PKG_MAIN_DIR)
	cp -dpR htmlMan $(PKG_MAIN_DIR)
	# ..............................................................................
	cd utils && $(MAKE) package
	cd sensors && $(MAKE) package
	cd logger && $(MAKE) package
	cd tm && $(MAKE) package
	cd pc && $(MAKE) package
	cd ipmi && $(MAKE) package
	cd cm && $(MAKE) package
	cd monitor && $(MAKE) package
	# ..............................................................................
	@echo "========================================================================="
	@echo "===== Full PKG in dir: \"$(PKG_MAIN_DIR)\"."
	@echo "========================================================================="
# ---------------------------------------------------------------------------------------
tarball: package
	if [ -f $(TARBALL_DIR)/$(TARBALL_NAME) ] ; then \
	  rm -f $(TARBALL_DIR)/$(TARBALL_NAME); \
	fi
	cd $(TARBALL_DIR) && tar -cvzf $(TARBALL_NAME) ./$(PKG_MAIN_NAME)
	@echo "========================================================================="
	@echo "===== Tarball: $(TARBALL_DIR)/$(TARBALL_NAME) created."
	@echo "========================================================================="
# ---------------------------------------------------------------------------------------
install:
	if [ -d $(INST_MAIN_DIR).old ] ; then rm -Rf $(INST_MAIN_DIR).old ; fi
	if [ -d $(INST_MAIN_DIR) ] ; \
	  then mv $(INST_MAIN_DIR) $(INST_MAIN_DIR).old ; \
	fi
	if [ ! -d $(INST_MAIN_DIR) ] ; then mkdir -p $(INST_MAIN_DIR) ; fi
	# ..............................................................................
	cp -p Makefile $(INST_MAIN_DIR)
	cp -p *.spec $(INST_MAIN_DIR)
	cp -p fmcVersion $(INST_MAIN_DIR)
	cp -p release $(INST_MAIN_DIR)
	cp -p $(DOCS) $(INST_MAIN_DIR)
	cp -p FILES.versions $(INST_MAIN_DIR)
	cp -p FMC.spec.in $(INST_MAIN_DIR)
	# ..............................................................................
	cp -dpR include $(INST_MAIN_DIR)
	cp -dpR make_scripts $(INST_MAIN_DIR)
	cp -dpR scripts $(INST_MAIN_DIR)
	cp -dpR skel $(INST_MAIN_DIR)
	cp -dpR man $(INST_MAIN_DIR)
	cp -dpR htmlMan $(INST_MAIN_DIR)
	cp -dpR sbin $(INST_MAIN_DIR)
	cp -dpR bin $(INST_MAIN_DIR)
	cp -dpR tests $(INST_MAIN_DIR)
	cp -dpR lib $(INST_MAIN_DIR)
	# ..............................................................................
	cd utils && $(MAKE) install
	cd sensors && $(MAKE) install
	cd logger && $(MAKE) install
	cd tm && $(MAKE) install
	cd pc && $(MAKE) install
	cd ipmi && $(MAKE) install
	cd cm && $(MAKE) install
	cd monitor && $(MAKE) install
	# ..............................................................................
	cd $(INST_MAIN_DIR)/.. && rm -rf FMC && ln -sf FMC-$(REV) FMC
	# ..............................................................................
# ---------------------------------------------------------------------------------------
rpms: tarball
	@echo "Building RPM file \"$(RPM_DIR)/RPMS/$(RPM_NAME)\"..."
	@echo "Specfile: $(PKG_MAIN_DIR)/$(SPECFILE_NAME)"
	@echo "Logfile: $(DEV_MAIN_DIR)/$(RPM_LOG)"
	# ..............................................................................
	if [ ! -d $(HOME)/rpmbuild/SOURCES ] ; then mkdir -p $(HOME)/rpmbuild/SOURCES ; fi
	if [ ! -d $(HOME)/rpmbuild/BUILD ]   ; then mkdir -p $(HOME)/rpmbuild/BUILD ; fi
	if [ ! -d $(HOME)/rpmbuild/RPMS ]    ; then mkdir -p $(HOME)/rpmbuild/RPMS ; fi
	if [ ! -d $(HOME)/rpmbuild/SPECS ]   ; then mkdir -p $(HOME)/rpmbuild/SPECS ; fi
	if [ ! -d $(HOME)/rpmbuild/SRPMS ]   ; then mkdir -p $(HOME)/rpmbuild/SRPMS ; fi
	# ..............................................................................
	@cd $(RPM_DIR) && cp -p $(TARBALL_DIR)/$(TARBALL_NAME) SOURCES/ && \
	cp -p $(PKG_MAIN_DIR)/$(SPECFILE_NAME) SPECS/ && \
	rpmbuild -ba SPECS/$(SPECFILE_NAME) \
        >$(DEV_MAIN_DIR)/$(RPM_LOG) 2>&1
	@echo "========================================================================="
	@echo "===== RPM  file \"$(RPM_DIR)/RPMS/$(ARCH)/$(RPM_NAME)\" ready."
	@echo "===== SRPM file \"$(RPM_DIR)/SRPMS/$(SRPM_NAME)\" ready."
	@echo "========================================================================="
# ---------------------------------------------------------------------------------------
copy_to_cern:
	@echo "$(LOC_RPMS_4_32)"
	@if [ -f $(LOC_RPMS_4_32) ]; then \
	  scp -p $(LOC_RPMS_4_32_A) $(CERN_RPMS_4_32); \
	fi
	@echo "$(LOC_RPMS_4_64)"
	@if [ -f $(LOC_RPMS_4_64) ]; then \
	  scp -p $(LOC_RPMS_4_64_A) $(CERN_RPMS_4_64); \
	fi
	@echo "$(LOC_SRPMS_4)"
	@if [ -f $(LOC_SRPMS_4) ]; then \
	  scp -p $(LOC_SRPMS_4) $(CERN_SRPMS_4); \
	fi
	@echo "$(LOC_RPMS_5_32)"
	@if [ -f $(LOC_RPMS_5_32) ]; then \
	  scp -p $(LOC_RPMS_5_32_A) $(CERN_RPMS_5_32); \
	fi
	@echo "$(LOC_RPMS_5_64)"
	@if [ -f $(LOC_RPMS_5_64) ]; then \
	  scp -p $(LOC_RPMS_5_64_A) $(CERN_RPMS_5_64); \
	fi
	@echo "$(LOC_SRPMS_5)"
	@if [ -f $(LOC_SRPMS_5) ]; then \
	  scp -p $(LOC_SRPMS_5) $(CERN_SRPMS_5); \
	fi
# ---------------------------------------------------------------------------------------
