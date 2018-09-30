/*****************************************************************************/
/*
 * $Log: getCpustatFromProc.c,v $
 * Revision 1.15  2011/09/27 11:51:14  galli
 * Increased PROC_STAT_SIZE 1024 -> 8192
 *
 * Revision 1.13  2008/10/09 09:53:06  galli
 * minor changes
 *
 * Revision 1.12  2007/08/10 13:04:56  galli
 * added function getCpustatSensorVersion()
 * compatible with libFMCutils v 2
 *
 * Revision 1.11  2006/10/23 07:01:42  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 1.10  2004/10/17 20:10:15  galli
 * cpustatInfo() function added
 *
 * Revision 1.9  2004/10/16 15:13:18  galli
 * Logs error to DIM logger
 *
 * Revision 1.6  2004/08/31 14:19:21  galli
 * first working version
 *
 * Revision 1.1  2004/08/28 10:59:53  galli
 * Initial revision
 * */
/*****************************************************************************/
/* see: /usr/src/linux-2.6.1/Documentation/filesystems/proc.txt              */
/*****************************************************************************/
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <errno.h>                                                  /* errno */
#include <string.h>                                  /* strerror(), strcmp() */
#include <syslog.h>                                              /* syslog() */
#include <sys/types.h>                                             /* open() */
#include <sys/stat.h>                                              /* open() */
#include <fcntl.h>                                                 /* open() */
#include <unistd.h>                                     /* read(), sysconf() */
#include <sys/time.h>                                      /* gettimeofday() */
#include <math.h>                                                  /* fabs() */
#include <values.h>                                               /* FLT_MIN */
/*****************************************************************************/
#include "getCpustatFromProc.h"                             /* struct cpuVal */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
#define PROC_STAT_SIZE 8192
#define MSG_SIZE 4000
/*****************************************************************************/
struct cpuTics
{
  unsigned long long user;
  unsigned long long nice;
  unsigned long long system;
  unsigned long long idle;
  unsigned long long iowait;
  unsigned long long irq;
  unsigned long long softirq;
};
/*****************************************************************************/
/* globals */
static char rcsid[]="$Id: getCpustatFromProc.c,v 1.15 2011/09/27 11:51:14 galli Exp galli $";
/*****************************************************************************/
struct cpusVal readCpu(int deBug, int errU)
{
  char cpuFileName[]="/proc/stat";
  int cpuFileFD;
  static struct cpusVal cpus;
  static struct cpuVal  *cpu;
  static struct cpuTics *cpuTcs;
  static struct cpuTics *oldCpuTcs;
  long long dUser,dNice,dSystem,dIdle,dIowait,dIrq,dSoftirq,dTot;
  static int isFirstCall=1;
  static unsigned cpuN;
  char cpuString[8];
  char procStatBuf[PROC_STAT_SIZE];
  int readBytes;
  static unsigned long ctxtCnt,oldCtxtCnt;
  long dCtxtCnt;
  int i,id;
  struct timezone tz;
  int sec,usec;
  float elapsedTime;
  static struct timeval now,lastTime;
  /*-------------------------------------------------------------------------*/
  if(deBug)
  {
    mPrintf(errU,DEBUG,__func__,0,"Entered...");
  }
  /*-------------------------------------------------------------------------*/
  if(isFirstCall)
  {
    /*.......................................................................*/
    /* evaluate processors number */
    cpuN=sysconf(_SC_NPROCESSORS_ONLN);
    if(cpuN<1)cpuN=1;
    /*.......................................................................*/
    /* allocate memory for array of structures */
    cpu=(struct cpuVal*)malloc((size_t)(cpuN+1)*sizeof(struct cpuVal));
    cpuTcs=(struct cpuTics*)malloc((size_t)(cpuN+1)*sizeof(struct cpuTics));
    oldCpuTcs=(struct cpuTics*)malloc((size_t)(cpuN+1)*sizeof(struct cpuTics));
    /*.......................................................................*/
    memset((char*)cpuTcs,0,(size_t)(cpuN+1)*sizeof(struct cpuTics));
    memset((char*)&now,0,sizeof(struct timeval));
    ctxtCnt=0;
  }
  /*-------------------------------------------------------------------------*/
  /* clean & prepare output structures */
  memset((char*)&cpus,0,sizeof(cpus));
  memset((char*)cpu,0,(size_t)(cpuN+1)*sizeof(struct cpuVal));
  cpus.cpuN=cpuN;
  cpus.version=rcsid;
  cpus.vals=cpu;
  /*-------------------------------------------------------------------------*/
  /* read /proc/stat in procStatBuf */
  cpuFileFD=open(cpuFileName,O_RDONLY);
  if(cpuFileFD==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s",cpuFileName,
            strerror(errno));
    return cpus;
  }
  if(lseek(cpuFileFD,0L,SEEK_SET)<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot lseek file \"%s\": %s",cpuFileName,
            strerror(errno));
    return cpus;
  }
  if((readBytes=read(cpuFileFD,procStatBuf,sizeof(procStatBuf)-1))<0) 
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot read file \"%s\": %s",cpuFileName,
            strerror(errno));
    return cpus;
  }
  close(cpuFileFD);
  procStatBuf[readBytes]='\0';
  /*-------------------------------------------------------------------------*/
  /* move cpu tics read last time from cpuTcs to oldCpuTcs */
  oldCtxtCnt=ctxtCnt;
  memcpy((char*)&lastTime,(char*)&now,sizeof(struct timeval));
  memcpy((char*)oldCpuTcs,(char*)cpuTcs,
         (size_t)(cpuN+1)*sizeof(struct cpuTics));
  ctxtCnt=0;
  memset((char*)&now,0,sizeof(struct timeval));
  memset((char*)cpuTcs,0,(size_t)(cpuN+1)*sizeof(struct cpuTics));
  /*-------------------------------------------------------------------------*/
  /* read CPU summary values */
  readBytes=sscanf(procStatBuf,"cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
                   &cpuTcs[0].user,&cpuTcs[0].nice,&cpuTcs[0].system,
                   &cpuTcs[0].idle,&cpuTcs[0].iowait,&cpuTcs[0].irq,
                   &cpuTcs[0].softirq);
  if(readBytes<4)
  {
    mPrintf(errU,ERROR,__func__,0,"Error reading file \"%s\": only %d Bytes "
            "read in line starting with \"cpu\"!",cpuFileName,readBytes);
    return cpus;
  }
  /*-------------------------------------------------------------------------*/
  /* read separate CPU values */
  for(i=0;i<cpus.cpuN;i++)
  {
    snprintf(cpuString,8,"cpu%u",i);
    readBytes=sscanf(strstr(procStatBuf,cpuString),
                     "cpu%u %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
                     &id,&cpuTcs[i+1].user,&cpuTcs[i+1].nice,
                     &cpuTcs[i+1].system,&cpuTcs[i+1].idle,
                     &cpuTcs[i+1].iowait,&cpuTcs[i+1].irq,
                     &cpuTcs[i+1].softirq);
    if(readBytes<4)
    {
      mPrintf(errU,ERROR,__func__,0,"Error reading file \"%s\": only %d Bytes "
              "read in line starting with \"%s\"!",cpuFileName,readBytes,
              cpuString);
      return cpus;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* read the total number of context switches across all CPUs */
  gettimeofday(&now,&tz);
  readBytes=sscanf(strstr(procStatBuf,"ctxt"),"ctxt %lu",&ctxtCnt);
  if(readBytes<1)
  {
    mPrintf(errU,ERROR,__func__,0,"Error reading file \"%s\": only %d Bytes "
            "read in line starting with \"ctxt\"!\n",cpuFileName,readBytes);
    return cpus;
  }
  /*-------------------------------------------------------------------------*/
  if(isFirstCall)
  {
    isFirstCall=0;
    for(i=0;i<=cpus.cpuN;i++)
    {
      cpus.vals[i].user=0.0;
      cpus.vals[i].nice=0.0;
      cpus.vals[i].system=0.0;
      cpus.vals[i].idle=0.0;
      cpus.vals[i].iowait=0.0;
      cpus.vals[i].irq=0.0;
      cpus.vals[i].softirq=0.0;
    }
    cpus.ctxtRate=0.0;
  }
  else
  {
    /*.......................................................................*/
    /* evaluate percentages from tics differences */
    for(i=0;i<=cpus.cpuN;i++)
    {
      dUser=cpuTcs[i].user-oldCpuTcs[i].user;
      dNice=cpuTcs[i].nice-oldCpuTcs[i].nice;
      dSystem=cpuTcs[i].system-oldCpuTcs[i].system;
      dIdle=cpuTcs[i].idle-oldCpuTcs[i].idle;
      if(dIdle<0)dIdle=0;                 /* can happend in smp kernel < 2.4 */
      dIowait=cpuTcs[i].iowait-oldCpuTcs[i].iowait;
      dIrq=cpuTcs[i].irq-oldCpuTcs[i].irq;
      dSoftirq=cpuTcs[i].softirq-oldCpuTcs[i].softirq;
      dTot=dUser+dNice+dSystem+dIdle+dIowait+dIrq+dSoftirq;
      if(dTot==0)dTot=1;
      cpus.vals[i].user=(float)dUser*100.0/dTot;
      cpus.vals[i].nice=(float)dNice*100.0/dTot;
      cpus.vals[i].system=(float)dSystem*100.0/dTot;
      cpus.vals[i].idle=(float)dIdle*100.0/dTot;
      cpus.vals[i].iowait=(float)dIowait*100.0/dTot;
      cpus.vals[i].irq=(float)dIrq*100.0/dTot;
      cpus.vals[i].softirq=(float)dSoftirq*100.0/dTot;
    }
    /*.......................................................................*/
    /* evaluate context switch rate */
    dCtxtCnt=ctxtCnt-oldCtxtCnt;
    if(now.tv_usec<lastTime.tv_usec)
    {
      now.tv_usec+=1000000;
      now.tv_sec-=1;
    }
    sec=now.tv_sec-lastTime.tv_sec;                       /* time difference */
    usec=now.tv_usec-lastTime.tv_usec;                    /* time difference */
    elapsedTime=(float)sec+((float)usec/(float)1000000.0);
    if(fabs(elapsedTime)<FLT_MIN)cpus.ctxtRate=0.0;
    else cpus.ctxtRate=(float)dCtxtCnt/elapsedTime;                /* ctxt/s */
  }
  /*-------------------------------------------------------------------------*/
  cpus.success=1;
  /*-------------------------------------------------------------------------*/
  return cpus;
}
/*****************************************************************************/
char *cpustatInfo(char *what)
{
  static char info[2048];
  /*-------------------------------------------------------------------------*/
  memset(info,0,sizeof(info));
  /*-------------------------------------------------------------------------*/
  if(!strcmp(what,"ls"))
  {
    snprintf(info,2048,"ctxtRate,idle,iowait,irq,nice,softirq,system,user,");
  }
  else if(!strcmp(what,"ctxtRate"))
  {
    snprintf(info,2048,"Total number of context switches per second across "
             "all CPUs.");
  }
  else if(!strcmp(what,"idle"))
  {
    snprintf(info,2048,"Time percentage the CPU has spent not working.");
  }
  else if(!strcmp(what,"iowait"))
  {
    snprintf(info,2048,"Time percentage the CPU has spent in waiting for I/O "
             "to complete.");
  }
  else if(!strcmp(what,"irq"))
  {
    snprintf(info,2048,"Time percentage the CPU has spent in servicing "
             "interrupts.");
  }
  else if(!strcmp(what,"nice"))
  {
    snprintf(info,2048,"Time percentage the CPU has spent in niced processes "
             "executing in user mode.");
  }
  else if(!strcmp(what,"softirq"))
  {
    snprintf(info,2048,"Time percentage the CPU has spent in servicing "
             "softirqs.");
  }
  else if(!strcmp(what,"system"))
  {
    snprintf(info,2048,"Time percentage the CPU has spent in processes "
             "executing in kernel mode.");
  }
  else if(!strcmp(what,"user"))
  {
    snprintf(info,2048,"Time percentage the CPU has spent in normal processes "
             "executing in user mode.");
  }
  else
  {
    snprintf(info,2048,"No informations about item: \"%s\".",what);
  }
  return info;
}
/*****************************************************************************/
/* getCpustatSensorVersion() - Returns the RCS identifier of this file.      */
/*****************************************************************************/
char *getCpustatSensorVersion()
{
  return rcsid;
}
/*****************************************************************************/
