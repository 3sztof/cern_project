Summary: Farm Monitoring and Control System for the LHCb on-line farm 
Name: FMC
Version: 3.9.14
Release: 1.el4
License: GPL
Group: LHCb/Online
URL: http://lhcbweb2.bo.infn.it/bin/view/LHCbBologna/FmcLinux
Source0: FMC-3.9.14-1.el4.x86_64.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildArch: x86_64
Prefix: /opt
Packager: Domenico Galli
Distribution: el4
# *****************************************************************************
# $Id: FMC.spec.in,v 1.8 2012/12/10 15:25:33 galli Exp $
# *****************************************************************************
%description
The aim of the Farm Monitoring and Control System (FMC) is to monitor and control each PC and to supervise the overall status of the on-line farm of the LHCb experiment at CERN.

The FMC System is composed of the following tools: (1) The Message Logger collects messages sent by the applications running on the farm nodes. (2) The Power Manager can switch off and on the farm nodes; moreover it monitors the physical parameters (temperatures, fan speeds, power supply voltages, etc.). (3) The Task Manager can start and stop processes on the farm nodes from a central console;  moreover it provides a real-time notification upon process terminations. (4) The Process Controller is in charge of keeping a list of applications running on the farm nodes, by interacting with the Task Manager and re-spawning the terminated processes if requested. (5) The Monitor System supervises the farm node operation (CPU and the memory load, network interface and TCP/IP stack parameters, rates of the interrupts raised by the network interface card and detailed status of the running processes).

The FMC is based on DIM (Distributed Information Management System) as network communication layer, it is accessible both through a command line interface and through the PVSS (Prozessvisualisierungs und Steuerungssystem) graphical interface, and it is interfaced to the Finite State Machine (FSM) of the LHCb Experiment Control System (ECS) in order to manage anomalous farm conditions.

The FMC is an integral part of the ECS, which is in charge of monitoring and controlling all on-line components; it uses the same tools (DIM, PVSS, FSM, etc.) to guarantee its complete integration and a coherent look and feel throughout the whole control system.
# *****************************************************************************
%prep
%setup -q -n %{name}-%{version}-%{release}.%{buildarch}
# *****************************************************************************
%build
make clean
make
# *****************************************************************************
%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/opt/FMC
make INST_MAIN_DIR=$RPM_BUILD_ROOT/opt/FMC-%{version} install
# *****************************************************************************
%clean
rm -rf $RPM_BUILD_ROOT
# *****************************************************************************
%files
%defattr(-,root,root,-)
/opt/FMC-%{version}
/opt/FMC
%doc
# *****************************************************************************
# REMEMBER:
# Install:
#   Run %pre of new package ($1=1)
#   Install new files
#   Run %post of new package ($1=1)
# Delete:
#   Run %preun of old package ($1=0)
#   Delete files
#   Run %postun of old package ($1=0)
# Upgrade
#   Run %pre of new package ($1=2)
#   Install new files
#   Run %post of new package ($1=2)
#   Run %preun of old package ($1=1)
#   Delete any old files not overwritten by newer ones
#   Run %postun of old package ($1=1)
# *****************************************************************************
%pre
# create the "online" user if it does not exist
grep -q online /etc/passwd
if [ $? -ne 0 ]; then                              # online user does not exist
  /usr/sbin/useradd online
fi
# remove FMC entries from /etc/inittab if any
grep '^fmc' /etc/inittab >/dev/null
if [ $? -eq 0 ]; then                   # there are some entries in the inittab
  cp /etc/inittab /etc/.inittab.rpmold
  sed '/FMC/d' /etc/.inittab.rpmold > /etc/inittab
  # re-read inittab and stop old FMC daemons if any
  /sbin/telinit q || /bin/true
  /bin/sleep 1 || /bin/true
  # kill old processes if any
  FMC_PSS="logViewer ipmiSrv pcSrv cmSrv tmSrv logSrv"
  for fmc_ps in ${FMC_PSS[@]} ; do
    /usr/bin/pkill $fmc_ps || /bin/true
  done
  /bin/sleep 1 || /bin/true
fi
FMC_PSS="osSrv coalSrv cpuinfoSrv cpustatSrv fsSrv irqSrv memSrv nifSrv psSrv rlSrv smartSrv tcpipSrv"
for fmc_ps in ${FMC_PSS[@]} ; do
  /usr/bin/pkill $fmc_ps || /bin/true
done
FMC_PSS="oldCoalSrv oldCpuinfoSrv oldCpustatSrv oldIrqSrv oldPsSrv oldFsSrv oldNifSrv oldMemSrv"
for fmc_ps in ${FMC_PSS[@]} ; do
  /usr/bin/pkill $fmc_ps || /bin/true
done
# now no FMC processes are running
/bin/sleep 1 || /bin/true
# *****************************************************************************
%post
if [ "$1" = "1" ] ; then                                 # install, not upgrade
  # add FMC library directory to the shared library path
  ldConfFile="/etc/ld.so.conf.d/fmc.conf"
  if [ ! -f ${ldConfFile} ] ; then
    echo $RPM_INSTALL_PREFIX/FMC/lib > ${ldConfFile}
  fi
fi                                                       # install, not upgrade
# new library version can be found in $RPM_INSTALL_PREFIX/FMC/lib
/sbin/ldconfig 
# -----------------------------------------------------------------------------
# give setuid permission to tmSrv and pcSrv
/bin/chown root:root $RPM_INSTALL_PREFIX/FMC/sbin/tmSrv || /bin/true
/bin/chown root:root $RPM_INSTALL_PREFIX/FMC/sbin/pcSrv || /bin/true
# rwsr-sr-x
/bin/chmod 6755 $RPM_INSTALL_PREFIX/FMC/sbin/tmSrv || /bin/true
/bin/chmod 6755 $RPM_INSTALL_PREFIX/FMC/sbin/pcSrv || /bin/true
# -----------------------------------------------------------------------------
# Configuration files needed:
#   1. /etc/sysconfig/dim   both worker-nodes and control-nodes, set by DIM RPM
#   2. /etc/sysconfig/fmc   both worker-nodes and control-nodes
#   3. /etc/rc.d/init.d/fmc both worker-nodes and control-nodes
#   4. /etc/tmSrv.allow     both worker-nodes and control-nodes
#   5. /etc/cmSrv.conf      both worker-nodes and control-nodes
#   6. /etc/cmSrv.allow     both worker-nodes and control-nodes
#   7. /etc/ipmiSrv.conf    only control-nodes
#   8. /etc/pcSrv.conf      only control-nodes
#   9. /etc/pcSrv.init      only control-nodes
#  10. /etc/pcSrv.allow     only control-nodes
# -----------------------------------------------------------------------------
# write FMC sys V start-up configuration file
sysVcfgFile="/etc/sysconfig/fmc"
oldString="PC_ARGS=\"-u online -g online --no-auth\""
newString="PC_ARGS=\"-u root -g root --no-auth\""
if [ -f $sysVcfgFile ] ; then
  newSysVcfgFile="/etc/sysconfig/fmc.rpmnew"
  cp $RPM_INSTALL_PREFIX/FMC/skel/etc_sysconfig_fmc $newSysVcfgFile
  echo "$newSysVcfgFile created."
  grep "${oldString}" "${sysVcfgFile}" >/dev/null
  if [ $? -eq 0 ]; then
    cp -p ${sysVcfgFile} ${sysVcfgFile}.rpmold
    cat ${sysVcfgFile}.rpmold | sed "s/${oldString}/${newString}/g" > ${sysVcfgFile}
  fi
else
  cp $RPM_INSTALL_PREFIX/FMC/skel/etc_sysconfig_fmc $sysVcfgFile
  echo "$sysVcfgFile created."
fi
/bin/chown root:root $sysVcfgFile || /bin/true
/bin/chmod 0644 $sysVcfgFile || /bin/true
# .............................................................................
# write FMC sys V start-up script
newFile="/etc/rc.d/init.d/fmc"
cp $RPM_INSTALL_PREFIX/FMC/skel/etc_rc.d_init.d_fmc $newFile
echo "File \"$newFile\" created."
/bin/chown root:root $newFile || /bin/true
/bin/chmod 0755 $newFile || /bin/true
# .............................................................................
# write FMC shell start-up script
newFile="/etc/profile.d/fmc.sh"
cp $RPM_INSTALL_PREFIX/FMC/skel/etc_profile.d_fmc.sh $newFile
echo "File \"$newFile\" created."
/bin/chown root:root $newFile || /bin/true
/bin/chmod 0755 $newFile || /bin/true
# .............................................................................
# write sample Configuration Manager Server configuration file
newFile="/etc/cmSrv.conf"
cp $RPM_INSTALL_PREFIX/FMC/skel/etc_cmSrv.conf $newFile
echo "File \"$newFile\" created."
/bin/chown root:root $newFile || /bin/true
/bin/chmod 0600 $newFile || /bin/true
# ...........................................................................
# write sample Configuration Manager Server access control file
newFile="/etc/cmSrv.allow"
cp $RPM_INSTALL_PREFIX/FMC/skel/etc_cmSrv.allow $newFile
echo "root@`hostname`" >> $newFile
echo "online@`hostname`" >> $newFile
echo "File \"$newFile\" created."
/bin/chown root:root $newFile || /bin/true
/bin/chmod 0600 $newFile || /bin/true
# -----------------------------------------------------------------------------
# write sample Task Manager Server access control file
newFile="/etc/tmSrv.allow"
if [ -f $newFile ] ; then
  newFile="/etc/tmSrv.allow.rpmnew"
fi
cp $RPM_INSTALL_PREFIX/FMC/skel/etc_tmSrv.allow $newFile
echo "root@`hostname`" >> $newFile
echo "online@`hostname`" >> $newFile
echo "File \"$newFile\" created."
/bin/chown root:root $newFile || /bin/true
/bin/chmod 0600 $newFile || /bin/true
# .............................................................................
# write sample Process Controller Server configuration file
newFile="/etc/pcSrv.conf"
if [ -f $newFile ] ; then
  newFile="/etc/pcSrv.conf.rpmnew"
fi
cp $RPM_INSTALL_PREFIX/FMC/skel/etc_pcSrv.conf $newFile
hostname|cut -d "." -f 1 >> $newFile
echo "File \"$newFile\" created."
/bin/chown root:root $newFile || /bin/true
/bin/chmod 0600 $newFile || /bin/true
# .............................................................................
# write sample Process Controller Server initalization file
# overwritten by the new one. Old saved as rpmsave
newFile="/etc/pcSrv.init"
if [ -f $newFile ] ; then
  oldFile="/etc/pcSrv.init.rpmsave"
  mv $newFile $oldFile
  echo "Old file \"$newFile\" moved to \"$oldFile\"."
fi
cp $RPM_INSTALL_PREFIX/FMC/skel/etc_pcSrv.init $newFile
echo "New file \"$newFile\" created."
/bin/chown root:root $newFile || /bin/true
/bin/chmod 0600 $newFile || /bin/true
# .............................................................................
# write sample Process Controller Server access control file
newFile="/etc/pcSrv.allow"
if [ -f $newFile ] ; then
  newFile="/etc/pcSrv.allow.rpmnew"
fi
cp $RPM_INSTALL_PREFIX/FMC/skel/etc_pcSrv.allow $newFile
echo "root@`hostname`" >> $newFile
echo "online@`hostname`" >> $newFile
echo "File \"$newFile\" created."
/bin/chown root:root $newFile || /bin/true
/bin/chmod 0600 $newFile || /bin/true
# .............................................................................
# write sample Power Manager configuration files
newFile="/etc/ipmiSrv.conf"
oldFile="/etc/ipmiSrv.conf.rpmold"
oldGlobFile="/etc/FMC.conf"
if [ -f $oldFile ] ; then
  mv $oldFile ${oldFile}er
  echo "File \"$oldFile\" moved to \"${oldFile}er\"."
fi
if [ -f $newFile ] ; then
  mv $newFile $oldFile
  echo "File \"$newFile\" moved to \"$oldFile\"."
fi
if [ -f $oldGlobFile ] ; then
  mv $oldGlobFile ${oldGlobFile}.rpmold
  echo "File \"$oldGlobFile\" moved to \"${oldGlobFile}.rpmold\"."
  oldGlobFile=${oldGlobFile}.rpmold
fi
cp $RPM_INSTALL_PREFIX/FMC/skel/etc_ipmiSrv.conf $newFile
echo "File \"$newFile\" created."
/bin/chown root:root $newFile || /bin/true
/bin/chmod 0600 $newFile || /bin/true
# .............................................................................
# retrieve old Power Manager configuration
if [ "$1" = "2" ] ; then                                 # upgrade, not install
  grep -q IPMI_USER $oldFile 
  if [ $? -eq 0 ]; then                            # IPMI_USER found in oldFile
     mv $oldFile $newFile
     echo "File \"$oldFile\" moved to \"$newFile\"."
  else                                         # IPMI_USER not found in oldFile
    if [ -f $oldGlobFile ] ; then                        # oldGlobFile existing
      # append old global IPMI config entries to new configuration file
      echo "# Global configuration records:" >> $newFile
      cat $oldGlobFile | /bin/egrep -v "^#" >> $newFile || /bin/true
      echo "Old file \"$oldGlobFile\" appended to \"$newFile\"."
      echo "# !!! THIS FILE IS OBSOLETE !!!" >> $oldGlobFile
      echo "# !!! DATA MOVED TO \"$newFile\" !!!" >> $oldGlobFile
    fi                                                   # oldGlobFile existing
    if [ -f $oldFile ] ; then                                # oldFile existing
      # append old per-node IPMI config entries to new configuration file
      echo "# Per-node configuration records:" >> $newFile
      cat $oldFile | /bin/egrep -v "^#" >> $newFile || /bin/true
      echo "Old file \"$oldFile\" appended to \"$newFile\"."
    fi                                                       # oldFile existing
  fi                                           # IPMI_USER not found in oldFile
fi                                                       # upgrade, not install
# .............................................................................
/etc/rc.d/init.d/fmc condrestart
/bin/sleep 1 || /bin/true
# -----------------------------------------------------------------------------
echo "FMC installation terminated."
if [ "$1" = "1" ] ; then                                 # install, not upgrade
  echo "Before starting the FMC tools, create the user \"online\" and customize"
  echo "the following files:"
  echo " 1 - /etc/sysconfig/dim (define DIM_DNS_NODE)."
  echo " 2 - /etc/sysconfig/fmc (select FMC tools to start and define options)"
  echo "    (see manpages for logSrv, tmSrv, cmSrv, pcSrv and ipmiSrv)."
  echo " 3 - /etc/ipmiSrv.conf (IPMI configuration)."
  echo " 4 - /etc/pcSrv.conf (Process Controller configuration)."
  echo " 5 - /etc/pcSrv.init (Monitor Servers to be started)."
  echo " 6 - /etc/cmSrv.conf (Configuration Manager configuration)."
  echo " 7 - /etc/cmSrv.allow (Configuration Manager access control list)."
  echo "To run FMC daemons with authentication, remove the \"--no-auth\" options"
  echo "in /etc/sysconfig/fmc and customize also the following files:"
  echo " 8 - /etc/tmSrv.allow (Task Manager access control list)."
  echo " 9 - /etc/pcSrv.allow (Process Controller access control list)."
  echo "To start FMC tools immediately, type:"
  echo "   service fmc start"
  echo "To start the FMC tools at bootstrap, using chkconfig, type:"
  echo "   chkconfig --levels 345 fmc on"
  echo "   chkconfig --levels 0126 fmc off"
fi
# *****************************************************************************
#%preun
# stop old processes
#/etc/rc.d/init.d/fmc stop
#/bin/sleep 1 || /bin/true
# *****************************************************************************
%postun
#set -x
if [ "$1" = "0" ] ; then                                  # delete, not upgrade
  # stop old processes
  /etc/rc.d/init.d/fmc stop
  /bin/sleep 1 || /bin/true
  # remove FMC library directory from the shared library path
  slpfx=$(echo $RPM_INSTALL_PREFIX/FMC/lib | sed 's!/!\\/!g')
  grep -q $RPM_INSTALL_PREFIX/FMC/lib /etc/ld.so.conf
  if [ $? -eq 0 ]; then
    cp /etc/ld.so.conf /etc/.ld.so.conf.rpmold
    sed "/$slpfx/d" /etc/.ld.so.conf.rpmold > /etc/ld.so.conf
    /sbin/ldconfig 
    rm /etc/.ld.so.conf.rpmold
  fi
  # rename FMC configuration files as rpmold
  if [ -f /etc/cmSrv.conf ] ; then
    mv /etc/cmSrv.conf /etc/cmSrv.conf.rpmold
  fi
  if [ -f /etc/cmSrv.allow ] ; then
    mv /etc/cmSrv.allow /etc/cmSrv.allow.rpmold
  fi
  if [ -f /etc/tmSrv.allow ] ; then
    mv /etc/tmSrv.allow /etc/tmSrv.allow.rpmold
  fi
  if [ -f /etc/ipmiSrv.conf ] ; then
    mv /etc/ipmiSrv.conf /etc/ipmiSrv.conf.rpmold
  fi
  if [ -f /etc/pcSrv.conf ] ; then
    mv /etc/pcSrv.conf /etc/pcSrv.conf.rpmold
  fi
  if [ -f /etc/pcSrv.init ] ; then
    mv /etc/pcSrv.init /etc/pcSrv.init.rpmold
  fi
  if [ -f /etc/pcSrv.allow ] ; then
    mv /etc/pcSrv.allow /etc/pcSrv.allow.rpmold
  fi
  if [ -f /etc/rc.d/init.d/fmc ] ; then
    rm -f /etc/rc.d/init.d/fmc
    rm -f /etc/rc.d/rc?.d/???fmc || /bin/true
  fi
  if [ -f /etc/sysconfig/fmc ] ; then
    mv /etc/sysconfig/fmc /etc/sysconfig/fmc.rpmold
  fi
fi                                                        # delete, not upgrade
# *****************************************************************************
%changelog
*  Thu Dec 06 2012 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.14 release 0
- Version compliant with Linux SL 6.2, kernel 2.6.32-220.17.1, gcc 4.4.6
- tmStart perform argument check (as far as possible...).
- tmLl print the oom_score of started processed.
- pcSrv can control processes setting their oom_score_adj at start-up
*  Mon Nov 07 2011 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.13 release 0
- tmSrv set oom_adj to -17 or oom_score_adj to -1000.
- tmSrv can set the oom_adj or oom_score_adj of started processes.
- memSrv publishes uint32_t "summary" data and uint64_t "details" data.
- memSrv don't send anymore error messages.
*  Mon Nov 07 2011 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.12 release 0
- Filesystem monitor fsSrv rewritten from scratch.
- fsSrv can cope with stalled nfs file systems.
- fsSrv calls statfs(2) in short living threads.
- fsSrv publishes float "summary" data and uint64_t "details" data.
- fsSrv publishes filesystem status (OK, ERROR, STALLED since <N> s) in "details" data.
- Deprecated "initlog" replaced with "logger" in start-up scripts.
*  Tue Mar 02 2010 David SVANTESSON <David.G.Svantesson@cern.ch>
- Version 3.9.11 release 1
- Now publishes total counters (64bit) for the nifSrv (additional to rates)
*  Wed Oct 07 2009 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.10 release 2
- bug fixed in psSrv
*  Tue Oct 06 2009 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.10 release 1
- bug fixed in psSrv
*  Mon Oct 05 2009 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.10 release 0
- Compliant with DIMv19r4
- Process Monitor Server (psSrv) publishes new summary services.
*  Thu Jul 23 2009 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.9 release 0
- Process Monitor Server (psSrv) publishes summary service in the format agreeded with Fernando Varela Rodriguez on July 22, 2009, in order to be interfaced with the PVSS application.
- Process Monitor Server (psSrv) publishes an additional command in the format agreeded with Fernando Varela Rodriguez on July 16, 2009, in order to be interfaced with the PVSS application.
- The curses-based Process Monitor client (psMonitor) has 2 new menus to start/stop monitoring processes on the farm.
*  Fri Feb 27 2009 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.8 release 1
- Compatibility with SLC5
*  Thu Feb 26 2009 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.8 release 0
- Added curses-based Process Monitor client (psMonitor), which permanently subscribes to available on-demand dynamical DIM services published by the Process Monitor Server (psSrv).
- The Process Monitor Server (psSrv) can get messages from syslog and reformat them in the FMC format (option -y | --syslog).
*  Tue Feb 10 2009 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.7 release 0
- The Message Logger Client (logViewer) handles SIGHUP when the output is directed to a file (closes and reopens the file to manage logrotate).
- The Message Logger Client (logViewer) blocks SIGPIPE when the output is directed to a FIFO in no-drop mode (to cope with a FIFO reader restart).
- The Message Logger Client (logViewer), when the output is directed to a FIFO but the FIFO is not yet in place, waits until the FIFO is ready and has a reader process connected to the other end.
- New Process Monitor Server (psSrv) supporting additional on-demand dynamical DIM services dedicated to single processes.
- The Process Monitor Client (psViewer) is compatible with the new psSrv.
- Added Process Monitor Settings Client (psSetProperties) to start/stop psSrv additional on-demand dynamical DIM services dedicated to single processes.
*  Wed Jan 21 2009 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.6 release 0
- The FMC package is now independent from libprocps.
- New Process Monitor Server (psSrv) and client (psViewer). The psSrv does not yet include new required features (process DIM services).
*  Tue Dec 03 2008 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.5 release 0
- The Power Manager Server (ipmiSrv) can read the IPMI password at runtime from a file. In this case in the ipmitool(1) command line the name of the file which contains the password is specified instead of the password itself. This fix the security hole of ipmitool(1) processes which terminates before they can mask the password in the command-line.
*  Mon Dec 01 2008 Niko Neufeld <niko.neufeld@cern.ch> -
- Version 3.9.4 release 1
- Removed explicit dependency on ipmitool - to make it more collaborative with openipmi and friends. Virtual package would be proper solution, but requires patching all other packages providing ipmitool.
*  Tue Nov 11 2008 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.4 release 0 
- The Task Manager Server (tmSrv) does never use the ptrace(2) workaround to retrieve the UTGID if the patch Don-t-truncate-proc-PID-environ-at-4096-characters is applied to the running kernel.
*  Fri Oct 31 2008 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.3 release 2 
- Bug fix in Power Manager Server (ipmiSrv)
*  Thu Oct 30 2008 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.3 release 1 
- Re-linked with dim_v17r11. N.B.: version 3.9.3 release 0 was linked with dim_v16r14 and since version v17r01 re-link is needed.
*  Wed Oct 29 2008 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.9.3 release 0
- The Process Controller Server (pcSrv) can dynamically subscribe to new Task Manager servers (tmSrv) started after the pcSrv start-up.
- The Process Controller Server (pcSrv) perform a Parameter Expansion of the string ${RUN_NODE} into the hostname of the node on which the process is started, in the "add" command.
- New Configuration Manager Server (cmSrv) and clients (cmGet, cmSet, cmUnset) which read, write and remove ASCII configuration files (with file_path and user@host permission check).
- The Power Manager Server (ipmiSrv) skips duplicate hostnames and its own hostname if found in the configuration file /etc/ipmiSrv.conf
- The Power Manager Server (ipmiSrv) can read global parameters IPMI_USER, IPMI_PASSWD and IPMI_CMD from configuration file /etc/ipmiSrv.conf (but the values of the process environment override those of /etc/ipmiSrv.conf).
- The DIM server names, the DIM service names and the DIM command names has the string "/FMC" prefixed.
- Bug fixed in cpuinfoSrv (segfault due to buffer overflow with many cpu cores).
- The Monitor Server/Client cpuinfoSrv/cpuinfoViewer provides also parameters: hyper-thread, multi-core, cpu_cores and core_id.
- The Network Interface Monitor Server (nifSrv) provides also the link speed (e.g. 1000Mb/s), the duplex state (e.g. Full), the port type (e.g. Twisted Pair), the auto-negotiation state (on/off) and the link-detection state (yes/no).
- The Monitor Servers cpuinfoSrv, cpustatSrv, fsSrv, memSrv and nifSrv have the DIM service format following the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28.
- New Monitor Server osSrv added, providing operating system name, distribution version, kernel version and architecture.
*  Wed Aug 13 2008 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.8.7 release 0
- Bug in Task Manager Server (tmSrv) fixed. It occurred for processes with a huge environment (>4096 B) without UTGID. To those processes was incorrectly attributed, in listing them, the UTGID of the last scanned process with huge environment and UTGID set.
*  Mon Jul 09 2008 Loic Brarda <loic.brarda@cern.ch> -
- Version 3.8.6 release 2
- init script log to local6 instead of local7 (boot messages) (patch2)
- added a condrestart command to the init script (patch1)
- modify the specs file to :
    kill the processes only if they were in the inittab
    restart them after update only if they were running
*  Mon Jul 08 2008 Loic Brarda <loic.brarda@cern.ch> -
- Version 3.8.6 release 1
- changed default start&stop priorities
*  Mon Jun 30 2008 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.8.6 release 0
- FMC servers started no more by using inittab. FMC servers started as services in /etc/rc.d/init.d. New scripts /etc/rc.d/init.d/fmc and /etc/sysconfig/fmc.
- New Message Logger tools (executables) logSetProperties and logGetProperties to set and get the properties of the Message Logger Servers.
- In Message Logger Client (logViewer): can dynamically subscribe to new servers started after logViewer start-up.
- In Message Logger Client (logViewer): in cmd-line options the choice of node pattern (-m) and logger pattern (-s) is separate.
- In Message Logger Server (logSrv): only one DIM SVC (get_properties) and one DIM CMD (set_properties) to get and set properties.
- In Message Logger Server (logSrv): DIM SVCs and CMDs changed.
- In Message Logger Server (logSrv): cmd-line option -E takes 0,1 as argument
- In Task Manager server (tmSrv): memory leak fixed.
- In Task Manager server (tmSrv): do not use libprocps no more (but libprocps is still used by process monitor server psSrv). Directly calls readdir_r(3) instead. Bug in the Task Manager should be fixed.
- In Task Manager server (tmSrv): can perform setuid/setgid at startup (new cmdline options -U and -G).
*  Thu Apr 10 2008 Niko NEUFELD <niko.neufels@cern.ch> -
- Version 3.8.5
- Workaround by Beat Jost on a bug in the Task Manager, function findUtgid().
*  Fri Mar  6 2008 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.8.4
- In Process Controller server (pcSrv): renamed duplicated cmd-line option ([-N maxStartN] -> [-M maxStartN]).
*  Fri Mar  5 2008 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.8.3
- In IPMI server (ipmiSrv): empty configuration file exception handled.
- In IPMI server (ipmiSrv): exit(1) if no node configured.
*  Fri Dec 28 2007 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.8.2
- Task Manager Server (tmSrv) can manage processes with a huge environment (>=4096 B) by using the ptrace(2) system call. If the environment size is <4096 the old method, using /proc/<tgid>/environ is still used.
- Task Manager Server (tmSrv) keeps a list of started processes (UTGID, TGID and CMD) including processes terminated within PERSISTENCE seconds (settable using --defunct-persistence PERSISTENCE cmd-line option). For terminated processes it keeps also exit_status/received_signal and termination time.
- Task Manager Server (tmSrv) sends to the Message Logger a message with UTGID, TGID, CMD and exit_status/received_signal when a child process terminates.
- Task Manager Server (tmSrv) publishes the DIM SVC longList with a list of processes started by tmSrv (UTGID, TGID and CMD) including processes terminated within PERSISTENCE seconds (settable using --defunct-persistence cmd-line option) with exit_status/received_signal and termination time.
- Process Monitor Server (psSrv) subscribes to the longList SVC of Task Manager Server (tmSrv), if tmSrv is running, to get the UTGIDs of huge environment processes and to force a psSrv update whenever a new process is started by the Task Manager or a Task Manager child terminates.
- RPM spec file supports Upgrade (will work upgrading from this release).
- RPM spec file installs Worker-Node configuration if (empty) file /etc/FMCwn exists, installs Control-Node configuration if file /etc/FMCwn does not exist.
- Added cmd-line application fmcVersionViewer to print the Server Version and the FMC Version of the running FMC servers. The cmd-line option -m NODE_PATTERN  allows to restrict to servers of nodes whose hostname matches the wildcard pattern NODE_PATTERN.  The cmd-line option -s SERVER_PATTERN allows to restrict to servers matching the wildcard pattern SERVER_PATTERN.
- All cmd-line clients supports accept multiple host pattern (-m, --hostname option repeatable).
- All cmd-line clients read DIM_DNS_NODE (in decreasing order of priority) from -N DIM_DNS_NODE command-line option, DIM_DNS_NODE environment variable and /etc/sysconfig/dim file.
- Added FMC version in executable output with -h cmd-line switch.
- logViewer, tmLs, tmLl, pcLl, pcLs, pcLss, pwStatus, ipmiViewer, cmViewer, coalViewer, cpuinfoViewer, cpustatViewer, fsViewer, ipmiViewer, irqViewer, memViewer, nifViewer, psViewer, smartViewer, tcpipViewer have the cmd-line option -V to print the server source and FMC versions installed on the reachable nodes.
- Improved check of ipmitool output in ipmiSrv.
- If no IPMI sensor of some kind (temperature, fas speed, voltage or current) is found, ipmiSrv publishes [IPMI values not available] instead of a null string. This should fix some fwdim problems.
- New cmd-line options --get-try-n (default: 5) and --cmd-try-n (default: 10) added to ipmiSrv to set the number of tries in reading status and issueing commands.
* Thu Oct 11 2007 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.8.1
- Thread number limitation in Power Manager Server ipmiSrv through the command-line option [-t, --max-threads-number MAX_THREAD_NUMBER (integer, optional)]. MAX_THREAD_NUMBER must be at least 6. If MAX_THREAD_NUMBER=0 no limit is set to the number of threads running simultaneously, like in previous versions.
- pwStatus is faster (uses DimInfo instead of DimCurrentInfo).
- pwStatus and ipmiViewer accept multiple host pattern (-m, --hostname option repeatable).
* Mon Oct  1 2007 Domenico GALLI <domenico.galli@bo.infn.it> - 
- Version 3.8.0
- Rhost-like authentication of Task Manager and Process Controller. Clients send username and hostname together with commands, servers check if sent username and hostname match at least one entry in the files /etc/tmSrv.allow (Task Manager) and /etc/pcSrv.allow (Process Controller), otherwise discard the command and log the illegal access attempt. Wildcard allowed in access control files. File format is described in the comments in the files.
- Command-line switch --no-authentication in tmSrv, tmStart, tmKill, tmStop, pcSrv, pcAdd and pcRm to obtain the unauthenticated behavior of the old versions.
- The RPM produces the sample access control files /etc/tmSrv.allow (comments + root & online on the current host) and /etc/pcSrv.allow (comments + root & online on the current host).
* Tue Sep 18 2007 Domenico GALLI <domenico.galli@bo.infn.it> - 
- Version 3.7.0
- ipmiSrv reads the per-node configuration file /etc/ipmiSrv.conf or the service /<HOSTNAME>/config_manager/ipmi/get. File format is described in the comments in /etc/ipmiSrv.conf. Node user "noUser" can be specified to avoid to send to IPMI the username string.
- new msgUtils.c. New functions to write to the logger: mPrintf(), rPrintf(), mfPrintf(), rfPrintf() which get, as argument, also the number tryN of write trials. If tryN==0 then the number of try is chosen equal to the severity (VERB=1, DEBUG=2, INFO=3, WARN=4, ERROR=5, FATAL=6). Delay between tries in seconds is 0.001, 0.002, 0.004, 0.008, 0.016, ... Newlines in a messages now break the messages in more messages, each one with its header. Message size can be up to 4096*10 characters. If message size exceeds 4096 chars, it is broken into more messages, each one with its header. Old functions printM(), printMF(), printR() and printRF() now deprecated and implemented as macro.
- New experimental monitor server and clients. Old version still included for compatibility, with the prefix old-.
- All monitor server, if don't find the DIM_DNS_NODE in their environment, try to read it in /etc/sysconfig/dim. A file different from /etc/sysconfig/dim can be specified with the -c command line option.
- The RPM produces the sample configuration files /etc/ipmiSrv.conf (only comments), /etc/pcSrv.conf (comments+current host) and /etc/pcSrv.init (comments+all the monitor servers).
* Mon Jul  3 2006 Niko NEUFELD <niko@pclhcb74.cern.ch> -
- Initial RPM build.
* Mon Feb 13 2006 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.3.
- Support for x86_64 architecture.
- Added IPMI sensors (temperature, fan speed battery) reading (using IPMI LAN interface) in IPMI server.
- New single access BMC arbitration in IPMI server.
- Arbitrary length command queue in IPMI server.
- Stack of new threads decreased in IPMI server.
- Added timestamp service for both IPMI power status and IPMI sensors readings in IPMI server.
- Removed files: getIPMISensor.c, IPMISensorSrv.c, psController.C.
- Added files: ipmiViewer.C 1.4.
* Tue Jan 10 2006 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.2 (Jan 10, 2006)
- added configuration manager
- modified IPMI power manager: (1) Can read configuration both from file and from cmSrv. (2) Uses SIGHUP instead of condition variable to signal the main thread the need of refresh. (3) Processes signals synchronously.
* Fri Nov 18 2005 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.1
- Changed DIM services for memory monitor.
* Tue Nov 15 2005 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 3.0 (Nov 15, 2005)
- getMemFromProc.c 1.4. Return value=-2 if quantity not found.
- cpustatSrv.c 2.4: Synchronous handling of signals. Define DIM error handler. Added service /HOSTNAME/cpustat/cpuN.  Added service /HOSTNAME/cpustat/name.
- nifSrv.c 2.11.  Compatibility with PVSS part version 3.  Removed service /HOSTNAME/nif/ethX.  Added service /HOSTNAME/nif/name.  Added service /HOSTNAME/nif/mac.  Added service /HOSTNAME/nif/bus.  Added service /HOSTNAME/nif/ip.  Added service /HOSTNAME/nif/ifN.  Added service /HOSTNAME/nif/ethIfN.
- Added tcpipViewer.C 1.7.
- Added nifViewer.C 1.4.
- Added irqViewer.C 1.1.
- Added cpuinfoViewer.C 1.2.
- Added cpustatViewer.C 1.1.
- Added memViewer.C 1.1.
- Added tcpipReset.C 1.2.
- Added nifReset.C 1.1.
* Wed Sep 28 2005 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 1.6
- logSrv.c 1.20. Synchronous SIGINT/SIGTERM handling. Open FIFO in O_RDONLY|O_NONBLOCK to avoid FIFO blocking.
- logViewer.C 1.8. Does not print strings beginning with ffffffff. fflush message output, for output reditection to a log file.
- tmStart.C 1.11, tmStop.C 1.3, tmKill.C 1.5, tmLs.C 1.3. More restrictive DIM wildcard pattern to speed-up browsing.
- Bug fixed in sensor getTCPIPfromProc.c 1.17.
- tcpipSrv.c 2.6. Synchronous SIGINT/SIGTERM/SIGHUP handling. Immediate reset. Uses DIM error handler.
- tcpipViewer.C 1.5 and tcpipReset.C 1.2 clients added.
- Include libproc-3.2.5.so.
- tmUtils.c 1.32 and getPsFromProc.c 1.11 modified to use libproc-3.2.5.so.
- tmStart.C 1.13 -m option commutes with option to tmSrv.
- getNIFfromProc.c 1.13 read (PCI) bus addresses of the interfaces.
- nifSrv.c 2.6. Sinchronous handling of signals. Immediate reset. Uses DIM error handler. Publish also the (PCI) bus addresses of the interfaces.
- nifReset.C 1.1 added.
* Fri Jun 24 2005 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 1.5
- Tested in RTTC 2005
* Wed Jan 12 2005 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 0.2
- Process Controller added. This is an executable which reads an XML configuration file, validate it against a DTD (document type definition), and keeps alive the processes described in for each node of the farm.  Process controller works contacting the Task Manager of each farm node.  Process restart is triggered by process dead: when a process started by the Task Manager dies, a SIGCHLD signal is sent to the task manager; the Task Manager signal handler schedules an update of the DIM list service which is performed within 0.1 s; the Process Controller infoHandler is triggered by DIM list service update and schedules a process restart which is performed within 0.1 s.  A respawn control is also implemented: if a process is started more than maxStartN times in checkPeriod seconds, then the process restart is disabled for disPeriod seconds. If maxStartN=-1, then respawn control is excluded, i.e. process can be restarted indefinitely. If disPeriod=-1, then the process-restart for the process, once disabled, is never re-enabled.- In Task Manager Server, added 4 optional command-line options: --schedpol scheduling_policy (default: 2) Set the Task Manager scheduling option (0=SCHED_OTHER (time sharing), 1=SCHED_FIFO, 2=SCHED_RR).  --mainprio main_thread_priority (default: 80) Set the static (real-time) priority og the Task Manager's main thread.  --ioprio I/O_thread_priority (default: 93) Set the static (real-time) priority og the Task Manager's I/O thread.  --timerprio timer_thread_priority (default: 10) Set the static (real-time) priority of the Task Manager's timer thread.
- In Task Manager Server, start command options "-e" and "-o" added to redirect stderr and stdout of started processes to DIM logger. Useful, e.g., to retrieve messages like: "Dec18-030952[ERROR]pcdom: /home/galli/prog/FMC-0.2/tm/examples/counter: error while loading shared libraries: libdim.so: cannot open shared object file: No such file or directory" printed by the dynamic linker on stderr.
- In Logger Server, server adds timestamp and severity to messages which miss them, e.g. messages from stderr of started processes.
- In Task Manager Server, when a process terminates spontaneously (i.e. not killed by Task Manager) the process list is updated immediately, without waiting for the 10 s periodic update (this does not work if Task Manager Server has been restarted).
- In Task Manager Server, when a process terminates spontaneously (i.e. not killed by Task Manager) a message is sent to the logger with either the exit status of the process (which may have been set as the argument to a call to exit() or as the argument for a return statement in the main program) or the signal number of the signal received which caused the child to stop (this does not work if Task Manager has been restarted). E.g.: "Dec18-030952[FATAL]pcdom: tmSrv: main(): waitpid(): Process whose TGID is 17963 exited. Exit code: 127." "Dec18-033255[FATAL]pcdom: tmSrv: main(): waitpid(): Process whose TGID is 18036 exited on signal 8 (SIGFPE)."
- Bug fix in Task Manager Server: duplicated UTGIDs.  SYMPTOMS: when a burst of many start commands is issued in a short time, a duplicate UTGID could be found.  DEFECT DESCRIPTION: If 2 processes are started at short time distance each other, when the second process is started, the first process's UTGID may be still not readable from procfs; therefore genUtgid() function can generate for the second process the same UTGID of the first one.  FIX: Task Manager startCmndHandler() does not return until either: the new process's UTGID is readable from procfs; the new process's TGID disappear (still-born process); a timeout of 4 seconds is expired.
- Bug fix in Task Manager Server: processes surviving stop command.  SYMPTOMS: when a burst of many stop commands are issued in a short time, some processes which ignore SIGTERM survive also SIGKILL.  DEFECT DESCRIPTION: the SIGKILL signal is sent by finishOffPs() function.  In previous version, the finishOffPs() function was started by a signal handler triggered by the expiration of a timer. The UTGID pattern of the processes to be killed was passed to finishOffPs() by means of a global variable. If 2 stop commands were sent at short time distance each other, when the first process was waiting for timer expiration, the second stop command might overwrite the UTGID pattern written on the global variable.  As a result the first process was never sent the SIGKILL.  FIX: finishOffPs() now runs in a separate thread which is passed the UTGID pattern as argument (local to thread) which sleeps for the required time and which frees memory space allocated for UTGID pattern after the SIGKILL is sent.
- Bug fix in Task Manager Server: dead-lock.  SYMPTOMS: when a burst of many stop commands is issued in a short time the Task Manager sometimes hung-up indefinitely. This happend only when the DIM service log-type is used (not when DIM logger log-type is used).  DEFECT DESCRIPTION: since a messages was sent to DIM by SIGCHLD signal handler following waitpid() call, it could happen that a message was sent to DIM while another messages was been processed by DIM. As a result, on one side DIM was waiting to finish processing the first message before starting processing the second one and, on the other side, signal handler was waiting to finish processing the second message before continueing sending the first message. A dead-lock condition was therefore established.  FIX: now SIGCHLD signal handler only set a sig_atomic_t variable (waitpidReq). The main function, in the main loop, test this variable and, if it finds the variable set, then calls waitpid() and send messages to logger.
- Memory leak fixed in Task Manager Server: cmd string.  SYMPTOMS: memory leak detected.  DEFECT DESCRIPTION: memory allocated in the heap for the cmd string was never released.  FIX: memory allocated in the heap for the cmd string is released before every rerutn statement.
- Memory leak fixed in Task Manager Server: new thread's memory in stop command handler.  SYMPTOMS: memory leak detected on stop command execution.  DEFECT DESCRIPTION: memory allocated by the new threads created by stopCmndHandler() was never released because those thread was never detached (as they finished their tasks, they were waiting for a pthread_join() call from the parent thread, as a "zombie" thread).
- Bug fixed in network Interface sensor (getNIFfromProc.c): error in counter reset handling.  SYMPTOMS: sensor reported negative data transfer rate when the kernel counter was reset.  DEFECT DESCRIPTION: kernel currently uses 32-bit counters for network interfaces. When the counters are reset (every 34 seconds at full rate) counter increment in a time interval is negative. In this case 2^32 must be added to counter difference. In the old version data were casted to 64-bit integer (long long) after the subtraction (new_value-old_value).  FIX: Data are casted to 64-bit integer (long long) before the subtraction.
* Sun Nov 21 2004 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 0.1
- In Task Manager Clients, "-s srvc_pattern" option removed.
- In Task Manager Clients, "-m hostname_pattern" option added.
- In Task Manager Clients, sequence "--" (end of option-scanning) is no more needed (but the -m flag must be the first flag).
- In Task Manager Server, "-D NAME=value" (repeatable) DIM command option added to allow clients to set process-specific environment variables.
* Tue Nov  4 2004 Domenico GALLI <domenico.galli@bo.infn.it> -
- Version 0.0
- Initial version.
