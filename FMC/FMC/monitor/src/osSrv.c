/*****************************************************************************/
/*
 * $Log: osSrv.c,v $
 * Revision 1.19  2009/10/03 20:31:12  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 1.13  2008/10/20 14:55:35  galli
 * groff manual
 *
 * Revision 1.5  2008/01/28 16:31:41  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 1.4  2008/01/23 09:34:39  galli
 * working version
 *
 * Revision 1.1  2008/01/22 19:36:15  galli
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>                                                /* exit() */
#include <unistd.h>                                               /* pause() */
#include <signal.h>                                              /* signal() */
#include <syslog.h>                                              /* syslog() */
#include <string.h>                                              /* strdup() */
#include <errno.h>
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
#include "getOsInfo.h"                        /* struct osInfo, readOsInfo() */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#include "osSrv.h"           /* SRV_NAME, DIM_SRV_LEN, DIM_SVC_LEN, labelV[] */
/*****************************************************************************/
/* globals */
int deBug=0;
int errU=L_DIM;                                                /* error unit */
int svcNum=0;
int sSvcNum=0;
int *svcId=NULL;
int successSvcId=0;
static char rcsid[]="$Id: osSrv.c,v 1.19 2009/10/03 20:31:12 galli Exp $";
char *sRcsid;
char *rcsidP,*sRcsidP;
/* data to be published */
char *dataBuf=NULL;                             /* data buffer               */
int dataBufS=0;                                 /* dataBuf size              */
int dataBufFS=0;                                /* dataBuf filled size       */
int dataBufCFS=0;                               /* dataBuf const filled size */
/* ************************************************************************* */
/* function prototype */
static void signalHandler(int signo);
static void dimErrorHandler(int severity,int errorCode,char *message);
void usage(int mode);
void shortUsage(void);
void dataSvcHandler(long *tag,int **address,int *size);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i=0;
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
  struct osInfo os;
  struct timeInfo timeData;
  time_t lastBootUpTime_t;
  char *localDateTime;
  char *lastBootUpTime;
  struct tm localDateTimeTm;
  struct tm lastBootUpTimeTm;
  /* data to be published */
  int fieldN=0;                                     /* number of data fields */
  char *labelBuf=NULL;                              /* label buffer          */
  int labelBufS=0;                                  /* labelBuf size         */
  int labelBufFS=0;                                 /* labelBuf filled size  */
  /* dim configuration */
  char *dimDnsNode=NULL;
  /*-------------------------------------------------------------------------*/
  sRcsid=getOsInfoSensorVersion();
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
  /* check non-option arguments */
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
  if(refreshPeriod>60 || refreshPeriod<0)
  {
    mPrintf(L_SYS|L_STD,FATAL,__func__,0,"REFRESH_TIME_INTERVAL must be in "
            "[0,60]");
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
  /* define server name /FMC/<HOSTNAME>/os */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /*-------------------------------------------------------------------------*/
  /* get data */
  for(i=0;i<5;i++)
  {
    os=readOsInfo(deBug,errU);
    if(os.success)break;
    dtq_sleep(1);
  }
  if(i>=5)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by readOsInfo()! "
            "Can't initialize service!");
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* debug printout */
  if(deBug)
  {
    mPrintf(errU,DEBUG,__func__,0,"Name: \"%s\".",os.name);
    mPrintf(errU,DEBUG,__func__,0,"Distribution: \"%s\".",os.distribution);
    mPrintf(errU,DEBUG,__func__,0,"Kernel: \"%s\".",os.kernel);
    mPrintf(errU,DEBUG,__func__,0,"Release: \"%s\".",os.release);
    mPrintf(errU,DEBUG,__func__,0,"Machine: \"%s\".",os.machine);
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate fields number */
  fieldN=sizeof(labelV)/sizeof(char*);
  /*-------------------------------------------------------------------------*/
  /* evaluate labelBuf size */
  for(i=0,labelBufS=0;i<fieldN;i++)labelBufS+=(1+strlen(labelV[i]));
  /* allocate space for labelBuf */
  labelBuf=(char*)malloc(labelBufS);
  /* fill labelBuf string array */
  for(i=0,labelBufFS=0;i<fieldN;i++)
  {
    sprintf(labelBuf+labelBufFS,"%s",labelV[i]);
    labelBufFS+=(1+strlen(labelBuf+labelBufFS));
  }
  /* evaluate dataBuf size */
  dataBufS=snprintf(NULL,0,"%s %s %s %s %s ",os.name,os.distribution,os.kernel,
                    os.release,os.machine);
  dataBufS+=2*(TIME_STR_LEN+1);
  /* allocate space for dataBuf */
  dataBuf=(char*)malloc(dataBufS*sizeof(char));
  /* fill in steady part of dataBuf string array */
  dataBufFS=0;
  sprintf(dataBuf+dataBufFS,"%s",os.name);
  dataBufFS+=(1+strlen(dataBuf+dataBufFS));
  sprintf(dataBuf+dataBufFS,"%s",os.distribution);
  dataBufFS+=(1+strlen(dataBuf+dataBufFS));
  sprintf(dataBuf+dataBufFS,"%s",os.kernel);
  dataBufFS+=(1+strlen(dataBuf+dataBufFS));
  sprintf(dataBuf+dataBufFS,"%s",os.release);
  dataBufFS+=(1+strlen(dataBuf+dataBufFS));
  sprintf(dataBuf+dataBufFS,"%s",os.machine);
  dataBufFS+=(1+strlen(dataBuf+dataBufFS));
  dataBufCFS=dataBufFS;
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
  /* define service names */
  svcNum=0; 
  /* define DIM service 0: /FMC/<HOSTNAME>/<SRV_NAME>/sensor_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsidP,1+strlen(sRcsidP),0,0);
  /* define DIM service 1: /FMC/<HOSTNAME>/<SRV_NAME>/server_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"server_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rcsidP,1+strlen(rcsidP),0,0);
  /* define DIM service 2: /FMC/<HOSTNAME>/<SRV_NAME>/fmc_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"fmc_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  /* define DIM service 3: /FMC/<HOSTNAME>/<SRV_NAME>/success */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"success");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&os.success,sizeof(int),0,0);
  successSvcId=svcId[svcNum-1];
  /* define DIM service 4: /FMC/<HOSTNAME>/<SRV_NAME>/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",labelBuf,labelBufS,0,0);
  /*-------------------------------------------------------------------------*/
  sSvcNum=svcNum;                                               /* sSvcNum=5 */
  /*-------------------------------------------------------------------------*/
  /* define DIM service 5: /FMC/<HOSTNAME>/<SRV_NAME>/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,dataSvcHandler,0);
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  dis_add_error_handler(dimErrorHandler);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  dis_start_serving(srvPath);
  /*-------------------------------------------------------------------------*/
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC Operating System Information DIM "
          "Server started (TGID=%u).  Using: \"%s\", \"%s\", \"FMC-%s\".",
          getpid(), rcsidP, sRcsidP, FMC_VERSION);
  /* ----------------------------------------------------------------------- */
  /* main loop */
  nextTime=time(NULL);
  for(i=0;;i++)
  {
    nextTime+=refreshPeriod;
    /* ..................................................................... */
    /* get data from sensors */
    timeData=readTimeInfo(errU,deBug);
    lastBootUpTime_t=timeData.now-timeData.upTime;
    /* ..................................................................... */
    localtime_r(&(timeData.now),&localDateTimeTm); 
    localtime_r(&lastBootUpTime_t,&lastBootUpTimeTm); 
    dataBufFS=dataBufCFS;
    strftime(dataBuf+dataBufFS,TIME_STR_LEN,"%Y %b %d %a %H:%M:%S %Z",
             &lastBootUpTimeTm);
    lastBootUpTime=dataBuf+dataBufFS;
    dataBufFS+=(1+strlen(dataBuf+dataBufFS));
    strftime(dataBuf+dataBufFS,TIME_STR_LEN,"%Y %b %d %a %H:%M:%S %Z",
             &localDateTimeTm);
    localDateTime=dataBuf+dataBufFS;
    dataBufFS+=(1+strlen(dataBuf+dataBufFS));
    dis_update_service(svcId[sSvcNum]);
    /* ..................................................................... */
    /* debug printout */
    if(deBug)
    {
      mPrintf(errU,DEBUG,__func__,0,"lastBootUpTime: \"%s\".",lastBootUpTime);
      mPrintf(errU,DEBUG,__func__,0,"localDateTime: \"%s\".",localDateTime);
    }
    /* ..................................................................... */
    /* sleep for waitTimeTS.tv_sec seconds, but wake-up immediately if a     */
    /* blocked signal is received, to process it (synchronously)             */
    now=time(NULL);
    if(nextTime>now)waitTimeTS.tv_sec=nextTime-now;
    else waitTimeTS.tv_sec=0;
    waitTimeTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&waitTimeTS);
    if(signo==SIGINT||signo==SIGTERM||signo==SIGHUP)signalHandler(signo);
    if(i%500==0 && i!=0)mPrintf(errU,DEBUG,__func__,0,"%d updates.",i);
  }
  return 0;
}
/* ************************************************************************* */
/* DIM SVC server handler */
void dataSvcHandler(long *tag,int **address,int *size)
{
  *address=(int*)dataBuf;
  *size=dataBufFS;
  return;
}
/* ************************************************************************* */
void dimErrorHandler(int severity,int errorCode,char *message)
{
  mPrintf(errU|L_SYS,severity+3,__func__,0,"%s! (error code = %d)",
          message,errorCode);
  if(severity==3)_exit(1);
  return;
}
/* ************************************************************************* */
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
}
/*****************************************************************************/
void shortUsage(void)
{
  char *shortUsageStr=
"SYNOPSIS\n"
"osSrv [-d...] [-N DIM_DNS_NODE] [-l LOGGER]\n"
"osSrv -h\n"
"Try \"osSrv -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  mPrintf(L_DIM|L_STD|L_SYS,INFO,__func__,0,"\n%s",shortUsageStr);
  exit(1);
}
/*****************************************************************************/
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
"osSrv.man\n"
"\n"
"..\n"
"%s"
".hw sensor\\[hy]dictionary\\[hy]file  max\\[hy]threads\\[hy]number \n"
".TH osSrv 8  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC Servers\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis osSrv\\ \\-\n"
"FMC  Operating System Information Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis osSrv\n"
".ShortOpt[] d...\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] l LOGGER\n"
".EndSynopsis\n"
".Synopsis osSrv\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"The FMC Operating System Information Server, \\fBosSrv\\fP(8) retrieves "
"information concerning the Operating System installed on the current farm "
"node and publish them using DIM.\n"
".PP\n"
"The DIM Name Server, looked up by \\fBosSrv\\fP to register the "
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
"Increase verbosity for debugging.\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef l \"\" LOGGER (integer)\n"
"Use the logger units defined in the \\fILOGGER\\fP mask to send diagnostic\n"
"and information messages. The \\fILOGGER\\fP mask can be the bitwise\n"
"\\fIOR\\fP of the following logger units:\n"
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
"UTGID of the osSrv process.\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Start the FMC Operating System Information Server, getting the DIM_DNS_NODE "
"from the file \\fI"DIM_CONF_FILE_NAME"\\fP and sending diagnostic messages "
"to the default FMC Message Logger:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/osSrv\n"
".\n"
".\n"
".SH DIM INFORMATION\n"
".PP\n"
".SS DIM Server Name\n"
".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"\n"
"where \\fIHOST_NAME\\fP is the host name of the current PC (running "
"\\fBosSrv\\fP(8)), as returned by the command \"hostname -s\", but "
"written in upper case characters.\n"
".\n"
".\n"
".SS Published DIM Services\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/server_version (string)\n"
"Publishes a NULL-terminated string containing the RCS revision label of "
"the \\fBosSrv.c\\fP source code of the current \\fBosSrv\\fP(8) "
"instance, as returned by the command \"ident /opt/FMC/sbin/osSrv\"; in "
"the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/sensor_version (string)\n"
"Publishes a NULL-terminated string containing the RCS revision label of "
"the \\fBgetOsInfo.c\\fP source code of the current "
"\\fBosSrv\\fP(8) instance, as returned by the command "
"\"ident /opt/FMC/lib/libFMCsensors*\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/fmc_version (string)\n"
"Publishes a NULL-terminated string containing the revision label of the FMC "
"package which includes the current \\fBosSrv\\fP(8) executable, in the "
"present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/success (int)\n"
"Publishes the integer 1 if data are retrieved, 0 otherwise. This services "
"can also be used to check whether the current instance of "
"\\fBosSrv\\fP(8) is up and running.\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/labels (string[7])\n"
"Publishes a sequence of 7 NULL-terminated strings containing the labels of "
"the published data:\n"
".IP\n"
"\"name\" \"distribution\" \"kernel\" \"release\" \"machine\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/data (string[7])\n"
"Publish a sequence of 7 NULL-terminated strings containing the published "
"data.\n"
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
".BR \\%%osViewer (1).\n"
".br\n"
".BR \\%%uname (1),\n"
".BR \\%%uname (2),\n"
".br\n"
".BR \\%%/proc/sys/kernel/ostype,\n"
".BR \\%%/proc/sys/kernel/osrelease,\n"
".BR \\%%/proc/sys/kernel/version,\n"
".BR \\%%/usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt.\n"
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
