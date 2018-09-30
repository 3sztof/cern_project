/*****************************************************************************/
/*
 * $Log: nifSrv.c,v $
 * Revision 2.34  2011/06/13 13:42:15  galli
 * Now publishes total counters (64bit) for the nifSrv (additional to rates) - David.G.Svantesson@cern.ch
 *
 * Revision 2.33  2009/10/03 20:28:25  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 2.29  2009/01/08 13:16:10  galli
 * uses sig2msg() instead of signal_number_to_name()
 *
 * Revision 2.22  2008/02/24 23:34:40  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 2.21  2007/12/21 23:35:45  galli
 * added SVC: /<HOSTNAME>/nif/active
 *
 * Revision 2.18  2007/10/27 06:24:17  galli
 * usage() prints FMC version
 * publishes fmc_version
 *
 * Revision 2.17  2007/09/20 12:13:10  galli
 * can get DIM_DNS_NODE from a configuration file
 * -c dim_conf_file command-line option added
 *
 * Revision 2.15  2007/08/11 09:10:55  galli
 * compatible with libFMCutils v 2
 * added function usage()
 *
 * Revision 2.14  2006/10/26 08:34:46  galli
 * const intervals between refreshes
 *
 * Revision 2.13  2006/10/24 22:57:47  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 2.12  2006/02/09 11:52:29  galli
 * x86_64 compatibility problems fixed
 *
 * Revision 2.11  2005/11/10 13:17:14  galli
 * cleanup
 *
 * Revision 2.9  2005/11/08 13:24:32  galli
 * added service /HOSTNAME/nif/ifN
 * added service /HOSTNAME/nif/ethDeviceN
 *
 * Revision 2.8  2005/11/08 12:26:55  galli
 * compatibility with PVSS part version 3
 * removed service /HOSTNAME/nif/ethX_addr
 * added service /HOSTNAME/nif/name
 * added service /HOSTNAME/nif/mac
 * added service /HOSTNAME/nif/bus
 * added service /HOSTNAME/nif/ip
 *
 * Revision 2.7  2005/09/20 08:59:38  galli
 * service name /HOSTNAME/nif/ethX changed in /HOSTNAME/nif/ethX_addr
 *
 * Revision 2.6  2005/09/15 14:35:50  galli
 * sinchronous handling of signals
 * immediate reset
 * define DIM error handler
 * publish also the (PCI) bus addresses of the interfaces
 *
 * Revision 2.4  2004/11/30 17:30:14  gregori
 * Add -u option to set refreshPeriod. Add eth_rate service.
 *
 * Revision 2.2  2004/11/18 09:40:48  gregori
 * reset command handler added
 * SIGHUP handler added
 *
 * Revision 2.1  2004/11/16 09:29:45  gregori
 * add reset command and reset signal
 *
 * Revision 2.0  2004/11/11 13:45:48  gregori
 * log msg, version msg, argument control
 *
 * Revision 1.1  2004/10/15 12:59:40  gregori
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>                                  /* EOF, snprintf(), etc. */
#include <stdlib.h>                           /* strtol(), realloc(), exit() */
#include <unistd.h>                           /* getopt(), getpid(), _exit() */
#include <signal.h>               /* pthread_sigmask(), sigtimedwait(), etc. */
#include <string.h>       /* strlen(), strncpy(), strdup(), strerror(), etc. */
#include <errno.h>                                                  /* errno */
#include <pwd.h>                                              /* getpwuid(3) */
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
#include "fmcTssUtils.h"                                          /* TSS_LEN */
/*---------------------------------------------------------------------------*/
/* in sensors/include */
#include "getNIFfromProc.h"                      /* struct nifCnt, readNIF() */
#include "getNIFinfoFromEthtool.h"         /* struct nifAttr_t, getNIFinfo() */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#include "nifSrv.h"               /* SRV_NAME, DIM_SRV_LEN, DIM_SVC_LEN, ... */
/*****************************************************************************/
/* globals */
int deBug=0;
int errU=L_DIM;                                                /* error unit */
int svcNum=0;                           /* number of services (for clean-up) */
int sSvcNum=0;
int cmdNum=0;                           /* number of commands (for clean-up) */
int *svcId=NULL;
int successSvcId=0;
int tssSvcId=0;
int *cmdId=NULL;
pid_t tgid;
static char rcsid[]="$Id: nifSrv.c,v 2.34 2011/06/13 13:42:15 galli Exp $"; 
char *sRcsid0,*sRcsid1;
char *rcsidP,*sRcsid0P,*sRcsid1P;
/* sensor data (must be accessible by the DIM Handler functions) */
nifsData_t nifs;
nifAttr_t *nifInf;
/*****************************************************************************/
void sumDataHandler(long *tag,int **address,int *size);
void detDataHandler(long *tag,int **address,int *size);
void detAdataHandler(long *tag,int **address,int *size);
void detMdataHandler(long *tag,int **address,int *size);
void detTdataHandler(long *tag,int **address,int *size);
static void signalHandler(int signo);
void dimErrorHandler(int severity,int errorCode,char *message);
static void resetCmdHandler(long *tag,int *cmnd,int *size);
static void logData(void);
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
  char s[DIM_SVC_LEN+1]="";
  /* data to be published */
  int sumFieldN=0;                          /* number of summary data fields */
  int detFieldN=0;                         /* number of detailed data fields */
  int dettotFieldN=0;                /* number of detailed total data fields */
  int ethFieldN=0;                   /* number of total Ethernet data fields */
  int deviceN=0;                                     /* number of network if */
  char device[DEVICE_LEN+1];
  char *deviceBuf=NULL;                            /* devices                */
  int deviceBufS=0;                                /* deviceBuf size         */
  int deviceBufBS=0;                               /* deviceBuf busy size    */
  char *sumLabelBuf=NULL;                           /* summary data labels   */
  int sumLabelBufS=0;                               /* sumLabelBuf size      */
  int sumLabelBufBS=0;                              /* sumLabelBuf busy size */
  char *sumUnitsBuf=NULL;                           /* summary data units    */
  int sumUnitsBufS=0;                               /* sumUnitsBuf size      */
  int sumUnitsBufBS=0;                              /* sumUnitsBuf busy size */
  char *detLabelBuf=NULL;                           /* detailed data labels  */
  int detLabelBufS=0;                               /* detLabelBuf size      */
  int detLabelBufBS=0;                              /* detLabelBuf busy size */
  char *detUnitsBuf=NULL;                           /* detailed data units   */
  int detUnitsBufS=0;                               /* detUnitsBuf size      */
  int detUnitsBufBS=0;                              /* detUnitsBuf busy size */
  char * dettotLabelBuf=NULL;                       /* detailed total data labels */
  int dettotLabelBufS=0;                            /* dettotLabelBuf size        */
  int dettotLabelBufBS=0;                           /* dettotLabelBuf busy size   */
  char *dettotUnitsBuf=NULL;                        /* detailed totals units      */
  int dettotUnitsBufS=0;                            /* dettotUnitsBuf size        */
  int dettotUnitsBufBS=0;                           /* dettotUnitsBuf busy size   */
  char *ethLabelBuf=NULL;                      /* total Ethernet data labels */
  int ethLabelBufS=0;                          /* ethLabelBuf size           */
  int ethLabelBufBS=0;                         /* ethLabelBuf busy size      */
  char *ethUnitsBuf=NULL;                      /* total Ethernet data units  */
  int ethUnitsBufS=0;                          /* ethUnitsBuf size           */
  int ethUnitsBufBS=0;                         /* ethUnitsBuf busy size      */
  float *ethDataBuf=NULL;                      /* total Ethernet data        */
  int ethDataBufS=0;                           /* ethDataBuf size            */
  /* dim configuration */
  char *dimDnsNode=NULL;
  /* ethtool */
  uid_t uid=-1;
  char *ethtoolCmd=NULL;
  int ethtoolEnabled=1;
  /*-------------------------------------------------------------------------*/
  sRcsid0=getNIFsensorVersion();
  sRcsid1=getNIFinfoSensorVersion();
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid, sRcsid0, sRcsid1 */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  sRcsid0P=strchr(sRcsid0,':')+2;
  for(i=0,p=sRcsid0P;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  sRcsid1P=strchr(sRcsid1,':')+2;
  for(i=0,p=sRcsid1P;i<5;i++)p=1+strchr(p,' ');
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
        ethtoolCmd=optarg;
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
  /* get ethtool(8) path */
  if(ethtoolCmd)                            /* set by the -E cmd-line option */
  {
    mPrintf(errU,DEBUG,__func__,0,"ethtool(8) path: \"%s\" (from -E "
            "command-line option).",ethtoolCmd);
  }
  else
  {
    /* try from the environment */
    ethtoolCmd=getenv("ETHTOOL_CMD");
    if(ethtoolCmd)
    {
      mPrintf(errU,DEBUG,__func__,0,"ethtool(8) path: \"%s\" (from "
              "ETHTOOL_CMD environment variable).",ethtoolCmd);
    }
    else
    {
      /* set the default path */
      ethtoolCmd=DFLT_ETHTOOL_PATH;
      mPrintf(errU,DEBUG,__func__,0,"ethtool(8) path: \"%s\" (default path).",
              ethtoolCmd);
    }
  }
  /*-------------------------------------------------------------------------*/
  /* check accessibility of ethtool */
  if(access(ethtoolCmd,X_OK)==-1)                           /* access denied */
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot execute the Ethtool command: "
            "\"%s\": %s! Link detection, speed, duplex setting etc. will "
            "be unaccessibles in all the network interfaces! The current "
            "process will continue without publishing these data!",ethtoolCmd,
            strerror(errno));
    ethtoolEnabled=0;
  }
  /* check if the current process is executed as user root */
  uid=getuid();
  if(uid!=0)
  {
    mPrintf(errU,ERROR,__func__,0,"The current process \"%s\" is executed by "
            "the user \"%s\". In order to retrieve the BUS Address and the "
            "Ethtool information (link detection, speed, duplex settings, "
            "etc.), the current process must be executed by the user "
            "\"root\" instead! The current process will continue without "
            "publishing these data!",argv[0],getpwuid(uid)->pw_name);
    ethtoolEnabled=0;
  }
  /*-------------------------------------------------------------------------*/
  /* In kernel <= 2.4.19 other threads returns different getpid(). So we     */
  /* save the pid of main thread in a global variable, to be used in         */
  /* resetCmdHandler(). In kernels >= 2.4.20 it makes no differences.       */
  tgid=getpid();
  /*-------------------------------------------------------------------------*/
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /*-------------------------------------------------------------------------*/
  /* Read the number of the network interfaces                               */
  for(i=0;i<5;i++)
  {
    nifs=readNIF(deBug,errU);                                 /* call sensor */
    if(nifs.success)break;
    dtq_sleep(1);
  }
  if(i>=5)
  {
    mPrintf(errU,FATAL,__func__,0,"Error flag returned by readNIF()! Can't "
            "initialize service!");
    exit(1);
  }
  deviceN=nifs.nifN;
  /*-------------------------------------------------------------------------*/
  /* allocate memory for nifInf */
  nifInf=(nifAttr_t*)malloc(deviceN*sizeof(nifAttr_t));
  /*-------------------------------------------------------------------------*/
  /* Test the ethtool interface */
  for(j=0;j<deviceN;j++)
  {
    if(strstr(nifs.data[j].name,"eth"))                /* Ethernet interface */
    {
      if(ethtoolEnabled)
      {
        for(i=0;i<5;i++)
        {
          /* call sensor */
          nifInf[j]=getNIFinfo(ethtoolCmd,nifs.data[j].name,deBug,errU);
          if(nifInf[j].success)break;
          dtq_sleep(1);
        }
        if(i>=5)
        {
          mPrintf(errU,ERROR,__func__,0,"Error flag returned by getNIFinfo() "
                  "on interface %s: %s",nifs.data[j].name,
                  nifInf[j].errorString);
        }
      }
      else                                              /* (!ethtoolEnabled) */
      {
        nifInf[j].success=1;
        nifInf[j].exitStatus=0;
        strcpy(nifInf[j].errorString,"");
        strcpy(nifInf[j].speed,"N/A");
        strcpy(nifInf[j].duplex,"N/A");
        strcpy(nifInf[j].portType,"N/A");
        strcpy(nifInf[j].autoNegotiation,"N/A");
        strcpy(nifInf[j].linkDetected,"N/A");
      }                                                 /* (!ethtoolEnabled) */
    }                                                  /* Ethernet interface */
    else                                                     /* not Ethernet */
    {
      nifInf[j].success=1;
      nifInf[j].exitStatus=0;
      strcpy(nifInf[j].errorString,"");
      strcpy(nifInf[j].speed,"N/D");
      strcpy(nifInf[j].duplex,"N/D");
      strcpy(nifInf[j].portType,"N/D");
      strcpy(nifInf[j].autoNegotiation,"N/D");
      strcpy(nifInf[j].linkDetected,"N/D");
    }                                                        /* not Ethernet */
  }
  /*-------------------------------------------------------------------------*/
  /* printout */
  for(j=0;j<deviceN;j++)
  {
    mPrintf(errU,INFO,__func__,0,"Interface: net_%02d, IF name: \"%s\", BUS "
            "address: \"%s\", MAC address: \"%s\", IP address: \"%s\", "
            "Speed: \"%s\", Duplex: \"%s\", Port type: \"%s\", "
            "Auto-negotiation: \"%s\", Link detected: \"%s\".",j,
            nifs.data[j].name,nifs.data[j].busAddress,nifs.data[j].hwAddress,
            nifs.data[j].ipAddress,nifInf[j].speed,nifInf[j].duplex,
            nifInf[j].portType,nifInf[j].autoNegotiation,
            nifInf[j].linkDetected);
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate fields number */
  sumFieldN=sizeof(sumLabelV)/sizeof(char*);
  detFieldN=sizeof(detLabelV)/sizeof(char*);
  dettotFieldN=sizeof(dettotLabelV)/sizeof(char*);
  ethFieldN=sizeof(ethLabelV)/sizeof(char*);
  /*-------------------------------------------------------------------------*/
  /* evaluate deviceBuf size */
  for(i=0,deviceBufS=0;i<deviceN;i++)
  {
    deviceBufS+=1+snprintf(NULL,0,"net_%02d",i);
  }
  /* allocate space for deviceBuf */
  deviceBuf=(char*)malloc(deviceBufS);
  /* fill deviceBuf string array */
  for(i=0,deviceBufBS=0;i<deviceN;i++)
  {
    snprintf(device,DEVICE_LEN+1,"net_%02d",i);
    sprintf(deviceBuf+deviceBufBS,"%s",device);
    deviceBufBS+=(1+strlen(deviceBuf+deviceBufBS));
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate sumLabelBuf & sumUnitsBuf size */
  for(i=0,sumLabelBufS=0,sumUnitsBufS=0;i<sumFieldN;i++)
  {
    sumLabelBufS+=(1+strlen(sumLabelV[i]));
    sumUnitsBufS+=(1+strlen(sumUnitsV[i]));
  }
  /* allocate space for sumLabelBuf & sumUnitsBuf */
  sumLabelBuf=(char*)malloc(sumLabelBufS);
  sumUnitsBuf=(char*)malloc(sumUnitsBufS);
  /* fill sumLabelBuf & sumUnitsBuf string array */
  for(i=0,sumLabelBufBS=0,sumUnitsBufBS=0;i<sumFieldN;i++)
  {
    sprintf(sumLabelBuf+sumLabelBufBS,"%s",sumLabelV[i]);
    sumLabelBufBS+=(1+strlen(sumLabelBuf+sumLabelBufBS));
    sprintf(sumUnitsBuf+sumUnitsBufBS,"%s",sumUnitsV[i]);
    sumUnitsBufBS+=(1+strlen(sumUnitsBuf+sumUnitsBufBS));
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate detLabelBuf & detUnitsBuf size */
  for(i=0,detLabelBufS=0,detUnitsBufS=0;i<detFieldN;i++)
  {
    detLabelBufS+=(1+strlen(detLabelV[i]));
    detUnitsBufS+=(1+strlen(detUnitsV[i]));
  }
  /* allocate space for detLabelBuf & detUnitsBuf */
  detLabelBuf=(char*)malloc(detLabelBufS);
  detUnitsBuf=(char*)malloc(detUnitsBufS);
  /* fill detLabelBuf & detUnitsBuf string array */
  for(i=0,detLabelBufBS=0,detUnitsBufBS=0;i<detFieldN;i++)
  {
    sprintf(detLabelBuf+detLabelBufBS,"%s",detLabelV[i]);
    detLabelBufBS+=(1+strlen(detLabelBuf+detLabelBufBS));
    sprintf(detUnitsBuf+detUnitsBufBS,"%s",detUnitsV[i]);
    detUnitsBufBS+=(1+strlen(detUnitsBuf+detUnitsBufBS));
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate dettotLabelBuf & dettotUnitsBuf size */
  for(i=0,dettotLabelBufS=0,dettotUnitsBufS=0;i<dettotFieldN;i++)
  {
    dettotLabelBufS+=(1+strlen(dettotLabelV[i]));
    dettotUnitsBufS+=(1+strlen(dettotUnitsV[i]));
  }
  /* allocate space for detLabelBuf & detUnitsBuf */
  dettotLabelBuf=(char*)malloc(dettotLabelBufS);
  dettotUnitsBuf=(char*)malloc(dettotUnitsBufS);
  /* fill detLabelBuf & detUnitsBuf string array */
  for(i=0,dettotLabelBufBS=0,dettotUnitsBufBS=0;i<dettotFieldN;i++)
  {
    sprintf(dettotLabelBuf+dettotLabelBufBS,"%s",dettotLabelV[i]);
    dettotLabelBufBS+=(1+strlen(dettotLabelBuf+dettotLabelBufBS));
    sprintf(dettotUnitsBuf+dettotUnitsBufBS,"%s",dettotUnitsV[i]);
    dettotUnitsBufBS+=(1+strlen(dettotUnitsBuf+dettotUnitsBufBS));
  }
   /*-------------------------------------------------------------------------*/
  /* evaluate ethLabelBuf & ethUnitsBuf size */
  for(i=0,ethLabelBufS=0,ethUnitsBufS=0;i<ethFieldN;i++)
  {
    ethLabelBufS+=(1+strlen(ethLabelV[i]));
    ethUnitsBufS+=(1+strlen(ethUnitsV[i]));
  }
  /* allocate space for ethLabelBuf & ethUnitsBuf */
  ethLabelBuf=(char*)malloc(ethLabelBufS);
  ethUnitsBuf=(char*)malloc(ethUnitsBufS);
  /* fill ethLabelBuf & ethUnitsBuf string array */
  for(i=0,ethLabelBufBS=0,ethUnitsBufBS=0;i<ethFieldN;i++)
  {
    sprintf(ethLabelBuf+ethLabelBufBS,"%s",ethLabelV[i]);
    ethLabelBufBS+=(1+strlen(ethLabelBuf+ethLabelBufBS));
    sprintf(ethUnitsBuf+ethUnitsBufBS,"%s",ethUnitsV[i]);
    ethUnitsBufBS+=(1+strlen(ethUnitsBuf+ethUnitsBufBS));
  }
  /*-------------------------------------------------------------------------*/
  /* allocate memory for eth_total data */
  ethDataBufS=ethFieldN*sizeof(float);
  ethDataBuf=(float*)malloc(ethDataBufS);
  memset(ethDataBuf,0,ethDataBufS);
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
  /* SVC 0: /FMC/<HOSTNAME>/net/ifs/server_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"server_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rcsidP,1+strlen(rcsidP),0,0);
  /* SVC 1: /FMC/<HOSTNAME>/net/ifs/sensor_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsid0P,1+strlen(sRcsid0P),0,
                                  0);
  /* SVC 2: /FMC/<HOSTNAME>/net/ifs/sensor_version2 */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version2");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsid1P,1+strlen(sRcsid1P),0,
                                  0);
  /* SVC 3: /FMC/<HOSTNAME>/net/ifs/fmc_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"fmc_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  /* SVC 4: /FMC/<HOSTNAME>/net/ifs/devices */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"devices");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",deviceBuf,deviceBufS,0,0);
  /* SVC 5: /FMC/<HOSTNAME>/net/ifs/summary/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sumLabelBuf,sumLabelBufS,0,0);
  /* SVC 6: /FMC/<HOSTNAME>/net/ifs/summary/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sumUnitsBuf,sumUnitsBufS,0,0);
  /* SVC 7: /FMC/<HOSTNAME>/net/ifs/details/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",detLabelBuf,detLabelBufS,0,0);
  /* SVC 8: /FMC/<HOSTNAME>/net/ifs/details/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",detUnitsBuf,detUnitsBufS,0,0);
  /* SVC 9: /FMC/<HOSTNAME>/net/ifs/details/total/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/total/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",dettotLabelBuf,dettotLabelBufS,0,0);
  /* SVC 10: /FMC/<HOSTNAME>/net/ifs/details/total/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/total/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",dettotUnitsBuf,dettotUnitsBufS,0,0);
  /* SVC 11: /FMC/<HOSTNAME>/net/ifs/eth_total/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"eth_total/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",ethLabelBuf,ethLabelBufS,0,0);
  /* SVC 12: /FMC/<HOSTNAME>/net/ifs/eth_total/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"eth_total/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",ethUnitsBuf,ethUnitsBufS,0,0);
  sSvcNum=svcNum;                                                    /* = 11 */
  /*-------------------------------------------------------------------------*/
  /* define dynamic DIM services */
  /* SVC 13: /FMC/<HOSTNAME>/net/ifs/success */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"success");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&nifs.success,sizeof(int),0,0);
  successSvcId=svcId[svcNum-1];
  /* SVC 14: /FMC/<HOSTNAME>/net/ifs/eth_total/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"eth_total/data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",ethDataBuf,4*sizeof(float),0,0);
  /* SVC 15: /FMC/<HOSTNAME>/net/ifs/time_since_reset */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"time_since_reset");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",nifs.stss,TSS_LEN+1,0,0);
  tssSvcId=svcId[svcNum-1];
  for(j=0;j<deviceN;j++)                    /* loop over network interfaces */
  {
    /* SVC 16+j*5: /FMC/<HOSTNAME>/net/ifs/summary/<device>/data */
    svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
    snprintf(s,DIM_SVC_LEN+1,"%s/summary/net_%02d/data",srvPath,j);
    svcId[svcNum-1]=dis_add_service(strdup(s),"F:4;C",0,0,sumDataHandler,j);
    /* SVC 17+j*5: /FMC/<HOSTNAME>/net/ifs/details/<device>/data */
    svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
    snprintf(s,DIM_SVC_LEN+1,"%s/details/net_%02d/data",srvPath,j);
    svcId[svcNum-1]=dis_add_service(strdup(s),"F:18;C",0,0,detDataHandler,j);
    /* SVC 18+j*5: /FMC/<HOSTNAME>/net/ifs/details/<device>/average_data */
    svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
    snprintf(s,DIM_SVC_LEN+1,"%s/details/net_%02d/average_data",srvPath,j);
    svcId[svcNum-1]=dis_add_service(strdup(s),"F:18;C",0,0,detAdataHandler,j);
    /* SVC 19+j*5: /FMC/<HOSTNAME>/net/ifs/details/<device>/maximum_data */
    svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
    snprintf(s,DIM_SVC_LEN+1,"%s/details/net_%02d/maximum_data",srvPath,j);
    svcId[svcNum-1]=dis_add_service(strdup(s),"F:18;C",0,0,detMdataHandler,j);
    /* SVC 20+j*5: /FMC/<HOSTNAME>/net/ifs/details/total/<device>/data */
    svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
    snprintf(s,DIM_SVC_LEN+1,"%s/details/total/net_%02d/data",srvPath,j);
    svcId[svcNum-1]=dis_add_service(strdup(s),"X:16;C",0,0,detTdataHandler,j);
  }
  /* svcNum=15+5*deviceN */
  /*-------------------------------------------------------------------------*/
  /* define reset DIM command */
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
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC Network Interfaces Monitor DIM "
          "Server started (TGID=%u). Using: \"%s\", \"%s\", \"%s\", "
          "\"FMC-%s\".",getpid(),rcsidP,sRcsid0P,sRcsid1P,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* main loop */
  nextTime=time(NULL);
  for(i=0;;i++)
  {
    nextTime+=refreshPeriod;
    /*.......................................................................*/
    /* get data from sensors */
    nifs=readNIF(deBug,errU);
    if(nifs.success)
    {
      if(ethtoolEnabled)
      {
        for(j=0;j<deviceN;j++)
        {
          if(strstr(nifs.data[j].name,"eth"))          /* Ethernet interface */
          {
            /* call ethtool sensor */
            nifInf[j]=getNIFinfo(ethtoolCmd,nifs.data[j].name,deBug,errU);
            if(!nifInf[j].success)
            {
              mPrintf(errU,ERROR,__func__,0,"Error flag returned by "
                      "getNIFinfo() on interface %s: %s",nifs.data[j].name,
                      nifInf[j].errorString);
            }
          }                                            /* Ethernet interface */
        }                                          /* for(j=0;j<deviceN;j++) */
      }                                                /* if(ethtoolEnabled) */
      ethDataBuf[0]=nifs.totalEthRx_bitRate;
      ethDataBuf[1]=nifs.totalEthRx_packetsRate;
      ethDataBuf[2]=nifs.totalEthTx_bitRate;
      ethDataBuf[3]=nifs.totalEthTx_packetsRate;
      if(deBug)logData();
      /* Update all dynamic DIM services */
      for(j=sSvcNum;j<svcNum;j++)dis_update_service(svcId[j]);
    }                                                    /* if(nifs.success) */
    else                                                /* if(!nifs.success) */
    {
      /* Update DIM services */
      dis_update_service(successSvcId);                           /* success */
      dis_update_service(tssSvcId);                                   /* tss */
      mPrintf(errU,ERROR,__func__,0,"Error flag returned by readNIF()!");
    }                                                   /* if(!nifs.success) */
    /*.......................................................................*/
    if(deBug)mPrintf(errU,DEBUG,__func__,0,"time since reset: %s",nifs.stss);
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
  }                                               /* for(i=0;;i++) main loop */
  return 0;
}
/*****************************************************************************/
static void logData(void)
{
  int j;
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,DEBUG,__func__,0,"time since reset: %s",nifs.stss);
  for(j=0;j<nifs.nifN;j++)
  {
    mPrintf(errU,DEBUG,__func__,0,"Interface: net_%02d, IF name: \"%s\", BUS "
            "address: \"%s\", MAC address: \"%s\", IP address: \"%s\", "
            "Speed: \"%s\", Duplex: \"%s\", Port type: \"%s\", "
            "Auto-negotiation: \"%s\", Link detected: \"%s\".",j,
            nifs.data[j].name,nifs.data[j].busAddress,nifs.data[j].hwAddress,
            nifs.data[j].ipAddress,nifInf[j].speed,nifInf[j].duplex,
            nifInf[j].portType,nifInf[j].autoNegotiation,
            nifInf[j].linkDetected);
    mPrintf(errU,DEBUG,__func__,0,"rxBitRate:          %8.3e %8.3e %8.3e",
            nifs.data[j].rate.rxBitRate,
            nifs.data[j].arate.rxBitRate,
            nifs.data[j].mrate.rxBitRate);
    mPrintf(errU,DEBUG,__func__,0,"rxFrameRate:        %8.3e %8.3e %8.3e",
            nifs.data[j].rate.rxFrameRate,
            nifs.data[j].arate.rxFrameRate,
            nifs.data[j].mrate.rxFrameRate);
    mPrintf(errU,DEBUG,__func__,0,"rxMulticastRate:    %8.3e %8.3e %8.3e",
            nifs.data[j].rate.rxMulticastRate,
            nifs.data[j].arate.rxMulticastRate,
            nifs.data[j].mrate.rxMulticastRate);
    mPrintf(errU,DEBUG,__func__,0,"rxCompressedRate:   %8.3e %8.3e %8.3e",
            nifs.data[j].rate.rxCompressedRate,
            nifs.data[j].arate.rxCompressedRate,
            nifs.data[j].mrate.rxCompressedRate);
    mPrintf(errU,DEBUG,__func__,0,"txBitRate:          %8.3e %8.3e %8.3e",
            nifs.data[j].rate.txBitRate,
            nifs.data[j].arate.txBitRate,
            nifs.data[j].mrate.txBitRate);
    mPrintf(errU,DEBUG,__func__,0,"txFrameRate:        %8.3e %8.3e %8.3e",
            nifs.data[j].rate.txFrameRate,
            nifs.data[j].arate.txFrameRate,
            nifs.data[j].mrate.txFrameRate);
    mPrintf(errU,DEBUG,__func__,0,"txCompressedRate:   %8.3e %8.3e %8.3e",
            nifs.data[j].rate.txCompressedRate,
            nifs.data[j].arate.txCompressedRate,
            nifs.data[j].mrate.txCompressedRate);
    mPrintf(errU,DEBUG,__func__,0,"rxBytePerFrame:     %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.rxBytePerFrame,
            nifs.data[j].aratio.rxBytePerFrame,
            nifs.data[j].mratio.rxBytePerFrame);
    mPrintf(errU,DEBUG,__func__,0,"rxErrorFrac:        %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.rxErrorFrac,
            nifs.data[j].aratio.rxErrorFrac,
            nifs.data[j].mratio.rxErrorFrac);
    mPrintf(errU,DEBUG,__func__,0,"rxDroppedFrac:      %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.rxDroppedFrac,
            nifs.data[j].aratio.rxDroppedFrac,
            nifs.data[j].mratio.rxDroppedFrac);
    mPrintf(errU,DEBUG,__func__,0,"rxFifoErrorFrac:    %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.rxFifoErrorFrac,
            nifs.data[j].aratio.rxFifoErrorFrac,
            nifs.data[j].mratio.rxFifoErrorFrac);
    mPrintf(errU,DEBUG,__func__,0,"rxFrameErrorFrac:   %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.rxFrameErrorFrac,
            nifs.data[j].aratio.rxFrameErrorFrac,
            nifs.data[j].mratio.rxFrameErrorFrac);
    mPrintf(errU,DEBUG,__func__,0,"txBytePerFrame:     %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.txBytePerFrame,
            nifs.data[j].aratio.txBytePerFrame,
            nifs.data[j].mratio.txBytePerFrame);
    mPrintf(errU,DEBUG,__func__,0,"txErrorFrac:        %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.txErrorFrac,
            nifs.data[j].aratio.txErrorFrac,
            nifs.data[j].mratio.txErrorFrac);
    mPrintf(errU,DEBUG,__func__,0,"txDroppedFrac:      %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.txDroppedFrac,
            nifs.data[j].aratio.txDroppedFrac,
            nifs.data[j].mratio.txDroppedFrac);
    mPrintf(errU,DEBUG,__func__,0,"txFifoErrorFrac:    %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.txFifoErrorFrac,
            nifs.data[j].aratio.txFifoErrorFrac,
            nifs.data[j].mratio.txFifoErrorFrac);
    mPrintf(errU,DEBUG,__func__,0,"collisionsFrac:     %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.collisionsFrac,
            nifs.data[j].aratio.collisionsFrac,
            nifs.data[j].mratio.collisionsFrac);
    mPrintf(errU,DEBUG,__func__,0,"txCarrierErrorFrac: %8.3e %8.3e %8.3e",
            nifs.data[j].ratio.txCarrierErrorFrac,
            nifs.data[j].aratio.txCarrierErrorFrac,
            nifs.data[j].mratio.txCarrierErrorFrac);
  }
}
/*****************************************************************************/
void sumDataHandler(long *tag,int **address,int *size)
{
  static char *sumDataBuf=NULL;                      /* summary data buffer  */
  static int sumDataBufS=0;                          /* sumDataBuf size      */
  int sumDataBufBS=0;                                /* sumDataBuf busy size */
  float totalRxErrorFrac,totalTxErrorFrac;
  /*-------------------------------------------------------------------------*/
  /* evaluate sumDataBuf size */
  sumDataBufBS=4*sizeof(float)+1+strlen(nifs.data[*tag].name)+
               1+strlen(nifs.data[*tag].busAddress)+
               1+strlen(nifs.data[*tag].hwAddress)+
               1+strlen(nifs.data[*tag].ipAddress);
  /* allocate memory for sumDataBuf */
  if(sumDataBufS<sumDataBufBS)
  {
    sumDataBufS=sumDataBufBS;
    sumDataBuf=(char*)realloc(sumDataBuf,sumDataBufS);
  }
  /* evaluate totalErrorFrac */
  totalRxErrorFrac=nifs.data[*tag].ratio.rxErrorFrac+
                   nifs.data[*tag].ratio.rxDroppedFrac+
                   nifs.data[*tag].ratio.rxFifoErrorFrac+
                   nifs.data[*tag].ratio.rxFrameErrorFrac;
  totalTxErrorFrac=nifs.data[*tag].ratio.txErrorFrac+
                   nifs.data[*tag].ratio.txDroppedFrac+
                   nifs.data[*tag].ratio.txFifoErrorFrac+
                   nifs.data[*tag].ratio.txCarrierErrorFrac;
  /* fill sumDataBuf mixed array */
  sumDataBufBS=0;
  memcpy(sumDataBuf+sumDataBufBS,&nifs.data[*tag].rate.rxBitRate,
         sizeof(float));
  sumDataBufBS+=sizeof(float);
  memcpy(sumDataBuf+sumDataBufBS,&nifs.data[*tag].rate.txBitRate,
         sizeof(float));
  sumDataBufBS+=sizeof(float);
  memcpy(sumDataBuf+sumDataBufBS,&totalRxErrorFrac,sizeof(float));
  sumDataBufBS+=sizeof(float);
  memcpy(sumDataBuf+sumDataBufBS,&totalTxErrorFrac,sizeof(float));
  sumDataBufBS+=sizeof(float);
  sprintf(sumDataBuf+sumDataBufBS,"%s",nifs.data[*tag].name);
  sumDataBufBS+=(1+strlen(sumDataBuf+sumDataBufBS));
  sprintf(sumDataBuf+sumDataBufBS,"%s",nifs.data[*tag].busAddress);
  sumDataBufBS+=(1+strlen(sumDataBuf+sumDataBufBS));
  sprintf(sumDataBuf+sumDataBufBS,"%s",nifs.data[*tag].hwAddress);
  sumDataBufBS+=(1+strlen(sumDataBuf+sumDataBufBS));
  sprintf(sumDataBuf+sumDataBufBS,"%s",nifs.data[*tag].ipAddress);
  sumDataBufBS+=(1+strlen(sumDataBuf+sumDataBufBS));
  /*-------------------------------------------------------------------------*/
  *address=(int*)sumDataBuf;
  *size=sumDataBufBS;
  return;
}
/*****************************************************************************/
void detDataHandler(long *tag,int **address,int *size)
{
  static char *detDataBuf=NULL;                      /* detailed data buffer */
  static int detDataBufS=0;                          /* detDataBuf size      */
  int detDataBufBS=0;                                /* detDataBuf busy size */
  /*-------------------------------------------------------------------------*/
  /* evaluate detDataBuf size */
  detDataBufBS=18*sizeof(float)+1+strlen(nifs.data[*tag].name)+
               1+strlen(nifs.data[*tag].busAddress)+
               1+strlen(nifs.data[*tag].hwAddress)+
               1+strlen(nifs.data[*tag].ipAddress)+
               1+strlen(nifInf[*tag].speed)+
               1+strlen(nifInf[*tag].duplex)+
               1+strlen(nifInf[*tag].portType)+
               1+strlen(nifInf[*tag].autoNegotiation)+
               1+strlen(nifInf[*tag].linkDetected);
  /* allocate memory for detDataBuf */
  if(detDataBufS<detDataBufBS)
  {
    detDataBufS=detDataBufBS;
    detDataBuf=(char*)realloc(detDataBuf,detDataBufS);
  }
  /* fill detDataBuf mixed array */
  detDataBufBS=0;
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].rate.rxBitRate,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].rate.rxFrameRate,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].rate.rxMulticastRate,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].rate.rxCompressedRate,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].rate.txBitRate,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].rate.txFrameRate,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].rate.txCompressedRate,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.rxBytePerFrame,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.rxErrorFrac,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.rxDroppedFrac,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.rxFifoErrorFrac,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.rxFrameErrorFrac,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.txBytePerFrame,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.txErrorFrac,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.txDroppedFrac,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.txFifoErrorFrac,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.collisionsFrac,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  memcpy(detDataBuf+detDataBufBS,&nifs.data[*tag].ratio.txCarrierErrorFrac,
         sizeof(float));
  detDataBufBS+=sizeof(float);
  sprintf(detDataBuf+detDataBufBS,"%s",nifs.data[*tag].name);
  detDataBufBS+=(1+strlen(detDataBuf+detDataBufBS));
  sprintf(detDataBuf+detDataBufBS,"%s",nifs.data[*tag].busAddress);
  detDataBufBS+=(1+strlen(detDataBuf+detDataBufBS));
  sprintf(detDataBuf+detDataBufBS,"%s",nifs.data[*tag].hwAddress);
  detDataBufBS+=(1+strlen(detDataBuf+detDataBufBS));
  sprintf(detDataBuf+detDataBufBS,"%s",nifs.data[*tag].ipAddress);
  detDataBufBS+=(1+strlen(detDataBuf+detDataBufBS));
  sprintf(detDataBuf+detDataBufBS,"%s",nifInf[*tag].speed);
  detDataBufBS+=(1+strlen(detDataBuf+detDataBufBS));
  sprintf(detDataBuf+detDataBufBS,"%s",nifInf[*tag].duplex);
  detDataBufBS+=(1+strlen(detDataBuf+detDataBufBS));
  sprintf(detDataBuf+detDataBufBS,"%s",nifInf[*tag].portType);
  detDataBufBS+=(1+strlen(detDataBuf+detDataBufBS));
  sprintf(detDataBuf+detDataBufBS,"%s",nifInf[*tag].autoNegotiation);
  detDataBufBS+=(1+strlen(detDataBuf+detDataBufBS));
  sprintf(detDataBuf+detDataBufBS,"%s",nifInf[*tag].linkDetected);
  detDataBufBS+=(1+strlen(detDataBuf+detDataBufBS));
  /*-------------------------------------------------------------------------*/
  *address=(int*)detDataBuf;
  *size=detDataBufBS;
  return;
}
/*****************************************************************************/
void detAdataHandler(long *tag,int **address,int *size)
{
  static char *detAdataBuf=NULL;             /* detailed average data buffer */
  static int detAdataBufS=0;                 /* detAdataBuf size             */
  int detAdataBufBS=0;                       /* detAdataBuf busy size        */
  /*-------------------------------------------------------------------------*/
  /* evaluate detAdataBuf size */
  detAdataBufBS=18*sizeof(float)+1+strlen(nifs.data[*tag].name)+
                1+strlen(nifs.data[*tag].busAddress)+
                1+strlen(nifs.data[*tag].hwAddress)+
                1+strlen(nifs.data[*tag].ipAddress)+
                1+strlen(nifInf[*tag].speed)+
                1+strlen(nifInf[*tag].duplex)+
                1+strlen(nifInf[*tag].portType)+
                1+strlen(nifInf[*tag].autoNegotiation)+
                1+strlen(nifInf[*tag].linkDetected);
  /* allocate memory for detAdataBuf */
  if(detAdataBufS<detAdataBufBS)
  {
    detAdataBufS=detAdataBufBS;
    detAdataBuf=(char*)realloc(detAdataBuf,detAdataBufS);
  }
  /* fill detAdataBuf mixed array */
  detAdataBufBS=0;
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].arate.rxBitRate,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].arate.rxFrameRate,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].arate.rxMulticastRate,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].arate.rxCompressedRate,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].arate.txBitRate,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].arate.txFrameRate,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].arate.txCompressedRate,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].aratio.rxBytePerFrame,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].aratio.rxErrorFrac,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].aratio.rxDroppedFrac,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].aratio.rxFifoErrorFrac,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].aratio.rxFrameErrorFrac,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].aratio.txBytePerFrame,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].aratio.txErrorFrac,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].aratio.txDroppedFrac,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].aratio.txFifoErrorFrac,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,&nifs.data[*tag].aratio.collisionsFrac,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  memcpy(detAdataBuf+detAdataBufBS,
         &nifs.data[*tag].aratio.txCarrierErrorFrac,
         sizeof(float));
  detAdataBufBS+=sizeof(float);
  sprintf(detAdataBuf+detAdataBufBS,"%s",nifs.data[*tag].name);
  detAdataBufBS+=(1+strlen(detAdataBuf+detAdataBufBS));
  sprintf(detAdataBuf+detAdataBufBS,"%s",nifs.data[*tag].busAddress);
  detAdataBufBS+=(1+strlen(detAdataBuf+detAdataBufBS));
  sprintf(detAdataBuf+detAdataBufBS,"%s",nifs.data[*tag].hwAddress);
  detAdataBufBS+=(1+strlen(detAdataBuf+detAdataBufBS));
  sprintf(detAdataBuf+detAdataBufBS,"%s",nifs.data[*tag].ipAddress);
  detAdataBufBS+=(1+strlen(detAdataBuf+detAdataBufBS));
  sprintf(detAdataBuf+detAdataBufBS,"%s",nifInf[*tag].speed);
  detAdataBufBS+=(1+strlen(detAdataBuf+detAdataBufBS));
  sprintf(detAdataBuf+detAdataBufBS,"%s",nifInf[*tag].duplex);
  detAdataBufBS+=(1+strlen(detAdataBuf+detAdataBufBS));
  sprintf(detAdataBuf+detAdataBufBS,"%s",nifInf[*tag].portType);
  detAdataBufBS+=(1+strlen(detAdataBuf+detAdataBufBS));
  sprintf(detAdataBuf+detAdataBufBS,"%s",nifInf[*tag].autoNegotiation);
  detAdataBufBS+=(1+strlen(detAdataBuf+detAdataBufBS));
  sprintf(detAdataBuf+detAdataBufBS,"%s",nifInf[*tag].linkDetected);
  detAdataBufBS+=(1+strlen(detAdataBuf+detAdataBufBS));
  /*-------------------------------------------------------------------------*/
  *address=(int*)detAdataBuf;
  *size=detAdataBufBS;
  return;
}
/*****************************************************************************/
void detMdataHandler(long *tag,int **address,int *size)
{
  static char *detMdataBuf=NULL;             /* detailed maximum data buffer */
  static int detMdataBufS=0;                 /* detMdataBuf size             */
  int detMdataBufBS=0;                       /* detMdataBuf busy size        */
  /*-------------------------------------------------------------------------*/
  /* evaluate detMdataBuf size */
  detMdataBufBS=18*sizeof(float)+1+strlen(nifs.data[*tag].name)+
               1+strlen(nifs.data[*tag].busAddress)+
               1+strlen(nifs.data[*tag].hwAddress)+
               1+strlen(nifs.data[*tag].ipAddress)+
               1+strlen(nifInf[*tag].speed)+
               1+strlen(nifInf[*tag].duplex)+
               1+strlen(nifInf[*tag].portType)+
               1+strlen(nifInf[*tag].autoNegotiation)+
               1+strlen(nifInf[*tag].linkDetected);
  /* allocate memory for detMdataBuf */
  if(detMdataBufS<detMdataBufBS)
  {
    detMdataBufS=detMdataBufBS;
    detMdataBuf=(char*)realloc(detMdataBuf,detMdataBufS);
  }
  /* fill detMdataBuf mixed array */
  detMdataBufBS=0;
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mrate.rxBitRate,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mrate.rxFrameRate,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mrate.rxMulticastRate,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mrate.rxCompressedRate,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mrate.txBitRate,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mrate.txFrameRate,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mrate.txCompressedRate,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mratio.rxBytePerFrame,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mratio.rxErrorFrac,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mratio.rxDroppedFrac,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mratio.rxFifoErrorFrac,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mratio.rxFrameErrorFrac,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mratio.txBytePerFrame,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mratio.txErrorFrac,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mratio.txDroppedFrac,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mratio.txFifoErrorFrac,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,&nifs.data[*tag].mratio.collisionsFrac,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  memcpy(detMdataBuf+detMdataBufBS,
         &nifs.data[*tag].mratio.txCarrierErrorFrac,
         sizeof(float));
  detMdataBufBS+=sizeof(float);
  sprintf(detMdataBuf+detMdataBufBS,"%s",nifs.data[*tag].name);
  detMdataBufBS+=(1+strlen(detMdataBuf+detMdataBufBS));
  sprintf(detMdataBuf+detMdataBufBS,"%s",nifs.data[*tag].busAddress);
  detMdataBufBS+=(1+strlen(detMdataBuf+detMdataBufBS));
  sprintf(detMdataBuf+detMdataBufBS,"%s",nifs.data[*tag].hwAddress);
  detMdataBufBS+=(1+strlen(detMdataBuf+detMdataBufBS));
  sprintf(detMdataBuf+detMdataBufBS,"%s",nifs.data[*tag].ipAddress);
  detMdataBufBS+=(1+strlen(detMdataBuf+detMdataBufBS));
  sprintf(detMdataBuf+detMdataBufBS,"%s",nifInf[*tag].speed);
  detMdataBufBS+=(1+strlen(detMdataBuf+detMdataBufBS));
  sprintf(detMdataBuf+detMdataBufBS,"%s",nifInf[*tag].duplex);
  detMdataBufBS+=(1+strlen(detMdataBuf+detMdataBufBS));
  sprintf(detMdataBuf+detMdataBufBS,"%s",nifInf[*tag].portType);
  detMdataBufBS+=(1+strlen(detMdataBuf+detMdataBufBS));
  sprintf(detMdataBuf+detMdataBufBS,"%s",nifInf[*tag].autoNegotiation);
  detMdataBufBS+=(1+strlen(detMdataBuf+detMdataBufBS));
  sprintf(detMdataBuf+detMdataBufBS,"%s",nifInf[*tag].linkDetected);
  detMdataBufBS+=(1+strlen(detMdataBuf+detMdataBufBS));
  /*-------------------------------------------------------------------------*/
  *address=(int*)detMdataBuf;
  *size=detMdataBufBS;
  return;
}
/*****************************************************************************/
void detTdataHandler(long *tag,int **address,int *size)
{
  static char *detTDataBuf=NULL;                     /* detailed data buffer */
  static int detTDataBufS=0;                          /* detDataBuf size      */
  int detTDataBufBS=0;                                /* detDataBuf busy size */
  /*-------------------------------------------------------------------------*/
  /* evaluate detDataBuf size */
  detTDataBufBS=16*sizeof(long long)+1+strlen(nifs.data[*tag].name)+
               1+strlen(nifs.data[*tag].busAddress)+
               1+strlen(nifs.data[*tag].hwAddress)+
               1+strlen(nifs.data[*tag].ipAddress)+
               1+strlen(nifInf[*tag].speed)+
               1+strlen(nifInf[*tag].duplex)+
               1+strlen(nifInf[*tag].portType)+
               1+strlen(nifInf[*tag].autoNegotiation)+
               1+strlen(nifInf[*tag].linkDetected);
  /* allocate memory for detDataBuf */
  if(detTDataBufS<detTDataBufBS)
  {
    detTDataBufS=detTDataBufBS;
    detTDataBuf=(char*)realloc(detTDataBuf,detTDataBufS);
  }
  /* fill detDataBuf mixed array */
  detTDataBufBS=0;
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.rx_bytes,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.rx_packets,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.rx_errors,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.rx_dropped,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.rx_fifo_errors,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.rx_frame_errors,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.rx_compressed,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.rx_multicast,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.tx_bytes,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.tx_packets,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.tx_errors,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.tx_dropped,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.tx_fifo_errors,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.collisions,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.tx_carrier_errors,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  memcpy(detTDataBuf+detTDataBufBS,&nifs.data[*tag].tcnt.tx_compressed,
         sizeof(long long));
  detTDataBufBS+=sizeof(long long);
  sprintf(detTDataBuf+detTDataBufBS,"%s",nifs.data[*tag].name);
  detTDataBufBS+=(1+strlen(detTDataBuf+detTDataBufBS));
  sprintf(detTDataBuf+detTDataBufBS,"%s",nifs.data[*tag].busAddress);
  detTDataBufBS+=(1+strlen(detTDataBuf+detTDataBufBS));
  sprintf(detTDataBuf+detTDataBufBS,"%s",nifs.data[*tag].hwAddress);
  detTDataBufBS+=(1+strlen(detTDataBuf+detTDataBufBS));
  sprintf(detTDataBuf+detTDataBufBS,"%s",nifs.data[*tag].ipAddress);
  detTDataBufBS+=(1+strlen(detTDataBuf+detTDataBufBS));
  sprintf(detTDataBuf+detTDataBufBS,"%s",nifInf[*tag].speed);
  detTDataBufBS+=(1+strlen(detTDataBuf+detTDataBufBS));
  sprintf(detTDataBuf+detTDataBufBS,"%s",nifInf[*tag].duplex);
  detTDataBufBS+=(1+strlen(detTDataBuf+detTDataBufBS));
  sprintf(detTDataBuf+detTDataBufBS,"%s",nifInf[*tag].portType);
  detTDataBufBS+=(1+strlen(detTDataBuf+detTDataBufBS));
  sprintf(detTDataBuf+detTDataBufBS,"%s",nifInf[*tag].autoNegotiation);
  detTDataBufBS+=(1+strlen(detTDataBuf+detTDataBufBS));
  sprintf(detTDataBuf+detTDataBufBS,"%s",nifInf[*tag].linkDetected);
  detTDataBufBS+=(1+strlen(detTDataBuf+detTDataBufBS));
  /*-------------------------------------------------------------------------*/
  *address=(int*)detTDataBuf;
  *size=detTDataBufBS;
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
    resetNIF();
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
"       nifSrv - FMC Network Interface Monitor Server\n"
"\n"
"SYNOPSIS\n"
"       nifSrv [-d] [-c DIM_CONF_FILE] [-l LOGGER] [-E ETHTOOL_PATH]\n"
"              [-u REFRESH_TIME_INTERVAL]\n"
"       nifSrv -h\n"
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
"              (must be in [0,20]s). Default: 20 s.\n"
"       -E ETHTOOL_PATH\n"
"              Use ETHTOOL_PATH as the path to the Ethtool executable.\n"
"              Default: \"/usr/sbin/ethtool\".\n"
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
"       ETHTOOL_CMD (string, optional)\n"
"              Path to the Ethtool executable. Default: \"/usr/sbin/ethtool\"."
"\n"
"              This definition can be overridden by means of the\n"
"              \"-E ETHTOOL_PATH\" command line option.\n"
"\n"
"PUBLISHED DIM COMMAND:\n"
"       /FMC/<HOSTNAME>/net/ifs/reset\n"
"\n"
"PUBLISHED DIM SERVICES:\n"
"       /FMC/<HOSTNAME>/net/ifs/fmc_version (string)\n"
"              \"%s\".\n"
"       /FMC/<HOSTNAME>/net/ifs/server_version (string)\n"
"              \"%s\".\n"
"       /FMC/<HOSTNAME>/net/ifs/sensor_version (string)\n"
"              \"%s\".\n"
"       /FMC/<HOSTNAME>/net/ifs/sensor_version2 (string)\n"
"              \"%s\".\n"
"       /FMC/<HOSTNAME>/net/ifs/success (int)\n"
"              1 if data are retrieved, 0 otherwise.\n"
"       /FMC/<HOSTNAME>/net/ifs/time_since_reset (string)\n"
"       /FMC/<HOSTNAME>/net/ifs/devices (string[])\n"
"              \"net_00\" \"net_01\" \"net_02\" ...\n"
"       /FMC/<HOSTNAME>/net/ifs/summary/labels (string[8])\n"
"              \"rx bit rate\" \"tx bit rate\" \"rx total error frac\"\n"
"              \"tx total error frac\" \"IF name\" \"BUS address\"\n"
"              \"MAC address\" \"IP address\"\n"
"       /FMC/<HOSTNAME>/net/ifs/summary/units (string[8])\n"
"              \"b/s\" \"b/s\" \"\" \"\" \"\" \"\" \"\" \"\"\n"
"       /FMC/<HOSTNAME>/net/ifs/summary/<device>/data (float[4],string[4])\n"
"       /FMC/<HOSTNAME>/net/ifs/details/labels (string[27])\n"
"              \"rx bit rate\" \"rx frame rate\" \"rx multicast rate\"\n"
"              \"rx compressed rate\" \"tx bit rate\" \"tx frame rate\"\n" 
"              \"tx compressed rate\" \"rx frame size\" \"rx error frac\"\n"
"              \"rx dropped frac\" \"rx fifo error frac\"\n"
"              \"rx frame error frac\" \"tx frame size\"\n"
"              \"tx error frac\" \"tx dropped frac\" \"tx fifo error frac\"\n" 
"              \"collision frac\" \"tx carrier error frac\" \"IF name\"\n" 
"              \"BUS address\" \"MAC address\" \"IP address\" \"Speed\"\n"
"              \"Duplex\" \"Port type\" \"Auto-negotiation\" \"Link detected\""
"\n"
"       /FMC/<HOSTNAME>/net/ifs/details/units (string[27])\n"
"              \"b/s\" \"frames/s\" \"frames/s\" \"frames/s\" \"b/s\" "
"\"frames/s\"\n"
"              \"frames/s\" \"B\" \"\" \"\" \"\" \"\" \"B\" \"\" "
"\"\" \"\" \"\" \"\" \"\" \"\"\n"
"              \"\" \"\" \"Mb/s\" \"\" \"\" \"\" \"\"\n"
"       /FMC/<HOSTNAME>/net/ifs/details/<device>/data (float[18],string[9])\n"
"       /FMC/<HOSTNAME>/net/ifs/details/<device>/average_data\n"
"              (float[18],string[9])\n"
"       /FMC/<HOSTNAME>/net/ifs/details/<device>/max_data\n"
"              (float[18],string[9])\n"
"       /FMC/<HOSTNAME>/net/ifs/eth_total/labels (string[4])\n"
"              \"rx bit rate\" \"rx frame rate\" \"tx bit rate\"\n" 
"              \"tx frame rate\"\n"
"       /FMC/<HOSTNAME>/net/ifs/eth_total/units (string[4])\n"
"              \"b/s\" \"frames/s\" \"b/s\" \"frames/s\"\n"
"       /FMC/<HOSTNAME>/net/ifs/eth_total/data (float[4])\n"
"\n"
"AUTHOR\n"
"       Domenico Galli <domenico.galli@bo.infn.it>\n"
"\n"
"VERSION\n"
"       Source:  %s\n"
"       Source:  %s\n"
"       Source:  %s\n"
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       ifconfig(8), ethtool(8), /proc/net/dev\n"
"       /usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/"
        "proc.txt\n"
,rcsidP,sRcsid0P,sRcsid1P,FMC_VERSION,rcsidP,sRcsid0P,sRcsid1P,FMC_VERSION
       );
  exit(1);
}
/*****************************************************************************/
