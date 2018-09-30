/*****************************************************************************/
/*
 * $Log: coalSrv.c,v $
 * Revision 2.27  2009/10/03 16:01:17  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 2.21  2009/01/07 10:14:42  galli
 * uses sig2msg() instead of signal_number_to_name()
 *
 * Revision 2.14  2007/10/26 13:40:52  galli
 * usage() prints FMC version
 * publishes fmc_version
 *
 * Revision 2.13  2007/09/20 09:12:54  galli
 * can get DIM_DNS_NODE from a configuration file
 * -c dim_conf_file command-line option added
 *
 * Revision 2.12  2007/09/05 12:29:58  galli
 * Changed DIM services:
 * only 1 data service for all the variable data
 * usage() modified
 *
 * Revision 2.10  2007/08/11 08:43:24  galli
 * compatible with libFMCutils v 2
 * added function usage()
 *
 * Revision 2.9  2006/10/26 08:09:18  galli
 * const intervals between refreshes
 *
 * Revision 2.8  2006/10/23 07:41:29  galli
 * bug fixed
 *
 * Revision 2.7  2006/10/22 21:58:56  galli
 * added service /<HOSTNAME>/coalescence/ifN
 *
 * Revision 2.6  2006/10/21 06:33:47  galli
 * fast reset
 *
 * Revision 2.4  2006/10/20 22:37:34  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 2.3  2006/02/09 11:47:14  galli
 * x86_64 compatibility problems fixed
 *
 * Revision 2.1  2004/11/18 09:39:40  gregori
 * reset command handler added
 * sighup handler added
 *
 * Revision 1.1  2004/11/08 15:59:41  gregori
 * Initial revision
 *
 */
/*****************************************************************************/
#include <stdio.h>                                  /* EOF, snprintf(), etc. */
#include <stdlib.h>                           /* strtol(), realloc(), exit() */
#include <unistd.h>                           /* getopt(), getpid(), _exit() */
#include <signal.h>               /* pthread_sigmask(), sigtimedwait(), etc. */
#include <string.h>       /* strlen(), strncpy(), strdup(), strerror(), etc. */
#include <errno.h>                                                  /* errno */
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
#include "getNIFfromProc.h"                    /* struct nifsData, readNIF() */
#include "getIrqFromProc.h"       /* struct interruptsStat, readInterrupts() */
#include "getCoalescence.h"          /* struct coalescenceFactors, getCoal() */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#include "coalSrv.h"                   /* SRV_NAME, DIM_SRV_LEN, DIM_SVC_LEN */
/*****************************************************************************/
/* globals */
int deBug=0;
int errU=L_DIM;                                                /* error unit */
int svcNum=0;
int sSvcNum=0;
int cmdNum=0;
int *svcId=NULL;
int *cmdId=NULL;
char *p=NULL;
float *dataBuf=NULL;                                  /* data                */
int dataBufS=0;                                       /* dataBuf size        */
char *labelBuf=NULL;                                  /* label               */
int labelBufS=0;                                      /* labelBuf size       */
pid_t tgid;
static char rcsid[]="$Id: coalSrv.c,v 2.27 2009/10/03 16:01:17 galli Exp $";
char *sRcsid0,*sRcsid1,*sRcsid2;
char *rcsidP,*sRcsid0P,*sRcsid1P,*sRcsid2P;
/*****************************************************************************/
/* function prototype */
static void signalHandler(int signo);
static void signalHupHandler(int signo);
static void resetCmdHandler(long *tag,int *cmnd,int *size);
static void dimErrorHandler(int severity,int errorCode,char *message);
void usage(char **argv);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j;
  /* pointers */
  char *p;
  /* getopt variables */
  int flag=0;
  /* signal handling */
  sigset_t signalMask;
  int signo=0;
  /* dim server parameters */
  char srvPath[DIM_SRV_LEN+1]="";
  char s[DIM_SVC_LEN+1]="";
  int successSvcId=0;
  int tssSvcId=0;
  /* sensor data */
  struct nifsData nifs;
  struct interruptsStat irqStat;
  struct coalescenceFactors coals;
  /* refresh period */
  int refreshPeriod=20;
  struct timespec waitTimeTS={20,0};
  time_t now,nextTime;
  /* dim configuration */
  char *dimDnsNode=NULL;
  /*-------------------------------------------------------------------------*/
  sRcsid0=getCoalSensorVersion();
  sRcsid1=getNIFsensorVersion();
  sRcsid2=getIRQsensorVersion();
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid, sRcsid0, sRcsid1, sRcsid2 */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  sRcsid0P=strchr(sRcsid0,':')+2;
  for(i=0,p=sRcsid0P;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  sRcsid1P=strchr(sRcsid1,':')+2;
  for(i=0,p=sRcsid1P;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  sRcsid2P=strchr(sRcsid2,':')+2;
  for(i=0,p=sRcsid2P;i<5;i++)p=1+strchr(p,' ');
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
  if(refreshPeriod>20 || refreshPeriod<0)
  {
    mPrintf(L_SYS|L_STD,FATAL,__func__,0,"refresh_time_interval must be in "
            "[0,20]");
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
  /* resetCmdHandler(). In kernels >= 2.4.20 it makes no differences.       */
  tgid=getpid();
  /*-------------------------------------------------------------------------*/
  /* define server name */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /*-------------------------------------------------------------------------*/
  /* reset counters */
  resetNIF();
  resetInterrupts();
  resetCoal();
  /*-------------------------------------------------------------------------*/
  /* get initial data */
  for(i=0;i<5;i++)
  {
    nifs=readNIF(deBug,errU);
    if(nifs.success)break;
    dtq_sleep(1);
  }
  if(i>=5)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by readNIF()! Can't "
            "initialize service!");
    exit(1);  
  }
  for(i=0;i<5;i++)
  {
    irqStat=readInterrupts(deBug,errU);
    if(irqStat.success)break;
    dtq_sleep(1);
  }
  if(i>=5)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by readInterrupts()! "
            "Can't initialize service!");
    exit(1);  
  }
  for(i=0;i<5;i++)
  {
    coals=getCoal(nifs,irqStat,deBug,errU);
    if(coals.success)break;
    dtq_sleep(1);
  }
  if(i>=5)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by getCoal()! Can't "
            "initialize service!");
    exit(1);  
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate memory for NIF names */
  labelBufS=0; 
  for(j=0,labelBufS=0;j<coals.ethN;j++)
  {
    labelBufS+=(1+strlen(coals.data[j].ifName));
  }
  /* allocate memory for NIF names */
  labelBuf=(char*)malloc(labelBufS);	
  memset(labelBuf,0,labelBufS);
  /* fill memory with NIF names */
  for(j=0,p=labelBuf;j<coals.ethN;j++)
  {
    sprintf(p,"%s",coals.data[j].ifName);
    p+=(1+strlen(p));
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate memory for data */
  dataBufS=3*coals.ethN*sizeof(float);
  /* allocate memory for data */
  dataBuf=(float*)malloc(dataBufS);
  memset(dataBuf,0,dataBufS);
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
  /* define DIM service 0: /<HOSTNAME>/Coalescence/server_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"server_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rcsidP,1+strlen(rcsidP),0,0);
  /* define DIM service 1: /<HOSTNAME>/Coalescence/sensor_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsid0P,
                                  1+strlen(sRcsid0P),0,0);
  /* define DIM service 2: /<HOSTNAME>/Coalescence/fmc_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"fmc_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  /* define DIM service 3: /<HOSTNAME>/Coalescence/ifN */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"ifN");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&coals.ethN,sizeof(int),0,0);
  /* define DIM service 4: /<HOSTNAME>/Coalescence/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",labelBuf,labelBufS,0,0);
  /*-------------------------------------------------------------------------*/
  sSvcNum=svcNum;                                                      /* =4 */
  /* define dynamic DIM services */
  /* define DIM service 5: /<HOSTNAME>/Coalescence/success */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"success");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&coals.success,sizeof(int),0,0);
  successSvcId=svcId[svcNum-1];
  /* define DIM service 6: /<HOSTNAME>/Coalescence/tss */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"tss");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",coals.stss,24,0,0);
  tssSvcId=svcId[svcNum-1];
  /* define DIM service 7: /<HOSTNAME>/Coalescence/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",dataBuf,dataBufS,0,0);
  /*-------------------------------------------------------------------------*/
  /* define DIM commands */
  cmdNum=0;
  cmdId=(int*)realloc(cmdId,(++cmdNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"reset");
  cmdId[cmdNum-1]=dis_add_cmnd(strdup(s),"C",resetCmdHandler,1);
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  dis_add_error_handler(dimErrorHandler);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  dis_start_serving(srvPath);
  /*-------------------------------------------------------------------------*/
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC Network Interrupt Coalescence "
          "Monitor DIM Server started (TGID=%u). Using: \"%s\", \"%s\", "
          "\"%s\", \"%s\", \"FMC-%s\".",getpid(),rcsidP,sRcsid0P,sRcsid1P,
          sRcsid2P,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* main loop */
  nextTime=time(NULL);
  for(i=0;;i++)
  {
    nextTime+=refreshPeriod;
    /*.......................................................................*/
    /* get data from sensors */
    for(j=0;j<5;j++)
    {
      nifs=readNIF(deBug,errU);
      if(nifs.success)break;
      dtq_sleep(1);
    }
    if(!nifs.success)
    {                	
       mPrintf(errU,ERROR,__func__,0,"Error flag returned by readNIF()!");
       coals.success=0;
       dis_update_service(successSvcId); 
       dis_update_service(tssSvcId); 
       dtq_sleep(1);
       continue;
    }
    for(j=0;j<5;j++)
    {
      irqStat=readInterrupts(deBug,errU);
      if(irqStat.success)break;
      dtq_sleep(1);
    }
    if(!irqStat.success)
    {
       mPrintf(errU,ERROR,__func__,0,"Error flag returned by "
               "readInterrupts()!");
       coals.success=0;
       dis_update_service(successSvcId); 
       dis_update_service(tssSvcId); 
       dtq_sleep(1);
       continue;
    }
    for(j=0;j<5;j++)
    {
      coals=getCoal(nifs,irqStat,deBug,errU);
      if(coals.success)break;
      dtq_sleep(1);
    }
    if(!coals.success)
    {  
       mPrintf(errU,ERROR,__func__,0,"Error flag returned by getCoal()!");
       coals.success=0;
       dis_update_service(successSvcId); 
       dis_update_service(tssSvcId); 
       dtq_sleep(1);
       continue;
    }
    /*.......................................................................*/
    /* Fill dataBuf with data */
    for(j=0;j<coals.ethN;j++)
    {
      dataBuf[j]=coals.data[j].coalFactor;
      dataBuf[j+coals.ethN]=coals.data[j].aCoalFactor;
      dataBuf[j+2*coals.ethN]=coals.data[j].mCoalFactor;
    }
    if(deBug)
    {
      for(j=0;j<coals.ethN;j++)
      {
        mPrintf(errU,DEBUG,__func__,0,"If: %-5s %8.3e %8.3e %8.3e",
                coals.data[j].ifName,coals.data[j].coalFactor,
                coals.data[j].aCoalFactor,coals.data[j].mCoalFactor);
      }
    }
    /*.......................................................................*/
    /* update dynamic DIM services */ 
    for(j=sSvcNum;j<svcNum;j++)dis_update_service(svcId[j]);
    if(deBug)mPrintf(errU,DEBUG,__func__,0,"time since start: %s",coals.stss);
    /*.......................................................................*/
    /* sleep for waitTimeTS.tv_sec seconds, but wake-up immediately if a     */
    /* blocked signal is received, to process it (synchronously)             */
    now=time(NULL);
    if(nextTime>now)waitTimeTS.tv_sec=nextTime-now;
    else waitTimeTS.tv_sec=0;
    waitTimeTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&waitTimeTS);
    if(signo==SIGHUP)signalHupHandler(signo);
    else if(signo!=-1)signalHandler(signo);
    if(i%500==0 && i!=0)mPrintf(errU,DEBUG,__func__,0,"%d updates.",i);
    /*.......................................................................*/
  }
  return 0;
}
/*****************************************************************************/
/* signalHandler() executed synchronously in the main thread. So don't warry */
/* about async-signal-safe.                                                  */
static void signalHandler(int signo)
{
  int i;
  /*-------------------------------------------------------------------------*/
  for(i=0;i<svcNum;i++)dis_remove_service(svcId[i]);
  for(i=0;i<cmdNum;i++)dis_remove_service(cmdId[i]);
  dis_stop_serving();
  if(signo==SIGTERM || signo==SIGINT)
  {
    mPrintf(errU|L_SYS,WARN,__func__,0,"Signal %d (%s) caught. "
          "Exiting...",signo,sig2msg(signo));
    _exit(0);
  }
  else
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"Signal %d (%s) caught. "
          "Exiting...",signo,sig2msg(signo));
    _exit(signo);
  }
}
/*****************************************************************************/
static void signalHupHandler(int signo)
{
  /* request a counter reset */
  resetNIF();
  resetInterrupts();
  resetCoal();
  return;
}
/*****************************************************************************/
void resetCmdHandler(long *tag,int *cmnd,int *size)
{
  if(*tag==1)
  {
    /* request a counter reset */
    kill(tgid,SIGHUP);
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
/*****************************************************************************/
void usage(char **argv)
{
  rPrintf(L_STD|L_SYS,2,
"\n"
"NAME\n"
"       coalSrv - FMC Network Interrupt Coalescence Monitor Server\n"
"\n"
"SYNOPSIS\n"
"       coalSrv [-d] [-c DIM_CONF_FILE] [-l LOGGER]\n"
"               [-u REFRESH_TIME_INTERVAL]\n"
"       coalSrv -h\n"
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
"AUTHOR\n"
"       Domenico Galli <domenico.galli@bo.infn.it>\n"
"\n"
"VERSION\n"
"       Source:  %s\n"
"       Source:  %s\n"
"       Source:  %s\n"
"       Source:  %s\n"
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       ifconfig(8)\n"
"       /proc/interrupts\n"
"       /proc/net/dev\n"
"       /usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt\n"
,rcsidP,sRcsid0P,sRcsid1P,sRcsid2P,FMC_VERSION
       );
  exit(1);
}
/*****************************************************************************/
