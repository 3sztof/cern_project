/*****************************************************************************/
/*
 * $Log: cpustatSrv.c,v $
 * Revision 2.32  2009/10/03 16:10:50  galli
 * minor changes
 *
 * Revision 2.31  2009/10/03 16:07:57  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 2.20  2008/02/01 11:15:51  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 2.15  2007/10/26 14:08:28  galli
 * bug fixed
 *
 * Revision 2.13  2007/10/26 14:03:55  galli
 * usage() prints FMC version
 * publishes fmc_version
 *
 * Revision 2.12  2007/09/20 10:14:22  galli
 * can get DIM_DNS_NODE from a configuration file
 * -c dim_conf_file command-line option added
 *
 * Revision 2.11  2007/09/06 08:16:55  galli
 * bug fixed
 *
 * Revision 2.10  2007/09/06 08:08:11  galli
 * Changed DIM services:
 * only 1 data service for all the variable data
 * usage() modified
 *
 * Revision 2.7  2007/08/11 22:45:43  galli
 * compatible with libFMCutils v 2
 * added function usage()
 *
 * Revision 2.6  2006/10/26 08:17:32  galli
 * const intervals between refreshes
 *
 * Revision 2.5  2006/10/23 06:53:00  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 2.4  2005/11/15 13:40:59  galli
 * bug fixed
 *
 * Revision 2.3  2005/11/10 13:07:01  galli
 * synchronous handling of signals
 * define DIM error handler
 * added service /HOSTNAME/cpu/stat/cpuN
 * added service /HOSTNAME/cpu/stat/names
 *
 * Revision 2.2  2004/11/18 10:08:17  gregori
 * bug fixed
 *
 * Revision 2.1  2004/11/18 09:40:48  gregori
 * ignore SIGHUP
 *
 * Revision 1.4  2004/10/22 16:27:58  gregori
 * log msg, version msg .
 *
 * Revision 1.1  2004/10/14 14:33:54  gregori
 * Initial revision
 *
 */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>                                       /* exit() strtol() */
#include <unistd.h>                                                /* exit() */
#include <signal.h>                                              /* signal() */
#include <syslog.h>                                              /* syslog() */
#include <string.h>
#include <errno.h>
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
#include "getCpustatFromProc.h"  /* struct cpuVal, struct cpusVal, readCpu() */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#include "cpustatSrv.h"           /* SRV_NAME, DIM_SRV_LEN, DIM_SVC_LEN, ... */
/*****************************************************************************/
/* globals */
int deBug=0;
int errU=L_DIM;                                                /* error unit */
int svcNum=0;                           /* number of services (for clean-up) */
int sSvcNum=0;
int *svcId=NULL;
int successSvcId=0;
pid_t tgid;
static char rcsid[]="$Id: cpustatSrv.c,v 2.32 2009/10/03 16:10:50 galli Exp $";
char *sRcsid;
char *rcsidP,*sRcsidP;
/*****************************************************************************/
/* function prototype */
static void signalHandler(int signo);
void dimErrorHandler(int severity,int errorCode,char *message);
static void logData(struct cpusVal cpus);
void usage(char **argv);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i=0,j=0;
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
  char srvPath[DIM_SRV_LEN+1]="";
  char s[DIM_SVC_LEN+1];
  /* sensor data */
  struct cpusVal cpus;
  /* data to be published */
  int ctxtFieldN=0;                                 /* number of data fields */
  int loadFieldN=0;                                 /* number of data fields */
  char device[DEVICE_LEN+1];
  char *deviceBuf=NULL;                            /* devices                */
  int deviceBufS=0;                                /* deviceBuf size         */
  int deviceBufBS=0;                               /* deviceBuf busy size    */
  char *ctxtLabelBuf=NULL;                         /* ctxt rate labels       */
  int ctxtLabelBufS=0;                             /* ctxtLabelBuf size      */
  int ctxtLabelBufBS=0;                            /* ctxtLabelBuf busy size */
  char *loadLabelBuf=NULL;                         /* load rate labels       */
  int loadLabelBufS=0;                             /* loadLabelBuf size      */
  int loadLabelBufBS=0;                            /* loadLabelBuf busy size */
  char *ctxtUnitsBuf=NULL;                         /* ctxt rate units        */
  int ctxtUnitsBufS=0;                             /* ctxtUnitsBuf size      */
  int ctxtUnitsBufBS=0;                            /* ctxtUnitsBuf busy size */
  char *loadUnitsBuf=NULL;                         /* load rate labels       */
  int loadUnitsBufS=0;                             /* loadUnitsBuf size      */
  int loadUnitsBufBS=0;                            /* loadUnitsBuf busy size */
  float **ctxtDataBuf=NULL;                        /* ctxt data buffer       */
  int *ctxtDataBufS=NULL;                          /* ctxtDataBuf size       */
  float **loadDataBuf=NULL;                        /* load data buffer       */
  int *loadDataBufS=NULL;                          /* loadDataBuf size       */
  /* dim configuration */
  char *dimDnsNode=NULL;
  /*-------------------------------------------------------------------------*/
  sRcsid=getCpustatSensorVersion();
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
        if(!optarg)usage(argv);           /* -h: open manual using less TODO */
        else usage(argv);             /* -hh: print raw groff to stdout TODO */
      default:
        mPrintf(L_STD|L_SYS,FATAL,__func__,0,"getopt(): invalid option "
                "\"-%c\"!",optopt);
        usage(argv);
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
    usage(argv);
  }
  if(argc-optind!=0)usage(argv);
  if(refreshPeriod>60 || refreshPeriod <0)
  {
    mPrintf(L_SYS|L_STD,FATAL,__func__,0,"refresh_time_interval must be in "
            "[0,60]");
    usage(argv);
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
  /* In kernel <= 2.4.19 other threads returns different getpid(). So we     */
  /* save the pid of main thread in a global variable, to be used in         */
  /* resetCmndHandler(). In kernels >= 2.4.20 it makes no differences.       */
  tgid=getpid();
  /*-------------------------------------------------------------------------*/
  /* define server name /FMC/<HOSTNAME>/cpustat */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /*-------------------------------------------------------------------------*/
  /* Read the number of CPUs (including siblings)                            */
  for(i=0;i<5;i++)
  {
    cpus=readCpu(deBug,errU);
    if(cpus.success)break;
    dtq_sleep(1);
  }
  if(i>=5)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by readCpu()! Can't "
            "initialize service!");
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate fields number */
  ctxtFieldN=sizeof(ctxtLabelV)/sizeof(char*);
  loadFieldN=sizeof(loadLabelV)/sizeof(char*);
  /*-------------------------------------------------------------------------*/
  /* evaluate deviceBuf size */
  for(i=0,deviceBufS=0;i<cpus.cpuN;i++)
  {
    deviceBufS+=1+snprintf(NULL,0,"core_%02d",i);
  }
  /* allocate space for deviceBuf */
  deviceBuf=(char*)malloc(deviceBufS);
  /* fill deviceBuf string array */
  for(i=0,deviceBufBS=0;i<cpus.cpuN;i++)
  {
    snprintf(device,DEVICE_LEN+1,"core_%02d",i);
    sprintf(deviceBuf+deviceBufBS,"%s",device);
    deviceBufBS+=(1+strlen(deviceBuf+deviceBufBS));
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate ctxtLabelBuf & loadLabelBuf size */
  for(i=0,ctxtLabelBufS=0;i<ctxtFieldN;i++)
  {
    ctxtLabelBufS+=(1+strlen(ctxtLabelV[i]));
  }
  for(i=0,loadLabelBufS=0;i<loadFieldN;i++)
  {
    loadLabelBufS+=(1+strlen(loadLabelV[i]));
  }
  /* allocate space for ctxtLabelBuf & loadLabelBuf */
  ctxtLabelBuf=(char*)malloc(ctxtLabelBufS);
  loadLabelBuf=(char*)malloc(loadLabelBufS);
  /* fill ctxtLabelBuf & loadLabelBuf string array */
  for(i=0,ctxtLabelBufBS=0;i<ctxtFieldN;i++)
  {
    sprintf(ctxtLabelBuf+ctxtLabelBufBS,"%s",ctxtLabelV[i]);
    ctxtLabelBufBS+=(1+strlen(ctxtLabelBuf+ctxtLabelBufBS));
  }
  for(i=0,loadLabelBufBS=0;i<loadFieldN;i++)
  {
    sprintf(loadLabelBuf+loadLabelBufBS,"%s",loadLabelV[i]);
    loadLabelBufBS+=(1+strlen(loadLabelBuf+loadLabelBufBS));
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate ctxtUnitsBuf & loadUnitsBuf size */
  for(i=0,ctxtUnitsBufS=0;i<ctxtFieldN;i++)
  {
    ctxtUnitsBufS+=(1+strlen(ctxtUnitsV[i]));
  }
  for(i=0,loadUnitsBufS=0;i<loadFieldN;i++)
  {
    loadUnitsBufS+=(1+strlen(loadUnitsV[i]));
  }
  /* allocate space for ctxtUnitsBuf & loadUnitsBuf */
  ctxtUnitsBuf=(char*)malloc(ctxtUnitsBufS);
  loadUnitsBuf=(char*)malloc(loadUnitsBufS);
  /* fill ctxtUnitsBuf & loadUnitsBuf string array */
  for(i=0,ctxtUnitsBufBS=0;i<ctxtFieldN;i++)
  {
    sprintf(ctxtUnitsBuf+ctxtUnitsBufBS,"%s",ctxtUnitsV[i]);
    ctxtUnitsBufBS+=(1+strlen(ctxtUnitsBuf+ctxtUnitsBufBS));
  }
  for(i=0,loadUnitsBufBS=0;i<loadFieldN;i++)
  {
    sprintf(loadUnitsBuf+loadUnitsBufBS,"%s",loadUnitsV[i]);
    loadUnitsBufBS+=(1+strlen(loadUnitsBuf+loadUnitsBufBS));
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate memory for ctxt data */
  ctxtDataBufS=(int*)malloc(1*sizeof(int));
  ctxtDataBufS[0]=1*sizeof(float);
  /* allocate memory for ctxt data */
  ctxtDataBuf=(float**)malloc(1*sizeof(float*));
  ctxtDataBuf[0]=(float*)malloc(ctxtDataBufS[0]);
  memset(ctxtDataBuf[0],0,ctxtDataBufS[0]);
  /* evaluate memory for load data */
  loadDataBufS=(int*)malloc((cpus.cpuN+1)*sizeof(int));  /* cpuN cores + ave */
  for(i=0;i<cpus.cpuN+1;i++)loadDataBufS[i]=loadFieldN*sizeof(float);
  /* allocate memory for load data */
  loadDataBuf=(float**)malloc((cpus.cpuN+1)*sizeof(float*));
  for(i=0;i<cpus.cpuN+1;i++)
  {
    loadDataBuf[i]=(float*)malloc(loadDataBufS[i]);
    memset(loadDataBuf[i],0,loadDataBufS[i]);
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
  /* define DIM service 0: /FMC/<HOSTNAME>/cpu/stat/sensor_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsidP,1+strlen(sRcsidP),0,0);
  /* define DIM service 1: /FMC/<HOSTNAME>/cpu/stat/server_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"server_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rcsidP,1+strlen(rcsidP),0,0);
  /* define DIM service 2: /FMC/<HOSTNAME>/cpu/stat/fmc_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"fmc_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  /* define DIM service 3: /FMC/<HOSTNAME>/cpu/stat/devices */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"devices");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",deviceBuf,deviceBufS,0,0);
  /* define DIM service 4: /FMC/<HOSTNAME>/cpu/stat/ctxt/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"ctxt/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",ctxtLabelBuf,ctxtLabelBufS,0,0);
  /* define DIM service 5: /FMC/<HOSTNAME>/cpu/stat/ctxt/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"ctxt/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",ctxtUnitsBuf,ctxtUnitsBufS,0,0);
  /* define DIM service 6: /FMC/<HOSTNAME>/cpu/stat/load/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"load/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",loadLabelBuf,loadLabelBufS,0,0);
  /* define DIM service 7: /FMC/<HOSTNAME>/cpu/stat/load/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"load/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",loadUnitsBuf,loadUnitsBufS,0,0);
  sSvcNum=svcNum;                                               /* sSvcNum=8 */
  /*-------------------------------------------------------------------------*/
  /* define dynamic DIM services */
  /* define DIM service 8: /FMC/<HOSTNAME>/cpu/stat/success */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"success");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&cpus.success,sizeof(int),0,0);
  successSvcId=svcId[svcNum-1];
  /* define DIM service 9: /FMC/<HOSTNAME>/cpu/stat/ctxt/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"ctxt/data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",ctxtDataBuf[0],ctxtDataBufS[0],
                                  0,0);
  /* define DIM service 10: /FMC/<HOSTNAME>/cpu/stat/load/average/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"load/average/data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",loadDataBuf[0],loadDataBufS[0],
                                  0,0);
  for(i=0;i<cpus.cpuN;i++)
  {
    /* define DIM service 11+i: /FMC/<HOSTNAME>/cpu/stat/load/core-XX/data */
    svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
    snprintf(s,DIM_SVC_LEN+1,"%s/load/core_%02d/%s",srvPath,i,"data");
    svcId[svcNum-1]=dis_add_service(strdup(s),"F",loadDataBuf[i+1],
                                    loadDataBufS[i+1],0,0);
  }
  /* svcNum=11+cpuN */
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  dis_add_error_handler(dimErrorHandler);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  dis_start_serving(srvPath);
  /*-------------------------------------------------------------------------*/
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC CPU Statistics Monitor DIM "
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
      cpus=readCpu(deBug,errU);
      if(cpus.success)break;
      dtq_sleep(1);
    }
    if(cpus.success)
    {
      /* compose loadDataBuf */
      ctxtDataBuf[0][0]=cpus.ctxtRate;
      for(i=0;i<cpus.cpuN+1;i++)
      {
        memset(loadDataBuf[i],0,loadDataBufS[i]);
        loadDataBuf[i][0]=cpus.vals[i].user;
        loadDataBuf[i][1]=cpus.vals[i].system;
        loadDataBuf[i][2]=cpus.vals[i].nice;
        loadDataBuf[i][3]=cpus.vals[i].idle;
        loadDataBuf[i][4]=cpus.vals[i].iowait;
        loadDataBuf[i][5]=cpus.vals[i].irq;
        loadDataBuf[i][6]=cpus.vals[i].softirq;
      }
      /* Update services */
      for(i=sSvcNum;i<svcNum;i++)dis_update_service(svcId[i]);   
      if(deBug)logData(cpus);
    }     
    else mPrintf(errU,ERROR,__func__,0,"Error flag returned by readCpu()!");
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
static void logData(struct cpusVal cpus)
{
  int i;
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,VERB,__func__,0,"Total user     %5.2f%%",cpus.vals[0].user);
  mPrintf(errU,VERB,__func__,0,"Total system   %5.2f%%",cpus.vals[0].system);
  mPrintf(errU,VERB,__func__,0,"Total nice     %5.2f%%",cpus.vals[0].nice);
  mPrintf(errU,VERB,__func__,0,"Total idle     %5.2f%%",cpus.vals[0].idle);
  mPrintf(errU,VERB,__func__,0,"Total iowait   %5.2f%%",cpus.vals[0].iowait);
  mPrintf(errU,VERB,__func__,0,"Total irq      %5.2f%%",cpus.vals[0].irq);
  mPrintf(errU,VERB,__func__,0,"Total softirq  %5.2f%%",cpus.vals[0].softirq);
  for(i=1;i<cpus.cpuN+1;i++)
  {
    mPrintf(errU,VERB,__func__,0,"CPU %u user     %5.2f%%",i-1,
            cpus.vals[i].user);
    mPrintf(errU,VERB,__func__,0,"CPU %u system   %5.2f%%",i-1,
            cpus.vals[i].system);
    mPrintf(errU,VERB,__func__,0,"CPU %u nice     %5.2f%%",i-1,
            cpus.vals[i].nice);
    mPrintf(errU,VERB,__func__,0,"CPU %u idle     %5.2f%%",i-1,
            cpus.vals[i].idle);
    mPrintf(errU,VERB,__func__,0,"CPU %u iowait   %5.2f%%",i-1,
            cpus.vals[i].iowait);
    mPrintf(errU,VERB,__func__,0,"CPU %u irq      %5.2f%%",i-1,
             cpus.vals[i].irq);
    mPrintf(errU,VERB,__func__,0,"CPU %u softirq  %5.2f%%",i-1,
            cpus.vals[i].softirq);
  }
  mPrintf(errU,VERB,__func__,0,"Ctxt rate      %8.3e ctxt/s\n",cpus.ctxtRate);
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
/*****************************************************************************/
void dimErrorHandler(int severity,int errorCode,char *message)
{
  mPrintf(errU|L_SYS,severity+3,__func__,0,"%s! (error code = %d)",
          message,errorCode);
  if(severity==3)_exit(1);
  return;
}
/*****************************************************************************/
void usage(char **argv)
{
  rPrintf(L_STD|L_SYS,2,
"\n"
"NAME\n"
"       cpustatSrv - FMC CPU Statistics Monitor Server\n"
"\n"
"SYNOPSIS\n"
"       cpustatSrv [-d] [-c DIM_CONF_FILE] [-l LOGGER]\n"
"                  [-u REFRESH_TIME_INTERVAL]\n"
"       cpustatSrv -h\n"
"\n"
"OPTIONS\n"
"       -h     Get basic usage help from the command line.\n"
"       -d     Increase verbosity for debugging.\n"
"       -c DIM_CONF_FILE (string, optional)\n"
"              If DIM_DNS_NODE is not defined in the environment, try to\n"
"              retrieve it from the file DIM_CONF_FILE.\n"
"              Default: DIM_CONF_FILE=\"/etc/sysconfig/dim\"\n"
"       -l LOGGER (integer, optional)\n"
"              Use loggers defined in LOGGER mask to send messages:\n"
"                     0x1 default DIM Message Logger (fifo: "
"/tmp/logSrv.fifo);\n"
"                     0x2 standard error;\n"
"                     0x4 syslog.\n"
"              Default: 0x1.\n"
"       -u REFRESH_TIME_INTERVAL (integer, optional)\n"
"              Set data refresh period to REFRESH_TIME_INTERVAL seconds.\n"
"              (must be in [0,60]s). Default: 20 s.\n"
"\n"
"ENVIRONMENT:\n"
"       DIM_DNS_NODE (string, mandatory if not defined in DIM_CONF_FILE)\n"
"              Hostname.domain or IP address of the DIM name server.\n"
"       LD_LIBRARY_PATH (string, mandatory, if not set using ldconfig)\n"
"              Variable in PATH format which must include the path to the\n"
"              libraries \"libdim\", \"libFMCutils\" and\n"
"              libFMCsensors.\n"
"       UTGID (string, optional)\n"
"              UTGID of the fsSrv process.\n"
"\n"
"PUBLISHED DIM SERVICES:\n"
"       /FMC/<HOSTNAME>/cpu/stat/fmc_version (string)\n"
"              \"%s\".\n"
"       /FMC/<HOSTNAME>/cpu/stat/server_version (string)\n"
"              \"%s\".\n"
"       /FMC/<HOSTNAME>/cpu/stat/sensor_version (string)\n"
"              \"%s\".\n"
"       /FMC/<HOSTNAME>/cpu/stat/success (int)\n"
"              1 if data are retrieved, 0 otherwise.\n"
"       /FMC/<HOSTNAME>/cpu/stat/devices (string[])\n"
"              \"core_00\" \"core_01\" \"core_02\" ...\n"
"              The CPU cores in the current node.\n"
"       /FMC/<HOSTNAME>/cpu/stat/ctxt/labels (string[1])\n"
"              \"ctxt_rate\".\n"
"       /FMC/<HOSTNAME>/cpu/stat/ctxt/units (string[1])\n"
"              \"Hz\".\n"
"       /FMC/<HOSTNAME>/cpu/stat/ctxt/data (float[1])\n"
"       /FMC/<HOSTNAME>/cpu/stat/load/labels (string[7])\n"
"              \"user\" \"system\" \"nice\" \"idle\" \"iowait\" \"irq\" "
"\"softirq\".\n"
"       /FMC/<HOSTNAME>/cpu/stat/load/units (string[7])\n"
"              \"%\" \"%\" \"%\" \"%\" \"%\" \"%\" \"%\".\n"
"       /FMC/<HOSTNAME>/cpu/stat/load/average/data (float[7])\n"
"       /FMC/<HOSTNAME>/cpu/stat/load/core_00/data (float[7])\n"
"       /FMC/<HOSTNAME>/cpu/stat/load/core_01/data (float[7])\n"
"       /FMC/<HOSTNAME>/cpu/stat/load/core_02/data (float[7])\n"
"       /FMC/<HOSTNAME>/cpu/stat/load/core_../data (float[7])\n"
"\n"
"AUTHOR\n"
"       Domenico Galli <domenico.galli@bo.infn.it>\n"
"\n"
"VERSION\n"
"       Source:  %s\n"
"       Source:  %s\n"
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       top(1), /proc/stat, proc(5)\n"
"       /usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt\n"
,rcsidP,sRcsidP,FMC_VERSION,rcsidP,sRcsidP,FMC_VERSION
       );
  exit(1);
}
/*****************************************************************************/
