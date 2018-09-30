/*****************************************************************************/
/*
 * $Log: smartSrv.c,v $
 * Revision 1.21  2009/10/03 20:33:59  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 1.17  2009/01/08 14:05:50  galli
 * uses sig2msg() instead of signal_number_to_name()
 *
 * Revision 1.13  2007/10/27 07:21:44  galli
 * usage() prints FMC version
 * publishes fmc_version
 *
 * Revision 1.12  2007/09/20 12:48:48  galli
 * can get DIM_DNS_NODE from a configuration file
 * -c dim_conf_file command-line option added
 *
 * Revision 1.10  2007/08/10 15:05:01  galli
 * compatible with libFMCutils v 2
 * added function usage()
 *
 * Revision 1.9  2007/08/07 13:16:49  galli
 * compatible with smartctl version 5.33
 *
 * Revision 1.7  2006/10/26 14:06:47  galli
 * compatible with libFMCutils v 1.0
 * const intervals between refreshes
 *
 * Revision 1.6  2006/07/05 09:32:14  galli
 * works also with SCSI disks
 *
 * Revision 1.2  2006/04/13 14:53:41  galli
 * read alternate smartctl path from SMARTCTL_CMD environment variable
 * check accessibility of smartctl
 * check if the current process is executed as user root
 *
 * Revision 1.1  2006/04/12 11:57:46  galli
 * Initial revision
 */
/*****************************************************************************/
/*  DIM SRV:                                                                 */
/*    /FMC/<HOSTNAME>/disk                                                   */
/*  DIM SVC:                                                                 */
/*    /FMC/<HOSTNAME>/disk/server_version                                    */
/*    /FMC/<HOSTNAME>/disk/sensor_version                                    */
/*    /FMC/<HOSTNAME>/disk/fmc_version                                       */
/*    /FMC/<HOSTNAME>/disk/success                                           */
/*    /FMC/<HOSTNAME>/disk/N                                                 */
/*    /FMC/<HOSTNAME>/disk/name                                              */
/*    /FMC/<HOSTNAME>/disk/<dev>/info/type                                   */
/*    /FMC/<HOSTNAME>/disk/<dev>/info/size                                   */
/*    /FMC/<HOSTNAME>/disk/<dev>/info/deviceModel                            */
/*    /FMC/<HOSTNAME>/disk/<dev>/info/serialNumber                           */
/*  DIM SVC (S)ATA disks:                                                    */
/*    /FMC/<HOSTNAME>/disk/<dev>/info/firmwareVersion                        */
/*    /FMC/<HOSTNAME>/disk/<dev>/info/ataVersion                             */
/*    /FMC/<HOSTNAME>/disk/<dev>/info/ataVersion                             */
/*    /FMC/<HOSTNAME>/disk/<dev>/info/ataStandard                            */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/N                                      */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/id                                     */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/name                                   */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/flag                                   */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/value                                  */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/worst                                  */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/threshold                              */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/type                                   */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/updated                                */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/whenFailed                             */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/rawValue                               */
/*  DIM SVC SCSI disks:                                                      */
/*    /FMC/<HOSTNAME>/disk/<dev>/info/transportProtocol                      */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/N                                      */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/name                                   */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/value                                  */
/*    /FMC/<HOSTNAME>/disk/<dev>/attr/units                                  */
/*    /FMC/<HOSTNAME>/disk/<dev>/err/N                                       */
/*    /FMC/<HOSTNAME>/disk/<dev>/err/name                                    */
/*    /FMC/<HOSTNAME>/disk/<dev>/err/value/read                              */
/*    /FMC/<HOSTNAME>/disk/<dev>/err/value/write                             */
/*    /FMC/<HOSTNAME>/disk/<dev>/err/value/verify                            */
/*****************************************************************************/
#include <stdio.h>                                  /* EOF, snprintf(), etc. */
#include <stdlib.h>                           /* strtol(), realloc(), exit() */
#include <unistd.h>                           /* getopt(), getpid(), _exit() */
#include <signal.h>               /* pthread_sigmask(), sigtimedwait(), etc. */
#include <syslog.h>                                              /* syslog() */
#include <string.h>       /* strlen(), strncpy(), strdup(), strerror(), etc. */
#include <errno.h>                                                  /* errno */
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>
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
#include "getSMART.h"      /* smartAtaAttrs_t physDiskData_t diskType_t etc. */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#include "smartSrv.h"             /* SRV_NAME, DIM_SRV_LEN, DIM_SVC_LEN, ... */
/*****************************************************************************/
/* globals */
int deBug=0;
int errU=L_DIM;                                                /* error unit */
int svcNum=0;
int cmdNum=0;
int *svcId=NULL;
int *cmdId=NULL;
char *smartctlCmd=NULL;
physDiskData_t disk;                                /* defined in getSMART.h */
char **devList=NULL;
char **typeList=NULL;
smartAtaAttrs_t *ataAttrs=NULL;                     /* defined in getSMART.h */
smartScsiAttrs_t *scsiAttrs=NULL;                   /* defined in getSMART.h */
smartScsiErrs_t *scsiErrs=NULL;                     /* defined in getSMART.h */
static char rcsid[]="$Id: smartSrv.c,v 1.21 2009/10/03 20:33:59 galli Exp $";
char *sRcsid;
char *rcsidP,*sRcsidP;
/*****************************************************************************/
/* function prototype */
static void signalHandler(int signo);
void dimErrorHandler(int severity,int errorCode,char *message);
int smartDataUpdate(void);
void ataAttrIdHandler(long *tag,int **address,int *size);
void ataAttrNameHandler(long *tag,int **address,int *size);
void ataAttrFlagHandler(long *tag,int **address,int *size);
void ataAttrValueHandler(long *tag,int **address,int *size);
void ataAttrWorstHandler(long *tag,int **address,int *size);
void ataAttrThesholdHandler(long *tag,int **address,int *size);
void ataAttrTypeHandler(long *tag,int **address,int *size);
void ataAttrUpdatedHandler(long *tag,int **address,int *size);
void ataAttrWhenfailedHandler(long *tag,int **address,int *size);
void ataAttrRawvalueHandler(long *tag,int **address,int *size);
void scsiAttrNameHandler(long *tag,int **address,int *size);
void scsiAttrValueHandler(long *tag,int **address,int *size);
void scsiAttrUnitsHandler(long *tag,int **address,int *size);
void scsiErrNameHandler(long *tag,int **address,int *size);
void scsiErrRvalueHandler(long *tag,int **address,int *size);
void scsiErrWvalueHandler(long *tag,int **address,int *size);
void scsiErrVvalueHandler(long *tag,int **address,int *size);
void usage(char **argv);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j;
  /* pointers */
  char *p=NULL;
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
  diskType_t dt;                                    /* defined in getSMART.h */
  ataDiskInfo_t ai;                                 /* defined in getSMART.h */
  scsiDiskInfo_t si;                                /* defined in getSMART.h */
  /* data to be published */
  char sType[4][8]={"Unknown","ATA","SATA","SCSI"};
  char *devString=NULL;
  int devStringLen=0;
  uint64_t *sizeList=NULL;
  char **deviceModelList=NULL;
  char **serialNumberList=NULL;
  char **firmwareVersionList=NULL;
  int *ataVersionList=NULL;
  char **ataStandardList=NULL;
  char **scsiTranspProtoList=NULL;
  uid_t uid=-1;
  int success=1;
  int sSvcNum=0;
  int successSvcId=0;
  /* dim configuration */
  char *dimDnsNode=NULL;
  /*-------------------------------------------------------------------------*/
  sRcsid=getSMARTsensorVersion();
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
  while((flag=getopt(argc,argv,"+l:dN:u:E:h::"))!=EOF)
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
      case 'E':
        smartctlCmd=optarg;
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
  /* get smartctl(8) path */
  if(smartctlCmd)                           /* set by the -E cmd-line option */
  {
    mPrintf(errU,DEBUG,__func__,0,"smartctl(8) path: \"%s\" (from -E "
            "command-line option).",smartctlCmd);
  }
  else
  {
    /* try from the environment */
    smartctlCmd=getenv("SMARTCTL_CMD");
    if(smartctlCmd)
    {
      mPrintf(errU,DEBUG,__func__,0,"smartctl(8) path: \"%s\" (from "
              "SMARTCTL_CMD environment variable).",smartctlCmd);
    }
    else
    {
      /* set the default path */
      smartctlCmd=DFLT_SMART_CTL_PATH;
      mPrintf(errU,DEBUG,__func__,0,"smartctl(8) path: \"%s\" (default path).",
              smartctlCmd);
    }
  }
  setSmartctlCmd(smartctlCmd);
  /*-------------------------------------------------------------------------*/
  /* check accessibility of smartctl(8) */
  if(access(smartctlCmd,X_OK)==-1)                          /* access denied */
  {
    mPrintf(errU,FATAL,__func__,0,"Can't execute smartctl(8) command: %s: %s!",
            smartctlCmd,strerror(errno));
    exit(1);
  }
  else
  {
    mPrintf(errU,DEBUG,__func__,0,"smartctl(8) command \"%s\" found and "
            "executable.",smartctlCmd);
  }
  /* check if the current process is executed as user root */
  uid=getuid();
  if(uid!=0)
  {
    mPrintf(errU,FATAL,__func__,0,"The process \"%s\" must be executed by the "
            "user \"root\" because the physical disk information and the "
            "SMART information can only be accessed by the user \"root\"!",
            argv[0]);
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* get the list of disks */
  disk=getPhysDisks(deBug,errU);
  if(!disk.success)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by getPhysDisks()! "
            "Exit status: %d. Error string: \"%s\". Can't initialize service!",
            disk.exitStatus,disk.errorString);
    exit(1);
  }
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Number of disk: %d",disk.N);
  if(deBug>1)
  {
    for(i=0;i<disk.N;i++)
      mPrintf(errU,DEBUG,__func__,0,"disk %d: name=\"%s\", size=%"PRIu64" B",i,
              disk.item[i].name,disk.item[i].size);
  }
  success=1;
  /*-------------------------------------------------------------------------*/
  /* define server name */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
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
  /* define DIM svc 0: /<HOSTNAME>/disk/server_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"server_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rcsidP,strlen(rcsidP)+1,0,0);
  /* define DIM svc 1: /<HOSTNAME>/disk/sensor_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsidP,strlen(sRcsidP)+1,0,0);
  /* define DIM svc 2: /<HOSTNAME>/disk/fmc_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"fmc_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",FMC_VERSION,
                                  strlen(FMC_VERSION)+1,0,0);
  /* define DIM svc 3: /<HOSTNAME>/disk/N */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"N");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&disk.N,sizeof(int),0,0);
  /*-------------------------------------------------------------------------*/
  /* allocate heap space */
  devList=(char**)malloc(disk.N*sizeof(char*));                       /* all */
  sizeList=(uint64_t*)malloc(disk.N*sizeof(uint64_t));                /* all */
  typeList=(char**)malloc(disk.N*sizeof(char*));                      /* all */
  deviceModelList=(char**)malloc(disk.N*sizeof(char*));               /* all */
  serialNumberList=(char**)malloc(disk.N*sizeof(char*));              /* all */
  firmwareVersionList=(char**)malloc(disk.N*sizeof(char*));   /* (S)ATA only */
  ataVersionList=(int*)malloc(disk.N*sizeof(int));            /* (S)ATA only */
  ataStandardList=(char**)malloc(disk.N*sizeof(char*));       /* (S)ATA only */
  scsiTranspProtoList=(char**)malloc(disk.N*sizeof(char*));     /* SCSI only */
  ataAttrs=(smartAtaAttrs_t*)malloc(disk.N*sizeof(smartAtaAttrs_t));
  scsiAttrs=(smartScsiAttrs_t*)malloc(disk.N*sizeof(smartScsiAttrs_t));
  scsiErrs=(smartScsiErrs_t*)malloc(disk.N*sizeof(smartScsiErrs_t));
  /*-------------------------------------------------------------------------*/
  /* fill constants for static services */
  for(i=0;i<disk.N;i++)
  {
    devList[i]=strdup(1+strrchr(disk.item[i].name,'/'));
    sizeList[i]=disk.item[i].size;
  }
  /* fill the string devString containing all the disk dev names */
  for(i=0;i<disk.N;i++)devStringLen+=(1+strlen(devList[i]));
  devString=(char*)malloc(devStringLen*sizeof(char));
  for(i=0,p=devString;i<disk.N;i++)
  {
    strcpy(p,devList[i]);
    p+=(1+strlen(devList[i]));
  }
  /*-------------------------------------------------------------------------*/
  /* define static DIM services (contd) */
  /* define DIM svc 4: /<HOSTNAME>/disk/name */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"name");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",devString,devStringLen,0,0);
  for(i=0;i<disk.N;i++)
  {
    /*.......................................................................*/
    /* define DIM svc 5+i*7 (ATA) or 4+i*5 (SCSI): */
    /* /<HOSTNAME>/disk/<dev>/info/size */
    svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
    snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"info/size");
    svcId[svcNum-1]=dis_add_service(strdup(s),"X",&sizeList[i],
                                    sizeof(uint64_t),0,0);
    /*.......................................................................*/
    /* get the disk type (ATA, SATA or SCSI) */
    dt=getDiskType(disk.item[i].name,deBug,errU);
    if(dt.success)typeList[i]=strdup(sType[dt.type]);
    else
    {
      mPrintf(errU,ERROR,__func__,0,"Disk: %s. Error flag returned by "
              "getDiskType()! SMART access failed. smartctl exit status: %d. "
              "Error String1: \"%s\"; Error String2: \"%s\"!",
              devList[i],dt.exitStatus,dt.errorString1,dt.errorString2);
      typeList[i]="Unreadable";
    }
    if(deBug)
    {
      mPrintf(errU,DEBUG,__func__,0,"Disk: %s, type: %s size: %"PRIu64" B = "
              "%u GiB",disk.item[i].name,sType[dt.type],disk.item[i].size,
              (unsigned)(disk.item[i].size/1024/1024/1024));
    }
    /*.......................................................................*/
    /* define static DIM services (contd) */
    /* define DIM svc 6+i*7 (ATA) or 5+i*5 (SCSI): */
    /* /<HOSTNAME>/disk/<dev>/info/type */
    svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
    snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"info/type");
    svcId[svcNum-1]=dis_add_service(strdup(s),"C",typeList[i],
                                    1+strlen(typeList[i]),0,0);
    /*.......................................................................*/
    if(dt.type==ATA||dt.type==SATA)
    {
      /* get ATA/SATA disk info */
      ai=getAtaInfo(disk.item[i].name,deBug,errU);
      if(!ai.success)
      {
        mPrintf(errU,ERROR,__func__,0,"Disk: /dev/%s. Error flag returned by "
                "getAtaInfo()! SMART access failed. smartctl exit status: %d. "
                "Error String: \"%s\"!",devList[i],ai.exitStatus,
                ai.errorString);
      }
      else
      {
        deviceModelList[i]=strdup(ai.deviceModel);
        serialNumberList[i]=strdup(ai.serialNumber);
        firmwareVersionList[i]=strdup(ai.firmwareVers);
        ataVersionList[i]=ai.ataVersion;
        ataStandardList[i]=strdup(ai.ataStandard);
        if(deBug)
        {
          mPrintf(errU,DEBUG,__func__,0,"(S)ATA Disk: /dev/%s; device model: "
                  "\"%s\"; serial number: \"%s\"; firmware version: \"%s\"; "
                  "ATA version: %d; ATA standard: \"%s\".",devList[i],
                  deviceModelList[i],serialNumberList[i],
                  firmwareVersionList[i],ataVersionList[i],ataStandardList[i]);
        }
        /*...................................................................*/
        /* define static DIM services (contd) */
        /* define DIM svc 7+i*7: /<HOSTNAME>/disk/<dev>/info/deviceModel */
        svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
        snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
                 "info/deviceModel");
        svcId[svcNum-1]=dis_add_service(strdup(s),"C",deviceModelList[i],
                                        1+strlen(deviceModelList[i]),0,0);
        /* define DIM svc 8+i*7: /<HOSTNAME>/disk/<dev>/info/serialNumber */
        svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
        snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
                 "info/serialNumber");
        svcId[svcNum-1]=dis_add_service(strdup(s),"C",serialNumberList[i],
                                        1+strlen(serialNumberList[i]),0,0);
        /* define DIM svc 9+i*7: /<HOSTNAME>/disk/<dev>/info/firmwareVersion */
        svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
        snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
                 "info/firmwareVersion");
        svcId[svcNum-1]=dis_add_service(strdup(s),"C",firmwareVersionList[i],
                                        1+strlen(firmwareVersionList[i]),0,0);
        /* define DIM svc 10+i*7: /<HOSTNAME>/disk/<dev>/info/ataVersion */
        svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
        snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
                 "info/ataVersion");
        svcId[svcNum-1]=dis_add_service(strdup(s),"I",&ataVersionList[i],
                                        sizeof(int),0,0);
        /* define DIM svc 11+i*7: /<HOSTNAME>/disk/<dev>/info/ataStandard */
        svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
        snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
                 "info/ataStandard");
        svcId[svcNum-1]=dis_add_service(strdup(s),"C",ataStandardList[i],
                                        1+strlen(ataStandardList[i]),0,0);
        /*...................................................................*/
      }                                                    /* if(ai.success) */
    }                                     /* if(dt.type==ATA||dt.type==SATA) */
    /*.......................................................................*/
    else if(dt.type==SCSI)
    {
      /* get SCSI disk info */
      si=getScsiInfo(disk.item[i].name,deBug,errU);
      if(!si.success)
      {
        mPrintf(errU,ERROR,__func__,0,"Disk: /dev/%s. Error flag returned by "
                "getScsiInfo()! SMART access failed. smartctl exit status: "
                "%d. Error String: \"%s\"!",devList[i],si.exitStatus,
                si.errorString);
      }
      else
      {
        deviceModelList[i]=strdup(si.deviceModel);
        serialNumberList[i]=strdup(si.serialNumber);
        scsiTranspProtoList[i]=strdup(si.transpProto);
        if(deBug)
        {
          mPrintf(errU,ERROR,__func__,0,"SCSI Disk: /dev/%s; device model: "
                  "\"%s\"; serial number: \"%s\"; transport protocol: \"%s\".",
                  devList[i],deviceModelList[i],serialNumberList[i],
                  scsiTranspProtoList[i]);
        }
        /* define DIM svc 7+i*5: /<HOSTNAME>/disk/<dev>/info/deviceModel */
        svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
        snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
                 "info/deviceModel");
        svcId[svcNum-1]=dis_add_service(strdup(s),"C",deviceModelList[i],
                                        1+strlen(deviceModelList[i]),0,0);
        /* define DIM svc 8+i*5: /<HOSTNAME>/disk/<dev>/info/serialNumber */
        svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
        snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
                 "info/serialNumber");
        svcId[svcNum-1]=dis_add_service(strdup(s),"C",serialNumberList[i],
                                        1+strlen(serialNumberList[i]),0,0);
        /* define DIM svc 9+i*5: */
        /* /<HOSTNAME>/disk/<dev>/info/transportProtocol */
        svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
        snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
                 "info/transportProtocol");
        svcId[svcNum-1]=dis_add_service(strdup(s),"C",scsiTranspProtoList[i],
                                         1+strlen(scsiTranspProtoList[i]),0,0);
      }                                                    /* if(si.success) */
    }                                                   /* if(dt.type==SCSI) */
    /*.......................................................................*/
  }                                                 /* for(i=0;i<disk.N;i++) */
  sSvcNum=svcNum;
  /*-------------------------------------------------------------------------*/
  /* update SMART data */
  smartDataUpdate();
  /* define dynamic DIM services */
  /* define DIM svc 5+7*disk.N (ATA) or 4+5*disk.N (SCSI): */
  /* /<HOSTNAME>/disk/success */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"success");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&success,sizeof(int),0,0);
  successSvcId=svcId[svcNum-1];
  for(i=0;i<disk.N;i++)
  {
    if(!strcmp(typeList[i],"ATA")||!strcmp(typeList[i],"SATA"))
    {
      /* (S)ATA attrs */
      /* define DIM svc 6+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/N */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/N");
      svcId[svcNum-1]=dis_add_service(strdup(s),"I",&ataAttrs[i].N,
                                      sizeof(int),0,0);
      /* define DIM svc 7+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/id */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/id");
      svcId[svcNum-1]=dis_add_service(strdup(s),"I",0,0,ataAttrIdHandler,i);
      /* define DIM svc 8+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/name */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/name");
      svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,ataAttrNameHandler,i);
      /* define DIM svc 9+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/flag */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/flag");
      svcId[svcNum-1]=dis_add_service(strdup(s),"I",0,0,ataAttrFlagHandler,i);
      /* define DIM svc 10+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/value */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/value");
      svcId[svcNum-1]=dis_add_service(strdup(s),"I",0,0,ataAttrValueHandler,i);
      /* define DIM svc 11+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/worst */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/worst");
      svcId[svcNum-1]=dis_add_service(strdup(s),"I",0,0,ataAttrWorstHandler,i);
      /* define DIM svc 12+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/threshold */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/threshold");
      svcId[svcNum-1]=dis_add_service(strdup(s),"I",0,0,
                                      ataAttrThesholdHandler,i);
      /* define DIM svc 13+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/type */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/type");
      svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,ataAttrTypeHandler,i);
      /* define DIM svc 14+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/updated */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/updated");
      svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,
                                      ataAttrUpdatedHandler,i);
      /* define DIM svc 15+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/whenFailed */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
               "attr/whenFailed");
      svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,
                                      ataAttrWhenfailedHandler,i);
      /* define DIM svc 16+7*disk.N: /<HOSTNAME>/disk/<dev>/attr/rawValue */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/rawValue");
      svcId[svcNum-1]=dis_add_service(strdup(s),"X",0,0,
                                      ataAttrRawvalueHandler,i);
    }
    else if(!strcmp(typeList[i],"SCSI"))
    {
      /* SCSI attrs */
      /* define DIM svc 6+5*disk.N /<HOSTNAME>/disk/<dev>/attr/N */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/N");
      svcId[svcNum-1]=dis_add_service(strdup(s),"I",&scsiAttrs[i].N,
                                      sizeof(int),0,0);
      /* define DIM svc 7+5*disk.N /<HOSTNAME>/disk/<dev>/attr/name */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/name");
      svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,scsiAttrNameHandler,i);
      /* define DIM svc 8+5*disk.N /<HOSTNAME>/disk/<dev>/attr/value */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/value");
      svcId[svcNum-1]=dis_add_service(strdup(s),"X",0,0,scsiAttrValueHandler,i);
      /* define DIM svc 9+5*disk.N /<HOSTNAME>/disk/<dev>/attr/units */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"attr/units");
      svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,
                                      scsiAttrUnitsHandler,i);
      /* SCSI errs */
      /* define DIM svc 10+5*disk.N /<HOSTNAME>/disk/<dev>/err/N */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"err/N");
      svcId[svcNum-1]=dis_add_service(strdup(s),"I",&scsiErrs[i].N,
                                      sizeof(int),0,0);
      /* define DIM svc 11+5*disk.N /<HOSTNAME>/disk/<dev>/err/name */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"err/name");
      svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,scsiErrNameHandler,i);
      /* define DIM svc 12+5*disk.N /<HOSTNAME>/disk/<dev>/err/value/read */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],"err/value/read");
      svcId[svcNum-1]=dis_add_service(strdup(s),"X",0,0,scsiErrRvalueHandler,i);
      /* define DIM svc 13+5*disk.N /<HOSTNAME>/disk/<dev>/err/value/write */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
               "err/value/write");
      svcId[svcNum-1]=dis_add_service(strdup(s),"X",0,0,scsiErrWvalueHandler,i);
      /* define DIM svc 14+5*disk.N /<HOSTNAME>/disk/<dev>/err/value/verify */
      svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
      snprintf(s,DIM_SVC_LEN+1,"%s/%s/%s",srvPath,devList[i],
               "err/value/verify");
      svcId[svcNum-1]=dis_add_service(strdup(s),"X",0,0,scsiErrVvalueHandler,i);
    }                                     /* if(!strcmp(typeList[i],"SCSI")) */
  }                                                 /* for(i=0;i<disk.N;i++) */
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  dis_add_error_handler(dimErrorHandler);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  dis_start_serving(srvPath);
  /*-------------------------------------------------------------------------*/
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC SMART Monitor DIM Server started "
          "(TGID=%u). Using: \"%s\", \"%s\", \"FMC-%s\".",getpid(),rcsidP,
          sRcsidP,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* main loop */
  nextTime=time(NULL);
  for(i=0;;i++)
  {
    nextTime+=refreshPeriod;
    /*.......................................................................*/
    /* update SMART data */
    success=smartDataUpdate();
    if(success)
    {
      /* update dynamic DIM services */
      for(j=sSvcNum;j<svcNum;j++)dis_update_service(svcId[j]);
    }
    else
    {
      mPrintf(errU,ERROR,__func__,0,"Error flag returned by "
              "smartDataUpdate()!");
      dis_update_service(successSvcId);
    }
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
int smartDataUpdate(void)
{
  int i=0,j=0;
  int success=1;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  for(i=0;i<disk.N;i++)
  {
    if(!strcmp(typeList[i],"ATA")||!strcmp(typeList[i],"SATA"))
    {
      /* get (S)ATA SMART attributes */
      ataAttrs[i]=readAtaSMARTattrs(disk.item[i].name,deBug,errU);
      if(ataAttrs[i].success)
      {
        if(deBug)
        {
          mPrintf(errU,DEBUG,__func__,0,"Disk: %s. Number of attributes: %d",
                  disk.item[i].name,ataAttrs[i].N);
          for(j=0;j<ataAttrs[i].N;j++)
          {
            mPrintf(errU,DEBUG,__func__,0,
                    "%3d %s %#4.4x %d %d %d %s %s %s %"PRIu64,
                    ataAttrs[i].item[j].id,
                    ataAttrs[i].item[j].name,
                    ataAttrs[i].item[j].flag,
                    ataAttrs[i].item[j].value,
                    ataAttrs[i].item[j].worst,
                    ataAttrs[i].item[j].threshold,
                    ataAttrs[i].item[j].type,
                    ataAttrs[i].item[j].updated,
                    ataAttrs[i].item[j].whenFailed,
                    ataAttrs[i].item[j].rawValue);
          }
        }
      }
      else
      {
        mPrintf(errU,ERROR,__func__,0,"Disk: /dev/%s. Error flag returned by "
                "readAtaSMARTattrs()! SMART access failed. smartctl exit "
                "status: %d. Error String: \"%s\"!",devList[i],
                ataAttrs[i].exitStatus,ataAttrs[i].errorString);
        success&=0;
      }
    }
    else if(!strcmp(typeList[i],"SCSI"))
    {
      /* get SCSI SMART attributes */
      scsiAttrs[i]=readScsiSMARTattrs(disk.item[i].name,deBug,errU);
      if(scsiAttrs[i].success)
      {
        if(deBug)
        {
          mPrintf(errU,DEBUG,__func__,0,"Disk: %s. Number of attributes: %d\n",
                  disk.item[i].name,scsiAttrs[i].N);
          for(j=0;j<scsiAttrs[i].N;j++)
          {
            mPrintf(errU,DEBUG,__func__,0,
                    "%s %"PRIu64" %s",
                    scsiAttrs[i].item[j].name,
                    scsiAttrs[i].item[j].value,
                    scsiAttrs[i].item[j].units);
          }
        }
      }
      else
      {
        mPrintf(errU,ERROR,__func__,0,"Disk: /dev/%s. Error flag returned by "
                "readScsiSMARTattrs()! SMART access failed. smartctl exit "
                "status: %d. Error String: \"%s\"!",devList[i],
                scsiAttrs[i].exitStatus,scsiAttrs[i].errorString);
        success&=0;
      }
      /* get SCSI SMART errors */
      scsiErrs[i]=readScsiSMARTerrs(disk.item[i].name,deBug,errU);
      scsiErrs[i].N=8;
      if(scsiErrs[i].success)
      {
        if(deBug)
        {
          mPrintf(errU,DEBUG,__func__,0,"Disk: %s. SCSI errors: ",
                  disk.item[i].name);
          mPrintf(errU,DEBUG,__func__,0,"EEC fast: r: %"PRIu64"; w: %"PRIu64
                  "; v: %" PRIu64".",
                  scsiErrs[i].eecFastR,scsiErrs[i].eecFastW,
                  scsiErrs[i].eecFastV);
          mPrintf(errU,DEBUG,__func__,0,"EEC delayed: r: %"PRIu64"; w: %"PRIu64
                  "; v: %" PRIu64".",
                  scsiErrs[i].eecDelR,scsiErrs[i].eecDelW,
                  scsiErrs[i].eecDelV);
          mPrintf(errU,DEBUG,__func__,0,"Repeated: r: %"PRIu64"; w: %"PRIu64
                  "; v: %" PRIu64".",
                  scsiErrs[i].reR,scsiErrs[i].reW,
                  scsiErrs[i].reV);
          mPrintf(errU,DEBUG,__func__,0,"Corrected errors: r: %"PRIu64"; w: %"
                  PRIu64"; v: %" PRIu64".",
                  scsiErrs[i].corrR,scsiErrs[i].corrW,
                  scsiErrs[i].corrV);
          mPrintf(errU,DEBUG,__func__,0,"Corr algo invoc: r: %"PRIu64"; w: %"
                  PRIu64"; v: %" PRIu64".",
                  scsiErrs[i].algInvR,scsiErrs[i].algInvW,
                  scsiErrs[i].algInvV);
          mPrintf(errU,DEBUG,__func__,0,"Processed GB: r: %.3f; w: %.3f; v: "
                  "%.3f.",
                  scsiErrs[i].procGBr,scsiErrs[i].procGBw,
                  scsiErrs[i].procGBv);
          mPrintf(errU,DEBUG,__func__,0,"Uncorrected errors: r: %"PRIu64"; w: %"
                  PRIu64"; v: %" PRIu64".",
                  scsiErrs[i].unCorrR,scsiErrs[i].unCorrW,
                  scsiErrs[i].unCorrV);
          mPrintf(errU,DEBUG,__func__,0,"Non-medium errors: %"PRIu64".",
                  scsiErrs[i].nonMedium);
        }
      }
      else
      {
        mPrintf(errU,ERROR,__func__,0,"Disk: /dev/%s. Error flag returned by "
                "readScsiSMARTerrs()! SMART access failed. smartctl exit "
                "status: %d. Error String: \"%s\"!",devList[i],
                scsiErrs[i].exitStatus,scsiErrs[i].errorString);
        success&=0;
      }
    }
  }
  return success;
}
/*****************************************************************************/
void ataAttrIdHandler(long *tag,int **address,int *size)
{
  int i=0;
  static int *attrIdList=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  attrIdList=(int*)realloc(attrIdList,ataAttrs[*tag].N*sizeof(int));
  memset(attrIdList,0,ataAttrs[*tag].N*sizeof(int));
  if(!strcmp(typeList[*tag],"ATA")||!strcmp(typeList[*tag],"SATA"))
  {
    for(i=0;i<ataAttrs[*tag].N;i++)attrIdList[i]=ataAttrs[*tag].item[i].id;
  }
  *address=(int*)attrIdList;
  *size=ataAttrs[*tag].N*sizeof(int);
}
/*****************************************************************************/
void ataAttrNameHandler(long *tag,int **address,int *size)
{
  static char *attrNameList=NULL;
  int attrNameListLen=0;
  int i=0;
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  if(!strcmp(typeList[*tag],"ATA")||!strcmp(typeList[*tag],"SATA"))
  {
    for(i=0;i<ataAttrs[*tag].N;i++)
    {
      attrNameListLen+=(1+strlen(ataAttrs[*tag].item[i].name));
    }
    attrNameList=(char*)realloc(attrNameList,attrNameListLen*sizeof(char));
    for(i=0,p=attrNameList;i<ataAttrs[*tag].N;i++)
    {
      strcpy(p,ataAttrs[*tag].item[i].name);
      p+=(1+strlen(ataAttrs[*tag].item[i].name));
    }
  }
  *address=(int*)attrNameList;
  *size=attrNameListLen;
}
/*****************************************************************************/
void ataAttrFlagHandler(long *tag,int **address,int *size)
{
  int i=0;
  static int *attrFlagList=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  attrFlagList=(int*)realloc(attrFlagList,ataAttrs[*tag].N*sizeof(int));
  memset(attrFlagList,0,ataAttrs[*tag].N*sizeof(int));
  if(!strcmp(typeList[*tag],"ATA")||!strcmp(typeList[*tag],"SATA"))
  {
    for(i=0;i<ataAttrs[*tag].N;i++)
      attrFlagList[i]=ataAttrs[*tag].item[i].flag;
  }
  *address=(int*)attrFlagList;
  *size=ataAttrs[*tag].N*sizeof(int);
}
/*****************************************************************************/
void ataAttrValueHandler(long *tag,int **address,int *size)
{
  int i=0;
  static int *attrValueList=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  attrValueList=(int*)realloc(attrValueList,ataAttrs[*tag].N*sizeof(int));
  memset(attrValueList,0,ataAttrs[*tag].N*sizeof(int));
  if(!strcmp(typeList[*tag],"ATA")||!strcmp(typeList[*tag],"SATA"))
  {
    for(i=0;i<ataAttrs[*tag].N;i++)
      attrValueList[i]=ataAttrs[*tag].item[i].value;
  }
  *address=(int*)attrValueList;
  *size=ataAttrs[*tag].N*sizeof(int);
}
/*****************************************************************************/
void ataAttrWorstHandler(long *tag,int **address,int *size)
{
  int i=0;
  static int *attrWorstList=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  attrWorstList=(int*)realloc(attrWorstList,ataAttrs[*tag].N*sizeof(int));
  memset(attrWorstList,0,ataAttrs[*tag].N*sizeof(int));
  if(!strcmp(typeList[*tag],"ATA")||!strcmp(typeList[*tag],"SATA"))
  {
    for(i=0;i<ataAttrs[*tag].N;i++)
      attrWorstList[i]=ataAttrs[*tag].item[i].worst;
  }
  *address=(int*)attrWorstList;
  *size=ataAttrs[*tag].N*sizeof(int);
}
/*****************************************************************************/
void ataAttrThesholdHandler(long *tag,int **address,int *size)
{
  int i=0;
  static int *attrThresholdList=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  attrThresholdList=(int*)realloc(attrThresholdList,ataAttrs[*tag].N
                                  *sizeof(int));
  memset(attrThresholdList,0,ataAttrs[*tag].N*sizeof(int));
  if(!strcmp(typeList[*tag],"ATA")||!strcmp(typeList[*tag],"SATA"))
  {
    for(i=0;i<ataAttrs[*tag].N;i++)
      attrThresholdList[i]=ataAttrs[*tag].item[i].threshold;
  }
  *address=(int*)attrThresholdList;
  *size=ataAttrs[*tag].N*sizeof(int);
}
/*****************************************************************************/
void ataAttrTypeHandler(long *tag,int **address,int *size)
{
  static char *attrTypeList=NULL;
  int attrTypeListLen=0;
  int i=0;
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  if(!strcmp(typeList[*tag],"ATA")||!strcmp(typeList[*tag],"SATA"))
  {
    for(i=0;i<ataAttrs[*tag].N;i++)
    {
      attrTypeListLen+=(1+strlen(ataAttrs[*tag].item[i].type));
    }
    attrTypeList=(char*)realloc(attrTypeList,attrTypeListLen*sizeof(char));
    for(i=0,p=attrTypeList;i<ataAttrs[*tag].N;i++)
    {
      strcpy(p,ataAttrs[*tag].item[i].type);
      p+=(1+strlen(ataAttrs[*tag].item[i].type));
    }
  }
  *address=(int*)attrTypeList;
  *size=attrTypeListLen;
}
/*****************************************************************************/
void ataAttrUpdatedHandler(long *tag,int **address,int *size)
{
  static char *attrUpdatedList=NULL;
  int attrUpdatedListLen=0;
  int i=0;
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  if(!strcmp(typeList[*tag],"ATA")||!strcmp(typeList[*tag],"SATA"))
  {
    for(i=0;i<ataAttrs[*tag].N;i++)
    {
      attrUpdatedListLen+=(1+strlen(ataAttrs[*tag].item[i].updated));
    }
    attrUpdatedList=(char*)realloc(attrUpdatedList,attrUpdatedListLen
                                   *sizeof(char));
    for(i=0,p=attrUpdatedList;i<ataAttrs[*tag].N;i++)
    {
      strcpy(p,ataAttrs[*tag].item[i].updated);
      p+=(1+strlen(ataAttrs[*tag].item[i].updated));
    }
  }
  *address=(int*)attrUpdatedList;
  *size=attrUpdatedListLen;
}
/*****************************************************************************/
void ataAttrWhenfailedHandler(long *tag,int **address,int *size)
{
  static char *attrWhenFailedList=NULL;
  int attrWhenFailedListLen=0;
  int i=0;
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  if(!strcmp(typeList[*tag],"ATA")||!strcmp(typeList[*tag],"SATA"))
  {
    for(i=0;i<ataAttrs[*tag].N;i++)
    {
      attrWhenFailedListLen+=(1+strlen(ataAttrs[*tag].item[i].whenFailed));
    }
    attrWhenFailedList=(char*)realloc(attrWhenFailedList,attrWhenFailedListLen
                                      *sizeof(char));
    for(i=0,p=attrWhenFailedList;i<ataAttrs[*tag].N;i++)
    {
      strcpy(p,ataAttrs[*tag].item[i].whenFailed);
      p+=(1+strlen(ataAttrs[*tag].item[i].whenFailed));
    }
  }
  *address=(int*)attrWhenFailedList;
  *size=attrWhenFailedListLen;
}
/*****************************************************************************/
void ataAttrRawvalueHandler(long *tag,int **address,int *size)
{
  int i=0;
  uint64_t *attrValueList=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  attrValueList=(uint64_t*)realloc(attrValueList,ataAttrs[*tag].N*
                                   sizeof(uint64_t));
  memset(attrValueList,0,ataAttrs[*tag].N*sizeof(uint64_t));
  if(!strcmp(typeList[*tag],"ATA")||!strcmp(typeList[*tag],"SATA"))
  {
    for(i=0;i<ataAttrs[*tag].N;i++)
      attrValueList[i]=ataAttrs[*tag].item[i].rawValue;
  }
  *address=(int*)attrValueList;
  *size=ataAttrs[*tag].N*sizeof(uint64_t);
}
/*****************************************************************************/
void scsiAttrNameHandler(long *tag,int **address,int *size)
{
  static char *attrNameList=NULL;
  int attrNameListLen=0;
  int i=0;
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  if(!strcmp(typeList[*tag],"SCSI"))
  {
    for(i=0;i<scsiAttrs[*tag].N;i++)
    {
      attrNameListLen+=(1+strlen(scsiAttrs[*tag].item[i].name));
    }
    attrNameList=(char*)realloc(attrNameList,attrNameListLen*sizeof(char));
    for(i=0,p=attrNameList;i<scsiAttrs[*tag].N;i++)
    {
      strcpy(p,scsiAttrs[*tag].item[i].name);
      p+=(1+strlen(scsiAttrs[*tag].item[i].name));
    }
  }
  *address=(int*)attrNameList;
  *size=attrNameListLen;
}
/*****************************************************************************/
void scsiAttrValueHandler(long *tag,int **address,int *size)
{
  int i=0;
  uint64_t *attrValueList=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  attrValueList=(uint64_t*)realloc(attrValueList,scsiAttrs[*tag].N*
                                   sizeof(uint64_t));
  memset(attrValueList,0,scsiAttrs[*tag].N*sizeof(uint64_t));
  if(!strcmp(typeList[*tag],"SCSI"))
  {
    for(i=0;i<scsiAttrs[*tag].N;i++)
      attrValueList[i]=scsiAttrs[*tag].item[i].value;
  }
  *address=(int*)attrValueList;
  *size=scsiAttrs[*tag].N*sizeof(uint64_t);
}
/*****************************************************************************/
void scsiAttrUnitsHandler(long *tag,int **address,int *size)
{
  static char *attrUnitsList=NULL;
  int attrUnitsListLen=0;
  int i=0;
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  if(!strcmp(typeList[*tag],"SCSI"))
  {
    for(i=0;i<scsiAttrs[*tag].N;i++)
    {
      attrUnitsListLen+=(1+strlen(scsiAttrs[*tag].item[i].units));
    }
    attrUnitsList=(char*)realloc(attrUnitsList,attrUnitsListLen*sizeof(char));
    for(i=0,p=attrUnitsList;i<scsiAttrs[*tag].N;i++)
    {
      strcpy(p,scsiAttrs[*tag].item[i].units);
      p+=(1+strlen(scsiAttrs[*tag].item[i].units));
    }
  }
  *address=(int*)attrUnitsList;
  *size=attrUnitsListLen;
}
/*****************************************************************************/
void scsiErrNameHandler(long *tag,int **address,int *size)
{
  static char *errNameList=NULL;
  int errNameListLen=0;
  int i=0;
  char *p=NULL;
  char *errName[8]={"Errors Corrected by EEC fast",
                    "Errors Corrected by EEC delayed",
                    "Repeated access",
                    "Total corrected errors",
                    "Correction algorithm invocations",
                    "Processed Megabytes",
                    "Total uncorrected errors",
                    "Non-medium error"};
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  if(!strcmp(typeList[*tag],"SCSI"))
  {
    for(i=0;i<scsiErrs[*tag].N;i++)
    {
      errNameListLen+=(1+strlen(errName[i]));
    }
    errNameList=(char*)realloc(errNameList,errNameListLen*sizeof(char));
    for(i=0,p=errNameList;i<scsiErrs[*tag].N;i++)
    {
      strcpy(p,errName[i]);
      p+=(1+strlen(errName[i]));
    }
  }
  *address=(int*)errNameList;
  *size=errNameListLen;
}
/*****************************************************************************/
void scsiErrRvalueHandler(long *tag,int **address,int *size)
{
  uint64_t *errValueList=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  errValueList=(uint64_t*)realloc(errValueList,scsiErrs[*tag].N*
                                   sizeof(uint64_t));
  memset(errValueList,0,scsiErrs[*tag].N*sizeof(uint64_t));
  if(!strcmp(typeList[*tag],"SCSI"))
  {
    errValueList[0]=scsiErrs[*tag].eecFastR;
    errValueList[1]=scsiErrs[*tag].eecDelR;
    errValueList[2]=scsiErrs[*tag].reR;
    errValueList[3]=scsiErrs[*tag].corrR;
    errValueList[4]=scsiErrs[*tag].algInvR;
    errValueList[5]=(uint64_t)(scsiErrs[*tag].procGBr*1024);
    errValueList[6]=scsiErrs[*tag].unCorrR;
    errValueList[7]=scsiErrs[*tag].nonMedium;
  }
  *address=(int*)errValueList;
  *size=scsiErrs[*tag].N*sizeof(uint64_t);
}
/*****************************************************************************/
void scsiErrWvalueHandler(long *tag,int **address,int *size)
{
  uint64_t *errValueList=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  errValueList=(uint64_t*)realloc(errValueList,scsiErrs[*tag].N*
                                   sizeof(uint64_t));
  memset(errValueList,0,scsiErrs[*tag].N*sizeof(uint64_t));
  if(!strcmp(typeList[*tag],"SCSI"))
  {
    errValueList[0]=scsiErrs[*tag].eecFastW;
    errValueList[1]=scsiErrs[*tag].eecDelW;
    errValueList[2]=scsiErrs[*tag].reW;
    errValueList[3]=scsiErrs[*tag].corrW;
    errValueList[4]=scsiErrs[*tag].algInvW;
    errValueList[5]=(uint64_t)(scsiErrs[*tag].procGBw*1024);
    errValueList[6]=scsiErrs[*tag].unCorrW;
    errValueList[7]=scsiErrs[*tag].nonMedium;
  }
  *address=(int*)errValueList;
  *size=scsiErrs[*tag].N*sizeof(uint64_t);
}
/*****************************************************************************/
void scsiErrVvalueHandler(long *tag,int **address,int *size)
{
  uint64_t *errValueList=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  errValueList=(uint64_t*)realloc(errValueList,scsiErrs[*tag].N*
                                   sizeof(uint64_t));
  memset(errValueList,0,scsiErrs[*tag].N*sizeof(uint64_t));
  if(!strcmp(typeList[*tag],"SCSI"))
  {
    errValueList[0]=scsiErrs[*tag].eecFastV;
    errValueList[1]=scsiErrs[*tag].eecDelV;
    errValueList[2]=scsiErrs[*tag].reV;
    errValueList[3]=scsiErrs[*tag].corrV;
    errValueList[4]=scsiErrs[*tag].algInvV;
    errValueList[5]=(uint64_t)(scsiErrs[*tag].procGBv*1024);
    errValueList[6]=scsiErrs[*tag].unCorrV;
    errValueList[7]=scsiErrs[*tag].nonMedium;
  }
  *address=(int*)errValueList;
  *size=scsiErrs[*tag].N*sizeof(uint64_t);
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
    return;
  }
  for(i=0;i<svcNum;i++)if(svcId[i])dis_remove_service(svcId[i]);
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
"       smartSrv - FMC SMART Monitor Server\n"
"\n"
"SYNOPSIS\n"
"       smartSrv [-d] [-c DIM_CONF_FILE] [-l LOGGER]\n"
"                [-u REFRESH_TIME_INTERVAL]\n"
"       smartSrv -h\n"
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
"       smartctl(8), /proc/partitions\n"
,rcsidP,sRcsidP,FMC_VERSION
       );
  exit(1);
}
/*****************************************************************************/
