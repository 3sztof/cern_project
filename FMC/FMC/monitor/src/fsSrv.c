/* ######################################################################### */
/*
 * $Log: fsSrv.c,v $
 * Revision 2.4  2011/11/08 15:27:47  galli
 * Bug fixed (file size overrun on 32-bit machines)
 *
 * Revision 2.3  2011/11/07 11:31:33  galli
 * man pages updated
 *
 * Revision 2.2  2011/11/03 09:09:40  galli
 * Rewritten from scratch.
 * Cope with stalled nfs file systems.
 * statfs(2) called in short living threads.
 * float summary data, uint64_t detailed data.
 * File system status published in detailed data.
 *
 * Revision 1.31  2011/09/27 11:45:57  galli
 * max refresh period 60 s -> 3600 s
 *
 * Revision 1.29  2009/10/03 16:11:40  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 1.19  2008/02/28 09:04:19  galli
 * #define FLOAT_DATA for PVSS compatibility
 *
 * Revision 1.18  2008/02/21 11:16:17  galli
 * bug fixed
 *
 * Revision 1.14  2008/02/16 00:20:14  galli
 * clean-up
 *
 * Revision 1.13  2008/02/15 23:37:51  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 1.7  2007/10/26 22:32:31  galli
 * usage() prints FMC version
 * publishes fmc_version
 *
 * Revision 1.6  2007/09/20 10:32:00  galli
 * can get DIM_DNS_NODE from a configuration file
 * -c dim_conf_file command-line option added
 *
 * Revision 1.3  2007/08/10 08:44:35  galli
 * compatible with libFMCutils v 2
 *
 * Revision 1.2  2006/10/26 09:41:45  galli
 * compatible with libFMCutils v 1.0
 * const intervals between refreshes
 *
 * Revision 1.1  2006/08/03 09:17:36  galli
 * Initial revision
 */
/* ######################################################################### */
/*  DIM SRV:                                                                 */
/*    /FMC/<HOSTNAME>/filesystems                                            */
/*  DIM SVC:                                                                 */
/*    /FMC/<HOSTNAME>/filesystems/server_version        C                    */
/*      string                                                               */
/*    /FMC/<HOSTNAME>/filesystems/sensor_version        C                    */
/*      string                                                               */
/*    /FMC/<HOSTNAME>/filesystems/fmc_version           C                    */
/*      string                                                               */
/*    /FMC/<HOSTNAME>/filesystems/success               I                    */
/*      int                                                                  */
/*    /FMC/<HOSTNAME>/filesystems/devices               C                    */
/*      string[]                                                             */
/*    /FMC/<HOSTNAME>/filesystems/summary/labels        C                    */
/*      string[4]                                                            */
/*    /FMC/<HOSTNAME>/filesystems/summary/units         C                    */
/*      string[4]                                                            */
/*    /FMC/<HOSTNAME>/filesystems/summary/<device>/data F:2;C                */
/*      float[2],string[2]                                                   */
/*    /FMC/<HOSTNAME>/filesystems/details/labels        C                    */
/*      string[12]                                                           */
/*    /FMC/<HOSTNAME>/filesystems/details/units         C                    */
/*      string[12]                                                           */
/*    /FMC/<HOSTNAME>/filesystems/details/<device>/data X:6;F:2;C            */
/*      unit64_t[6],float[2],string[5]                                       */
/* ######################################################################### */
/* Location of DIM calls:                                                    */
/* dis_add_service():                                                        */
/*   main()                                                                  */
/*   fslAdd()                                                                */
/* dis_remove_service():                                                     */
/*   fslRm()                                                                 */
/*   signalHandler()                                                         */
/* dis_start_serving():                                                      */
/*   main()                                                                  */
/*   fslAdd()                                                                */
/*   fslRm()                                                                 */
/* dis_update_service():                                                     */
/*   main()                                                                  */
/* dis_add_error_handler():                                                  */
/*   main()                                                                  */
/* dis_stop_serving():                                                       */
/*   signalHandler()                                                         */
/* ######################################################################### */
/* deBug mask:                                                               */
/*   0x0001 print in/out functions                                           */
/*   0x0002 print active threads after timeout                               */
/*   0x0004 print file system data                                           */
/*   0x0008 print update handler calls                                       */
/* ######################################################################### */
#include <stdio.h>                                  /* EOF, snprintf(), etc. */
#include <stdlib.h>                           /* strtol(), realloc(), exit() */
#include <unistd.h>                           /* getopt(), getpid(), _exit() */
#include <signal.h>               /* pthread_sigmask(), sigtimedwait(), etc. */
#include <string.h>       /* strlen(), strncpy(), strdup(), strerror(), etc. */
#include <errno.h>                                                  /* errno */
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>                                            /* uint64_t */
#include <sys/wait.h>         /* waitpid(), WIFEXITED(), WEXITSTATUS(), etc. */
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
#include "getFS.h"                  /* fsAttr_t, fsAttrs_t, getFileSystems() */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#define _SERVER 1
#include "fsSrv.h"                /* SRV_NAME, DIM_SRV_LEN, DIM_SVC_LEN, ... */
/* ######################################################################### */
/* globals */
int deBug=0x0;                                                 /* debug mask */
int errU=L_DIM;                                                /* error unit */
int svcNum=0;
int sSvcNum=0;
int *svcId=NULL;
int devicesSvcId=0;
int successSvcId=0;
static char rcsid[]="$Id: fsSrv.c,v 2.4 2011/11/08 15:27:47 galli Exp galli $";
char *sRcsid;
char *rcsidP,*sRcsidP;
char srvPath[DIM_SRV_LEN+1]="";
/* thread identifier of the main thread */
pthread_t main_ptid;
/* file system list with usage */
fs_list_t fsList={NULL,NULL,0};
/* lock for cuncurrent data access */
pthread_mutex_t usageLock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t globalLock=PTHREAD_MUTEX_INITIALIZER;
/* ************************************************************************* */
int main(int argc,char **argv)
{
  /* counters */
  int i;
  /* pointers */
  char *p;
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
  char s[DIM_SVC_LEN+1]="";
  /* data to be published */
  int sumFieldN=0;
  int detFieldN=0;
  int sumLabelBufS=0;
  int sumLabelBufBS=0;
  char *sumLabelBuf=NULL;
  int sumUnitsBufS=0;
  int sumUnitsBufBS=0;
  char *sumUnitsBuf=NULL;
  int detLabelBufS=0;
  int detLabelBufBS=0;
  char *detLabelBuf=NULL;
  int detUnitsBufS=0;
  int detUnitsBufBS=0;
  char *detUnitsBuf=NULL;
  /* dim configuration */
  char *dimDnsNode=NULL;
  /* command-line options */
  int doGetDummyFs=0;
  int doGetRemoteFs=0;
  /* filesystem data */
  fs_data_t *fs=NULL;
  int fsN;
  /* thread timeout handling */
  long long thrToMax;                    /* max timeout for thread execution */
  long long thrTo;                   /* dynamic timeout for thread execution */
  long long thrToMin=1000000LL;   /* min timeout for thread execution = 1 ms */
  struct timespec delay;
  /*-------------------------------------------------------------------------*/
  sRcsid=getFSsensorVersion();
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
  while((flag=getopt(argc,argv,"+l:d:N:u:aDh::"))!=EOF)
  {
    switch(flag)
    {
      case 'l':
        errU=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'd':
        deBug=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'u':
        refreshPeriod=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'a':
        doGetRemoteFs=1;
        break;
      case 'D':
        doGetDummyFs=1;
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
  /* define server name /FMC/<HOSTNAME>/fs */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /*-------------------------------------------------------------------------*/
  /* evaluate fields number */
  sumFieldN=sizeof(sumLabelV)/sizeof(char*);
  detFieldN=sizeof(detLabelV)/sizeof(char*);
  /* evaluate sumLabelBuf, sumUnitsBuf, detLabelBuf and detUnitsBuf size */
  for(i=0,sumLabelBufS=0,sumUnitsBufS=0;i<sumFieldN;i++)
  {
    sumLabelBufS+=(1+strlen(sumLabelV[i]));
    sumUnitsBufS+=(1+strlen(sumUnitsV[i]));
  }
  for(i=0,detLabelBufS=0,detUnitsBufS=0;i<detFieldN;i++)
  {
    detLabelBufS+=(1+strlen(detLabelV[i]));
    detUnitsBufS+=(1+strlen(detUnitsV[i]));
  }
  /* allocate space for sumLabelBuf, sumUnitsBuf, detLabelBuf and detUnitsBuf */
  sumLabelBuf=(char*)malloc(sumLabelBufS);
  sumUnitsBuf=(char*)malloc(sumUnitsBufS);
  detLabelBuf=(char*)malloc(detLabelBufS);
  detUnitsBuf=(char*)malloc(detUnitsBufS);
  /* fill sumLabelBuf, sumUnitsBuf, detLabelBuf and detUnitsBuf string arrays */
  for(i=0,sumLabelBufBS=0,sumUnitsBufBS=0;i<sumFieldN;i++)
  {
    sprintf(sumLabelBuf+sumLabelBufBS,"%s",sumLabelV[i]);
    sumLabelBufBS+=(1+strlen(sumLabelBuf+sumLabelBufBS));
    sprintf(sumUnitsBuf+sumUnitsBufBS,"%s",sumUnitsV[i]);
    sumUnitsBufBS+=(1+strlen(sumUnitsBuf+sumUnitsBufBS));
  }
  for(i=0,detLabelBufBS=0,detUnitsBufBS=0;i<detFieldN;i++)
  {
    sprintf(detLabelBuf+detLabelBufBS,"%s",detLabelV[i]);
    detLabelBufBS+=(1+strlen(detLabelBuf+detLabelBufBS));
    sprintf(detUnitsBuf+detUnitsBufBS,"%s",detUnitsV[i]);
    detUnitsBufBS+=(1+strlen(detUnitsBuf+detUnitsBufBS));
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
  svcNum=0;
  /* define static DIM services */
  /* define DIM service 0: /FMC/<HOSTNAME>/filesystems/server_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"server_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rcsidP,strlen(rcsidP)+1,0,0);
  /* define DIM service 1: /FMC/<HOSTNAME>/filesystems/sensor_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsidP,strlen(sRcsidP)+1,0,0);
  /* define DIM service 2: /FMC/<HOSTNAME>/filesystems/fmc_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"fmc_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",FMC_VERSION,
                                  strlen(FMC_VERSION)+1,0,0);
  /* define DIM service 3: /FMC/<HOSTNAME>/filesystems/summary/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sumLabelBuf,sumLabelBufS,0,0);
  /* define DIM service 4: /FMC/<HOSTNAME>/filesystems/summary/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sumUnitsBuf,sumUnitsBufS,0,0);
  /* define DIM service 5: /FMC/<HOSTNAME>/filesystems/details/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",detLabelBuf,detLabelBufS,0,0);
  /* define DIM service 6: /FMC/<HOSTNAME>/filesystems/details/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",detUnitsBuf,detUnitsBufS,0,0);
  /*-------------------------------------------------------------------------*/
  sSvcNum=svcNum;
  /* define variable DIM services */
  /* define DIM service 7: /FMC/<HOSTNAME>/filesystems/success */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"success");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&fsList.success,sizeof(int),0,
                                  0);
  successSvcId=svcId[svcNum-1];
  /* define DIM service 8: /FMC/<HOSTNAME>/filesystems/devices */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"devices");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,devicesHandler,0);
  devicesSvcId=svcId[svcNum-1];
  /* (dynamic DIM services are defined and removed in fslAdd() and fslRm()) */
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  dis_add_error_handler(dimErrorHandler);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  dis_start_serving(srvPath);
  /*-------------------------------------------------------------------------*/
  /* save the thread identifier of the main thread                           */
  main_ptid=pthread_self();
  /*-------------------------------------------------------------------------*/
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC File System Monitor DIM Server "
          "started (TGID=%u). Using: \"%s\", \"%s\", \"FMC-%s\".",getpid(),
          rcsidP,sRcsidP,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* set timeout for short living threads */
  thrToMax=refreshPeriod-2;
  if(thrToMax<1)thrToMax=1;
  thrToMax*=1000000000LL;                                         /* s -> ns */
  /*-------------------------------------------------------------------------*/
  /* main loop */
  nextTime=time(NULL);
  for(i=0;;i++)
  {
    nextTime+=refreshPeriod;
    /*.......................................................................*/
    updateFsList(doGetDummyFs,doGetRemoteFs);
    for(fs=fsList.head,fsN=0;fs;fs=fs->next)
    {
      fsN++;
      /* get fs usage */
      startGetFsUsage(fs);
    }
    /* ..................................................................... */
    /* wait for thread termination */
    for(thrTo=thrToMin;thrTo<thrToMax/2;thrTo*=2)
    {
      int cnt=0;                                      /* busy thread counter */
      long long to=thrTo;
      /* convert to: ns -> timespec */
      delay.tv_sec=0;
      while(to>999999999LL)
      {
        delay.tv_sec++;
        to-=1000000000LL;
      }
      delay.tv_nsec=(long)to;
      /* wait */
      signo=sigtimedwait(&signalMask,NULL,&delay);
      if(signo==SIGINT||signo==SIGTERM||signo==SIGHUP)signalHandler(signo);
      /* check if all threads have done */
      for(fs=fsList.head;fs;fs=fs->next)
      {
        pthread_mutex_lock(&usageLock);
        if(fs->usage.busy==1)cnt++;
        pthread_mutex_unlock(&usageLock);
      }
      if(cnt==0)break;                              /* all threads have done */
      if(deBug&0x2)
      {
        mPrintf(errU,VERB,__func__,0,"%d active thr after %7.2e s.",
                cnt,(float)thrTo*1e-9);
      }
    }
    /* ..................................................................... */
    updateSuccessFlag();
    dis_update_service(successSvcId);
    dis_update_service(devicesSvcId);
    for(fs=fsList.head;fs;fs=fs->next)
    {
      dis_update_service(fs->sumSvcId);
      dis_update_service(fs->detSvcId);
    }
    /* ..................................................................... */
    /* print fs usage */
    if(deBug&0x0004)fslPrint();
    /*.......................................................................*/
    /* sleep for waitTimeTS.tv_sec seconds, but wake-up immediately if a     */
    /* blocked signal is received, to process it (synchronously)             */
    now=time(NULL);
    if(nextTime>now)waitTimeTS.tv_sec=nextTime-now;
    else waitTimeTS.tv_sec=0;
    waitTimeTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&waitTimeTS);
    if(signo==SIGINT||signo==SIGTERM||signo==SIGHUP)signalHandler(signo);
    if(i%500==0 && i!=0)mPrintf(errU,DEBUG,__func__,0,"%d updates.",i);
    /*.......................................................................*/
  }
  return 0;
} 
/*****************************************************************************/
/* devicesHandler()                                                          */
/* Update device list data service                                           */
/* SVC: "/FMC/<HOSTNAME>/filesystems/devices"                                */
/* DIM Format: "C"                                                           */
/* DATA: list of filesystem Ids: fs_00, fs_01, fs_02, ...                    */
/* WARNING: devicesHandler() accesses global variable fsList                 */
/*****************************************************************************/
void devicesHandler(long *tag,int **address,int *size)
{
  char device[DEVICE_LEN+1];
  static char *deviceBuf=NULL;                        /* device buffer       */
  static int deviceBufS=0;                            /* deviceBuf size      */
  int deviceBufBS=0;                                  /* deviceBuf busy size */
  fs_data_t *fs=NULL;
  /*-------------------------------------------------------------------------*/
  /* evaluate deviceBuf size */
  for(fs=fsList.head,deviceBufBS=0;fs;fs=fs->next)
  {
    deviceBufBS+=1+snprintf(NULL,0,"fs_%02d",fs->id);
  }
  /* allocate memory for deviceBuf */
  if(deviceBufS<deviceBufBS)
  {
    deviceBufS=deviceBufBS;
    deviceBuf=(char*)realloc(deviceBuf,deviceBufS);
  }
  /* fill deviceBuf string array */
  for(fs=fsList.head,deviceBufBS=0;fs;fs=fs->next)
  {
    snprintf(device,DEVICE_LEN+1,"fs_%02d",fs->id);
    sprintf(deviceBuf+deviceBufBS,"%s",device);
    deviceBufBS+=(1+strlen(deviceBuf+deviceBufBS));
  }
  /*-------------------------------------------------------------------------*/
  *address=(int*)deviceBuf;
  *size=deviceBufBS;
  printOutFuncOK;
  return;
}
/*****************************************************************************/
/* fsSumHandler()                                                            */
/* Update summary data services:                                             */
/* SVC: "/FMC/<HOSTNAME>/filesystems/summary/<ID>/data"                      */
/* DIM Format: "F:2;C"                                                       */
/* DATA: totalUser (F), usedUserP (F), name (C), mp (C).                     */
/* WARNING: fsSumHandler() accesses global variable fsList                   */
/*****************************************************************************/
void fsSumHandler(long *tag,int **address,int *size)
{
  fs_data_t *fs=NULL;
  static char *dataBuf=NULL;                            /* data buffer       */
  static int dataBufS=0;                                /* dataBuf size      */
  int dataBufBS=0;                                      /* dataBuf busy size */
  /*-------------------------------------------------------------------------*/
  /* Lock mutex if TIMED update (this function is running in DIM I/O thread) */
  /* Do not lock mutex if MONITORED update (this function is running on the  */
  /* main thread) because it is already locked by fsUpdate()                 */
  if(!pthread_equal(pthread_self(),main_ptid))
  {
    if(pthread_mutex_lock(&globalLock))eExit("pthread_mutex_lock()");
    if(deBug&0x9)mPrintf(errU,DEBUG,__func__,0,"entered locked, id=%lu.",*tag);
  }
  else
  {
    if(deBug&0x9)
      mPrintf(errU,DEBUG,__func__,0,"entered unlocked, id=%lu.",*tag);
  }
  /*-------------------------------------------------------------------------*/
  for(fs=fsList.head;fs;fs=fs->next)
  {
    if(fs->id==*tag)
    {
      float totalUser;
      float totalRoot;
      float usedUserP;
      /*.....................................................................*/
      /* evaluate totalRoot and usedUserP */
      totalUser=(float)fs->usage.totalUser;
      totalRoot=(float)fs->usage.totalRoot;
      if(fs->usage.totalUser==0)usedUserP=0.;
      else usedUserP=(float)fs->usage.used*100./totalUser;
      /*.....................................................................*/
      /* evaluate dataBuf size */
      dataBufBS=2*sizeof(float)+1+strlen(fs->name)+1+strlen(fs->mp);
      /*.....................................................................*/
      /* allocate memory for dataBuf */
      if(dataBufS<dataBufBS)
      {
        dataBufS=dataBufBS;
        dataBuf=(char*)realloc(dataBuf,dataBufS);
      }
      /*.....................................................................*/
      /* fill dataBuf mixed array */
      dataBufBS=0;
      memcpy(dataBuf+dataBufBS,&totalRoot,sizeof(float));
      dataBufBS+=sizeof(float);
      memcpy(dataBuf+dataBufBS,&usedUserP,sizeof(float));
      dataBufBS+=sizeof(float);
      sprintf(dataBuf+dataBufBS,"%s",fs->name);
      dataBufBS+=(1+strlen(dataBuf+dataBufBS));
      sprintf(dataBuf+dataBufBS,"%s",fs->mp);
      dataBufBS+=(1+strlen(dataBuf+dataBufBS));
      /*.....................................................................*/
    }
  }
  /*-------------------------------------------------------------------------*/
  if(!pthread_equal(pthread_self(),main_ptid))
  {
    if(pthread_mutex_unlock(&globalLock))eExit("pthread_mutex_unlock()");
  }
  /*-------------------------------------------------------------------------*/
  *address=(int*)dataBuf;
  *size=dataBufBS;
  return;
}
/* ************************************************************************* */
/* fsDetHandler()                                                            */
/* Update detailed data services:                                            */
/* SVC: "/FMC/<HOSTNAME>/filesystems/details/<ID>/data"                      */
/* Update detailed data services                                             */
/* DIM Format: "F:2;C"                                                       */
/* DATA: totalUser (F), usedUserP (F), name (C), mp (C).                     */
/* WARNING: fsDetHandler() accesses global variable fsList                   */
/* ************************************************************************* */
void fsDetHandler(long *tag,int **address,int *size)
{
  fs_data_t *fs=NULL;
  static char *dataBuf=NULL;                            /* data buffer       */
  static int dataBufS=0;                                /* dataBuf size      */
  int dataBufBS=0;                                      /* dataBuf busy size */
  struct tm lastUpdateL;
  char lastUpdateS[TIME_LEN+1];                            /* Oct12-134923\0 */
  /* ----------------------------------------------------------------------- */
  /* Lock mutex if TIMED update (this function is running in DIM I/O thread) */
  /* Do not lock mutex if MONITORED update (this function is running on the  */
  /* main thread) because it is already locked by fsUpdate()                 */
  if(!pthread_equal(pthread_self(),main_ptid))
  {
    if(pthread_mutex_lock(&globalLock))eExit("pthread_mutex_lock()");
    if(deBug&0x9)mPrintf(errU,DEBUG,__func__,0,"entered locked, id=%lu.",*tag);
  }
  else
  {
    if(deBug&0x9)
      mPrintf(errU,DEBUG,__func__,0,"entered unlocked, id=%lu.",*tag);
  }
  /* ----------------------------------------------------------------------- */
  for(fs=fsList.head;fs;fs=fs->next)
  {
    if(fs->id==*tag)
    {
      uint64_t minfree;
      uint64_t totalRoot;
      uint64_t totalUser;
      uint64_t used;
      uint64_t availRoot;
      uint64_t availUser;
      float usedUserP;
      float usedRootP;
      char status[128];                  /* OK, ERROR:..., STALLED since X s */
      /* ................................................................... */
      /* evaluate data to publish */
      minfree=(uint64_t)fs->usage.minfree;
      totalRoot=(uint64_t)fs->usage.totalRoot;
      totalUser=(uint64_t)fs->usage.totalUser;
      used=(uint64_t)fs->usage.used;
      availRoot=(uint64_t)fs->usage.availRoot;
      availUser=(uint64_t)fs->usage.availUser;
      if(totalRoot==0)usedRootP=0.;
      else usedRootP=(float)used*100./(float)totalRoot;
      if(totalUser==0)usedUserP=0.;
      else usedUserP=(float)used*100./(float)totalUser;
      if(fs->usage.stalled && fs->usage.busy)
      {
        snprintf(status,128,"STALLED since %ld s",fs->usage.stalled);
      }
      else if(fs->usage.rv)
      {
        snprintf(status,128,"ERROR: %s",strerror(fs->usage.err));
      }
      else 
      {
        strcpy(status,"OK");
      }
      /* ................................................................... */
      localtime_r(&fs->usage.lastUpdate,&lastUpdateL);
      strftime(lastUpdateS,TIME_LEN+1,"%b%d-%H%M%S",&lastUpdateL);
      /* ................................................................... */
      /* evaluate dataBuf size */
      dataBufBS=6*sizeof(uint64_t)+2*sizeof(float)+1+strlen(fs->name)+
                1+strlen(fs->type)+1+strlen(fs->mp)+1+strlen(lastUpdateS)+
                1+strlen(status);
      /* ................................................................... */
      /* allocate memory for dataBuf */
      if(dataBufS<dataBufBS)
      {
        dataBufS=dataBufBS;
        dataBuf=(char*)realloc(dataBuf,dataBufS);
      }
      /* ................................................................... */
      /* fill dataBuf mixed array */
      dataBufBS=0;
      memcpy(dataBuf+dataBufBS,&minfree,sizeof(uint64_t));
      dataBufBS+=sizeof(uint64_t);
      memcpy(dataBuf+dataBufBS,&totalRoot,sizeof(uint64_t));
      dataBufBS+=sizeof(uint64_t);
      memcpy(dataBuf+dataBufBS,&totalUser,sizeof(uint64_t));
      dataBufBS+=sizeof(uint64_t);
      memcpy(dataBuf+dataBufBS,&used,sizeof(uint64_t));
      dataBufBS+=sizeof(uint64_t);
      memcpy(dataBuf+dataBufBS,&availRoot,sizeof(uint64_t));
      dataBufBS+=sizeof(uint64_t);
      memcpy(dataBuf+dataBufBS,&availUser,sizeof(uint64_t));
      dataBufBS+=sizeof(uint64_t);
      memcpy(dataBuf+dataBufBS,&usedRootP,sizeof(float));
      dataBufBS+=sizeof(float);
      memcpy(dataBuf+dataBufBS,&usedUserP,sizeof(float));
      dataBufBS+=sizeof(float);
      sprintf(dataBuf+dataBufBS,"%s",fs->name);
      dataBufBS+=(1+strlen(dataBuf+dataBufBS));
      sprintf(dataBuf+dataBufBS,"%s",fs->type);
      dataBufBS+=(1+strlen(dataBuf+dataBufBS));
      sprintf(dataBuf+dataBufBS,"%s",fs->mp);
      dataBufBS+=(1+strlen(dataBuf+dataBufBS));
      sprintf(dataBuf+dataBufBS,"%s",lastUpdateS);
      dataBufBS+=(1+strlen(dataBuf+dataBufBS));
      sprintf(dataBuf+dataBufBS,"%s",status);
      dataBufBS+=(1+strlen(dataBuf+dataBufBS));
      /* ................................................................... */
    }
  }
  /* ----------------------------------------------------------------------- */
  if(!pthread_equal(pthread_self(),main_ptid))
  {
    if(pthread_mutex_unlock(&globalLock))eExit("pthread_mutex_unlock()");
  }
  /* ----------------------------------------------------------------------- */
  *address=(int*)dataBuf;
  *size=dataBufBS;
  return;
}
/* ************************************************************************* */
/* signalHandler() executed synchronously in the main thread. So don't warry */
/* about async-signal-safe.                                                  */
/* ************************************************************************* */
static void signalHandler(int signo)
{
  int i;
  fs_data_t *el;
  /*-------------------------------------------------------------------------*/
  if(signo==SIGHUP)
  {
    mPrintf(errU|L_SYS,WARN,__func__,0,"Signal %d (%s) ignored. "
            "Continuing...",signo,sig2msg(signo));
  }
  else if(signo==SIGTERM || signo==SIGINT)
  {
    for(i=0;i<svcNum;i++)dis_remove_service(svcId[i]);
    if(pthread_mutex_lock(&globalLock))eExit("pthread_mutex_lock()");
    for(el=fsList.head;el;el=el->next)
    {
      dis_remove_service(el->sumSvcId);
      dis_remove_service(el->detSvcId);
    }
    if(pthread_mutex_unlock(&globalLock))eExit("pthread_mutex_unlock()");
    dis_stop_serving();
    mPrintf(errU|L_SYS,WARN,__func__,0,"Signal %d (%s) caught. "
          "Exiting...",signo,sig2msg(signo));
    _exit(0);
  }
  else
  {
    for(i=0;i<svcNum;i++)dis_remove_service(svcId[i]);
    if(pthread_mutex_lock(&globalLock))eExit("pthread_mutex_lock()");
    for(el=fsList.head;el;el=el->next)
    {
      dis_remove_service(el->sumSvcId);
      dis_remove_service(el->detSvcId);
    }
    if(pthread_mutex_unlock(&globalLock))eExit("pthread_mutex_unlock()");
    dis_stop_serving();
    mPrintf(errU|L_SYS,FATAL,__func__,0,"Signal %d (%s) caught. "
          "Exiting...",signo,sig2msg(signo));
    _exit(signo);
  }
  return;
}
/*****************************************************************************/
void dimErrorHandler(int severity,int errorCode,char *message)
{
  mPrintf(errU|L_SYS,severity+3,__func__,0,"%s! (error code = %d)",
          message,errorCode);
  if(severity==3)_exit(1);
  return;
}
/* ######################################################################### */
/* Data access functions                                                     */
/* ######################################################################### */
/* updateFsList()                                                            */
/* Updates the list fsList of the mounted filesystems.                       */
/* Calls readFs() to get current filesystem list                             */
/* WARNING: updateFsList() function uses static data                         */
/* WARNING: updateFsList() accesses global variable fsList                   */
/* ************************************************************************* */
int updateFsList(int doGetDummyFs,int doGetRemoteFs)
{
  static fs_arr_t fsArr={0,NULL};
  int i;
  int rv;
  fs_data_t *el=NULL;
  fs_data_t *next=NULL;
  fs_data_t rmEl;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  /* Read file systems from file /etc/mtab */
  rv=readFs(doGetDummyFs,doGetRemoteFs,&fsArr);
  if(rv)
  {
    exit(1);
  }
  /* ----------------------------------------------------------------------- */
  /* add to fsList filesystems in fsArr */
  for(i=0;i<fsArr.fsN;i++)
  {
    rv=fslAdd(fsArr.fs[i].name,fsArr.fs[i].mp,fsArr.fs[i].type);
    if(deBug&0x1 && rv==-1)
    {
      mPrintf(errU,VERB,__func__,0,"File system \"%s\" already in the list.",
              fsArr.fs[i].name);
    }
    else if(rv==-2)
    {
      mPrintf(errU,FATAL,__func__,0,"Too many (>100) file system mounted!");
      printOutFuncNOK;
      return 1;
    }
    else if(!rv)
    {
      mPrintf(errU,INFO,__func__,0,"File system \"%s\" (%s) mounted on \"%s\".",
              fsArr.fs[i].name,fsArr.fs[i].type,fsArr.fs[i].mp);
    }
  }
  /* ----------------------------------------------------------------------- */
  /* remove from fsList filesystems not in fsArr */
  for(el=fsList.head;el;)
  {
    int found=0;
    for(i=0;i<fsArr.fsN;i++)
    {
      if(!strcmp(el->mp,fsArr.fs[i].mp))
      {
        found=1;
        break;
      }
    }
    if(!found)                                       /* list node dismounted */
    {
      next=el->next;                                    /* save next pointer */
      rmEl=fslRm(el->mp);                                /* remove list node */
      if(rmEl.id>=0)                                         /* node removed */
      {
        mPrintf(errU,INFO,__func__,0,"File system \"%s\" (%s, mounted on "
                "\"%s\") has been dismounted.", rmEl.name,rmEl.type,rmEl.mp);
      }
      else if(rmEl.id==-1)                               /* node not removed */
      {
        mPrintf(errU,ERROR,__func__,0,"File system \"%s\" (%s, mounted on "
                "\"%s\") not in list (should never happen).", el->name,
                el->type,el->mp);
      }
      else if(rmEl.id==-2)                               /* node not removed */
      {
        mPrintf(errU,WARN,__func__,0,"File system \"%s\" (%s, mounted on "
                "\"%s\") is in use by a previous call to statfs(2)!",el->name,
                el->type,el->mp);
      }
      el=next;                                   /* go to saved next pointer */
    }
    else                                          /* list node still mounted */
    {
      el=el->next;                                     /* go to next pointer */
    }
  }
  printOutFuncOK;
  return 0;
}
/* ************************************************************************* */
/* updateSuccessFlag()                                                       */
/*   Set the global success flag fsList->success.                            */
/*   RETURN VALUE and fsList->success:                                       */
/*     1: OK                                                                 */
/*    -2: At least 1 statfs(2) call returned error.                          */
/*    -4: At least 1 statfs(2) call hung.                                    */
/*    -6: 1 statfs(2) call returned error and at least 1 statfs(2) call hung.*/
/* WARNING: updateSuccessFlag() accesses global variable fsList              */
/* ************************************************************************* */
int updateSuccessFlag(void)
{
  int errMask=0x0;
  fs_data_t *fs=NULL;
  /* ----------------------------------------------------------------------- */
  pthread_mutex_lock(&usageLock);
  for(fs=fsList.head;fs;fs=fs->next)
  {
    if(fs->usage.rv)errMask|=0x2;
    if(fs->usage.stalled)errMask|=0x4;
  }
  pthread_mutex_unlock(&usageLock);
  if(!errMask)fsList.success=1;                                        /* OK */
  else fsList.success=-errMask;                             /* error or hung */
  return fsList.success;
} 
/* ************************************************************************* */
/* startGetFsUsage()                                                         */
/* Starts a new thread executing getFsUsage() function.                      */
/* Get the filesystem usage of filesystem fs->name from the OS               */
/* and store it in fs->usage                                                 */
/* ************************************************************************* */
int startGetFsUsage(fs_data_t *fs)
{
  pthread_t tid;
  pthread_attr_t thr_attr;
  int rv;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  /* set thread attributes */
  rv=pthread_attr_init(&thr_attr);
  if(rv)
  {
    mPrintf(errU,ERROR,__func__,0,"pthread_attr_init(): %s!",strerror(rv));
    printOutFuncNOK;
    return 1;
  }
  rv=pthread_attr_setinheritsched(&thr_attr,PTHREAD_INHERIT_SCHED);
  if(rv)
  {
    mPrintf(errU,ERROR,__func__,0,"pthread_attr_setinheritsched(): %s!",
            strerror(rv));
    printOutFuncNOK;
    return 1;
  }
  rv=pthread_attr_setdetachstate(&thr_attr,PTHREAD_CREATE_DETACHED);
  if(rv)
  {
    mPrintf(errU,ERROR,__func__,0,"pthread_attr_setdetachstate(): %s!",
            strerror(rv));
    printOutFuncNOK;
    return 1;
  }
  /* start a new thread executing getFsUsage() */
  rv=pthread_create(&tid,&thr_attr,getFsUsage,fs);
  if(rv)
  {
    mPrintf(errU,ERROR,__func__,0,"pthread_create(): %s!",strerror(rv));
    return 1;
  }
  printOutFuncOK;
  return 0;
}
/* ######################################################################### */
/* END Data access functions                                                 */
/* ######################################################################### */
/* ######################################################################### */
/* List Management Functions                                                 */
/* ######################################################################### */
/* fslAdd()                                                                  */
/* RETURN VALUE:                                                             */
/*   0: OK                                                                   */
/*  -1: Filesystem already in the list                                       */
/*  -2: Too many (>100) filesystems                                          */
/* WARNING: fslAdd() accesses global variable fsList                         */
/* ************************************************************************* */
int fslAdd(char *name,char *mp,char *type)
{
  fs_data_t *el=NULL;
  int newId;
  char s[DIM_SVC_LEN+1]="";
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  /* try to find the filesystem in the list */
  pthread_mutex_lock(&usageLock);
  for(el=fsList.head;el;el=el->next)
  {
    if(!strncmp(name,el->name,NAME_LEN) &&
       !strncmp(mp,el->mp,MP_LEN) &&
       !strncmp(type,el->type,TYPE_LEN))
    {
      break;
    }
  }
  pthread_mutex_unlock(&usageLock);
  /*-------------------------------------------------------------------------*/
  if(el)                                   /* filesystem already in the list */
  {
    printOutFuncOK;
    return -1;
  }
  /*-------------------------------------------------------------------------*/
  /* find first free id */
  newId=fslNewId();
  if(newId==-1)                               /* Too many (>100) filesystems */
  {
    printOutFuncNOK;
    return -2;
  }
  /*-------------------------------------------------------------------------*/
  /* create new element */
  pthread_mutex_lock(&usageLock);
  el=(fs_data_t*)malloc(sizeof(fs_data_t));
  memset(el,0,sizeof(fs_data_t));
  memset(&el->usage,0,sizeof(fs_usage_t));
  el->id=newId;
  el->usageLock=&usageLock;
  snprintf(el->name,NAME_LEN+1,"%s",name);
  snprintf(el->mp,MP_LEN+1,"%s",mp);
  snprintf(el->type,TYPE_LEN+1,"%s",type);
  el->usage.lastUpdate=time(NULL);
  /*-------------------------------------------------------------------------*/
  /* add DIM service */
  snprintf(s,DIM_SVC_LEN+1,"%s/summary/fs_%02d/data",srvPath,el->id);
  el->sumSvcId=dis_add_service(s,"F:2;C",0,0,fsSumHandler,el->id);
  snprintf(s,DIM_SVC_LEN+1,"%s/details/fs_%02d/data",srvPath,el->id);
  el->detSvcId=dis_add_service(s,"X:6;F:2;C",0,0,fsDetHandler,el->id);
  dis_start_serving(srvPath);
  /*-------------------------------------------------------------------------*/
  /* set links */
  el->prev=NULL;
  el->next=NULL;
  if(fsList.tail)                  /* filesystem list has at least 1 element */
  {
    el->prev=fsList.tail;
    fsList.tail->next=el;
  }
  else                                    /* filesystem list has no elements */
  {
    fsList.head=el;
  }
  fsList.tail=el;
  pthread_mutex_unlock(&usageLock);
  printOutFuncOK;
  return 0;
}
/* ************************************************************************* */
/* fslRm()                                                                   */
/* RETURN VALUE:                                                             */
/*   removed list node                                                       */
/* RETURN VALUE.id:                                                          */
/*  >=0: OK                                                                  */
/*   -1: mount-point not in the list                                         */
/*   -2: mount-point in use by statfs(2)                                     */
/* WARNING: fslRm() accesses global variable fsList                          */
/* ************************************************************************* */
fs_data_t fslRm(char *mp)
{
  fs_data_t *el=NULL;
  fs_data_t *next=NULL;
  fs_data_t *prev=NULL;
  fs_data_t rmEl={0,"","","",{0,0,0,0,0,0,0,0,0,0},NULL,NULL};
  int busy;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  /* find mount-point in the list */
  pthread_mutex_lock(&usageLock);
  for(el=fsList.head;el;el=el->next)if(!strcmp(el->mp,mp))break;
  pthread_mutex_unlock(&usageLock);
  /* ----------------------------------------------------------------------- */
  if(!el)                               /* mount-point not found in the list */
  {
    rmEl.id=-1;
    printOutFuncNOK;
    return rmEl;
  }
  /* ----------------------------------------------------------------------- */
  pthread_mutex_lock(&usageLock);
  busy=el->usage.busy;
  pthread_mutex_unlock(&usageLock);
  if(busy)                                /* mount-point in use by statfs(2) */
  {
    rmEl.id=-2;
    printOutFuncNOK;
    return rmEl;
  }
  /*-------------------------------------------------------------------------*/
  /* remove DIM service */
  dis_remove_service(el->sumSvcId);
  dis_remove_service(el->detSvcId);
  dis_start_serving(srvPath);
  /* ----------------------------------------------------------------------- */
  /* remove node from the list */
  pthread_mutex_lock(&usageLock);
  memcpy(&rmEl,el,sizeof(fs_data_t));
  next=el->next;
  prev=el->prev;
  if(el->next)el->next->prev=prev;
  if(el->prev)el->prev->next=next;
  if(!prev)fsList.head=next;
  if(!next)fsList.tail=prev;
  free(el);
  pthread_mutex_unlock(&usageLock);
  /* ----------------------------------------------------------------------- */
  printOutFuncOK;
  return rmEl;
}
/* ************************************************************************* */
/* fslPrint()                                                                */
/* Print file system list and usage to the logger for debugging              */
/* WARNING: fslPrint() accesses global variable fsList                       */
/* ************************************************************************* */
void fslPrint(void)
{
  fs_data_t *el=NULL;
  struct tm lastUpdateL;
  char lastUpdateS[TIME_LEN+1];                            /* Oct12-134923\0 */
  char status[128];                      /* OK, ERROR:..., STALLED since X s */
  char *msg=NULL;
  int msgL=0;
  char *p;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  pthread_mutex_lock(&usageLock);
  if(!fsList.tail)
  {
    mPrintf(errU,INFO,__func__,0,"(No filesystems mounted)");
    pthread_mutex_unlock(&usageLock);
    printOutFuncOK;
    return;
  }
  for(el=fsList.head;el;el=el->next)
  {
    /* ..................................................................... */
    msgL=0;
    msg=(char*)realloc(msg,msgL+1);
    msg[0]='\0';
    /* ..................................................................... */
    localtime_r(&el->usage.lastUpdate,&lastUpdateL);
    strftime(lastUpdateS,TIME_LEN+1,"%b%d-%H%M%S",&lastUpdateL);
    if(el->usage.stalled)
    {
      snprintf(status,128,"STALLED since %ld s",el->usage.stalled);
    }
    else if(el->usage.rv)
    {
      snprintf(status,128,"ERROR: %s",strerror(el->usage.err));
    }
    else 
    {
      strcpy(status,"OK");
    }
    /* ..................................................................... */
    msgL+=snprintf(NULL,0,"Id=\"fs_%02d\" ty=\"%s\" fs=\"%s\" mp=\"%s\" "
                   "mf=%"PRIu64" tr=%"PRIu64" tu=%"PRIu64" u=%"PRIu64" "
                   "ar=%"PRIu64" au=%"PRIu64" upd=%s status=%s.",el->id,
                   el->type,el->name,el->mp,el->usage.minfree,
                   el->usage.totalRoot,el->usage.totalUser,el->usage.used,
                   el->usage.availRoot,el->usage.availUser,lastUpdateS,status);
    msg=(char*)realloc(msg,msgL+1);
    p=strchr(msg,'\0');
    sprintf(p,"Id=\"fs_%02d\" ty=\"%s\" fs=\"%s\" mp=\"%s\" mf=%"PRIu64" "
            "tr=%"PRIu64" tu=%"PRIu64" u=%"PRIu64" ar=%"PRIu64" au=%"PRIu64" "
            "upd=%s status=%s.",el->id,el->type,el->name,el->mp,
            el->usage.minfree,el->usage.totalRoot,el->usage.totalUser,
            el->usage.used,el->usage.availRoot,el->usage.availUser,lastUpdateS,
            status);
    mPrintf(errU,VERB,__func__,0,"%s",msg);
    /* ..................................................................... */
  }
  pthread_mutex_unlock(&usageLock);
  if(msg)free(msg);
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
/* fslNewId()                                                                */
/* Get a new and not used Identifier for a mounted file system               */
/* From 0 to 99, round-robin                                                 */
/* WARNING: fslNewId() function uses static data                             */
/* WARNING: fslNewId() accesses global variable fsList                       */
/* ************************************************************************* */
int fslNewId(void)
{
  fs_data_t *el=NULL;
  int id;
  int count;
  static int maxId=-1;
  /*-------------------------------------------------------------------------*/
  /* find first free id after maxId */
  for(id=maxId+1,count=0;;id++,count++)
  {
    int idFound=0;
    if(count>99)return -1;            /* too many (>100) file system mounted */
    if(id>99)id-=100;
    for(el=fsList.head;el;el=el->next)
    {
      if(el->id==id)
      {
        idFound=1;
        break;
      }
    }
    if(!idFound)break;
  }
  /*-------------------------------------------------------------------------*/
  maxId=id;
  return id;
}
/* ######################################################################### */
/* END List Management Functions                                             */
/* ######################################################################### */
/* ######################################################################### */
/* Usage Functions                                                           */
/* ######################################################################### */
void shortUsage(void)
{
  char *shortUsageStr=
"SYNOPSIS\n"
"fsSrv [-d DEBUG_MASK] [-N DIM_DNS_NODE] [-l LOGGER] [-a] [-D]\n"
"      [-u REFRESH_TIME_INTERVAL]\n"
"fsSrv -h\n"
"Try \"fsSrv -h\" for more information.\n";
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
  /*-------------------------------------------------------------------------*/
  if(!mode)formatter="/usr/bin/groff -man -Tascii | /usr/bin/less -iRs";
  else formatter="/bin/cat";
  fpOut=popen(formatter,"w");
  fprintf(fpOut,
".ig\n"
"fsSrv.man\n"
"\n"
"..\n"
"%s"
".hw sensor\\[hy]dictionary\\[hy]file  max\\[hy]threads\\[hy]number \n"
".TH fsSrv 8  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC Servers\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis fsSrv\\ \\-\n"
"FMC Filesystem Monitor Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis fsSrv\n"
".ShortOpt[] d DEBUG_MASK\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] l LOGGER\n"
".ShortOpt[] a\n"
".ShortOpt[] D\n"
".ShortOpt[] u REFRESH_TIME_INTERVAL\n"
".EndSynopsis\n"
".Synopsis fsSrv\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"The FMC Filesystem Monitor Server, \\fBfsSrv\\fP(8) retrieves information "
"concerning the file system disk space usage on the current farm node and "
"publishes them using DIM.\n"
".PP\n"
"The DIM Name Server, looked up by \\fBfsSrv\\fP to register the "
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
".OptDef d \"\" DEBUG_MASK (hexadecimal integer)\n"
"Set debug mask to \\fIDEBUG_MASK\\fP. Can be set to 0x000...0x7ff. The "
"\\fIDEBUG_MASK\\fP is the bitwise \\fIOR\\fP of the following integers with "
"one bit set:\n"
".PP\n"
".RS\n"
"0x0001 print in/out functions;\n"
".br\n"
"0x0002 print active threads after timeout;\n"
".br\n"
"0x0004 print file system data;\n"
".br\n"
"0x0008 print update handler calls;\n"
".RE\n"
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
".OptDef a \"\"\n"
"Include also remote filesystems (e.g.: nfs, afs, gpfs, etc.). "
"\\fBDefault\\fP: includes only local filesystems.\n"
".\n"
".OptDef D \"\"\n"
"Include also dummy filesystems (e.g.: proc, sysfs, devpts, etc.). "
"\\fBDefault\\fP: includes only \"real\" filesystems.\n"
".\n"
".OptDef u \"\" REFRESH_TIME_INTERVAL (integer)\n"
"Set the data refresh period to \\fIREFRESH_TIME_INTERVAL\\fP seconds. (must "
"be in the range [0,3600]s). \\fBDefault\\fP: 20 s.\n"
".\n"
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
"UTGID of the \\fBfsSrv\\fP(8) process.\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Start the FMC Filesystem Monitor Server, sending diagnostic messages to the "
"default FMC Message Logger and showing only local filesystems:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/fsSrv\n"
".PP\n"
"Start the FMC Filesystem Monitor Server, sending diagnostic messages to the "
"default FMC Message Logger and to the system syslog facility, and showing "
"only local filesystems:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/fsSrv -l 0x5\n"
".PP\n"
"Set the delay time interval between two subsequent refresh to four seconds:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/fsSrv -u 4\n"
".PP\n"
"Show both local and network (afs, nfs, gpfs, etc.) filesystems:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/fsSrv -a\n"
".PP\n"
"Show local, network filesystems and \"dummy\" filesystems (procfs, sysfs, devpts, tmpfs, rpc_pipefs, binfmt_misc, etc.):\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/fsSrv -a -D\n"
".PP\n"
".\n"
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
"the \\fBfsSrv.c\\fP source code of\n"
"the current \\fBfsSrv\\fP(8) instance, as returned by the command\n"
"\"ident /opt/FMC/sbin/fsSrv\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/sensor_version\n"
"\\fBOutput format\\fP: \"C\" (NULL-terminated string).\n"
".br\n"
"Publishes a NULL-terminated string containing the RCS revision label of\n"
"the \\fBgetFS.c\\fP source code used in\n"
"the current \\fBfsSrv\\fP(8) instance, as returned by the command\n"
"\"ident /opt/FMC/lib/libFMCsensors*\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/fmc_version\n"
"\\fBOutput format\\fP: \"C\" (NULL-terminated string).\n"
".br\n"
"Publishes a NULL-terminated string containing the revision label of the FMC\n"
"package which includes the current \\fBfsSrv\\fP(8) executable, in the\n"
"present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/success\n"
"\\fBOutput format\\fP: \"I\" (integer).\n"
".RS\n"
".TP\n"
" 1:\nOK;\n"
".TP\n"
"-2:\nAt least 1 statfs(2) call returned error;\n"
".TP\n"
"-4:\nAt least 1 statfs(2) call hung;\n"
".TP\n"
"-6:\nAt least 1 statfs(2) call returned error and at least 1 statfs(2) call "
"hung;\n"
".RE\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/devices\n"
"\\fBOutput format\\fP: \"C\" (a sequence of NULL-terminated strings).\n"
".br\n"
"The file system identifiers in the current node:\n"
".br\n"
"\"fs_00\" \"fs_01\" \"fs_02\" ...\n"
".PP\n"
"\\fBSummary SVCs\\fP\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/labels\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 4 NULL-terminated strings).\n"
".br\n"
"Publishes the labels referring to the data published in the SVC "
"\".../summary/fs_XX/data\":\n"
".br\n"
"\"TotalRoot\" \"UseU%%\" \"Filesystem\" \"Mounted on\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/units\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 4 NULL-terminated strings).\n"
".br\n"
"Publishes the units referring to the data published in the SVC "
"\".../summary/fs_XX/data\":\n"
".br\n"
"\"KiB\" \"%%\" \"\" \"\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/\\fIDEVICE\\fP/data\n"
"\\fBOutput format\\fP: \"F:2;C;C\".\n"
".br\n"
"Publishes the data of the filesystem \\fIDEVICE\\fP.\n"
".br\n"
"The data sequence is the following:\n"
".RS\n"
".TP\n"
"\\fBTotalRoot\\fP (float)\n"
"The total size of the filesystem accessible by the user \"root\" in KiB.\n"
".TP\n"
"\\fBUseU%%\\fP (float)\n"
"Used percentage of the total filesystem size accessible by a non-privileged "
"user (=Used*100/TotalUser).\n"
".TP\n"
"\\fBFilesystem\\fP (NULL-terminated string)\n"
"The filesystem device name.\n"
".TP\n"
"\\fBMounted on\\fP (NULL-terminated string)\n"
"The filesystem mount point.\n"
".RE\n"
".PP\n"
"\\fBDetailed SVCs\\fP\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/labels\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 12 NULL-terminated strings).\n"
".br\n"
"Publishes the labels referring to the data published in the SVC "
"\".../details/fs_XX/data\":\n"
".br\n"
"\"Minfree\" \"TotalRoot\" \"TotalUser\" \"Used\"\n"
"\"AvailRoot\" \"AvailUser\" \"UseR%%\" \"UseU%%\"\n"
"\"Filesystem\" \"Type\" \"Mounted on\" \"Time\" \"Status\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/labels\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 12 NULL-terminated strings).\n"
".br\n"
"Publishes the units referring to the data published in the SVC "
"\".../details/fs_XX/data\":\n"
".br\n"
"\"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"%%\"\n"
"\"%%\" \"\" \"\" \"\" \"mmmdd-HHMMSS\" \"\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/\\fIDEVICE\\fP/data\n"
"\\fBOutput format\\fP: \"X:6;F:2;C;C;C;C;C\".\n"
".br\n"
"Publishes the data of the filesystem \\fIDEVICE\\fP.\n"
".br\n"
"The data sequence is the following:\n"
".RS\n"
".TP\n"
"\\fBMinfree\\fP (uint64_t)\n"
"The size (in KiB) of the filesystem space available to the user \"root\"\n"
"but unavailable to any non-privileged users (=TotalRoot-Used-AvailUser).\n"
".TP\n"
"\\fBTotalRoot\\fP (uint64_t)\n"
"The total size of the filesystem available to the user \"root\" in KiB.\n"
".TP\n"
"\\fBTotalUser\\fP (uint64_t)\n"
"The total size of the filesystem available to non-privileged users in KiB\n"
"(=Used+AvailUser).\n"
".TP\n"
"\\fBUsed\\fP (uint64_t)\n"
"The size of the used filesystem space in KiB.\n"
".TP\n"
"\\fBAvailRoot\\fP (uint64_t)\n"
"Size of the filesystem space available to the user \"root\" in KiB\n"
"(=TotalRoot-Used).\n"
".TP\n"
"\\fBAvailUser\\fP (uint64_t)\n"
"Size of the filesystem space available to non-privileged users in KiB.\n"
".TP\n"
"\\fBUseR%%\\fP (float)\n"
"Used percentage of the total filesystem size accessible by the user \n"
"\"root\" (=Used*100/TotalRoot).\n"
".TP\n"
"\\fBUseU%%\\fP (float)\n"
"Used percentage of the total filesystem size accessible by a non-privileged\n"
"user (=Used*100/TotalUser).\n"
".TP\n"
"\\fBFilesystem\\fP (NULL-terminated string)\n"
"The filesystem device name.\n"
".TP\n"
"\\fBType\\fP (NULL-terminated string)\n"
"The filesystem type (e.g.: ext3, nfs, afs, etc.).\n"
".TP\n"
"\\fBMounted on\\fP (NULL-terminated string)\n"
"The filesystem mount point.\n"
".TP\n"
"\\fBTime\\fP (NULL-terminated string)\n"
"The time corresponding to the last data access.\n"
".TP\n"
"\\fBStatus\\fP (NULL-terminated string)\n"
"The filesystem status at the last data access. Can be:\n"
".br\n"
"\"\\fBOK\\fP\";\n"
".br\n"
"\"\\fBSTALLED since <N> s\\fP\";\n"
".br\n"
"\"\\fBERROR: <error message>\\fP\".\n"
".RE\n"
".RE\n"
".\n"
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
".\n"
".SH SEE ALSO\n"
".ad l\n"
".BR \\%%fsViewer (1),\n"
".br\n"
".BR \\%%setmntent (3),\n"
".BR \\%%getmntent_r (3),\n"
".BR \\%%endmntent (3),\n"
".BR \\%%statfs (2),\n"
".br\n"
".BR \\%%df (1).\n"
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
/* ######################################################################### */
/* END Usage Functions                                                       */
/* ######################################################################### */
