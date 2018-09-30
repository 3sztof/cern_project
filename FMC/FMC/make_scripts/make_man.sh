#!/bin/bash
BIN_DIR=bin
SBIN_DIR=sbin
msgUtilsVer=`ident utils/src/fmcMsgUtils.c|grep Id|awk '{print $2" "$3" "$4" "$5" "$6}'`
fmcVer=FMC-`cat fmcVersion`
fmcUtilsVer=`ls lib/libFMCutils-?.?.so|cut -d '/' -f 2|cut -d '.' -f 1-2 | \
             sed "s/-/ v /g"`
fmcRef=`make_scripts/make_ref`
echo "msgUtilsVer=$msgUtilsVer"
echo "fmcVer=$fmcVer"
echo "fmcUtilsVer=$fmcUtilsVer"
echo "fmcRef="
echo "------------------------------------------------------------------------"
echo "$fmcRef"
echo "------------------------------------------------------------------------"
now=`date +"%Y-%m-%d"`
# Old Version man8 (FMC Servers)
for n in `ls $SBIN_DIR/*Srv` ; do
  echo $n | grep -q old >/dev/null
  if [ $? -ne 0 ] ; then
    nb=`/bin/basename $n`
    echo ${nb}.8
    $n -h > man/man8/${nb}.8.txt 2>&1
  fi
done
# Old Version man1 (FMC User Commands)
for n in `ls $BIN_DIR/*` ; do
  echo $n | grep -q old >/dev/null
  if [ $? -ne 0 ] ; then
    nb=`/bin/basename $n`
    echo ${nb}.1
    $n -h > man/man1/${nb}.1.txt 2>&1
  fi
done
# man8 (FMC Servers)
for n in $SBIN_DIR/logSrv $SBIN_DIR/tmSrv $SBIN_DIR/pcSrv $SBIN_DIR/cmSrv $SBIN_DIR/ipmiSrv $SBIN_DIR/cpuinfoSrv $SBIN_DIR/osSrv $SBIN_DIR/psSrv $SBIN_DIR/fsSrv $SBIN_DIR/memSrv ; do
  echo $n | grep -q old >/dev/null
  if [ $? -ne 0 ] ; then
    nb=`/bin/basename $n`
    echo ${nb}.8.gz
    rm -f man/man8/${nb}.8.txt
    $n -hh > man/man8/${nb}.8 2>&1
    gzip -f man/man8/${nb}.8
  fi
done
# man1 (FMC User Commands)
for n in $BIN_DIR/mPrint $BIN_DIR/logViewer $BIN_DIR/logGetProperties $BIN_DIR/logSetProperties $BIN_DIR/tmStart $BIN_DIR/tmKill $BIN_DIR/tmStop $BIN_DIR/tmLs $BIN_DIR/tmLl  $BIN_DIR/pcAdd  $BIN_DIR/pcRm $BIN_DIR/pcLs $BIN_DIR/pcLl $BIN_DIR/pcLss $BIN_DIR/cmSet $BIN_DIR/cmUnset $BIN_DIR/cmGet $BIN_DIR/pwStatus $BIN_DIR/pwSwitch $BIN_DIR/ipmiViewer $BIN_DIR/fmcSrvViewer $BIN_DIR/fmcVersionViewer $BIN_DIR/osViewer $BIN_DIR/cpuinfoViewer $BIN_DIR/psViewer $BIN_DIR/psSetProperties $BIN_DIR/psMonitor $BIN_DIR/fsViewer $BIN_DIR/memViewer ; do
  echo $n | grep -q old >/dev/null
  if [ $? -ne 0 ] ; then
    nb=`/bin/basename $n`
    echo ${nb}.1.gz
    rm -f man/man1/${nb}.1.txt
    $n -hh > man/man1/${nb}.1 2>&1
    gzip -f man/man1/${nb}.1
  fi
done
# man3 (FMC Programmer's Manual)
for n in utils/man/mPrintf.3 utils/man/mfPrintf.3  utils/man/rPrintf.3 utils/man/rfPrintf.3 utils/man/loggerOpen.3 utils/man/loggerClose.3  utils/man/dfltLoggerOpen.3 utils/man/dfltLoggerClose.3 utils/man/getDfltFifoFD.3 ; do
  nb=`/bin/basename $n`
  echo ${nb}.gz
  cat $n | sed "s!___msgUtilsVer___!${msgUtilsVer}!g" | 
           sed "s!___fmcUtilsVer___!${fmcUtilsVer}!g" |
           sed "s!___fmcVer___!${fmcVer}!g" | 
           sed "s!___fmcRef___!${fmcRef}!g" | 
           sed "s!___now___!${now}!g" > man/man3/${nb}
  gzip -f man/man3/${nb}
done
# man1 (FMC User Commands)
echo FMC.1.gz
cat utils/man/FMC.1 | sed "s!___msgUtilsVer___!${msgUtilsVer}!g" | 
                      sed "s!___fmcUtilsVer___!${fmcUtilsVer}!g" |
                      sed "s!___fmcVer___!${fmcVer}!g" | 
                      sed "s!___fmcRef___!${fmcRef}!g" | 
                      sed "s!___now___!${now}!g" > man/man1/FMC.1
gzip -f man/man1/FMC.1
cd man/man1
ln -sf FMC.1.gz fmc.1.gz 
exit 0
