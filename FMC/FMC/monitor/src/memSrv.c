/*****************************************************************************/
/*
 * $Log: memSrv.c,v $
 * Revision 3.1  2011/11/21 12:26:38  galli
 * usage() and shortUsage() rewritten
 *
 * Revision 3.0  2011/11/17 11:34:11  galli
 * uint32_t summary data, uint64_t details data
 *
 * Revision 2.29  2009/10/03 17:25:49  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 2.23  2008/02/28 12:06:49  galli
 * #define FLOAT_DATA and #define UINT32_DATA for PVSS compatibility
 *
 * Revision 2.22  2008/02/21 11:40:38  galli
 * bug fixed
 *
 * Revision 2.20  2008/02/21 10:07:15  galli
 * published uint64_t instead of unsigned long
 *
 * Revision 2.15  2008/02/04 07:45:45  galli
 * bug fixed
 *
 * Revision 2.14  2008/02/03 23:28:00  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 2.11  2007/10/26 23:06:11  galli
 * usage() prints FMC version
 * publishes fmc_version
 *
 * Revision 2.10  2007/09/20 10:44:59  galli
 * can get DIM_DNS_NODE from a configuration file
 * -c dim_conf_file command-line option added
 *
 * Revision 2.8  2007/08/11 21:59:28  galli
 * compatible with libFMCutils v 2
 * added function usage()
 *
 * Revision 2.7  2006/10/26 08:31:48  galli
 * const intervals between refreshes
 *
 * Revision 2.6  2006/10/23 22:31:19  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 2.5  2005/11/18 09:51:27  galli
 * publishes 2 array services instead of 33 scalar services:
 * /<HOSTNAME>/mem/name and /<HOSTNAME>/mem/data
 *
 * Revision 2.4  2005/11/10 14:43:39  galli
 * synchronous handling of signals
 * define DIM error handler
 *
 * Revision 2.1  2004/11/18 09:40:48  gregori
 * ignore SIGHUP
 *
 * Revision 1.3  2004/10/22 16:37:52  gregori
 * log msg, version msg, argument control
 *
 * Revision 1.1  2004/10/15 09:12:19  gregori
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>                                  /* EOF, snprintf(), etc. */
#include <stdlib.h>                           /* strtol(), realloc(), exit() */
#include <unistd.h>                           /* getopt(), getpid(), _exit() */
#include <signal.h>               /* pthread_sigmask(), sigtimedwait(), etc. */
#include <string.h>       /* strlen(), strncpy(), strdup(), strerror(), etc. */
#include <errno.h>                                                  /* errno */
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>                                  /* uint32_t, uint64_t */
#include <sys/wait.h>                                  /* wait(2) in usage() */
/*---------------------------------------------------------------------------*/
#include <dis.h>                                           /* for DIM server */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcMacro.h"                                             /* eExit() */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
#include "fmcDate.h"                                     /* compilation date */
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcSrvNameUtils.h"                               /* getSrvPrefix() */
#include "fmcMsgUtils.h"           /* dfltLoggerOpen(), mPrintf(), rPrintf() */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "fmcSigUtils.h"                                        /* sig2msg() */
/*---------------------------------------------------------------------------*/
/* in sensors/include */
#include "getMemFromProc.h"     /* struct memVal, readMem(), getItems(), ... */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#define _SERVER 1
#include "memSrv.h"               /* SRV_NAME, DIM_SRV_LEN, DIM_SVC_LEN, ... */
/*****************************************************************************/
/* globals */
int deBug=0;
int errU=L_DIM;                                                /* error unit */
int svcNum=0;                           /* number of services (for clean-up) */
int sSvcNum=0;
int *svcId=NULL;
int successSvcId=0;
static char rcsid[]="$Id: memSrv.c,v 3.1 2011/11/21 12:26:38 galli Exp galli $";
char *sRcsid;
char *rcsidP,*sRcsidP;
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j;
  /* pointers */
  char *p;
  unsigned long *dp;   /* pointer to mems.data. Unsigned long. Not uint64_t! */
  /* getopt variables */
  int flag=0;
  /* time refresh parameters and variables */
  int refreshPeriod=20;
  struct timespec waitTimeTS={20,0};
  time_t now,nextTime;
  /* signal handling */
  sigset_t signalMask;
  int signo=0;
  /* dim server parameters */
  char srvPath[DIM_SRV_LEN+1];
  char s[DIM_SVC_LEN+1];
  /* sensor data */
  struct memVal mems;
  /* data to be published */
  int detFieldN=0;
  int detLabelBufS=0;
  char *detLabelBuf=NULL;
  int detUnitsBufS=0;
  int detUnitsBufBS=0;
  char *detUnitsBuf=NULL;
  int detDataBufS=0;
  uint64_t *detDataBuf=NULL;
  int sumFieldN=0;
  int sumLabelBufS=0;
  int sumLabelBufBS=0;
  char *sumLabelBuf=NULL;
  int sumUnitsBufS=0;
  int sumUnitsBufBS=0;
  char *sumUnitsBuf=NULL;
  int sumDataBufS=0;
  uint32_t *sumDataBuf=NULL;
  /* dim configuration */
  char *dimDnsNode=NULL;
  /*-------------------------------------------------------------------------*/
  sRcsid=getMemSensorVersion();
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid, sRcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  sRcsidP=strchr(sRcsid,':')+2;
  for(i=0,p=sRcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt(argc,argv,"+l:dN:u:h::"))!=EOF)
  {
    switch(flag)
    {
      case 'l':
        errU=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'd':
        deBug++;
        break;
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'u':
        refreshPeriod=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'h':
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        mPrintf(L_STD|L_SYS,FATAL,__func__,0,"getopt(): invalid option "
                "\"-%c\"!",optopt);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* check command line options */
  if(optind<argc)
  {
    char msg[4096];
    char *msgP;
    msgP=msg;
    sprintf(msgP,"Invalid non-option command-line arguments: ");
    msgP=strchr(msgP,'\0');
    while(optind<argc)
    {
      sprintf(msgP,"\"%s\" ",argv[optind++]);
      msgP=strchr(msgP,'\0');
    }
    sprintf(msgP,".");
    mPrintf(L_STD|L_SYS,FATAL,__func__,0,msg);
    shortUsage();
  }
  if(argc-optind!=0)shortUsage();
  if(refreshPeriod>3600 || refreshPeriod <0)
  {
    mPrintf(L_SYS|L_STD,FATAL,__func__,0,"refresh_time_interval must be in "
            "[0,3600]");
    shortUsage();
  }
  /*-------------------------------------------------------------------------*/
  /* open error logger */
  if(errU&L_DIM)
  {
    if(dfltLoggerOpen(10,0,ERROR,VERB,0)==-1)
    {
      mPrintf(L_STD|L_SYS,FATAL,__func__,0,"Can't initialize error logger! "
              "Exiting...");
      exit(1);
    }
  }
  /*-------------------------------------------------------------------------*/
  /* set the DIM DNS node */
  if(dimDnsNode)
  {
    mPrintf(errU,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from -N command-line "
            "option).",dimDnsNode);
  }
  else
  {
    dimDnsNode=getenv("DIM_DNS_NODE");
    if(dimDnsNode)
    {
      mPrintf(errU,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from DIM_DNS_NODE "
              "environment variable).",dimDnsNode);
    }
    else
    {
      dimDnsNode=getDimDnsNode(DIM_CONF_FILE_NAME,0,1);
      if(dimDnsNode)
      {
        mPrintf(errU,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from "
                "\""DIM_CONF_FILE_NAME"\" file).",dimDnsNode);
      }
      else
      {
        mPrintf(errU,FATAL,__func__,0,"DIM Name Server (DIM_DNS_NODE) not "
                "defined!\nDIM Name Server can be defined (in decreasing "
                "order of priority):\n"
                "  1. Specifying the -N DIM_DNS_NODE command-line option;\n"
                "  2. Specifying the DIM_DNS_NODE environment variable;\n"
                "  3. Defining the DIM_DNS_NODE in the file "
                "\""DIM_CONF_FILE_NAME"\".");
        exit(1);
      }
    }
  }
  if(setenv("DIM_DNS_NODE",dimDnsNode,1))
  {
    mPrintf(errU,FATAL,__func__,0,"setenv(): %s!",strerror(errno));
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* define server name /FMC/<HOSTNAME>/memory */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /*-------------------------------------------------------------------------*/
  /* check if sensor is working */
  for(i=0;i<5;i++)
  {
    mems=readMem(deBug,errU);
    if(mems.success)break;
    dtq_sleep(1);
  }
  if(i>=5)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by readMem()! Can't "
            "initialize service!");
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate detailed fields number */
  detFieldN=getItemsN();
  /*.........................................................................*/
  /* evaluate detLabelBuf size */
  detLabelBufS=getItemsSz();
  /* fill detLabelBuf string array */
  detLabelBuf=getItems();
  /*.........................................................................*/
  /* evaluate detUnitsBuf size */
  for(i=0,detUnitsBufS=0,p=detLabelBuf;i<detFieldN;i++)
  {
    if(strncmp(p,"HugePages_",10))
      detUnitsBufS+=(1+strlen("KiB"));
    else
      detUnitsBufS+=(1+strlen(""));
    p=1+strchr(p,'\0');
  }
  /* allocate space for detUnitsBuf */
  detUnitsBuf=(char*)malloc(detUnitsBufS);
  /* fill detUnitsBuf string array */
  for(i=0,detUnitsBufBS=0,p=detLabelBuf;i<detFieldN;i++)
  {
    if(strncmp(p,"HugePages_",10))
      sprintf(detUnitsBuf+detUnitsBufBS,"%s","KiB");
    else
      sprintf(detUnitsBuf+detUnitsBufBS,"%s","");
    detUnitsBufBS+=(1+strlen(detUnitsBuf+detUnitsBufBS));
    p=1+strchr(p,'\0');
  }
  /*.........................................................................*/
  /* evaluate memory for detailed data */
  detDataBufS=detFieldN*sizeof(uint64_t);
  /* allocate memory for detailed data */
  detDataBuf=(uint64_t*)malloc(detDataBufS);
  memset(detDataBuf,0,detDataBufS);
  /*.........................................................................*/
  /* fill initial detDataBuf data */
  for(i=0;i<detFieldN;i++)
  {
    if(mems.data[i]==~1UL)detDataBuf[i]=0xfffffffffffffffe;
    else detDataBuf[i]=(uint64_t)mems.data[i];
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate summary fields number */
  sumFieldN=sizeof(sumLabelV)/sizeof(char*);
  /*.........................................................................*/
  /* evaluate sumLabelBuf size */
  for(i=0,sumLabelBufS=0;i<sumFieldN;i++)
  {
    sumLabelBufS+=(1+strlen(sumLabelV[i]));
  }
  /* allocate space for sumLabelBuf */
  sumLabelBuf=(char*)malloc(sumLabelBufS);
  /* fill sumLabelBuf string array */
  for(i=0,sumLabelBufBS=0;i<sumFieldN;i++)
  {
    sprintf(sumLabelBuf+sumLabelBufBS,"%s",sumLabelV[i]);
    sumLabelBufBS+=(1+strlen(sumLabelBuf+sumLabelBufBS));
  }
  /*.........................................................................*/
  /* evaluate sumUnitsBuf size */
  for(i=0,sumUnitsBufS=0;i<sumFieldN;i++)
  {
    sumUnitsBufS+=(1+strlen(sumUnitsV[i]));
  }
  /* allocate space for sumUnitsBuf */
  sumUnitsBuf=(char*)malloc(sumUnitsBufS);
  /* fill sumUnitsBuf string array */
  for(i=0,sumUnitsBufBS=0;i<sumFieldN;i++)
  {
    sprintf(sumUnitsBuf+sumUnitsBufBS,"%s",sumUnitsV[i]);
    sumUnitsBufBS+=(1+strlen(sumUnitsBuf+sumUnitsBufBS));
  }
  /*.........................................................................*/
  /* evaluate memory for summary data */
  sumDataBufS=sumFieldN*sizeof(uint32_t);
  /* allocate memory for summary data */
  sumDataBuf=(uint32_t*)malloc(sumDataBufS);
  memset(sumDataBuf,0,sumDataBufS);
  /*.........................................................................*/
  /* fill initial sumDataBuf data */
  for(p=detLabelBuf,dp=mems.data;p<detLabelBuf+detLabelBufS;
      p+=(1+strlen(p)),dp++)
  {
    for(i=0;i<sumFieldN;i++)
    {
      if(!strcmp(p,sumLabelV[i]))
      {
        if(*dp==~1UL)sumDataBuf[i]=~1U;
        else sumDataBuf[i]=(uint32_t)*dp;
      }
    }
  }
  /*-------------------------------------------------------------------------*/
  /* Block SIGINT, SIGTERM and SIGHUP, to be handled synchronously by        */
  /* sigtimedwait().                                                         */
  /* Signals must be blocked before the dim_init() call, which creates 2 new */
  /* threads, to keep the signals blocked in all the 3 threads.              */
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigaddset(&signalMask,SIGHUP);
  if(pthread_sigmask(SIG_BLOCK,&signalMask,NULL))eExit("pthread_sigmask()");
  /*-------------------------------------------------------------------------*/
  /* Start DIM. Here threads becomes 3 */
  dim_init();
  /*-------------------------------------------------------------------------*/
  /* define static DIM services */
  svcNum=0;
  /* define DIM service 0: /FMC/<HOSTNAME>/mem/sensor_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsidP,1+strlen(sRcsidP),0,0);
  /* define DIM service 1: /FMC/<HOSTNAME>/mem/server_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"server_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rcsidP,1+strlen(rcsidP),0,0);
  /* define DIM service 2: /FMC/<HOSTNAME>/mem/fmc_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"fmc_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  /* define DIM service 3: /FMC/<HOSTNAME>/mem/details/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",detLabelBuf,detLabelBufS,0,0);
  /* define DIM service 4: /FMC/<HOSTNAME>/mem/details/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",detUnitsBuf,detUnitsBufS,0,0);
  /* define DIM service 5: /FMC/<HOSTNAME>/mem/summary/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sumLabelBuf,sumLabelBufS,0,0);
  /* define DIM service 6: /FMC/<HOSTNAME>/mem/summary/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sumUnitsBuf,sumUnitsBufS,0,0);
  sSvcNum=svcNum;                                               /* sSvcNum=7 */
  /*-------------------------------------------------------------------------*/
  /* define dynamic DIM services */
  /* define DIM service 7: /FMC/<HOSTNAME>/mem/success */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"success");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&mems.success,sizeof(int),0,0);
  successSvcId=svcId[svcNum-1];
  /* define DIM service 8: /FMC/<HOSTNAME>/mem/summary/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",sumDataBuf,sumDataBufS,0,0);
  /* define DIM service 9: /FMC/<HOSTNAME>/mem/details/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"X",detDataBuf,detDataBufS,0,0);
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  dis_add_error_handler(dimErrorHandler);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  dis_start_serving(srvPath);
  /*-------------------------------------------------------------------------*/
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC Memory Monitor DIM "
          "Server started (TGID=%u). Using: \"%s\", \"%s\", \"FMC-%s\".",
          getpid(),rcsidP,sRcsidP,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* main loop */
  nextTime=time(NULL);
  for(j=0;;j++) 
  {
    nextTime+=refreshPeriod;
    /* get data from sensors */
    for(i=0;i<5;i++)
    {
      mems=readMem(deBug,errU);
      if(mems.success)break;
      dtq_sleep(1);
    }
    if(mems.success)
    {     
      /* compose detDataBuf */
      for(i=0;i<detFieldN;i++)
      {
        if(mems.data[i]==~1UL)detDataBuf[i]=0xfffffffffffffffe;
        else detDataBuf[i]=(uint64_t)mems.data[i];
      }
      /* compose sumDataBuf */
      for(p=detLabelBuf,dp=mems.data;p<detLabelBuf+detLabelBufS;
          p+=(1+strlen(p)),dp++)
      {
        for(i=0;i<sumFieldN;i++)
        {
          if(!strcmp(p,sumLabelV[i]))
          {
            if(*dp==~1UL)sumDataBuf[i]=~1U;
            else sumDataBuf[i]=(uint32_t)*dp;
          }
        }
      }
      /* Update services */
      for(i=sSvcNum;i<svcNum;i++)dis_update_service(svcId[i]);
      if(deBug)logData(detFieldN,detLabelBuf,mems);
    }
    else mPrintf(errU,ERROR,__func__,0,"Error flag returned by readMem()!");
    dis_update_service(successSvcId);                             /* success */
    /*.......................................................................*/
    /* sleep for waitTimeTS.tv_sec seconds, but wake-up immediately if a     */
    /* blocked signal is received, to process it (synchronously)             */
    now=time(NULL);
    if(nextTime>now)waitTimeTS.tv_sec=nextTime-now;
    else waitTimeTS.tv_sec=0;
    waitTimeTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&waitTimeTS);
    if(signo==SIGINT||signo==SIGTERM||signo==SIGHUP)signalHandler(signo);
    if(j%500==0 && j!=0)mPrintf(errU,DEBUG,__func__,0,"%d updates.",j);
    /*.......................................................................*/
  }                                                         /* for(j=0;;j++) */
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
static void logData(int detFieldN,char *detLabelBuf,struct memVal mems)
{
  int i;
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  for(i=0,p=detLabelBuf;i<detFieldN;i++,p=1+strchr(p,'\0'))
  {
    mPrintf(errU,DEBUG,__func__,0,"%14s %lu kB",p,mems.data[i]);
  }
  return;
}
/*****************************************************************************/
/* signalHandler() executed synchronously in the main thread. So don't warry */
/* about async-signal-safe.                                                  */
static void signalHandler(int signo)
{
  int i;
  /*-------------------------------------------------------------------------*/
  if(signo==SIGHUP)
  {
    mPrintf(errU|L_SYS,WARN,__func__,0,"Signal %d (%s) ignored. "
          "Continuing...",signo,sig2msg(signo));
  }
  else if(signo==SIGTERM || signo==SIGINT)
  {
    for(i=0;i<svcNum;i++)dis_remove_service(svcId[i]);
    dis_stop_serving();
    mPrintf(errU|L_SYS,WARN,__func__,0,"Signal %d (%s) caught. "
          "Exiting...",signo,sig2msg(signo));
    _exit(0);
  }
  else
  {
    for(i=0;i<svcNum;i++)dis_remove_service(svcId[i]);
    dis_stop_serving();
    mPrintf(errU|L_SYS,FATAL,__func__,0,"Signal %d (%s) caught. "
          "Exiting...",signo,sig2msg(signo));
    _exit(signo);
  }
  return;
  /*-------------------------------------------------------------------------*/
}
/* ************************************************************************* */
void dimErrorHandler(int severity,int errorCode,char *message)
{
  mPrintf(errU|L_SYS,severity+3,__func__,0,"%s! (error code = %d)",
          message,errorCode);
  if(severity==3)_exit(1);
  return;
}
/* ######################################################################### */
/* Usage Functions                                                           */
/* ######################################################################### */
void shortUsage(void)
{
  char *shortUsageStr=
"SYNOPSIS\n"
"memSrv [-d] [-N DIM_DNS_NODE] [-l LOGGER] [-u REFRESH_TIME_INTERVAL]\n"
"memSrv -h\n"
"Try \"memSrv -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  mPrintf(L_DIM|L_STD|L_SYS,INFO,__func__,0,"\n%s",shortUsageStr);
  exit(1);
}
/* ************************************************************************* */
void usage(int mode)
{
  FILE *fpOut;
  int status;
  char *formatter;
  /* ----------------------------------------------------------------------- */
  if(!mode)formatter="/usr/bin/groff -man -Tascii | /usr/bin/less -iRs";
  else formatter="/bin/cat";
  fpOut=popen(formatter,"w");
  fprintf(fpOut,
".ig\n"
"memSrv.man\n"
"\n"
"..\n"
"%s"
".hw sensor\\[hy]dictionary\\[hy]file  max\\[hy]threads\\[hy]number \n"
".TH memSrv 8  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC Servers\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis memSrv\\ \\-\n"
"FMC Memory Monitor Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis memSrv\n"
".ShortOpt[] d\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] l LOGGER\n"
".ShortOpt[] u REFRESH_TIME_INTERVAL\n"
".EndSynopsis\n"
".Synopsis memSrv\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"The FMC Memory Monitor Server, \\fBmemSrv\\fP(8), retrieves information "
"concerning the memory usage on the current farm node and publishes them "
"using DIM.\n"
".PP\n"
"The DIM Name Server, looked up by \\fBmemSrv\\fP to register the "
"provided DIM services, can be chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\\fB"DIM_CONF_FILE_NAME"\\fP.\n"
".RE\n"
".\n"
".\n"
".SH OPTIONS\n"
".\n"
".OptDef h \"\"\n"
"Get basic usage help from the command line then exit.\n"
".\n"
".OptDef d \"\"\n"
"Increase output verbosity for debugging.\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef l \"\" LOGGER (integer)\n"
"Use the logger units defined in the \\fILOGGER\\fP mask to send diagnostic\n"
"and information messages. The \\fILOGGER\\fP mask can be the bitwise\n"
"\\fIOR\\fP of the following logger units:\n"
".\n"
".RS\n"
".TP\n"
"0x1\nthe default FMC Message Logger (fifo: /tmp/logSrv.fifo);\n"
".TP\n"
"0x2\nthe standard error stream;\n"
".TP\n"
"0x4\nthe operating system syslog facility.\n"
".PP\n"
"\\fBDefault\\fP: \\fILOGGER\\fP = 0x1.\n"
".RE\n"
".\n"
".\n"
".OptDef u \"\" REFRESH_TIME_INTERVAL (integer)\n"
"Set the data refresh period to \\fIREFRESH_TIME_INTERVAL\\fP seconds. (must "
"be in the range [0,3600]s). \\fBDefault\\fP: 20 s.\n"
".\n"
".SH ENVIRONMENT\n"
".TP\n"
".EnvVar DIM_DNS_NODE \\ (string,\\ mandatory\\ if\\ not\\ defined\\ "
"otherwise,\\ see\\ above)\n"
"Host name of the node which is running the DIM DNS.\n"
".\n"
".TP\n"
".EnvVar LD_LIBRARY_PATH \\ (string,\\ mandatory\\ if\\ not\\ set\\ using\\ "
"ldconfig)\n"
"Must include the path to the libraries \"libdim\", \"libFMCutils\" and "
"\"libFMCsensors\".\n"
".\n"
".TP\n"
".EnvVar UTGID \\ (string,\\ optional)\n"
"UTGID of the \\fBmemSrv\\fP(8) process.\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Start the FMC Memory Monitor Server, sending diagnostic messages to the "
"default FMC Message Logger:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/memSrv\n"
".PP\n"
"Start the FMC Filesystem Monitor Server, sending diagnostic messages to the "
"default FMC Message Logger and to the system syslog facility:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/memSrv -l 0x5\n"
".PP\n"
"Set the delay time interval between two subsequent refreshes to four "
"seconds:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/memSrv -u 4\n"
".PP\n"
".\n"
".SH DIM INFORMATION\n"
".\n"
".SS DIM Server Name\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"\n"
"where \\fIHOSTNAME\\fP is the host name of the current PC, as returned by\n"
"the command \"hostname -s\", but written in upper case characters.\n"
".\n"
".\n"
".SS Published DIM Services\n"
".\n"
"\\fBGeneric SVCs\\fP\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/server_version\n"
"\\fBOutput format\\fP: \"C\" (NULL-terminated string).\n"
".br\n"
"Publishes a NULL-terminated string containing the RCS revision label of\n"
"the \\fBmemSrv.c\\fP source code of\n"
"the current \\fBmemSrv\\fP(8) instance, as returned by the command\n"
"\"ident /opt/FMC/sbin/memSrv\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/sensor_version\n"
"\\fBOutput format\\fP: \"C\" (NULL-terminated string).\n"
".br\n"
"Publishes a NULL-terminated string containing the RCS revision label of\n"
"the \\fBgetMemFromProc.c\\fP source code used in\n"
"the current \\fBmemSrv\\fP(8) instance, as returned by the command\n"
"\"ident /opt/FMC/lib/libFMCsensors*\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/fmc_version\n"
"\\fBOutput format\\fP: \"C\" (NULL-terminated string).\n"
".br\n"
"Publishes a NULL-terminated string containing the revision label of the FMC\n"
"package which includes the current \\fBmemSrv\\fP(8) executable, in the\n"
"present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/success\n"
"\\fBOutput format\\fP: \"I\" (integer).\n"
".br\n"
"Publishes the integer 1 if memory data are retrieved, 0 otherwise. This "
"services can be used to check whether the current instance of \\fBmemSrv\\fP "
"is up and running.\n"
".PP\n"
"\\fBSummary SVCs\\fP\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/labels\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 4 NULL-terminated strings).\n"
".br\n"
"Publishes the labels referring to the data published in the SVC "
"\".../summary/data\":\n"
".br\n"
"\"MemTotal\" \"MemFree\" \"SwapTotal\" \"SwapFree\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/units\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 4 NULL-terminated strings).\n"
".br\n"
"Publishes the units referring to the data published in the SVC "
"\".../summary/data\":\n"
".br\n"
"\"KiB\" \"KiB\" \"KiB\" \"KiB\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/data\n"
"\\fBOutput format\\fP: \"L\" (a sequence of 4 uint32_t integers).\n"
".br\n"
"Publishes the Summary Memory data.\n"
".br\n"
"The data sequence is the following:\n"
".RS\n"
".TP\n"
"\\fBMemTotal\\fP (uint32_t)\n"
"The total usable ram (i.e. physical ram minus a few reserved bits and the "
"kernel binary code).\n"
".TP\n"
"\\fBMemFree\\fP (uint32_t)\n"
"The amount of free memory.\n"
".TP\n"
"\\fBSwapTotal\\fP (uint32_t)\n"
"The total amount of physical swap memory.\n"
".TP\n"
"\\fBSwapFree\\fP (uint32_t)\n"
"The total amount of swap memory free.\n"
".RE\n"
".PP\n"
"\\fBDetailed SVCs\\fP\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/labels\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 44 NULL-terminated strings).\n"
".br\n"
"Publishes the labels referring to the data published in the SVC "
"\".../detail/data\":\n"
".br\n"
"\"Active\" \"ActiveAnon\" \"ActiveCache\" \"AnonPages\" \"Bounce\" "
"\"Buffers\" \"Cached\" \"CommitLimit\" \"Committed_AS\" \"Dirty\" "
"\"HighFree\" \"HighTotal\" \"HugePages_Free\" \"HugePages_Rsvd\" "
"\"HugePages_Total\" \"Hugepagesize\" \"Inact_clean\" \"Inact_dirty\" "
"\"Inact_laundry\" \"Inact_target\" \"Inactive\" \"LowFree\" \"LowTotal\" "
"\"LowUsed\" \"Mapped\" \"MemFree\" \"MemShared\" \"MemTotal\" \"MemUsed\" "
"\"NFS_Unstable\" \"PageTables\" \"ReverseMaps\" \"SReclaimable\" "
"\"SUnreclaim\" \"Slab\" \"SwapCached\" \"SwapFree\" \"SwapTotal\" "
"\"SwapUsed\" \"VmallocChunk\" \"VmallocTotal\" \"VmallocUsed\" "
"\"Writeback\" \"WritebackTmp\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/units\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 44 NULL-terminated strings).\n"
".br\n"
"Publishes the units referring to the data published in the SVC "
"\".../detail/data\":\n"
".br\n"
"\"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" "
"\"KiB\" \"KiB\" \"KiB\" \"\" \"\" \"\" \"KiB\" \"KiB\" \"KiB\" "
"\"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" "
"\"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" "
"\"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/data\n"
"\\fBOutput format\\fP: \"X\" (a sequence of 44 uint64_t integers).\n"
".br\n"
"Publishes the Summary Memory data.\n"
".br\n"
"The data sequence is the following:\n"
".RS\n"
".TP\n"
"\\fBActive\\fP (uint64_t)\n"
"Amount of memory that has been used more recently and usually not reclaimed "
"unless absolutely necessary.\n"
".TP\n"
"\\fBActiveAnon\\fP (uint64_t)\n"
"Amount of program memory that has been used more recently. Specific of Linux "
"kernel 2.4.\n"
".TP\n"
"\\fBActiveCache\\fP (uint64_t)\n"
"Amount of cache memory that has been used more recently. Specific of Linux "
"kernel 2.4.\n"
".TP\n"
"\\fBAnonPages\\fP (uint64_t)\n"
"Non-file backed pages mapped into userspace page tables.\n"
".TP\n"
"\\fBBounce\\fP (uint64_t)\n"
"Memory used for block device \"bounce buffers\".\n"
".TP\n"
"\\fBBuffers\\fP (uint64_t)\n"
"Relatively temporary storage for raw disk blocks shouldn't get tremendously "
"large (20MB or so).\n"
".TP\n"
"\\fBCached\\fP (uint64_t)\n"
"In-memory cache for files read from the disk (the pagecache). Doesn't "
"include SwapCached.\n"
".TP\n"
"\\fBCommitLimit\\fP (uint64_t)\n"
"Based on the overcommit ratio ('vm.overcommit_ratio'), this is the total "
"amount of memory currently available to be allocated on the system. This "
"limit is only adhered to if strict overcommit accounting is enabled (mode 2 "
"in 'vm.overcommit_memory'). The CommitLimit is calculated with the following "
"formula: CommitLimit = ('vm.overcommit_ratio' * Physical RAM) + Swap. For "
"example, on a system with 1 GiB of physical RAM and 7 GiB of swap with a "
"'vm.overcommit_ratio' of 30 it would yield a CommitLimit of 7.3 GiB.\n"
".TP\n"
"\\fBCommitted_AS\\fP (uint64_t)\n"
"The amount of memory presently allocated on the system. The committed memory "
"is a sum of all of the memory which has been allocated by processes, even if "
"it has not been \"used\" by them as of yet. A process which malloc()'s 1 GiB "
"of memory, but only touches 300 MiB of it will only show up as using 300 MiB "
"of memory even if it has the address space allocated for the entire 1 GiB. "
"This 1 GiB is memory which has been \"committed\" to by the VM and can be "
"used at any time by the allocating application. With strict overcommit "
"enabled on the system (mode 2 in 'vm.overcommit_memory'), allocations which "
"would exceed the CommitLimit (detailed above) will not be permitted. This is "
"useful if one needs to guarantee that processes will not fail due to lack of "
"memory once that memory has been successfully allocated.\n"
".TP\n"
"\\fBDirty\\fP (uint64_t)\n"
"The amount of memory which is waiting to get written back to the disk.\n"
".TP\n"
"\\fBHighFree\\fP (uint64_t)\n"
"The amount of High Memory free. High memory is all memory above ~860 MiB of "
"physical memory. High memory areas are for use by userspace programs, or for "
"the pagecache. The kernel must use tricks to access this memory, making it "
"slower to access than lowmem.\n"
".TP\n"
"\\fBHighTotal\\fP (uint64_t)\n"
"The total amount of High Memory. High memory is all memory above ~860 MiB of "
"physical memory. High memory areas are for use by userspace programs, or for "
"the pagecache. The kernel must use tricks to access this memory, making it "
"slower to access than lowmem.\n"
".TP\n"
"\\fBHugePages_Free\\fP (uint64_t)\n"
"The number of the Linux Huge Pages in the pool that are not yet allocated. "
"Hugepages is a mechanism that allows the Linux kernel to utilize the "
"multiple page size capabilities of modern hardware architectures, by using "
"\"Transaction Lookaside Buffers\" (TLB) in the CPU architecture. The default "
"page size is 4 KiB in the x86 architecture, while the Huge Page Size is "
"typically 2048 KiB.\n"
".TP\n"
"\\fBHugePages_Rsvd\\fP (uint64_t)\n"
"The number of huge pages for which a commitment to allocate from the pool "
"has been made, but no allocation has yet been made. Reserved huge pages "
"guarantee that an application will be able to allocate a huge page from the "
"pool of huge pages at fault time. Hugepages is a mechanism that allows the "
"Linux kernel to utilize the multiple page size capabilities of modern "
"hardware architectures, by using \"Transaction Lookaside Buffers\" (TLB) in "
"the CPU architecture. The default page size is 4 KiB in the x86 "
"architecture, while the Huge Page Size is typically 2048 KiB.\n"
".TP\n"
"\\fBHugePages_Total\\fP (uint64_t)\n"
"The size of the pool of huge pages. Hugepages is a mechanism that allows the "
"Linux kernel to utilize the multiple page size capabilities of modern "
"hardware architectures, by using \"Transaction Lookaside Buffers\" (TLB) in "
"the CPU architecture. The default page size is 4 KiB in the x86 "
"architecture, while the Huge Page Size is typically 2048 KiB.\n"
".TP\n"
"\\fBHugepagesize\\fP (uint64_t)\n"
"The size of a Linux Huge Page. Hugepages is a mechanism that allows the "
"Linux kernel to utilize the multiple page size capabilities of modern "
"hardware architectures, by using \"Transaction Lookaside Buffers\" (TLB) in "
"the CPU architecture. The default page size is 4 KiB in the x86 "
"architecture, while the Huge Page Size is typically 2048 KiB.\n"
".TP\n"
"\\fBInact_clean\\fP (uint64_t)\n"
"The amount of memory which has been less recently used and has been already "
"copied to the swap device. Because clean pages are already synchronized with "
"respect to their backing store, the OS can reuse Inact_clean pages without "
"having to write the page to a swap device. Specific of kernel 2.4.\n"
".TP\n"
"\\fBInact_dirty\\fP (uint64_t)\n"
"The amount of memory which has been less recently used and has not yet been "
"copied to the swap device. When memory is required, the OS chooses to steal "
"Inact_clean pages before swapping Inact_dirty pages. Specific of kernel "
"2.4.\n"
".TP\n"
"\\fBInact_laundry\\fP (uint64_t)\n"
"The amount of memory which has been less recently used and which is being "
"copied to the swap device. Specific of kernel 2.4.\n"
".TP\n"
"\\fBInact_target\\fP (uint64_t)\n"
"The amount of memory which OS ought to have, to be possibly reclaimed for "
"other purposes, calculated as the sum of Active, Inact_dirty, and "
"Inact_clean divided by 5. When exceeded, the kernel will not do work to move "
"pages from active to inactive. This is an indicator of when page cleaning "
"should be performed. Specific of kernel 2.4.\n"
".TP\n"
"\\fBInactive\\fP (uint64_t)\n"
"The amount of memory which has been less recently used. It is more eligible "
"to be reclaimed for other purposes. Since kernel 2.5.41+. In kernel 2.4 this "
"value is the sum Inact_dirty + Inact_laundry + Inact_clean.\n"
".TP\n"
"\\fBLowFree\\fP (uint64_t)\n"
"The amount of low memory free. Low memory is memory which can be used for "
"everything that high memory can be used for, but it is also available for "
"the kernel's use for its own data structures. Among many other things, it is "
"where everything from the Slab is allocated. Bad things happen when you're "
"out of low memory.\n"
".TP\n"
"\\fBLowTotal\\fP (uint64_t)\n"
"The total amount of low memory. Low memory is memory which can be used for "
"everything that high memory can be used for, but it is also available for "
"the kernel's use for its own data structures. Among many other things, it is "
"where everything from the Slab is allocated. Bad things happen when you're "
"out of low memory.\n"
".TP\n"
"\\fBLowUsed\\fP (uint64_t)\n"
"The amount of low memory used. Low memory is memory which can be used for "
"everything that high memory can be used for, but it is also available for "
"the kernel's use for its own data structures. Among many other things, it is "
"where everything from the Slab is allocated. Bad things happen when you're "
"out of low memory.\n"
".TP\n"
"\\fBMapped\\fP (uint64_t)\n"
"The amount of memory used to store files which have been mmapped, such as "
"libraries. Since kernel 2.5.41+..\n"
".TP\n"
"\\fBMemFree\\fP (uint64_t)\n"
"The amount of free memory (The sum of LowFree + HighFree).\n"
".TP\n"
"\\fBMemShared\\fP (uint64_t)\n"
"The amount of memory shared between processes. Set to zero since kernel 2.4.\n"
".TP\n"
"\\fBMemTotal\\fP (uint64_t)\n"
"The total usable ram (i.e. physical ram minus a few reserved bits and the "
"kernel binary code). = LowTotal + HighTotal.\n"
".TP\n"
"\\fBMemUsed\\fP (uint64_t)\n"
"The amount of used memory (The difference MemTotal - MemFree).\n"
".TP\n"
"\\fBNFS_Unstable\\fP (uint64_t)\n"
"NFS pages sent to the server, but not yet committed to stable storage.\n"
".TP\n"
"\\fBPageTables\\fP (uint64_t)\n"
"Amount of memory dedicated to the lowest level of page tables.\n"
".TP\n"
"\\fBReverseMaps\\fP (uint64_t)\n"
"The number of reverse mappings performed.\n"
".TP\n"
"\\fBSReclaimable\\fP (uint64_t)\n"
"Part of Slab, that might be reclaimed, such as caches.\n"
".TP\n"
"\\fBSUnreclaim\\fP (uint64_t)\n"
"Part of Slab, that cannot be reclaimed on memory pressure.\n"
".TP\n"
"\\fBSlab\\fP (uint64_t)\n"
"The amount of memory used for in-kernel data structures cache. Since kernel "
"2.5.41+.\n"
".TP\n"
"\\fBSwapCached\\fP (uint64_t)\n"
"Amount of memory that once was swapped out, is swapped back in but still "
"also is in the swapfile (if memory is needed it doesn't need to be swapped "
"out AGAIN because it is already in the swapfile. This saves I/O).\n"
".TP\n"
"\\fBSwapFree\\fP (uint64_t)\n"
"The amount of swap memory free. Swap space is memory which has been "
"evicted from RAM, and is temporarily on the disk.\n"
".TP\n"
"\\fBSwapTotal\\fP (uint64_t)\n"
"The total amount of physical swap space available. Swap space is memory "
"which has been evicted from RAM, and is temporarily on the disk.\n"
".TP\n"
"\\fBSwapUsed\\fP (uint64_t)\n"
"The amount of swap space used. Swap space is memory which has been "
"evicted from RAM, and is temporarily on the disk.\n"
".TP\n"
"\\fBVmallocChunk\\fP (uint64_t)\n"
"Largest contiguous block of vmalloc area which is free. Vmalloc area is a "
"memory area which is seen by the kernel as a contiguous range of addresses "
"in the virtual address space, although the pages are not consecutive in "
"physical memory.\n"
".TP\n"
"\\fBVmallocTotal\\fP (uint64_t)\n"
"Total size of vmalloc memory area. Vmalloc area is a memory area which is "
"seen by the kernel as a contiguous range of addresses in the virtual address "
"space, although the pages are not consecutive in physical memory.\n"
".TP\n"
"\\fBVmallocUsed\\fP (uint64_t)\n"
"Amount of vmalloc memory area which is used. Vmalloc area is a memory area "
"which is seen by the kernel as a contiguous range of addresses in the "
"virtual address space, although the pages are not consecutive in physical "
"memory.\n"
".TP\n"
"\\fBWriteback\\fP (uint64_t)\n"
"Amount of memory which is actively being written back to the disk.\n"
".TP\n"
"\\fBWritebackTmp\\fP (uint64_t)\n"
"Memory used by FUSE for temporary writeback buffers.\n"
".RE\n"
".\n"
".SH AVAILABILITY\n"
".\n"
"Information on how to get the FMC (Farm Monitoring and Control System)\n"
"package and related information is available at the web sites:\n"
".PP\n"
".ad l\n"
".URL https://\\:lhcbweb.bo.infn.it/\\:twiki\\:/\\:bin/\\:view.cgi/\\:"
"LHCbBologna/\\:FmcLinux \"FMC Linux Development Homepage\".\n"
".PP\n"
".URL http://\\:itcobe\\:.\\:web\\:.\\:cern\\:.\\:ch\\:/\\:itcobe/\\:"
"Projects/\\:Framework/\\:"
"Download/\\:Components/\\:SystemOverview\\:/\\:fwFMC/\\:welcome.html "
"\"CERN ITCOBE: FW Farm Monitor and Control\".\n"
".PP\n"
".URL http://\\:lhcb-daq.web.cern.ch/\\:lhcb-daq/\\:online-rpm-repo\\:/\\:"
"index.html \"LHCb Online RPMs\".\n"
".ad b\n"
".PP\n"
".\n"
".\n"
".SH AUTHORS\n"
".\n"
".MTO domenico.galli@bo.infn.it \"Domenico Galli\"\n"
".\n"
".\n"
".SH VERSION\n"
".\n"
"Source:  %s\n"
".br\n"
"Source:  %s\n"
".br\n"
"Package: FMC-%s\n"
".\n"
".SH SEE ALSO\n"
".ad l\n"
".BR \\%%memViewer (1),\n"
".br\n"
".BR \\%%vmstat (8),\n"
".BR \\%%ps (1),\n"
".BR \\%%top (1),\n"
".BR \\%%/proc/meminfo,\n"
".br\n"
".BR \\%%http://www.kernel.org/doc/Documentation/filesystems/proc.txt.\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro
,rcsidP,sRcsidP,FMC_VERSION
,rcsidP,sRcsidP,FMC_VERSION
       );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(0);
}
/*****************************************************************************/
