/*****************************************************************************/
/*
 * $Log: irqSrv.c,v $
 * Revision 2.24  2009/10/03 17:23:10  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 2.17  2007/12/12 14:47:43  galli
 * bug fixed
 *
 * Revision 2.14  2007/10/26 22:33:20  galli
 * usage() prints FMC version
 * publishes fmc_version
 *
 * Revision 2.13  2007/09/20 10:37:48  galli
 * can get DIM_DNS_NODE from a configuration file
 * -c dim_conf_file command-line option added
 *
 * Revision 2.12  2007/09/07 13:58:25  galli
 * added service total_rates
 *
 * Revision 2.11  2007/09/04 12:58:58  galli
 * data order changed
 *
 * Revision 2.10  2007/09/04 12:49:49  galli
 * bug fixed
 *
 * Revision 2.9  2007/09/04 11:33:03  galli
 * usage() modified
 *
 * Revision 2.8  2007/09/04 11:27:49  galli
 * Changed DIM services:
 * only 1 data service for all the variable data
 *
 * Revision 2.7  2007/08/11 22:25:47  galli
 * compatible with libFMCutils v 2
 * added function usage()
 *
 * Revision 2.6  2006/10/26 08:22:00  galli
 * const intervals between refreshes
 *
 * Revision 2.5  2006/10/23 16:05:06  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 2.4  2006/02/09 11:50:22  galli
 * x86_64 compatibility problems fixed
 *
 * Revision 2.1  2004/11/18 09:40:48  gregori
 * reset command handler added
 * SIGHUP handler added
 *
 * Revision 2.0  2004/11/11 13:46:52  gregori
 *
 * Revision 1.1  2004/10/15 11:59:49  gregori
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
#include "getIrqFromProc.h"                         /* struct interruptsStat */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#include "irqSrv.h"               /* SRV_NAME, DIM_SRV_LEN, DIM_SVC_LEN, ... */
/*****************************************************************************/
/* globals */     
int deBug=0;                
int errU=L_DIM;                                                /* error unit */
int svcNum=0;
int sSvcNum=0;
int cmdNum=0;
struct interruptsStat irqStat;
int *svcId=NULL;
int *cmdId=NULL;
int irqLineLen;
char *dataBuf=NULL;                                   /* data                */
int dataBufS=0;                                       /* dataBuf size        */
int oldDataBufS=0;                                    /* old dataBuf size    */
int dataBufBS=0;                                      /* dataBuf busy size   */
float *totalRatesBuf=NULL;                            /* totalRates          */
int totalRatesBufS=0;                                 /* totalRatesBuf size  */
int oldTotalRatesBufS=0;                              /* old totalRates size */
pid_t tgid;
static char rcsid[]="$Id: irqSrv.c,v 2.24 2009/10/03 17:23:10 galli Exp $";
char *sRcsid;
char *rcsidP,*sRcsidP;
/*****************************************************************************/
/* function prototype */
static void signalHandler(int signo);
static void signalHupHandler(int signo);
static void resetCmndHandler(long *tag,int *cmnd,int *size);
void dimErrorHandler(int severity,int errorCode,char *message);
void dataSvcHandler(long *tag,int **address,int *size);
void totalRatesSvcHandler(long *tag,int **address,int *size);
void usage(char **argv);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j,k;
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
  char s[DIM_SVC_LEN+1]="";
  /* dim configuration */
  char *dimDnsNode=NULL;
  /*-------------------------------------------------------------------------*/
  sRcsid=getIRQsensorVersion();
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
  if(refreshPeriod>60 || refreshPeriod<0)
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
  /* define server name */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /*-------------------------------------------------------------------------*/
  /* reset counters */
  resetInterrupts();
  /* read data */
  for(i=0;i<5;i++)
  {
    irqStat=readInterrupts(deBug,errU);
    if(irqStat.success)break;
    dtq_sleep(1);
  }
  if(i>=5)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by readIntrrupts()! "
            "Can't initialize service!");
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate space needed for data concerning 1 interrupt source */
  irqLineLen=DEVICE_LEN+1+NUM_LEN+1+3*irqStat.cpuN*(RATE_LEN+1);
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
  /* define DIM service 0: /<HOSTNAME>/IRQ/server_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"server_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rcsidP,1+strlen(rcsidP),0,0);
  /* define DIM service 1: /<HOSTNAME>/IRQ/sensor_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsidP,1+strlen(sRcsidP),0,0);
  /* define DIM service 2: /<HOSTNAME>/IRQ/fmc_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"fmc_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  /* define DIM service 3: /<HOSTNAME>/IRQ/cpuN */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"cpuN");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&irqStat.cpuN,sizeof(int),
                                  0,0);
  sSvcNum=svcNum;
  /*-------------------------------------------------------------------------*/
  /* define dynamic DIM services */
  /* define DIM service 4: /<HOSTNAME>/IRQ/tss */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"tss");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",irqStat.stss,24,0,0);
  /* define DIM service 5: /<HOSTNAME>/IRQ/success */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"success");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&irqStat.success,sizeof(int),
                                  0,0);
  /*-------------------------------------------------------------------------*/
  /* define dynamic DIM services with variable array size */
  /* define DIM service 6: /<HOSTNAME>/IRQ/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,dataSvcHandler,0);
  /* define DIM service 7: /<HOSTNAME>/IRQ/total_rates */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"total_rates");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",0,0,totalRatesSvcHandler,0);
  /*-------------------------------------------------------------------------*/
  /* define DIM commands */
  cmdId=(int*)realloc(cmdId,(++cmdNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"reset");
  cmdId[cmdNum-1]=dis_add_cmnd(strdup(s),"C",resetCmndHandler,1);
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  dis_add_error_handler(dimErrorHandler);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  dis_start_serving(srvPath);
  /*-------------------------------------------------------------------------*/
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC Interrupt Monitor DIM Server "
          "started (TGID=%u). Using: \"%s\", \"%s\", \"FMC-%s\".",getpid(),
          rcsidP,sRcsidP,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* main loop */
  nextTime=time(NULL);
  for(k=0;;k++)
  {    
    nextTime+=refreshPeriod;
    for(i=0;i<5;i++)
    {
      irqStat=readInterrupts(deBug,errU);
      if(irqStat.success)break;
      dtq_sleep(1);
    }
    if(irqStat.success)
    {
      /*.....................................................................*/
      /* clean dataBuf */
      if(dataBufS)memset(dataBuf,0,dataBufS);
      /*.....................................................................*/
      /* (re)allocate space for dataBuf if needed */
      oldDataBufS=dataBufS;
      dataBufS=irqStat.srcN*irqLineLen;
      if(dataBufS>oldDataBufS)
      {
        if(deBug)
        {
          mPrintf(errU,VERB,__func__,9, "String buffer %d B, busy: %d B, "
                  "maxIrqLineSize: %d B. Reallocate buffer to %d B.",
                  oldDataBufS,dataBufBS,irqLineLen,dataBufS);
        }
        dataBuf=(char*)realloc(dataBuf,dataBufS);
        /* clean the new elements of dataBuf */
        memset(dataBuf+oldDataBufS,0,dataBufS-oldDataBufS);
      }
      /*.....................................................................*/
      /* fill dataBuf */
      for(j=0,dataBufBS=0;j<irqStat.srcN;j++)
      {
        /* source name */
        snprintf(dataBuf+dataBufBS,DEVICE_LEN+1,"%s",irqStat.srcName[j]);
        dataBufBS+=(1+strlen(dataBuf+dataBufBS));
        /* IRQ number */
        snprintf(dataBuf+dataBufBS,NUM_LEN+1,"%d",irqStat.irqN[j]);
        dataBufBS+=(1+strlen(dataBuf+dataBufBS));
        for(i=0;i<irqStat.cpuN;i++)
        {
          snprintf(dataBuf+dataBufBS,NUM_LEN+1,"%8.3e",irqStat.irqRate[j][i]);
          dataBufBS+=(1+strlen(dataBuf+dataBufBS));
        }
        for(i=0;i<irqStat.cpuN;i++)
        {
          snprintf(dataBuf+dataBufBS,NUM_LEN+1,"%8.3e",irqStat.aIrqRate[j][i]);
          dataBufBS+=(1+strlen(dataBuf+dataBufBS));
        }
        for(i=0;i<irqStat.cpuN;i++)
        {
          snprintf(dataBuf+dataBufBS,NUM_LEN+1,"%8.3e",irqStat.mIrqRate[j][i]);
          dataBufBS+=(1+strlen(dataBuf+dataBufBS));
        }
      }
      /*.....................................................................*/
      /* clean totalRatesBuf */
      if(totalRatesBufS)memset(totalRatesBuf,0,totalRatesBufS);
      /*.....................................................................*/
      /* (re)allocate space for totalRatesBuf if needed */
      oldTotalRatesBufS=totalRatesBufS;
      totalRatesBufS=irqStat.srcN*sizeof(float);
      if(totalRatesBufS>oldTotalRatesBufS)
      {
        if(deBug)
        {
          mPrintf(errU,VERB,__func__,9, "Float buffer %d B. Reallocate buffer "
                  "to %d B.",oldTotalRatesBufS,totalRatesBufS);
        }
        totalRatesBuf=(float*)realloc(totalRatesBuf,totalRatesBufS);
        /* clean the new elements of totalRateBuf */
        memset(totalRatesBuf+oldTotalRatesBufS,0,
               totalRatesBufS-oldTotalRatesBufS);
      }
      /*.....................................................................*/
      /* fill totalRateBuf */
      for(j=0;j<irqStat.srcN;j++)
      {
        float sum;
        for(i=0,sum=0;i<irqStat.cpuN;i++)
        {
          sum+=irqStat.irqRate[j][i];
        }
        totalRatesBuf[j]=sum;
      }
      /*.....................................................................*/
      /* update dynamic DIM services */
      for(i=sSvcNum;i<svcNum;i++)dis_update_service(svcId[i]);
      /*.....................................................................*/
      if(deBug>1)
      {
        mPrintf(errU,DEBUG,__func__,0,"CPU number        %u",irqStat.cpuN);
        mPrintf(errU,DEBUG,__func__,0,"IRQ source number %u",irqStat.srcN);
        for(i=0;i<irqStat.cpuN;i++)
        {
          for(j=0;j<irqStat.srcN;j++)
          {
            mPrintf(errU,VERB,__func__,0,"rate(\"%s\",CPU%02d)=%f",
                    irqStat.srcName[j],i,irqStat.irqRate[j][i]);
            mPrintf(errU,VERB,__func__,0,"arate(\"%s\",CPU%02d)=%f",
                    irqStat.srcName[j],i,irqStat.aIrqRate[j][i]);
            mPrintf(errU,VERB,__func__,0,"mrate(\"%s\",CPU%02d)=%f",
                    irqStat.srcName[j],i,irqStat.mIrqRate[j][i]);
          }
        }
        mPrintf(errU,DEBUG,__func__,0,"time since start: %s",irqStat.stss);
      }
      /*.....................................................................*/
    }                                                 /* if(irqStat.success) */
    else /* !irqStat.success */
    {
      dis_update_service(svcId[3]);                               /* success */
      dis_update_service(svcId[4]);                                   /* tss */
      mPrintf(errU,ERROR,__func__,0," Error flag returned by "
              "readInterrupts()!");
    }
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
    if(k%500==0 && k!=0)mPrintf(errU,DEBUG,__func__,0,"%d updates.",k);
    /*.......................................................................*/
  }
  return 0;
}
/*****************************************************************************/
void dataSvcHandler(long *tag,int **address,int *size)
{
  *address=(int*)dataBuf;
  *size=dataBufBS;
}
/*****************************************************************************/
void totalRatesSvcHandler(long *tag,int **address,int *size)
{
  *address=(int*)totalRatesBuf;
  *size=totalRatesBufS;
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
  resetInterrupts();
  return;
}
/*****************************************************************************/
static void resetCmndHandler(long *tag,int *cmnd,int *size)
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
"       irqSrv - FMC Interrupt Monitor Server\n"
"\n"
"SYNOPSIS\n"
"       irqSrv [-d] [-c DIM_CONF_FILE] [-l LOGGER] [-u REFRESH_TIME_INTERVAL]\n"
"       irqSrv -h\n"
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
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       /proc/interrupts\n"
"       /usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/"
        "proc.txt\n"
,rcsidP,sRcsidP,FMC_VERSION
       );
  exit(1);
}
/*****************************************************************************/
