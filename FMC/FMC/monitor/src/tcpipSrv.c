/*****************************************************************************/
/*
 * $Log: tcpipSrv.c,v $
 * Revision 2.19  2009/10/03 20:36:03  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 2.17  2009/01/09 11:38:55  galli
 * uses sig2msg() instead of signal_number_to_name()
 *
 * Revision 2.13  2007/10/27 07:33:36  galli
 * usage() prints FMC version
 * publishes fmc_version
 *
 * Revision 2.12  2007/09/20 12:52:37  galli
 * can get DIM_DNS_NODE from a configuration file
 * -c dim_conf_file command-line option added
 *
 * Revision 2.11  2007/08/11 21:38:29  galli
 * compatible with libFMCutils v 2
 * added function usage()
 *
 * Revision 2.10  2006/10/26 08:40:20  galli
 * const intervals between refreshes
 *
 * Revision 2.9  2006/10/25 15:06:32  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 2.8  2006/02/09 11:55:30  galli
 * x86_64 compatibility problems fixed
 *
 * Revision 2.6  2005/08/28 08:35:03  galli
 * define DIM error handler
 *
 * Revision 2.4  2005/08/27 10:26:09  galli
 * sinchronous handling of signals
 * immediate reset
 *
 * Revision 2.3  2005/08/27 09:27:52  galli
 * read sleep time from command line
 *
 * Revision 2.1  2004/11/18 09:40:48  gregori
 * reset command handler added
 * SIGHUP handler added
 *
 * Revision 2.0  2004/11/11 13:45:20  gregori
 * log msg, version msg, argument control
 *
 * Revision 1.1  2004/10/14 15:17:49  gregori
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>                                  /* EOF, snprintf(), etc. */
#include <stdlib.h>                           /* strtol(), realloc(), exit() */
#include <unistd.h>                           /* getopt(), getpid(), _exit() */
#include <signal.h>               /* pthread_sigmask(), sigtimedwait(), etc. */
#include <syslog.h>                                              /* syslog() */
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
#include "getTCPIPfromProc.h"                             /* struct tcpipCnt */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#include "tcpipSrv.h"             /* SRV_NAME, DIM_SRV_LEN, DIM_SVC_LEN, ... */
/*****************************************************************************/
/* globals */
int deBug=0;
int errU=L_DIM;                                                /* error unit */
int svcNum=0;                           /* number of services (for clean-up) */
int sSvcNum=0;
int cmdNum=0;
int *svcId=NULL;
int *cmdId=NULL;
pid_t tgid;
static char rcsid[]="$Id: tcpipSrv.c,v 2.19 2009/10/03 20:36:03 galli Exp $";
char *sRcsid;
char *rcsidP,*sRcsidP;
/*****************************************************************************/
/* function prototype */
static void signalHandler(int signo);
void dimErrorHandler(int severity,int errorCode,char *message);
static void resetCmndHandler(long *tag,int *cmnd,int *size);
static void logData(struct tcpipData tcpip);
void usage(char **argv);
/*****************************************************************************/
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
  char srvPath[DIM_SRV_LEN+1]="";
  char s[DIM_SVC_LEN+1]="";
  /* sensor data */
  struct tcpipData tcpip;                    /* defined in getTCPIPfromProc.h */
  /* data to be published */
  int nRates=10;
  int nRatios=20;
  char *rateNameLs=NULL;
  char *ratioNameLs=NULL;
  int rateNameLsL=0;
  int ratioNameLsL=0;
  /* dim configuration */
  char *dimDnsNode=NULL;
  /*-------------------------------------------------------------------------*/
  sRcsid=getTCPIPsensorVersion();
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
  /* define server name */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /*-------------------------------------------------------------------------*/
  /* get initial data */
  for(i=0;i<5;i++)
  {
    tcpip=readTCPIP(deBug,errU);
    if(tcpip.success)break;
    dtq_sleep(1);
  }
  if(i>=5)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by readTCPIP()! Can't "
            "initialize service!");
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* fill static array */
  rateNameLsL=0;
  ratioNameLsL=0;
  for(i=0;i<nRates;i++)rateNameLsL+=(1+strlen(rateNames[i]));
  for(i=0;i<nRatios;i++)ratioNameLsL+=(1+strlen(ratioNames[i]));
  rateNameLs=(char*)malloc(rateNameLsL*sizeof(char));
  ratioNameLs=(char*)malloc(ratioNameLsL*sizeof(char));
  memset(rateNameLs,0,rateNameLsL);
  memset(ratioNameLs,0,ratioNameLsL);
  for(i=0,p=rateNameLs;i<nRates;i++)
  {
    strcpy(p,rateNames[i]);
    p+=(1+strlen(rateNames[i]));
  }
  for(i=0,p=ratioNameLs;i<nRatios;i++)
  {
    strcpy(p,ratioNames[i]);
    p+=(1+strlen(ratioNames[i]));
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
  /* define DIM service 0: /<HOSTNAME>/tcpip/server_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"server_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rcsidP,1+strlen(rcsidP),0,0);
  /* define DIM service 1: /<HOSTNAME>/tcpip/sensor_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsidP,1+strlen(sRcsidP),0,0);
  /* define DIM service 2: /<HOSTNAME>/tcpip/fmc_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"fmc_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  /* define DIM service 3: /<HOSTNAME>/tcpip/Nrate */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"Nrate");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&nRates,sizeof(int),0,0);
  /* define DIM service 4: /<HOSTNAME>/tcpip/Nratio */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"Nratio");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&nRatios,sizeof(int),0,0);
  /* define DIM service 5: /<HOSTNAME>/tcpip/name-rate */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"name-rate");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rateNameLs,rateNameLsL,0,0);
  /* define DIM service 6: /<HOSTNAME>/tcpip/name-ratio */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"name-ratio");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",ratioNameLs,ratioNameLsL,0,0);
  /*-------------------------------------------------------------------------*/
  /* define dynamic DIM services */
  sSvcNum=svcNum;                                                      /* =6 */
  /* define DIM service 7: /<HOSTNAME>/tcpip/success */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"success");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&tcpip.success,sizeof(int),0,0);
  /* define DIM service 8: /<HOSTNAME>/tcpip/tss */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"tss");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",tcpip.stss,24*sizeof(char),0,0);
  /* define DIM service 9: /<HOSTNAME>/tcpip/data-rate */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"data-rate");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",&(tcpip.rate),
                                  sizeof(tcpip.rate),0,0);
  /* define DIM service 10: /<HOSTNAME>/tcpip/data-ratio */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"data-ratio");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",&(tcpip.ratio),
                                  sizeof(tcpip.ratio),0,0);
  /* define DIM service 11: /<HOSTNAME>/tcpip/data-arate */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"data-arate");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",&(tcpip.arate),
                                  sizeof(tcpip.arate),0,0);
  /* define DIM service 12: /<HOSTNAME>/tcpip/data-aratio */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"data-aratio");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",&(tcpip.aratio),
                                  sizeof(tcpip.aratio),0,0);
  /* define DIM service 13: /<HOSTNAME>/tcpip/data-mrate */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"data-mrate");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",&(tcpip.mrate),
                                  sizeof(tcpip.mrate),0,0);
  /* define DIM service 14: /<HOSTNAME>/tcpip/data-mratio */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"data-mratio");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",&(tcpip.mratio),
                                  sizeof(tcpip.mratio),0,0);
  /*-------------------------------------------------------------------------*/
  /* define DIM commands */
  cmdNum=0;
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
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC TCP/IP Stack Monitor DIM "
          "Server started (TGID=%u). Using: \"%s\", \"%s\", \"FMC-%s\".",
          getpid(),rcsidP,sRcsidP,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* main loop */
  nextTime=time(NULL);
  for(i=0;;i++)
  {
    nextTime+=refreshPeriod;
    /* get data from sensors */
    tcpip=readTCPIP(deBug,errU);
    if(tcpip.success)
    { 
      int j;
      for(j=sSvcNum;j<svcNum;j++)dis_update_service(svcId[j]);
      if(deBug)logData(tcpip);
    }  
    else 
    {
      dis_update_service(svcId[6]);                               /* success */
      dis_update_service(svcId[7]);                                   /* tss */
      mPrintf(errU,ERROR,__func__,0,"Error flag returned by readTCPIT()!");
    }
    /*.......................................................................*/
    if(deBug)mPrintf(errU,DEBUG,__func__,0,"time since start: %s",tcpip.stss);
    /*.......................................................................*/
    /* sleep for waitTimeTS.tv_sec seconds, but wake-up immediately if a     */
    /* blocked signal is received, to process it (synchronously)             */
    now=time(NULL);
    if(nextTime>now)waitTimeTS.tv_sec=nextTime-now;
    else waitTimeTS.tv_sec=0;
    waitTimeTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&waitTimeTS);
    if(signo!=-1)signalHandler(signo);
    if(i%500==0 && i!=0)mPrintf(errU,DEBUG,__func__,0,"%d updates.",i);
    /*.......................................................................*/
  }
  return 0;
} 
/*****************************************************************************/
static void logData(struct tcpipData tcpip)
{
  mPrintf(errU,DEBUG,__func__,0,"IP InReceivesRate:      %8.3e %8.3e %8.3e",
          tcpip.rate.ip.InReceivesRate,
          tcpip.arate.ip.InReceivesRate,
          tcpip.mrate.ip.InReceivesRate);
  mPrintf(errU,DEBUG,__func__,0,"IP InDeliversRate:      %8.3e %8.3e %8.3e",
          tcpip.rate.ip.InDeliversRate,
          tcpip.arate.ip.InDeliversRate,
          tcpip.mrate.ip.InDeliversRate);
  mPrintf(errU,DEBUG,__func__,0,"IP ForwDatagramsRate:   %8.3e %8.3e %8.3e",
          tcpip.rate.ip.ForwDatagramsRate,
          tcpip.arate.ip.ForwDatagramsRate,
          tcpip.mrate.ip.ForwDatagramsRate);
  mPrintf(errU,DEBUG,__func__,0,"IP OutRequestsRate:     %8.3e %8.3e %8.3e",
          tcpip.rate.ip.OutRequestsRate,
          tcpip.arate.ip.OutRequestsRate,
          tcpip.mrate.ip.OutRequestsRate);
  mPrintf(errU,DEBUG,__func__,0,"IP ReasmReqdsRate:      %8.3e %8.3e %8.3e",
          tcpip.rate.ip.ReasmReqdsRate,
          tcpip.arate.ip.ReasmReqdsRate,
          tcpip.mrate.ip.ReasmReqdsRate);
  mPrintf(errU,DEBUG,__func__,0,"IP FragReqdsRate:       %8.3e %8.3e %8.3e",
          tcpip.rate.ip.FragReqdsRate,
          tcpip.arate.ip.FragReqdsRate,
          tcpip.mrate.ip.FragReqdsRate);
  mPrintf(errU,DEBUG,__func__,0,"TCP InSegsRate:         %8.3e %8.3e %8.3e",
          tcpip.rate.tcp.InSegsRate,
          tcpip.arate.tcp.InSegsRate,
          tcpip.mrate.tcp.InSegsRate);
  mPrintf(errU,DEBUG,__func__,0,"TCP OutSegsRate:        %8.3e %8.3e %8.3e",
          tcpip.rate.tcp.OutSegsRate,
          tcpip.arate.tcp.OutSegsRate,
          tcpip.mrate.tcp.OutSegsRate);
  mPrintf(errU,DEBUG,__func__,0,"UDP InDatagramsRate:    %8.3e %8.3e %8.3e",
          tcpip.rate.udp.InDatagramsRate,
          tcpip.arate.udp.InDatagramsRate,
          tcpip.mrate.udp.InDatagramsRate);
  mPrintf(errU,DEBUG,__func__,0,"UDP OutDatagramsRate:   %8.3e %8.3e %8.3e",
          tcpip.rate.udp.OutDatagramsRate,
          tcpip.arate.udp.OutDatagramsRate,
          tcpip.mrate.udp.OutDatagramsRate);
  mPrintf(errU,DEBUG,__func__,0,"IP InHdrErrorsFrac:     %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.InHdrErrorsFrac,
          tcpip.aratio.ip.InHdrErrorsFrac,
          tcpip.mratio.ip.InHdrErrorsFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP InAddrErrorsFrac:    %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.InAddrErrorsFrac,
          tcpip.aratio.ip.InAddrErrorsFrac,
          tcpip.mratio.ip.InAddrErrorsFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP InUnknownProtosFrac: %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.InUnknownProtosFrac,
          tcpip.aratio.ip.InUnknownProtosFrac,
          tcpip.mratio.ip.InUnknownProtosFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP InDiscardsFrac:      %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.InDiscardsFrac,
          tcpip.aratio.ip.InDiscardsFrac,
          tcpip.mratio.ip.InDiscardsFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP ForwDatagramsFrac:   %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.ForwDatagramsFrac,
          tcpip.aratio.ip.ForwDatagramsFrac,
          tcpip.mratio.ip.ForwDatagramsFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP InDeliversFrac:      %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.InDeliversFrac,
          tcpip.aratio.ip.InDeliversFrac,
          tcpip.mratio.ip.InDeliversFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP ReasmReqdsFrac:      %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.ReasmReqdsFrac,
          tcpip.aratio.ip.ReasmReqdsFrac,
          tcpip.mratio.ip.ReasmReqdsFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP ReasmTimeoutFrac:    %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.ReasmTimeoutFrac,
          tcpip.aratio.ip.ReasmTimeoutFrac,
          tcpip.mratio.ip.ReasmTimeoutFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP ReasmFailsFrac:      %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.ReasmFailsFrac,
          tcpip.aratio.ip.ReasmFailsFrac,
          tcpip.mratio.ip.ReasmFailsFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP ReasmOKsFrac:        %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.ReasmOKsFrac,
          tcpip.aratio.ip.ReasmOKsFrac,
          tcpip.mratio.ip.ReasmOKsFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP OutDiscardsFrac:     %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.OutDiscardsFrac,
          tcpip.aratio.ip.OutDiscardsFrac,
          tcpip.mratio.ip.OutDiscardsFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP OutNoRoutesFrac:     %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.OutNoRoutesFrac,
          tcpip.aratio.ip.OutNoRoutesFrac,
          tcpip.mratio.ip.OutNoRoutesFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP FragReqdsFrac:       %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.FragReqdsFrac,
          tcpip.aratio.ip.FragReqdsFrac,
          tcpip.mratio.ip.FragReqdsFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP FragFailsFrac:       %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.FragFailsFrac,
          tcpip.aratio.ip.FragFailsFrac,
          tcpip.mratio.ip.FragFailsFrac);
  mPrintf(errU,DEBUG,__func__,0,"IP FragCreatesFrac:     %8.3e %8.3e %8.3e",
          tcpip.ratio.ip.FragCreatesFrac,
          tcpip.aratio.ip.FragCreatesFrac,
          tcpip.mratio.ip.FragCreatesFrac);
  mPrintf(errU,DEBUG,__func__,0,"TCP RetransSegsFrac:    %8.3e %8.3e %8.3e",
          tcpip.ratio.tcp.RetransSegsFrac,
          tcpip.aratio.tcp.RetransSegsFrac,
          tcpip.mratio.tcp.RetransSegsFrac);
  mPrintf(errU,DEBUG,__func__,0,"TCP OutRstsFrac:        %8.3e %8.3e %8.3e",
          tcpip.ratio.tcp.OutRstsFrac,
          tcpip.aratio.tcp.OutRstsFrac,
          tcpip.mratio.tcp.OutRstsFrac);
  mPrintf(errU,DEBUG,__func__,0,"TCP InErrsFrac:         %8.3e %8.3e %8.3e",
          tcpip.ratio.tcp.InErrsFrac,
          tcpip.aratio.tcp.InErrsFrac,
          tcpip.mratio.tcp.InErrsFrac);
  mPrintf(errU,DEBUG,__func__,0,"UDP NoPortsFrac:        %8.3e %8.3e %8.3e",
          tcpip.ratio.udp.NoPortsFrac,
          tcpip.aratio.udp.NoPortsFrac,
          tcpip.mratio.udp.NoPortsFrac);
  mPrintf(errU,DEBUG,__func__,0,"UDP InErrorsFrac:       %8.3e %8.3e %8.3e",
          tcpip.ratio.udp.InErrorsFrac,
          tcpip.aratio.udp.InErrorsFrac,
          tcpip.mratio.udp.InErrorsFrac);
  mPrintf(errU,DEBUG,__func__,0,"time since start: %s",tcpip.stss);
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
    mPrintf(errU|L_SYS,DEBUG,__func__,0,"Signal %d (%s) caught. "
            "Resetting counters...",signo,sig2msg(signo));
    resetTCPIP();
  }
  else
  {
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
  return;
}
/*****************************************************************************/
void resetCmndHandler(long *tag,int *cmnd,int *size)
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
"       tcpipSrv - FMC TCP/IP Stack Monitor Server\n"
"\n"
"SYNOPSIS\n"
"       tcpipSrv [-d] [-c DIM_CONF_FILE] [-l LOGGER]\n"
"                [-u REFRESH_TIME_INTERVAL]\n"
"       tcpipSrv -h\n"
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
"       netstat(8), /proc/net/snmp\n"
"       /usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt\n"
,rcsidP,sRcsidP,FMC_VERSION
       );
  exit(1);
}
/*****************************************************************************/
