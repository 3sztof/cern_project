/*****************************************************************************/
/*
 * $Log: cpuinfoSrv.c,v $
 * Revision 2.37  2009/10/03 16:06:03  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 2.31  2008/10/21 12:47:11  galli
 * publishes also boolean parameters hyperThread & multiCore
 *
 * Revision 2.30  2008/10/21 11:37:14  galli
 * publishes also parameters core_id & cpu_cores
 *
 * Revision 2.28  2008/10/20 12:54:47  galli
 * groff manual
 * use sig2msg() instead of signal_number_to_name()
 *
 * Revision 2.23  2008/01/31 23:39:51  galli
 * all cpuInfo struct elements are char* (to accomodate "N/A")
 *
 * Revision 2.22  2008/01/30 14:00:16  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 2.17  2008/01/23 16:38:14  galli
 * separate services for each cpu core
 * as agreeded with Fernando Varela Rodriguez on 2008/01/23
 *
 * Revision 2.11  2007/10/26 22:38:45  galli
 * bug fixed
 *
 * Revision 2.10  2007/10/26 13:53:22  galli
 * usage() prints FMC version
 * publishes fmc_version
 *
 * Revision 2.9  2007/09/20 10:09:54  galli
 * can get DIM_DNS_NODE from a configuration file
 * -c dim_conf_file command-line option added
 *
 * Revision 2.8  2007/08/24 12:58:40  galli
 * added units service
 *
 * Revision 2.6  2007/08/24 09:37:06  galli
 * added cpuN service
 *
 * Revision 2.5  2007/08/24 09:19:54  galli
 * service reorganization
 *
 * Revision 2.4  2007/08/11 23:00:17  galli
 * compatible with libFMCutils v 2
 * added function usage()
 *
 * Revision 2.3  2006/10/22 22:01:48  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 2.1  2004/11/18 09:40:48  gregori
 * ignore SIGHUP
 *
 * Revision 1.1  2004/10/22 16:20:21  gregori
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
#include "getCpuinfoFromProc.h"            /* struct cpusInfo, readCpuinfo() */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#include "cpuinfoSrv.h"       /* DIM_SRV_LEN, DIM_SVC_LEN, labelV[], unitV[] */
/*****************************************************************************/
/* globals */
int deBug=0;
int errU=L_DIM;                                                /* error unit */
int svcNum=0;
int *svcId=NULL;
int successSvcId=0;
pid_t tgid;
static char rcsid[]="$Id: cpuinfoSrv.c,v 2.37 2009/10/03 16:06:03 galli Exp $";
char *sRcsid;
char *rcsidP,*sRcsidP;
/*****************************************************************************/
/* function prototype */
static void signalHandler(int signo);
static void dimErrorHandler(int severity,int errorCode,char *message);
void usage(int mode);
void shortUsage(void);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i=0;
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
  /* sensor data */
  struct cpusInfo cpus;
  /* data to be published */
  int fieldN=0;                                     /* number of data fields */
  char device[DEVICE_LEN+1];
  char *deviceBuf=NULL;                             /* device buffer         */
  int deviceBufS=0;                                 /* deviceBuf size        */
  int deviceBufBS=0;                                /* deviceBuf filled size */
  char *labelBuf=NULL;                              /* label buffer          */
  int labelBufS=0;                                  /* labelBuf size         */
  int labelBufBS=0;                                 /* labelBuf filled size  */
  char *unitBuf=NULL;                               /* unit buffer           */
  int unitBufS=0;                                   /* unitBuf size          */
  int unitBufBS=0;                                  /* unitBuf filled size   */
  char **dataBuf=NULL;                              /* data buffer           */
  int *dataBufS=NULL;                               /* dataBuf size          */
  int *dataBufBS=NULL;                              /* dataBuf filled size   */
  /* dim configuration */
  char *dimDnsNode=NULL;
  /*-------------------------------------------------------------------------*/
  sRcsid=getCpuinfoSensorVersion();
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
  while((flag=getopt(argc,argv,"+h::l:dN:"))!=EOF)
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
  /* define server name /FMC/<HOSTNAME>/cpuinfo */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /*-------------------------------------------------------------------------*/
  /* get data */
  for(i=0;i<5;i++)
  {
    cpus=readCpuinfo(deBug,errU);
    if(cpus.success)break;
    dtq_sleep(1);
  }
  if(i>=5)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by readCpuinfo()! "
            "Can't initialize service!");
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate fields number */
  fieldN=sizeof(labelV)/sizeof(char*);
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
  /* evaluate labelBuf size */
  for(i=0,labelBufS=0;i<fieldN;i++)labelBufS+=(1+strlen(labelV[i]));
  /* allocate space for labelBuf */
  labelBuf=(char*)malloc(labelBufS);
  /* fill labelBuf string array */
  for(i=0,labelBufBS=0;i<fieldN;i++)
  {
    sprintf(labelBuf+labelBufBS,"%s",labelV[i]);
    labelBufBS+=(1+strlen(labelBuf+labelBufBS));
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate unitBuf size */
  for(i=0,unitBufS=0;i<fieldN;i++)unitBufS+=(1+strlen(unitV[i]));
  /* allocate space for unitBuf */
  unitBuf=(char*)malloc(unitBufS);
  /* fill unitBuf string array */
  for(i=0,unitBufBS=0;i<fieldN;i++)
  {
    sprintf(unitBuf+unitBufBS,"%s",unitV[i]);
    unitBufBS+=(1+strlen(unitBuf+unitBufBS));
  }
  /*-------------------------------------------------------------------------*/
  /* allocate space for dataBuf, dataBufS, dataBufBS */
  dataBuf=(char**)malloc(cpus.cpuN*sizeof(char*));
  dataBufS=(int*)malloc(cpus.cpuN*sizeof(int));
  dataBufBS=(int*)malloc(cpus.cpuN*sizeof(int));
  memset(dataBuf,0,cpus.cpuN*sizeof(char*));
  memset(dataBufS,0,cpus.cpuN*sizeof(int));
  memset(dataBufBS,0,cpus.cpuN*sizeof(int));
  /*-------------------------------------------------------------------------*/
  /* compose dataBuf string array */
  for(i=0;i<cpus.cpuN;i++)
  {
    int siblings, cpuCores;
    char *hyperThread="no";
    char *multiCore="no";
    /*.......................................................................*/
    siblings=(int)strtol(cpus.info[i].siblings,(char**)NULL,10);
    cpuCores=(int)strtol(cpus.info[i].cpu_cores,(char**)NULL,10);
    if(cpuCores>1 && siblings==cpuCores)
    {
      hyperThread="no";
      multiCore="yes";
    }
    else if(cpuCores==1 && siblings>1)
    {
      hyperThread="yes";
      multiCore="no";
    }
    else if(cpuCores>1 && siblings>cpuCores)
    {
      hyperThread="yes";
      multiCore="yes";
    }
    else
    {
      hyperThread="no";
      multiCore="no";
    }
    /*.......................................................................*/
    dataBufS[i]=snprintf(NULL,0,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s ",
                         cpus.info[i].vendor_id,
                         cpus.info[i].cpu_family,
                         cpus.info[i].model,
                         cpus.info[i].model_name,
                         cpus.info[i].stepping,
                         cpus.info[i].cpu_MHz,
                         cpus.info[i].kb_cache_size,
                         hyperThread,
                         multiCore,
                         cpus.info[i].physical_id,
                         cpus.info[i].siblings,
                         cpus.info[i].core_id,
                         cpus.info[i].cpu_cores,
                         cpus.info[i].bogomips);
    dataBuf[i]=(char*)realloc(dataBuf[i],dataBufS[i]*sizeof(char));
    /*.......................................................................*/
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].vendor_id);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].cpu_family);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].model);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].model_name);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].stepping);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].cpu_MHz);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].kb_cache_size);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",hyperThread);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",multiCore);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].physical_id);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].siblings);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].core_id);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].cpu_cores);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
    sprintf(dataBuf[i]+dataBufBS[i],"%s",cpus.info[i].bogomips);
    dataBufBS[i]+=(1+strlen(dataBuf[i]+dataBufBS[i]));
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
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&cpus.success,sizeof(int),0,0);
  successSvcId=svcId[svcNum-1];
  /* define DIM service 4: /FMC/<HOSTNAME>/<SRV_NAME>/devices */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"devices");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",deviceBuf,deviceBufS,0,0);
  /* define DIM service 5: /FMC/<HOSTNAME>/<SRV_NAME>/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",labelBuf,labelBufS,0,0);
  /* define DIM service 6: /FMC/<HOSTNAME>/<SRV_NAME>/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",unitBuf,unitBufS,0,0);
  /* define DIM service 7+i: /FMC/<HOSTNAME>/<SRV_NAME>/core_XX/data */
  for(i=0;i<cpus.cpuN;i++)
  {
    svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
    snprintf(s,DIM_SVC_LEN+1,"%s/core_%02d/%s",srvPath,i,"data");
    svcId[svcNum-1]=dis_add_service(strdup(s),"C",dataBuf[i],dataBufS[i],0,0);
  }
  /*-------------------------------------------------------------------------*/
  /* debug printout */
  if(deBug)
  {
    mPrintf(errU,DEBUG,__func__,0,"CPU number: %d",i,cpus.cpuN);
    for(i=0;i<cpus.cpuN;i++)
    {
      mPrintf(errU,DEBUG,__func__,0,"CPU %u vendor_id:    %s",i,
              cpus.info[i].vendor_id);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u cpu_family    %d",i,
              cpus.info[i].cpu_family);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u model         %d",i,
              cpus.info[i].model);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u model_name    %s",i,
              cpus.info[i].model_name);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u stepping      %d",i,
              cpus.info[i].stepping);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u cpu_MHz       %f",i,
              cpus.info[i].cpu_MHz);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u kb_cache_size %d",i,
              cpus.info[i].kb_cache_size);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u physical_id   %d",i,
              cpus.info[i].physical_id);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u siblings      %d",i,
              cpus.info[i].siblings);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u core_id       %d",i,
              cpus.info[i].core_id);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u cpu_cores     %d",i,
              cpus.info[i].cpu_cores);
      mPrintf(errU,DEBUG,__func__,0,"CPU %u bogomips      %f",i,
              cpus.info[i].bogomips);
    }
  }
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  dis_add_error_handler(dimErrorHandler);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  dis_start_serving(srvPath);
  /*-------------------------------------------------------------------------*/
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC CPU Information DIM Server "
          "started (TGID=%u). Using: \"%s\", \"%s\", \"FMC-%s\".", getpid(),
          rcsidP, sRcsidP, FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* main loop */
  for(i=0;;i++)
  {
    /* wait for signal, to process it synchronously */
    signo=sigwaitinfo(&signalMask,NULL);
    if(signo==SIGINT||signo==SIGTERM||signo==SIGHUP)signalHandler(signo);
  }
  return 0;
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
"cpuinfoSrv [-d...] [-N DIM_DNS_NODE] [-l LOGGER]\n"
"cpuinfoSrv -h\n"
"Try \"cpuinfoSrv -h\" for more information.\n";
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
"cpuinfoSrv.man\n"
"\n"
"..\n"
"%s"
".hw sensor\\[hy]dictionary\\[hy]file  max\\[hy]threads\\[hy]number \n"
".TH cpuinfoSrv 8  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC Servers\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis cpuinfoSrv\\ \\-\n"
"FMC CPU Information Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis cpuinfoSrv\n"
".ShortOpt[] d...\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] l LOGGER\n"
".EndSynopsis\n"
".Synopsis cpuinfoSrv\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"The FMC CPU Information Server, \\fBcpuinfoSrv\\fP(8) retrieves information "
"concerning the CPU(s) installed on the current farm node and publish them "
"using DIM.\n"
".PP\n"
"The DIM Name Server, looked up by \\fBcpuinfoSrv\\fP to register the "
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
"UTGID of the cpuinfoSrv process.\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Start the FMC CPU Information Server, getting the DIM_DNS_NODE from the file "
"\\fI"DIM_CONF_FILE_NAME"\\fP and sending diagnostic messages to the default "
"FMC Message Logger:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/cpuinfoSrv\n"
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
"\\fBcpuinfoSrv\\fP(8)), as returned by the command \"hostname -s\", but "
"written in upper case characters.\n"
".\n"
".\n"
".SS Published DIM Services\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/server_version (string)\n"
"Publishes a NULL-terminated string containing the RCS revision label of "
"the \\fBcpuinfoSrv.c\\fP source code of the current \\fBcpuinfoSrv\\fP(8) "
"instance, as returned by the command \"ident /opt/FMC/sbin/cpuinfoSrv\"; in "
"the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/sensor_version (string)\n"
"Publishes a NULL-terminated string containing the RCS revision label of "
"the \\fBgetCpuinfoFromProc.c\\fP source code of the current "
"\\fBcpuinfoSrv\\fP(8) instance, as returned by the command "
"\"ident /opt/FMC/lib/libFMCsensors*\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/fmc_version (string)\n"
"Publishes a NULL-terminated string containing the revision label of the FMC "
"package which includes the current \\fBcpuinfoSrv\\fP(8) executable, in the "
"present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/success (int)\n"
"Publishes the integer 1 if data are retrieved, 0 otherwise. This services "
"can also be used to check whether the current instance of "
"\\fBcpuinfoSrv\\fP(8) is up and running.\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/devices (string[])\n"
"Publishes a sequence of NULL-terminated strings containing the labels of "
"the CPU cores in the current node, e.g.:\n"
".IP\n"
"\"core_00\" \"core_01\" \"core_02\" ...\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/labels (string[14])\n"
"Publishes a sequence of 14 NULL-terminated strings containing the labels of "
"the published quntities:\n"
".IP\n"
"\"vendor id\" \"family\" \"model\" \"model name\" \"stepping\" \"clock\" "
"\"cache size\" \"hyper-thread\" \"multi-core\" \"physical id\" \"siblings\" "
"\"core id\" \"cpu cores\" \"performance\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/units (string[14])\n"
"Publishes a sequence of 14 NULL-terminated strings containing the units of "
"the published quntities:\n"
".IP\n"
"\"\" \"\" \"\" \"\" \"\" \"MHz\" \"KiB\" \"\" \"\" \"\" \"\" \"\" \"\" "
"\"bogomips\".\n"
".\n"
".PP\n"
".ad l\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/core_00/data (string[14])\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/core_01/data (string[14])\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/core_02/data (string[14])\n"
SVC_HEAD"/\\fIHOST_NAME\\fP/"SRV_NAME"/core_../data (string[14])\n"
".IP\n"
"Publish a sequence of 14 NULL-terminated strings containing the published "
"quntities for the core 0, 1, 2, etc.:\n"
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
".BR \\%%cpuinfoViewer (1).\n"
".br\n"
".BR \\%%/proc/cpuinfo\n"
".br\n"
".BR \\%%/usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt\n"
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
