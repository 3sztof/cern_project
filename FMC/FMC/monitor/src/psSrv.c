/* ######################################################################### */
/* 
 * $Log: psSrv.c,v $
 * Revision 3.30  2009/11/11 16:13:55  galli
 * minor changes
 *
 * Revision 3.29  2009/10/06 17:41:56  galli
 * bug fixed in summary integer SVCs
 *
 * Revision 3.28  2009/10/05 11:40:37  galli
 * bug fixed in summary character SVCs
 *
 * Revision 3.27  2009/10/04 22:09:54  galli
 * added summary services TGID, VSIZE and RSS
 *
 * Revision 3.26  2009/10/03 15:56:55  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 3.25  2009/07/22 23:27:27  galli
 * added command "cmd"
 *
 * Revision 3.24  2009/07/22 21:22:50  galli
 * workaround (timeout) for service handlers deadlock
 * new summary services
 *
 * Revision 3.23  2009/07/20 07:52:26  galli
 * more checks in CMDs
 *
 * Revision 3.13  2009/01/30 14:11:55  galli
 * Process monitoring dynamical SVCs
 *
 * Revision 3.9  2009/01/25 22:43:48  galli
 * Manages CMDs startMonitoring and stopMonitoring
 *
 * Revision 3.8  2009/01/22 10:16:10  galli
 * do not depend on kernel-devel headers
 *
 * Revision 3.0  2009/01/09 14:22:34  galli
 * compatible with getPsFromProc.c v2.13 (temporary version)
 *
 * Revision 2.31  2007/12/17 15:18:58  galli
 * Forced SVC update triggered by tmSrv SVC update
 *
 * Revision 2.29  2007/12/12 13:11:14  galli
 * Added cmd-line option -C CMD_PATTERN
 * Added cmd-line option -U UTGID_PATTERN
 *
 * Revision 2.28  2007/12/11 15:52:30  galli
 * For processes with a huge environment (>4095 B) contact the Task Manager
 * SVC longList, if available, to get the UTGID.
 *
 * Revision 2.25  2007/10/26 23:15:59  galli
 * usage() prints FMC version
 * publishes fmc_version
 *
 * Revision 2.24  2007/09/20 12:42:07  galli
 * can get DIM_DNS_NODE from a configuration file
 * -c dim_conf_file command-line option added
 *
 * Revision 2.22  2007/09/02 21:51:11  galli
 * added service "types"
 *
 * Revision 2.21  2007/08/25 10:18:39  galli
 * added services "labels" and "units"
 *
 * Revision 2.16  2007/08/23 07:02:43  galli
 * clean buffer before filling it
 *
 * Revision 2.14  2007/08/22 06:57:29  galli
 * added datum: pgrp (alias pgid)
 *
 * Revision 2.13  2007/08/21 13:11:01  galli
 * added data: vm_lock, vm_data, vm_stack, vm_exe, vm_lib
 * dynamically allocated data buffer (can only increase in size)
 * to save memory
 *
 * Revision 2.12  2007/08/21 08:30:24  galli
 * duplicated variable removed
 *
 * Revision 2.9  2007/08/20 09:05:32  galli
 * compatible with libFMCsensors v 1.1
 *
 * Revision 2.8  2007/08/20 07:32:39  galli
 * Changed DIM services:
 * only 1 data service for all the data
 *
 * Revision 2.5  2007/08/10 22:38:32  galli
 * compatible with libFMCutils v 2
 * added function usage()
 *
 * Revision 2.4  2006/10/26 08:37:56  galli
 * const intervals between refreshes
 *
 * Revision 2.3  2006/10/25 13:05:54  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 2.1  2004/11/18 09:40:48  gregori
 * ignore SIGHUP
 *
 * Revision 1.2  2004/10/22 16:43:52  gregori
 * log msg, version msg, argument control
 *
 * Revision 1.1  2004/10/15 15:15:46  gregori
 * Initial revision
 * */
/*****************************************************************************/
/* deBug mask:                                                               */
/*   0x0001 in/out functions                                                 */
/*   0x0002 file2strvec()/file2str() diagnostics                             */
/*   0x0004 getTasks() diagnosttics                                          */
/*   0x0008 incoherence in read data                                         */
/*   0x0010 print process and task numbers                                   */
/*   0x0020 print process data from sensor                                   */
/*   0x0040 print task data from sensor                                      */
/*   0x0080 huge environment management                                      */
/*   0x0100 data buffer allocation operations                                */
/*   0x0200 timed/forced updates                                             */
/*   0x0400 dynamical SVC creation/remotion                                  */
/* ######################################################################### */
#define _GNU_SOURCE                                           /* asprintf(3) */
#include <stdio.h>                                  /* EOF, snprintf(), etc. */
#include <stdlib.h>                           /* strtol(), realloc(), exit() */
#include <unistd.h>                           /* getopt(), getpid(), _exit() */
#include <signal.h>               /* pthread_sigmask(), sigtimedwait(), etc. */
#include <syslog.h>                                              /* syslog() */
#define _GNU_SOURCE                                           /* strndupa(3) */
#include <string.h>       /* strlen(), strncpy(), strdup(), strerror(), etc. */
#include <errno.h>                                                  /* errno */
#include <fnmatch.h>                                            /* fnmatch() */
#include <getopt.h>                                         /* getopt_long() */
#include <argz.h>           /* argz_create_sep(), argz_len(), argz_extract() */
#include <inttypes.h>                                            /* uint64_t */
#include <sys/wait.h>         /* waitpid(), WIFEXITED(), WEXITSTATUS(), etc. */
/*---------------------------------------------------------------------------*/
#include <dic.h>                      /* dic_info_service(), MONITORED, etc. */
#include <dis.h>                  /* dis_add_cmnd(), dis_add_service(), etc. */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcMacro.h"     /* eExit(), likely(), unlikely(), printOutFuncOK() */
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
#include "getPsFromProc.h"                        /* getTaskData(), taskData */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#define SERVER 1
#include "psSrv.h"                /* SRV_NAME, DIM_SRV_LEN, DIM_SVC_LEN, ... */
/* ######################################################################### */
/* macro to free-up memory m-allocated by start/stopMonitorCmdHandler() */
#define FREE_CMD_HANDLER_HEAP \
do{ \
  if(cmd) \
  { \
    free(cmd); \
    cmd=NULL; \
  } \
  if(argz) \
  { \
    free(argz); \
    argz=NULL; \
    argz_len=0; \
    free(rArgv); \
    rArgv=NULL; \
    rArgc=0; \
  } \
}while(0)
/*****************************************************************************/
#define FREE_FLTR \
do{ \
  if(fltr) \
  { \
    int i; \
    if(fltr->utgid)free(fltr->utgid); \
    if(fltr->cmd)free(fltr->cmd); \
    for(i=0;i<10;i++) \
    { \
      if(fltr->args[i])free(fltr->args[i]); \
    } \
    free(fltr); \
    fltr=NULL; \
  } \
}while(0)
/* ######################################################################### */
/* type definition */
/*---------------------------------------------------------------------------*/
/* element of list of published dynamical SVCs */
typedef struct dynSvcListEl
{
  unsigned sumSvcId;
  unsigned detSvcId;
  int updated;
  int status;
  uint64_t hStatus; 
  time_t lastUpdateTime;
  pid_t tid;
  uint32_t nlwp;
  uint32_t processor;
  uint32_t vm_size;
  uint32_t vm_lock;
  uint32_t vm_rss;
  uint32_t vm_data;
  uint32_t vm_stack;
  uint32_t vm_exe;
  uint32_t vm_lib;
  uint32_t share;
  int32_t rtprio;
  int32_t nice;
  int32_t prio;
  float pcpu;
  float pmem;
  float minFltRate;
  float majFltRate;
  char cmd[CMD_LEN+1];
  char utgid[UTGID_LEN+1];
  char euser[USER_LEN+1];
  char lastUpdateTimeStr[UPDATETIME_LEN+1];
  char *cmdline;
  struct dynSvcListEl *prev;
  struct dynSvcListEl *next;
}dynSvcListEl_t;
/* list of published dynamical SVCs */
typedef struct dynSvcList
{
  struct timespec timeout;
  pthread_mutex_t mutex;
  dynSvcListEl_t *head;
  dynSvcListEl_t *tail;
}dynSvcList_t;
/*---------------------------------------------------------------------------*/
/* element of list of task filters */
typedef struct tkFltr
{
  pid_t tid;
  char *utgid;
  char *cmd;
  char *args[10];
  struct tkFltr *prev;
  struct tkFltr *next;
}tkFltr_t;
/* list of task filters */
typedef struct tkFltrList
{
  struct timespec timeout;
  pthread_mutex_t mutex;
  tkFltr_t *head;
  tkFltr_t *tail;
}tkFltrList_t;
/* ######################################################################### */
/* globals */
int deBug=0;
int errU=L_DIM;                                                /* error unit */
int svcNum=0;                               /* number of SVCs (for clean-up) */
int sSvcNum=0;                         /* number os static SVCs (for update) */
int pSvcNum=0;          /* number os periodically changing SVCs (for update) */
int *svcId=NULL;
int fltrSvcId=0;
int cmdNum=0;                               /* number of SVCs (for clean-up) */
int *cmdId=NULL;
pid_t tgid;
/* subscribed services */
char tmLlSvcPath[DIM_SVC_LEN+1]="";
int tmLlSvcID;
/* service contents */
char *sRcsid;                                         /* 0) sensor version   */
static char rcsid[]="$Id: psSrv.c,v 3.30 2009/11/11 16:13:55 galli Exp $";
char *rcsidP,*sRcsidP;
int success=0;
unsigned pN=0;                                             /* process number */
unsigned tN=0;                                                /* task number */
int updates=0;                                                    /* updates */
int ssFieldN=0;                  /* number of data fields in spsDataBuf line */
int dsFieldN=0;       /* number of data fields in dpsDataBuf/dtsDataBuf line */
int ssLineLen=0;                          /* max length of a spsDataBuf line */
int dsLineLen=0;               /* max length of a dpsDataBuf/dtsDataBuf line */
int smFieldN=0;                  /* number of data fields in smLabelBuf line */
int dmFieldN=0;                  /* number of data fields in smLabelBuf line */
int smLineLen=0;                          /* max length of a dmLabelBuf line */
int dmLineLen=0;                          /* max length of a dmLabelBuf line */
/* summary string SVC labels */
char *ssLabelBuf=NULL;
int ssLabelBufS=0;                                        /* ssLabelBuf size */
int ssLabelBufBS=0;                                  /* ssLabelBuf busy size */
/* summary string SVC unit */
char *ssUnitBuf=NULL;
int ssUnitBufS=0;                                          /* ssUnitBuf size */
int ssUnitBufBS=0;                                    /* ssUnitBuf busy size */
/* detailed string SVC labels */
char *dsLabelBuf=NULL;
int dsLabelBufS=0;                                        /* dsLabelBuf size */
int dsLabelBufBS=0;                                  /* dsLabelBuf busy size */
/* detailed string SVC unit */
char *dsUnitBuf=NULL;
int dsUnitBufS=0;                                          /* dsUnitBuf size */
int dsUnitBufBS=0;                                    /* dsUnitBuf busy size */
/* detailed string SVC type */
char *dsTypeBuf=NULL;
int dsTypeBufS=0;                                         /* dsTypeBufS size */
int dsTypeBufBS=0;                                   /* dsTypeBufS busy size */
/* summary process string data SVC */
char *spsDataBuf=NULL;
int spsDataBufS=0;                                        /* spsDataBuf size */
int oldSpsDataBufS=0;                                 /* old spsDataBuf size */
int spsDataBufBS=0;                                  /* spsDataBuf busy size */
/* summary process string data SVC */
char *spsCmdBuf=NULL;
char *spsCmdlineBuf=NULL;
char *spsUserBuf=NULL;
int spsCmdBufS=0;
int spsCmdlineBufS=0;
int spsUserBufS=0;
int oldSpsCmdBufS=0;
int oldSpsCmdlineBufS=0;
int oldSpsUserBufS=0;
int spsCmdBufBS=0;
int spsCmdlineBufBS=0;
int spsUserBufBS=0;
/* summary process number data SVC */
float *spsPcpuBuf=NULL;
float *spsPmemBuf=NULL;
uint32_t *spsVsizeBuf=NULL;
uint32_t *spsRssBuf=NULL;
uint32_t *spsTgidBuf=NULL;
int spsPcpuBufBS=0;
int spsPmemBufBS=0;
int spsVsizeBufBS=0;
int spsRssBufBS=0;
int spsTgidBufBS=0;
long maxPn=0;                                 /* maximum number of processes */
long oldMaxPn=0;                          /* old maximum number of processes */
/* detailed process string data SVC */
char *dpsDataBuf=NULL;
int dpsDataBufS=0;                                        /* dpsDataBuf size */
int oldDpsDataBufS=0;                                 /* old dpsDataBuf size */
int dpsDataBufBS=0;                                  /* dpsDataBuf busy size */
/* detailed task string data SVC */
char *dtsDataBuf=NULL;
int dtsDataBufS=0;                                        /* dtsDataBuf size */
int oldDtsDataBufS=0;                                 /* old dtsDataBuf size */
int dtsDataBufBS=0;                                  /* dtsDataBuf busy size */
/* summary monitored SVC labels */
char *smLabelBuf=NULL;
int smLabelBufS=0;                                        /* smLabelBuf size */
int smLabelBufBS=0;                                  /* smLabelBuf busy size */
/* summary monitored SVC unit */
char *smUnitBuf=NULL;
int smUnitBufS=0;                                          /* smUnitBuf size */
int smUnitBufBS=0;                                    /* smUnitBuf busy size */
/* detailed monitored SVC labels */
char *dmLabelBuf=NULL;
int dmLabelBufS=0;                                        /* dmLabelBuf size */
int dmLabelBufBS=0;                                  /* dmLabelBuf busy size */
/* detailed monitored SVC unit */
char *dmUnitBuf=NULL;
int dmUnitBufS=0;                                          /* dmUnitBuf size */
int dmUnitBufBS=0;                                    /* dmUnitBuf busy size */
/* Task Manager data */
char *tmLlBuf=NULL;
int tmLlBufSz=0;
int tmLlBufBSz=0;
struct timespec tmLlTimeout={1,0};
pthread_mutex_t tmLlLock;
int dataReceived=0;
/* forced update */
pid_t main_tid=0;                                  /* tid of the main thread */
pthread_t main_ptid;                 /* thread identifier of the main thread */
/* dynamical services */
/* task filter list */
tkFltrList_t fl={{1,0},PTHREAD_MUTEX_INITIALIZER,NULL,NULL};
/* dynamical SVC list */
dynSvcList_t dsl={{1,0},PTHREAD_MUTEX_INITIALIZER,NULL,NULL};
/*****************************************************************************/
/* function prototype */
/*---------------------------------------------------------------------------*/
/* signal handler */
static void signalHandler(int signo);
/* DIM error handler */
static void disErrorHandler(int severity,int errorCode,char *message);
static void dicErrorHandler(int severity,int errorCode,char *message);
/* DIM SVC server handler */
/* sps: summary process                                                      */
/* dps: detailed process                                                     */
/* dts: detailed tasks                                                       */
/* dpm: detailed process monitored                                           */
/* fl:  filter                                                               */
void spsDataSvcHandler(long *tag,int **address,int *size);
void spsCmdSvcHandler(long *tag,int **address,int *size);
void spsCmdlineSvcHandler(long *tag,int **address,int *size);
void spsUserSvcHandler(long *tag,int **address,int *size);
void spsPcpuSvcHandler(long *tag,int **address,int *size);
void spsPmemSvcHandler(long *tag,int **address,int *size);
void spsVsizeSvcHandler(long *tag,int **address,int *size);
void spsRssSvcHandler(long *tag,int **address,int *size);
void spsTgidSvcHandler(long *tag,int **address,int *size);
void dpsDataSvcHandler(long *tag,int **address,int *size);
void dtsDataSvcHandler(long *tag,int **address,int *size);
void flSvcHandler(long *tag,int **address,int *size);
void spmDataSvcHandler(long *tag,int **address,int *size);
void dpmDataSvcHandler(long *tag,int **address,int *size);
/* DIM SVC client handler */
void *tmLlUpdateHandler(long *nodeN,int *address,int *size);
/* DIM CMD handler */
void cmdHandler(long *tag,int *cmnd,int *size);
void startMonitorCmdHandler(long *tag,int *cmnd,int *size);
void stopMonitorCmdHandler(long *tag,int *cmnd,int *size);
/* utilities */
int tkMatch(tkFltr_t *fltr,taskData_t *prcsData);
int tkMatch2(tkFltr_t *fltr,dynSvcListEl_t *ds);
int fltrCmp(tkFltr_t *f1, tkFltr_t *f2);                  /* compare filters */
char *filter2str(tkFltr_t *fltr);              /* print a filter in a string */
void printFl();                                         /* print filter list */
char *tmLlGetUtgid(pid_t tgid);
/* other functions */
void usage(int mode);
void shortUsage(void);
/* ######################################################################### */
int main(int argc,char **argv)
{
  /* counters */
  int i;
  unsigned pNc=0;                                          /* process number */
  unsigned tNc=0;                                             /* task number */
  /* pointers */
  char *sP;
  /* getopt variables */
  int flag=0;
  /* time refresh parameters and variables */
  int refreshPeriod=20;
  struct timespec waitTimeTS={20,0};
  time_t now,nextTime;
  struct tm lastUpdateTimeL;
  /* signal handling */
  sigset_t signalMask;
  int signo=0;
  /* dim server parameters */
  char srvPath[DIM_SRV_LEN+1]="";
  char s[DIM_SVC_LEN+1]="";
  int doUpdateDimDns=0;
  /* dim client parameters */
  static char noTmLink=-1;
  /* sensor data */
  tasksData_t prcs;
  int rv;
  /* command-line parameters */
  int persistence=6;
  unsigned persistenceMask;
  char *selectUtgid=NULL;
  char *selectCmd=NULL;
  /* dim configuration */
  char *dimDnsNode=NULL;
  /* debug */
  char *deBugS=NULL;
  /* ======================================================================= */
  sRcsid=getPSsensorVersion();
  /* ======================================================================= */
  /* Strip rcsid, sRcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,sP=rcsidP;i<5;i++)sP=1+strchr(sP,' ');
  *(sP-1)='\0';
  sRcsidP=strchr(sRcsid,':')+2;
  for(i=0,sP=sRcsidP;i<5;i++)sP=1+strchr(sP,' ');
  *(sP-1)='\0';
  /* ======================================================================= */
  /* parameters */
  /* max length of a spsDataBuf line */
  ssLineLen = CMD_LEN+1                   /* sensors/include/getPsFromProc.h */
              + UTGID_LEN+1                              /* include/fmcPar.h */
              + ID_LEN+1                          /* monitor/include/psSrv.h */
              + USER_LEN+1                /* sensors/include/getPsFromProc.h */
              + PCPU_LEN+1                        /* monitor/include/psSrv.h */
              + PMEM_LEN+1                        /* monitor/include/psSrv.h */
              + STAT_LEN+1                /* sensors/include/getPsFromProc.h */
              + 2*(MEM_LEN+1)                     /* monitor/include/psSrv.h */
              + CMD_LINE_LEN+1;           /* sensors/include/getPsFromProc.h */
  /* max length of a dpsDataBuf/dtsDataBuf line */
  dsLineLen = CMD_LEN+1                   /* sensors/include/getPsFromProc.h */
              + UTGID_LEN+1                              /* include/fmcPar.h */
              + ID_LEN+1                          /* monitor/include/psSrv.h */
              + USER_LEN+1                /* sensors/include/getPsFromProc.h */
              + TTY_LEN+1                 /* sensors/include/getPsFromProc.h */
              + ID_LEN+1                          /* monitor/include/psSrv.h */
              + 5*(ID_LEN+1)                      /* monitor/include/psSrv.h */
              + 3*(USER_LEN+1)            /* sensors/include/getPsFromProc.h */
              + 4*(GROUP_LEN+1)           /* sensors/include/getPsFromProc.h */
              + SCH_LEN+1                 /* sensors/include/getPsFromProc.h */
              + RTPRIO_LEN+1              /* sensors/include/getPsFromProc.h */
              + NICE_LEN+1                /* sensors/include/getPsFromProc.h */
              + PRIO_LEN+1                /* sensors/include/getPsFromProc.h */
              + PCPU_LEN+1                        /* monitor/include/psSrv.h */
              + PMEM_LEN+1                        /* monitor/include/psSrv.h */
              + 2*(FLT_LEN+1)                     /* monitor/include/psSrv.h */
              + STAT_LEN+1                /* sensors/include/getPsFromProc.h */
              + PSR_LEN+1                         /* monitor/include/psSrv.h */
              + 8*(MEM_LEN+1)                     /* monitor/include/psSrv.h */
              + 4*(SIG_LEN+1)             /* sensors/include/getPsFromProc.h */
              + STARTTIME_LEN+1           /* sensors/include/getPsFromProc.h */
              + ELAPSEDTIME_LEN+1         /* sensors/include/getPsFromProc.h */
              + CPUTIME_LEN+1             /* sensors/include/getPsFromProc.h */
              + CMD_LINE_LEN+1;           /* sensors/include/getPsFromProc.h */
  /* ======================================================================= */
  /* process command-line options */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt(argc,argv,"+l:dN:u:C:U:p:h::"))!=EOF)
  {
    switch(flag)
    {
      case 'l':
        errU=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'd':
        deBug=0xffff;
        break;
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'u':
        refreshPeriod=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'C':
        selectCmd=optarg;
        break;
      case 'U':
        selectUtgid=optarg;
        break;
      case 'p':
        persistence=(int)strtol(optarg,(char**)NULL,0);
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
  /* ======================================================================= */
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
  if(refreshPeriod>60 || refreshPeriod<0)
  {
    mPrintf(L_SYS|L_STD,FATAL,__func__,0,"REFRESH_TIME_INTERVAL must be in "
            "[0,60]");
    shortUsage();
  }
  if(persistence<0 || persistence>31)
  {
    mPrintf(L_SYS|L_STD,FATAL,__func__,0,"PERSISTENCE must be in [0,31]!");
    shortUsage();
  }
  persistenceMask=(1LL<<(persistence+1))-1;
  /* ======================================================================= */
  /* get debug mask from environment */
  deBugS=getenv("deBug");
  if(deBugS)deBug=(int)strtol(deBugS,(char**)NULL,0);
  /* ======================================================================= */
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
  /* ======================================================================= */
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
  /* ======================================================================= */
  /* evaluate fields number */
  ssFieldN=sizeof(ssLabelV)/sizeof(char*);
  dsFieldN=sizeof(dsLabelV)/sizeof(char*);
  smFieldN=sizeof(smLabelV)/sizeof(char*);
  dmFieldN=sizeof(dmLabelV)/sizeof(char*);
  /* ======================================================================= */
  /* evaluate ssLabelBuf size */
  for(i=0,ssLabelBufS=0;i<ssFieldN;i++)
  {
    ssLabelBufS+=(1+strlen(ssLabelV[i]));
  }
  /* allocate space for ssLabelBuf */
  ssLabelBuf=(char*)malloc(ssLabelBufS);
  /* fill ssLabelBuf string array */
  for(i=0,ssLabelBufBS=0;i<ssFieldN;i++)
  {
    sprintf(ssLabelBuf+ssLabelBufBS,"%s",ssLabelV[i]);
    ssLabelBufBS+=(1+strlen(ssLabelBuf+ssLabelBufBS));
  }
  /* ======================================================================= */
  /* evaluate ssUnitBuf size */
  for(i=0,ssUnitBufS=0;i<ssFieldN;i++)
  {
    ssUnitBufS+=(1+strlen(ssUnitV[i]));
  }
  /* allocate space for ssUnitBuf */
  ssUnitBuf=(char*)malloc(ssUnitBufS);
  /* fill ssUnitBuf string array */
  for(i=0,ssUnitBufBS=0;i<ssFieldN;i++)
  {
    sprintf(ssUnitBuf+ssUnitBufBS,"%s",ssUnitV[i]);
    ssUnitBufBS+=(1+strlen(ssUnitBuf+ssUnitBufBS));
  }
  /* ======================================================================= */
  /* evaluate dsLabelBuf size */
  for(i=0,dsLabelBufS=0;i<dsFieldN;i++)
  {
    dsLabelBufS+=(1+strlen(dsLabelV[i]));
  }
  /* allocate space for dsLabelBuf */
  dsLabelBuf=(char*)malloc(dsLabelBufS);
  /* fill dsLabelBuf string array */
  for(i=0,dsLabelBufBS=0;i<dsFieldN;i++)
  {
    sprintf(dsLabelBuf+dsLabelBufBS,"%s",dsLabelV[i]);
    dsLabelBufBS+=(1+strlen(dsLabelBuf+dsLabelBufBS));
  }
  /* ======================================================================= */
  /* evaluate dsUnitBuf size */
  for(i=0,dsUnitBufS=0;i<dsFieldN;i++)
  {
    dsUnitBufS+=(1+strlen(dsUnitV[i]));
  }
  /* allocate space for dsUnitBuf */
  dsUnitBuf=(char*)malloc(dsUnitBufS);
  /* fill dsUnitBuf string array */
  for(i=0,dsUnitBufBS=0;i<dsFieldN;i++)
  {
    sprintf(dsUnitBuf+dsUnitBufBS,"%s",dsUnitV[i]);
    dsUnitBufBS+=(1+strlen(dsUnitBuf+dsUnitBufBS));
  }
  /* ======================================================================= */
  /* evaluate dsTypeBuf size */
  for(i=0,dsTypeBufS=0;i<dsFieldN;i++)
  {
    dsTypeBufS+=(1+strlen(dsTypeV[i]));
  }
  /* allocate space for dsTypeBuf */
  dsTypeBuf=(char*)malloc(dsTypeBufS);
  /* fill dsTypeBuf string array */
  for(i=0,dsTypeBufBS=0;i<dsFieldN;i++)
  {
    sprintf(dsTypeBuf+dsTypeBufBS,"%s",dsTypeV[i]);
    dsTypeBufBS+=(1+strlen(dsTypeBuf+dsTypeBufBS));
  }
  /* ======================================================================= */
  /* evaluate smLabelBuf size */
  for(i=0,smLabelBufS=0;i<smFieldN;i++)
  {
    smLabelBufS+=(1+strlen(smLabelV[i]));
  }
  /* allocate space for smLabelBuf */
  smLabelBuf=(char*)malloc(smLabelBufS);
  /* fill smLabelBuf string array */
  for(i=0,smLabelBufBS=0;i<smFieldN;i++)
  {
    sprintf(smLabelBuf+smLabelBufBS,"%s",smLabelV[i]);
    smLabelBufBS+=(1+strlen(smLabelBuf+smLabelBufBS));
  }
  /* ======================================================================= */
  /* evaluate smUnitBuf size */
  for(i=0,smUnitBufS=0;i<smFieldN;i++)
  {
    smUnitBufS+=(1+strlen(smUnitV[i]));
  }
  /* allocate space for smUnitBuf */
  smUnitBuf=(char*)malloc(smUnitBufS);
  /* fill smUnitBuf string array */
  for(i=0,smUnitBufBS=0;i<smFieldN;i++)
  {
    sprintf(smUnitBuf+smUnitBufBS,"%s",smUnitV[i]);
    smUnitBufBS+=(1+strlen(smUnitBuf+smUnitBufBS));
  }
  /* ======================================================================= */
  /* evaluate dmLabelBuf size */
  for(i=0,dmLabelBufS=0;i<dmFieldN;i++)
  {
    dmLabelBufS+=(1+strlen(dmLabelV[i]));
  }
  /* allocate space for dmLabelBuf */
  dmLabelBuf=(char*)malloc(dmLabelBufS);
  /* fill dmLabelBuf string array */
  for(i=0,dmLabelBufBS=0;i<dmFieldN;i++)
  {
    sprintf(dmLabelBuf+dmLabelBufBS,"%s",dmLabelV[i]);
    dmLabelBufBS+=(1+strlen(dmLabelBuf+dmLabelBufBS));
  }
  /* ======================================================================= */
  /* evaluate dmUnitBuf size */
  for(i=0,dmUnitBufS=0;i<dmFieldN;i++)
  {
    dmUnitBufS+=(1+strlen(dmUnitV[i]));
  }
  /* allocate space for dmUnitBuf */
  dmUnitBuf=(char*)malloc(dmUnitBufS);
  /* fill dmUnitBuf string array */
  for(i=0,dmUnitBufBS=0;i<dmFieldN;i++)
  {
    sprintf(dmUnitBuf+dmUnitBufBS,"%s",dmUnitV[i]);
    dmUnitBufBS+=(1+strlen(dmUnitBuf+dmUnitBufBS));
  }
  /* ======================================================================= */
  if(unlikely(deBug & 0x0100))
  {
    mPrintf(errU,VERB,__func__,0,"Max basic process line length=%d",ssLineLen);
    mPrintf(errU,VERB,__func__,0,"Max detailed process/task line length=%d",
            dsLineLen);
  }
  /* ======================================================================= */
  /* In kernel <= 2.4.19 other threads returns different getpid(). So we     */
  /* save the pid of main thread in a global variable, to be used in         */
  /* resetCmndHandler(). In kernels >= 2.4.20 it makes no differences.       */
  tgid=getpid();
  /* ======================================================================= */
  /* Block SIGINT, SIGTERM and SIGHUP, to be handled synchronously by        */
  /* sigtimedwait().                                                         */
  /* Signals must be blocked before the dim_init() call, which creates 2 new */
  /* threads, to keep the signals blocked in all the 3 threads.              */
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigaddset(&signalMask,SIGHUP);
  if(pthread_sigmask(SIG_BLOCK,&signalMask,NULL))eExit("pthread_sigmask()");
  /* ======================================================================= */
  /* Start DIM. Here threads becomes 3 */
  dim_init();
  /* ======================================================================= */
  /* define server name */
  //snprintf(srvPath,DIM_SRV_LEN+1,"%s_NEW/%s",getSrvPrefix(),SRV_NAME);
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /* ======================================================================= */
  /* define static DIM SVCs */
  svcNum=0;
  /* define DIM service 0: /FMC/<HOST>/ps/sensor_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"sensor_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",sRcsidP,1+strlen(sRcsidP),0,0);
  /* define DIM service 1: /FMC/<HOST>/ps/server_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"server_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",rcsidP,1+strlen(rcsidP),0,0);
  /* define DIM service 2: /FMC/<HOST>/ps/fmc_version */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"fmc_version");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  /* define DIM service 3: /FMC/<HOST>/ps/summary/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",ssLabelBuf,ssLabelBufS,0,0);
  /* define DIM service 4: /FMC/<HOST>/ps/summary/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",ssUnitBuf,ssUnitBufS,0,0);
  /* define DIM service 5: /FMC/<HOST>/ps/details/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",dsLabelBuf,dsLabelBufS,0,0);
  /* define DIM service 6: /FMC/<HOST>/ps/details/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",dsUnitBuf,dsUnitBufS,0,0);
  /* define DIM service 7: /FMC/<HOST>/ps/details/types */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/types");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",dsTypeBuf,dsTypeBufS,0,0);
  /* define DIM service 8: /FMC/<HOST>/ps/summary/monitored/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/monitored/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",smLabelBuf,smLabelBufS,0,0);
  /* define DIM service 9: /FMC/<HOST>/ps/summary/monitored/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/monitored/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",smUnitBuf,smUnitBufS,0,0);
  /* define DIM service 10: /FMC/<HOST>/ps/details/monitored/labels */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/monitored/labels");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",dmLabelBuf,dmLabelBufS,0,0);
  /* define DIM service 11: /FMC/<HOST>/ps/details/monitored/units */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/monitored/units");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",dmUnitBuf,dmUnitBufS,0,0);
  sSvcNum=svcNum;                                              /* sSvcNum=12 */
  /* ======================================================================= */
  /* define periodically changing dynamic DIM SVCs */
  /* define DIM service 12: /FMC/<HOST>/ps/success */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"success");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&success,sizeof(int),0,0);
  /* define DIM service 13: /FMC/<HOST>/ps/nprocs */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"nprocs");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&pN,sizeof(int),0,0);
  /* define DIM service 14: /FMC/<HOST>/ps/ntasks */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"ntasks");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&tN,sizeof(int),0,0);
  /* define DIM service 15: /FMC/<HOST>/ps/updates */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"updates");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",&updates,sizeof(int),0,0);
  /* define DIM service 16: /FMC/<HOST>/ps/summary/processes/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/processes/data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,spsDataSvcHandler,0);
  /* define DIM service 17: /FMC/<HOST>/ps/summary/processes/data/CMD */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/processes/data/CMD");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,spsCmdSvcHandler,0);
  /* define DIM service 18: /FMC/<HOST>/ps/summary/processes/data/CMDLINE */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/processes/data/CMDLINE");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,spsCmdlineSvcHandler,0);
  /* define DIM service 19: /FMC/<HOST>/ps/summary/processes/data/USER */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/processes/data/USER");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,spsUserSvcHandler,0);
  /* define DIM service 20: /FMC/<HOST>/ps/summary/processes/data/PCPU */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/processes/data/PCPU");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",0,0,spsPcpuSvcHandler,0);
  /* define DIM service 21: /FMC/<HOST>/ps/summary/processes/data/PMEM */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/processes/data/PMEM");
  svcId[svcNum-1]=dis_add_service(strdup(s),"F",0,0,spsPmemSvcHandler,0);
  /* define DIM service 22: /FMC/<HOST>/ps/summary/processes/data/VSIZE */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/processes/data/VSIZE");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",0,0,spsVsizeSvcHandler,0);
  /* define DIM service 23: /FMC/<HOST>/ps/summary/processes/data/RSS */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/processes/data/RSS");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",0,0,spsRssSvcHandler,0);
  /* define DIM service 24: /FMC/<HOST>/ps/summary/processes/data/TGID */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"summary/processes/data/TGID");
  svcId[svcNum-1]=dis_add_service(strdup(s),"L",0,0,spsTgidSvcHandler,0);
  /* define DIM service 25: /FMC/<HOST>/ps/details/processes/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/processes/data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,dpsDataSvcHandler,0);
  /* define DIM service 26: /FMC/<HOST>/ps/details/tasks/data */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"details/tasks/data");
  svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,dtsDataSvcHandler,0);
  pSvcNum=svcNum;                                              /* pSvcNum=27 */
  /* ======================================================================= */
  /* define changing on command dynamic DIM SVCs */
  /* define DIM service 24: /FMC/<HOST>/ps/monitored/properties */
  svcId=(int*)realloc(svcId,(++svcNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"monitored/properties");
  fltrSvcId=svcId[svcNum-1]=dis_add_service(strdup(s),"C",0,0,flSvcHandler,0);
  /* svcNum=28 */
  /* ======================================================================= */
  /* define DIM CMDs */
  /* define DIM command 0: /FMC/<HOST>/ps/startMonitoring */
  cmdId=(int*)realloc(cmdId,(++cmdNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"startMonitoring");
  cmdId[cmdNum-1]=dis_add_cmnd(strdup(s),"C",startMonitorCmdHandler,1);
  /* define DIM command 1: /FMC/<HOST>/ps/stopMonitoring */
  cmdId=(int*)realloc(cmdId,(++cmdNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"stopMonitoring");
  cmdId[cmdNum-1]=dis_add_cmnd(strdup(s),"C",stopMonitorCmdHandler,1);
  /* define DIM command 2: /FMC/<HOST>/ps/cmd */
  cmdId=(int*)realloc(cmdId,(++cmdNum)*sizeof(int));
  snprintf(s,DIM_SVC_LEN+1,"%s/%s",srvPath,"cmd");
  cmdId[cmdNum-1]=dis_add_cmnd(strdup(s),"C",cmdHandler,1);
  /* cmdNum=3 */
  /* ======================================================================= */
  /* subscibe to Task Manager longList SVC, if available */
  snprintf(tmLlSvcPath,sizeof(tmLlSvcPath),"%s/task_manager/longList",
           getSrvPrefix());
  tmLlSvcID=dic_info_service(tmLlSvcPath,MONITORED,0,0,0,tmLlUpdateHandler,
                             0,&noTmLink,1);
  /* wait for the first execution of tmLlUpdateHandler() */
  for(dataReceived=0;!dataReceived;)dim_wait();
  mPrintf(errU,INFO,__func__,0,"FMC Task Manager SVC: \"%s\" subscribed.",
          tmLlSvcPath);
  /* ======================================================================= */
  /* define DIM error handler */
  dis_add_error_handler(disErrorHandler);
  dic_add_error_handler(dicErrorHandler);
  /* ======================================================================= */
  /* Start DIM server */
  dis_start_serving(srvPath);
  /* ======================================================================= */
  /* save the tid of the main thread, to send it a SIGHUP using kill() */
  main_tid=getpid();
  /* save the thread identifier of the main thread                           */
  main_ptid=pthread_self();
  /* ======================================================================= */
  /* initialize sensor */
  rv=initTasks(&prcs);
  if(rv)
  {
    mPrintf(errU,FATAL,__func__,0,"Failed initializing process sensor!");
    exit(rv);
  }
  /* ======================================================================= */
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC Process Monitor DIM Server started "
          "(TGID=%u). Using: \"%s\", \"%s\", \"FMC-%s\".",getpid(),rcsidP,
          sRcsidP,FMC_VERSION);
  /* ======================================================================= */
  /* main loop */
  nextTime=time(NULL);
  for(updates=0;;updates++)                             /* loop over updates */
  {
    dynSvcListEl_t *ds=NULL;
    /* --------------------------------------------------------------------- */
    nextTime+=refreshPeriod;
    /* get data from sensor */
    rv=getTasks(&prcs);                           /* get process & task data */
    success=!rv;
    /* --------------------------------------------------------------------- */
    /* If envSz==4096 there is a strong suspect that the running Linux       */
    /* kernel truncates the data in /proc/PID/environ. The kernel patch:     */
    /* "Don-t-truncate-proc-PID-environ-at-4096-characters" appears in       */
    /* version 2.6.9-78.0.1. In case of environ truncation, the FMC Task     */
    /* Manager uses the ptrace(2) workaround and the psSrv contacts the      */
    /* running Task Manager (if any) to get the UTGID.                       */
    for(i=0;i<prcs.n;i++)                             /* loop over processes */
    {
      if(unlikely(unlikely(prcs.data[i].envSz==4096) &&
                  unlikely(!strcmp(prcs.data[i].utgid,"N/A"))))
      {
        char *p;
        p=tmLlGetUtgid(prcs.data[i].tgid);    /* try to get UTGID from tmSrv */
        if(unlikely(deBug & 0x0080))
        {
          mPrintf(errU,INFO,__func__,0,"Process TGID=%d, CMD=\"%s\" has a "
                  "huge environment (>%d B) and the running kernel on this "
                  "node truncates the data in /proc/<PID>/environ. UTGID got "
                  "from /proc/<PID>/environ: \"%s\". UTGID got from the Task "
                  "Manager: \"%s\".",prcs.data[i].tgid,prcs.data[i].cmd,4096,
                  prcs.data[i].utgid,p);
        }
        if(*p)snprintf(prcs.data[i].utgid,UTGID_LEN+1,"%s",p);
      }
    }                                                 /* loop over processes */
    /* --------------------------------------------------------------------- */
    if(unlikely(deBug & 0x0020))
    {
      /* log process data */
      for(i=0;i<prcs.n;i++)
      {
         mPrintf(errU,DEBUG,__func__,0,"process n:%d, CMD:\"%s\", "
                 "UTGID:\"%s\", TID:%d, USER:\"%s\", TTY:\"%s\", NLWP:%d, "
                 "TGID:%d, PPID:%d, PGID:%d, SID:%d, TPGID:%d, FUSER:\"%s\", "
                 "RUSER:\"%s\", SUSER:\"%s\", GROUP:\"%s\", FGROUP:\"%s\", "
                 "RGROUP:\"%s\", SGROUP:\"%s\", SCH:%s, RTPRIO:%s, "
                 "NICE:%s, PRIO:%s, %%CPU:%.1f, %%MEM:%.1f, MIN_FLT:%.1f, "
                 "MAJ_FLT:%.1f, STAT:%s, PSR:%d, VSIZE:%lu, LOCK:%lu, "
                 "RSS:%lu, DATA:%lu, STACK:%lu, EXE:%lu, LIB:%lu, SHARE:%ld, "
                 "IGNORED:%s, PENDING:%s, CATCHED:%s, BLOCKED:%s, "
                 "STARTED:\"%s\", ELAPSED:%s, CPUTIME:%s, CMDLINE:\"%s\".", 
                 i, prcs.data[i].cmd, prcs.data[i].utgid, prcs.data[i].tid,
                 prcs.data[i].euser, prcs.data[i].ttyS, prcs.data[i].nlwp,
                 prcs.data[i].tgid, prcs.data[i].ppid, prcs.data[i].pgrp,
                 prcs.data[i].sid, prcs.data[i].tpgid, prcs.data[i].fuser,
                 prcs.data[i].ruser, prcs.data[i].suser, prcs.data[i].egroup,
                 prcs.data[i].fgroup, prcs.data[i].rgroup, prcs.data[i].sgroup,
                 prcs.data[i].schedS, prcs.data[i].rtprioS, prcs.data[i].niceS,
                 prcs.data[i].prioS, prcs.data[i].pcpu, prcs.data[i].pmem,
                 prcs.data[i].minFltRate, prcs.data[i].majFltRate,
                 prcs.data[i].state, prcs.data[i].processor,
                 prcs.data[i].vm_size, prcs.data[i].vm_lock,
                 prcs.data[i].vm_rss, prcs.data[i].vm_data,
                 prcs.data[i].vm_stack, prcs.data[i].vm_exe,
                 prcs.data[i].vm_lib, prcs.data[i].share,
                 prcs.data[i].sigignored, prcs.data[i].sigpending,
                 prcs.data[i].sigcatched, prcs.data[i].sigblocked,
                 prcs.data[i].startTime, prcs.data[i].elapsedTime,
                 prcs.data[i].cpuTime, prcs.data[i].cmdline);
      }
    }
    /* --------------------------------------------------------------------- */
    /* set process number and task number */
    for(i=0,pNc=0;i<prcs.n;i++)
    {
      if(selectCmd && fnmatch(selectCmd,prcs.data[i].cmd,0))continue;
      if(selectUtgid && fnmatch(selectUtgid,prcs.data[i].utgid,0))continue;
      pNc++;
    }
    pN=pNc;
    tN=tNc;
    /* --------------------------------------------------------------------- */
    if(unlikely(deBug & 0x0010))
    {
      mPrintf(errU,DEBUG,__func__,0,"Selected process number: %d; selected "
              "task number: %d.",pN,tN);
    }
    /* --------------------------------------------------------------------- */
    /* clean spsDataBuf, dpsDataBuf and dtsDataBuf */
    if(spsDataBufS)memset(spsDataBuf,0,spsDataBufS);
    spsDataBufBS=0;
    if(dpsDataBufS)memset(dpsDataBuf,0,dpsDataBufS);
    dpsDataBufBS=0;
    if(dtsDataBufS)memset(dtsDataBuf,0,dtsDataBufS);
    dtsDataBufBS=0;
    /* --------------------------------------------------------------------- */
    /* clean spsCmdBuf, spsCmdlineBuf and spsUserBuf */
    if(spsCmdBufS)memset(spsCmdBuf,0,spsCmdBufS);
    spsCmdBufBS=0;
    if(spsCmdlineBufS)memset(spsCmdlineBuf,0,spsCmdlineBufS);
    spsCmdlineBufBS=0;
    if(spsUserBufS)memset(spsUserBuf,0,spsUserBufS);
    spsUserBufBS=0;
    /* --------------------------------------------------------------------- */
    /* evaluate max pN (process number) value */
    if(maxPn<pN)maxPn=pN;
    /* --------------------------------------------------------------------- */
    /* allocate and clean space for:                                         */
    /* spsPcpuBuf, spsPmemBuf, spsVsizeBuf, spsRssBuf and spsTgidBuf         */
    if(maxPn>oldMaxPn)
    {
      spsPcpuBuf=(float*)realloc(spsPcpuBuf,maxPn*sizeof(float));
      spsPmemBuf=(float*)realloc(spsPmemBuf,maxPn*sizeof(float));
      spsVsizeBuf=(uint32_t*)realloc(spsVsizeBuf,maxPn*sizeof(uint32_t));
      spsRssBuf=(uint32_t*)realloc(spsRssBuf,maxPn*sizeof(uint32_t));
      spsTgidBuf=(uint32_t*)realloc(spsTgidBuf,maxPn*sizeof(uint32_t));
    }
    oldMaxPn=maxPn;
    memset(spsPcpuBuf,0,maxPn*sizeof(float));
    memset(spsPmemBuf,0,maxPn*sizeof(float));
    memset(spsVsizeBuf,0,maxPn*sizeof(uint32_t));
    memset(spsRssBuf,0,maxPn*sizeof(uint32_t));
    memset(spsTgidBuf,0,maxPn*sizeof(uint32_t));
    spsPcpuBufBS=0;
    spsPmemBufBS=0;
    spsVsizeBufBS=0;
    spsRssBufBS=0;
    spsTgidBufBS=0;
    /* --------------------------------------------------------------------- */
    /* fill-in process services */
    for(i=0,pNc=0;i<prcs.n;i++)                       /* loop over processes */
    {
      /* ................................................................... */
      /* (re)allocate space for spsDataBuf */
      if(spsDataBufS-spsDataBufBS<ssLineLen+1)
      {
        oldSpsDataBufS=spsDataBufS;
        spsDataBufS+=(3*ssLineLen+1);
        if(unlikely(deBug & 0x0100))
        {
          mPrintf(errU,VERB,__func__,9, "Summary process buffer %d B, busy: "
                  "%d B, maxLineSize: %d B. Reallocate buffer to %d B.",
                  oldSpsDataBufS,spsDataBufBS,ssLineLen,spsDataBufS);
        }
        spsDataBuf=(char*)realloc(spsDataBuf,spsDataBufS);
        /* clean the new elements of spsDataBuf */
        memset(spsDataBuf+oldSpsDataBufS,0,spsDataBufS-oldSpsDataBufS);
      }
      /* (re)allocate space for spsCmdBuf */
      if(spsCmdBufS-spsCmdBufBS<CMD_LEN+1)
      {
        oldSpsCmdBufS=spsCmdBufS;
        spsCmdBufS+=(3*CMD_LEN+1);
        if(unlikely(deBug & 0x0100))
        {
          mPrintf(errU,VERB,__func__,9, "Summary CMD buffer %d B, busy: "
                  "%d B, maxLineSize: %d B. Reallocate buffer to %d B.",
                  oldSpsCmdBufS,spsCmdBufBS,CMD_LEN,spsCmdBufS);
        }
        spsCmdBuf=(char*)realloc(spsCmdBuf,spsCmdBufS);
        /* clean the new elements of spsCmdBuf */
        memset(spsCmdBuf+oldSpsCmdBufS,0,spsCmdBufS-oldSpsCmdBufS);
      }
      /* (re)allocate space for spsCmdlineBuf */
      if(spsCmdlineBufS-spsCmdlineBufBS<CMD_LINE_LEN+1)
      {
        oldSpsCmdlineBufS=spsCmdlineBufS;
        spsCmdlineBufS+=(3*CMD_LINE_LEN+1);
        if(unlikely(deBug & 0x0100))
        {
          mPrintf(errU,VERB,__func__,9, "Summary CMD_LINE buffer %d B, busy: "
                  "%d B, maxLineSize: %d B. Reallocate buffer to %d B.",
                  oldSpsCmdlineBufS,spsCmdlineBufBS,CMD_LINE_LEN,
                  spsCmdlineBufS);
        }
        spsCmdlineBuf=(char*)realloc(spsCmdlineBuf,spsCmdlineBufS);
        /* clean the new elements of spsCmdlineBuf */
        memset(spsCmdlineBuf+oldSpsCmdlineBufS,0,
               spsCmdlineBufS-oldSpsCmdlineBufS);
      }
      /* (re)allocate space for spsUserBuf */
      if(spsUserBufS-spsUserBufBS<USER_LEN+1)
      {
        oldSpsUserBufS=spsUserBufS;
        spsUserBufS+=(3*USER_LEN+1);
        if(unlikely(deBug & 0x0100))
        {
          mPrintf(errU,VERB,__func__,9, "Summary USER buffer %d B, busy: "
                  "%d B, maxLineSize: %d B. Reallocate buffer to %d B.",
                  oldSpsUserBufS,spsUserBufBS,USER_LEN,spsUserBufS);
        }
        spsUserBuf=(char*)realloc(spsUserBuf,spsUserBufS);
        /* clean the new elements of spsUserBuf */
        memset(spsUserBuf+oldSpsUserBufS,0,spsUserBufS-oldSpsUserBufS);
      }
      /* (re)allocate space for dpsDataBuf */
      if(dpsDataBufS-dpsDataBufBS<dsLineLen+1)
      {
        oldDpsDataBufS=dpsDataBufS;
        dpsDataBufS+=(3*dsLineLen+1);
        if(unlikely(deBug & 0x0100))
        {
          mPrintf(errU,VERB,__func__,9, "Details process buffer %d B, busy: "
                  "%d B, maxLineSize: %d B. Reallocate buffer to %d B.",
                  oldDpsDataBufS,dpsDataBufBS,dsLineLen,dpsDataBufS);
        }
        dpsDataBuf=(char*)realloc(dpsDataBuf,dpsDataBufS);
        /* clean the new elements of dpsDataBuf */
        memset(dpsDataBuf+oldDpsDataBufS,0,dpsDataBufS-oldDpsDataBufS);
      }
      /* ................................................................... */
      if(selectCmd && fnmatch(selectCmd,prcs.data[i].cmd,0))continue;
      if(selectUtgid && fnmatch(selectUtgid,prcs.data[i].utgid,0))continue;
      /* ................................................................... */
      /* compose summary process string data SVC */
      /*  1: CMD, COMMAND, cmd */
      snprintf(spsCmdBuf+spsCmdBufBS,CMD_LEN+1,"%s",prcs.data[i].cmd);
      spsCmdBufBS+=(1+strlen(spsCmdBuf+spsCmdBufBS));
      /*  2: CMDLINE, CMD, ARGS, COMMAND, cmdline */
      snprintf(spsCmdlineBuf+spsCmdlineBufBS,SUM_CMDLINE_LEN+1,"%s",
               prcs.data[i].cmdline);
      spsCmdlineBufBS+=(1+strlen(spsCmdlineBuf+spsCmdlineBufBS));
      /*  3: USER, EUSER, euser */
      snprintf(spsUserBuf+spsUserBufBS,USER_LEN+1,"%s",prcs.data[i].euser);
      spsUserBufBS+=(1+strlen(spsUserBuf+spsUserBufBS));
      /*  4: %CPU, C, PCPU, pcpu */
      spsPcpuBuf[spsPcpuBufBS]=prcs.data[i].pcpu;
      spsPcpuBufBS++;
      /*  5: %MEM, PMEM, pmem */
      spsPmemBuf[spsPmemBufBS]=prcs.data[i].pmem;
      spsPmemBufBS++;
      /*  6: VSIZE, VSZ, VIRT, vm_size */
      spsVsizeBuf[spsVsizeBufBS]=prcs.data[i].vm_size;
      spsVsizeBufBS++;
      /*  7:  RSS, RSZ, RES, RESIDENT, vm_rss */
      spsRssBuf[spsRssBufBS]=prcs.data[i].vm_rss;
      spsRssBufBS++;
      /*  8: TGID, PID, tgid */
      spsTgidBuf[spsTgidBufBS]=prcs.data[i].tgid;
      spsTgidBufBS++;
      /* ................................................................... */
      /* compose summary process string data SVC */
      /*  1: CMD, COMMAND, cmd */
      snprintf(spsDataBuf+spsDataBufBS,CMD_LEN+1,"%s",prcs.data[i].cmd);
      spsDataBufBS+=(1+strlen(spsDataBuf+spsDataBufBS));
      /*  2: UTGID, utgid */
      snprintf(spsDataBuf+spsDataBufBS,UTGID_LEN+1,"%s",prcs.data[i].utgid);
      spsDataBufBS+=(1+strlen(spsDataBuf+spsDataBufBS));
      /*  3: TGID, PID, tgid */
      snprintf(spsDataBuf+spsDataBufBS,ID_LEN+1,"%d",prcs.data[i].tgid);
      spsDataBufBS+=(1+strlen(spsDataBuf+spsDataBufBS));
      /*  4: USER, EUSER, euser */
      snprintf(spsDataBuf+spsDataBufBS,USER_LEN+1,"%s",prcs.data[i].euser);
      spsDataBufBS+=(1+strlen(spsDataBuf+spsDataBufBS));
      /*  5: %CPU, C, PCPU, pcpu */
      snprintf(spsDataBuf+spsDataBufBS,PCPU_LEN+1,"%.3f",prcs.data[i].pcpu);
      spsDataBufBS+=(1+strlen(spsDataBuf+spsDataBufBS));
      /*  6: %MEM, PMEM, pmem */
      snprintf(spsDataBuf+spsDataBufBS,PMEM_LEN+1,"%.3f",prcs.data[i].pmem);
      spsDataBufBS+=(1+strlen(spsDataBuf+spsDataBufBS));
      /*  7: STAT, S, state */
      snprintf(spsDataBuf+spsDataBufBS,STAT_LEN+1,"%s",prcs.data[i].state);
      spsDataBufBS+=(1+strlen(spsDataBuf+spsDataBufBS));
      /*  8: VSIZE, VSZ, VIRT, vm_size */
      snprintf(spsDataBuf+spsDataBufBS,MEM_LEN+1,"%lu",prcs.data[i].vm_size);
      spsDataBufBS+=(1+strlen(spsDataBuf+spsDataBufBS));
      /*  9: RSS, RSZ, RES, RESIDENT, vm_rss */
      snprintf(spsDataBuf+spsDataBufBS,MEM_LEN+1,"%lu",prcs.data[i].vm_rss);
      spsDataBufBS+=(1+strlen(spsDataBuf+spsDataBufBS));
      /* 10: STARTED, STARTTIME, START, starttime */
      snprintf(spsDataBuf+spsDataBufBS,STARTTIME_LEN+1,"%s",
               prcs.data[i].startTime);
      /* 11: CMDLINE, CMD, ARGS, COMMAND, cmdline */
      snprintf(spsDataBuf+spsDataBufBS,CMD_LINE_LEN+1,"%s",
               prcs.data[i].cmdline);
      spsDataBufBS+=(1+strlen(spsDataBuf+spsDataBufBS));
      /* ................................................................... */
      /* compose details process string data SVC */
      /*  1: CMD, COMMAND, cmd */
      snprintf(dpsDataBuf+dpsDataBufBS,CMD_LEN+1,"%s",prcs.data[i].cmd);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /*  2: UTGID, utgid */
      snprintf(dpsDataBuf+dpsDataBufBS,UTGID_LEN+1,"%s",prcs.data[i].utgid);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /*  3: TID, LWP, SPID, tid */
      snprintf(dpsDataBuf+dpsDataBufBS,ID_LEN+1,"%d",prcs.data[i].tid);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /*  4: USER, EUSER, euser */
      snprintf(dpsDataBuf+dpsDataBufBS,USER_LEN+1,"%s",prcs.data[i].euser);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /*  5: TTY, TT, tty */
      snprintf(dpsDataBuf+dpsDataBufBS,TTY_LEN+1,"%s",prcs.data[i].ttyS);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /*  6: NLWP, THCNT, nlwp */
      snprintf(dpsDataBuf+dpsDataBufBS,ID_LEN+1,"%d",prcs.data[i].nlwp);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* ................................................................... */
      /*  7: TGID, PID, tgid */
      snprintf(dpsDataBuf+dpsDataBufBS,ID_LEN+1,"%d",prcs.data[i].tgid);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /*  8: PPID, ppid */
      snprintf(dpsDataBuf+dpsDataBufBS,ID_LEN+1,"%d",prcs.data[i].ppid);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /*  9: PGID, PGRP, pgrp */
      snprintf(dpsDataBuf+dpsDataBufBS,ID_LEN+1,"%d",prcs.data[i].pgrp);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 10: SID, SESS, SESSION, TGID, sid */
      snprintf(dpsDataBuf+dpsDataBufBS,ID_LEN+1,"%d",prcs.data[i].sid);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 11: TPGID, tpgid */
      snprintf(dpsDataBuf+dpsDataBufBS,ID_LEN+1,"%d",prcs.data[i].tpgid);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* ................................................................... */
      /* 12: FUSER, fuser */
      snprintf(dpsDataBuf+dpsDataBufBS,USER_LEN+1,"%s",prcs.data[i].fuser);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 13: RUSER, ruser */
      snprintf(dpsDataBuf+dpsDataBufBS,USER_LEN+1,"%s",prcs.data[i].ruser);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 14: SUSER, suser */
      snprintf(dpsDataBuf+dpsDataBufBS,USER_LEN+1,"%s",prcs.data[i].suser);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 15: GROUP, EGROUP, egroup */
      snprintf(dpsDataBuf+dpsDataBufBS,GROUP_LEN+1,"%s",prcs.data[i].egroup);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 16: FGROUP, fgroup */
      snprintf(dpsDataBuf+dpsDataBufBS,GROUP_LEN+1,"%s",prcs.data[i].fgroup);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 17: RGROUP, rgroup */
      snprintf(dpsDataBuf+dpsDataBufBS,GROUP_LEN+1,"%s",prcs.data[i].rgroup);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 18: SGROUP, sgroup */
      snprintf(dpsDataBuf+dpsDataBufBS,GROUP_LEN+1,"%s",prcs.data[i].sgroup);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* ................................................................... */
      /* 19: SCH, CLS, POL, sched */
      snprintf(dpsDataBuf+dpsDataBufBS,SCH_LEN+1,"%s",prcs.data[i].schedS);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 20: RTPRIO, rtprio */
      snprintf(dpsDataBuf+dpsDataBufBS,RTPRIO_LEN+1,"%s",prcs.data[i].rtprioS);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 21: NICE, NI, nice */
      snprintf(dpsDataBuf+dpsDataBufBS,NICE_LEN+1,"%s",prcs.data[i].niceS);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 22: PRIO, PRI, PR, prio */
      snprintf(dpsDataBuf+dpsDataBufBS,PRIO_LEN+1,"%s",prcs.data[i].prioS);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* ................................................................... */
      /* 23: %CPU, C, PCPU, pcpu */
      snprintf(dpsDataBuf+dpsDataBufBS,PCPU_LEN+1,"%.3f",prcs.data[i].pcpu);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 24: %MEM, PMEM, pmem */
      snprintf(dpsDataBuf+dpsDataBufBS,PMEM_LEN+1,"%.3f",prcs.data[i].pmem);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 25: MIN_FLT, minFltRate */
      snprintf(dpsDataBuf+dpsDataBufBS,FLT_LEN+1,"%.2e",
               prcs.data[i].minFltRate);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 26: MAJ_FLT, majFltRate */
      snprintf(dpsDataBuf+dpsDataBufBS,FLT_LEN+1,"%.2e",
               prcs.data[i].majFltRate);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 27: STAT, S, state */
      snprintf(dpsDataBuf+dpsDataBufBS,STAT_LEN+1,"%s",prcs.data[i].state);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 28: PSR, P. processor */
      snprintf(dpsDataBuf+dpsDataBufBS,PSR_LEN+1,"%d",prcs.data[i].processor);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* ................................................................... */
      /* 29: VSIZE, VSZ, VIRT, vm_size */
      snprintf(dpsDataBuf+dpsDataBufBS,MEM_LEN+1,"%lu",prcs.data[i].vm_size);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 30: LOCK, vm_lock */
      snprintf(dpsDataBuf+dpsDataBufBS,MEM_LEN+1,"%lu",prcs.data[i].vm_lock);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 31: RSS, RSZ, RES, RESIDENT, vm_rss */
      snprintf(dpsDataBuf+dpsDataBufBS,MEM_LEN+1,"%lu",prcs.data[i].vm_rss);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 32: DATA, vm_data */
      snprintf(dpsDataBuf+dpsDataBufBS,MEM_LEN+1,"%lu",prcs.data[i].vm_data);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 33: STACK, vm_stack */
      snprintf(dpsDataBuf+dpsDataBufBS,MEM_LEN+1,"%lu",prcs.data[i].vm_stack);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 34: EXE, vm_exe */
      snprintf(dpsDataBuf+dpsDataBufBS,MEM_LEN+1,"%lu",prcs.data[i].vm_exe);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 35: LIB, vm_lib */
      snprintf(dpsDataBuf+dpsDataBufBS,MEM_LEN+1,"%lu",prcs.data[i].vm_lib);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 36: SHARE, SHR, share */
      snprintf(dpsDataBuf+dpsDataBufBS,MEM_LEN+1,"%ld",prcs.data[i].share);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* ................................................................... */
      /* 37: IGNORED, SIGIGNORE, sigignored */
      snprintf(dpsDataBuf+dpsDataBufBS,SIG_LEN+1,"%s",prcs.data[i].sigignored);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 38: PENDING, SIGPEND, sigpending */
      snprintf(dpsDataBuf+dpsDataBufBS,SIG_LEN+1,"%s",prcs.data[i].sigpending);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 39: CATCHED, SIGCATCH, CAUGHT, sigcatched */
      snprintf(dpsDataBuf+dpsDataBufBS,SIG_LEN+1,"%s",prcs.data[i].sigcatched);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 40: BLOCKED, SIGBLOCKED, sigblocked */
      snprintf(dpsDataBuf+dpsDataBufBS,SIG_LEN+1,"%s",prcs.data[i].sigblocked);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* ................................................................... */
      /* 41: STARTED, STARTTIME, START, starttime */
      snprintf(dpsDataBuf+dpsDataBufBS,STARTTIME_LEN+1,"%s",
               prcs.data[i].startTime);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 42: ELAPSED, ELAPSEDTIME, elapsedtime */
      snprintf(dpsDataBuf+dpsDataBufBS,ELAPSEDTIME_LEN+1,"%s",
               prcs.data[i].elapsedTime);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* 43: CPUTIME, TIME, cputime */
      snprintf(dpsDataBuf+dpsDataBufBS,CPUTIME_LEN+1,"%s",prcs.data[i].cpuTime);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* ................................................................... */
      /* 44: CMDLINE, CMD, ARGS, COMMAND, cmdline */
      snprintf(dpsDataBuf+dpsDataBufBS,CMD_LINE_LEN+1,"%s",
               prcs.data[i].cmdline);
      dpsDataBufBS+=(1+strlen(dpsDataBuf+dpsDataBufBS));
      /* ................................................................... */
    }                                                 /* loop over processes */
    /*-----------------------------------------------------------------------*/
    /* fill-in task services */
    /*-----------------------------------------------------------------------*/
    /* Update periodically updated DIM services */
    for(i=sSvcNum;i<pSvcNum;i++)dis_update_service(svcId[i]);
    /*-----------------------------------------------------------------------*/
    /* Begin management of dynamical DIM services */
    /*-----------------------------------------------------------------------*/
    /* Initialize new values for monitored processes (dynamical SVCs).       */
    /* Data are set to zero, so that they remain zero in terminated          */
    /* processes.                                                            */
    /* The bits in ds->hStatus are the process status (1 = running,          */
    /* 0 = terminated) in the last persistence+1 updates. Status of the last */
    /* update is the least significant bit of ds->hStatus. If the processes  */
    /* is no more monitored, then all bits in ds->hStatus are set to zero.   */
    if(pthread_mutex_lock(&dsl.mutex))eExit("pthread_mutex_lock()");
    for(ds=dsl.head;ds;ds=ds->next)       /* loop over active dynamical SVCs */
    {
      if(unlikely(deBug & 0x0400))
      {
        mPrintf(errU,DEBUG,__func__,0,"Frame %d. Monitored task: CMD=%s "
                "UTGID=%s TID=%d: hStatus=%#8.8x.",updates,ds->cmd,ds->utgid,
                ds->tid,ds->hStatus);
      }
      ds->updated=0;
      ds->status=0;
      ds->hStatus<<=1;
      ds->hStatus&=persistenceMask;
      ds->nlwp=0;
      ds->processor=0;
      ds->vm_size=0;
      ds->vm_lock=0;
      ds->vm_rss=0;
      ds->vm_data=0;
      ds->vm_stack=0;
      ds->vm_exe=0;
      ds->vm_lib=0;
      ds->share=0;
      ds->rtprio=0;
      ds->nice=0;
      ds->prio=0;
      ds->pcpu=0.0;
      ds->pmem=0.0;
      ds->minFltRate=0.0;
      ds->majFltRate=0.0;
    }                                     /* loop over active dynamical SVCs */
    if(pthread_mutex_unlock(&dsl.mutex))eExit("pthread_mutex_unlock()");
    /*-----------------------------------------------------------------------*/
    /* Mark processes no more monitored due to change in filter settings, by */
    /* setting ds->hStatus=0.                                                */
    if(pthread_mutex_lock(&dsl.mutex))eExit("pthread_mutex_lock()");
    for(ds=dsl.head;ds;ds=ds->next)       /* loop over active dynamical SVCs */
    {
      int isSel=0;
      tkFltr_t *fltr=NULL;
      /* ................................................................... */
      if(pthread_mutex_lock(&fl.mutex))eExit("pthread_mutex_lock()");
      for(fltr=fl.head,isSel=0;fltr;fltr=fltr->next)    /* loop over filters */
      {
        if(!tkMatch2(fltr,ds))
        {
          isSel=1;
          break;
        }
      }                                               /* loop over filters */
      if(pthread_mutex_unlock(&fl.mutex))eExit("pthread_mutex_unlock()");
      /* ................................................................... */
      if(!isSel)     /* current process does NOT match any filter expression */
      {
        if(unlikely(deBug & 0x0400))
        {
          mPrintf(errU,DEBUG,__func__,0,"Frame %d. No more monitored task: "
                  "TID=%d, UTGID=\"%s\", CMD=\"%s\", CMDLINE=\"%s\".",updates,
                  ds->tid,ds->utgid,ds->cmd,
                  ds->cmdline);
        }
        ds->hStatus=0;
      }              /* current process does NOT match any filter expression */
    }
    if(pthread_mutex_unlock(&dsl.mutex))eExit("pthread_mutex_unlock()");
    /*-----------------------------------------------------------------------*/
    /* If doUpdateDimDns is set to 1 due to at least one SVC added or        */
    /* removed, then dis_start_serving() is called at the end of the cycle.  */
    doUpdateDimDns=0;
    /*-----------------------------------------------------------------------*/
    /* Remove from the monitored process list dsl:                           */
    /* a) the monitored processes terminated since more than _persistence_   */
    /*    updates;                                                           */
    /* b) the processes no more monitored due to change in filter settings.  */
    /* In both cases ds->hStatus=0.                                          */
    if(pthread_mutex_lock(&dsl.mutex))eExit("pthread_mutex_lock()");
    for(ds=dsl.head;ds;)                  /* loop over active dynamical SVCs */
    {
      if(!(ds->hStatus))                       /* task expired or unselected */
      {
        dynSvcListEl_t *next=NULL;
        dynSvcListEl_t *prev=NULL;
        char *utgid=ds->utgid;
        if(!strcmp(utgid,"N/A"))utgid="";
        /* ................................................................. */
        mPrintf(errU,INFO,__func__,0,"Stop monitoring task: "
                "CMD=\"%s\", UTGID=\"%s\", TID=%d, CMDLINE=\"%s\".",
                ds->cmd,ds->utgid,ds->tid,ds->cmdline);
        /* ................................................................. */
        /* remove DIM SVCs */
        dis_remove_service(ds->sumSvcId);
        if(unlikely(deBug & 0x0400))
        {
          snprintf(s,DIM_SVC_LEN+1,
                   "%s/summary/monitored/processes/%s{%s}%d/data",
                   srvPath,ds->cmd,utgid,ds->tid);
          mPrintf(errU,DEBUG,__func__,0,"Dynamical process monitor DIM SVC "
                  "\"%s\" removed.",s);
        }
        dis_remove_service(ds->detSvcId);
        if(unlikely(deBug & 0x0400))
        {
          snprintf(s,DIM_SVC_LEN+1,
                   "%s/details/monitored/processes/%s{%s}%d/data",
                   srvPath,ds->cmd,utgid,ds->tid);
          mPrintf(errU,DEBUG,__func__,0,"Dynamical process monitor DIM SVC "
                  "\"%s\" removed.",s);
        }
        doUpdateDimDns=1;                /* need to call dis_start_serving() */
        /* ................................................................. */
        /* detach the node ds from the list dsl and relink the dsl list */
        next=ds->next;
        prev=ds->prev;
        if(ds->next)ds->next->prev=prev;
        if(ds->prev)ds->prev->next=next;
        if(!prev)dsl.head=next;
        if(!next)dsl.tail=prev;
        /* ................................................................. */
        /* free memory allocated by the removed node */
        if(ds->cmdline)
        {
          free(ds->cmdline);
          ds->cmdline=NULL;
        }
        if(ds)
        {
          free(ds);
          ds=NULL;
        }
        /* ................................................................. */
        /* move to the next node of the list dsl */
        ds=next; 
        /* ................................................................. */
      }                                        /* task expired or unselected */
      else                                  /* task still alive and selected */
      {
        /* move to the next node of the list dsl */
        ds=ds->next;
      }                                     /* task still alive and selected */
    }                                     /* loop over active dynamical SVCs */
    if(pthread_mutex_unlock(&dsl.mutex))eExit("pthread_mutex_unlock()");
    /*-----------------------------------------------------------------------*/
    /* Refresh dynamical SVCs */
    for(i=0;i<prcs.n;i++)                             /* loop over processes */
    {
      int isSel=0;
      tkFltr_t *fltr=NULL;
      /* ................................................................... */
      /* Check if the current process has to be monitored (isSel=1) by       */
      /* comparing CMD, TID, UTGID, CMDLINE against filter expression (fltr) */
      if(pthread_mutex_lock(&fl.mutex))eExit("pthread_mutex_lock()");
      for(fltr=fl.head,isSel=0;fltr;fltr=fltr->next)    /* loop over filters */
      {
        if(!tkMatch(fltr,&prcs.data[i]))
        {
          isSel=1;
          break;
        }
      }                                                 /* loop over filters */
      if(pthread_mutex_unlock(&fl.mutex))eExit("pthread_mutex_unlock()");
      /* ................................................................... */
      if(isSel)                   /* the current process has to be monitored */
      {
        dynSvcListEl_t *ds=NULL;
        if(unlikely(deBug & 0x0400))
        {
          mPrintf(errU,VERB,__func__,0,"Frame %d. Task: CMD=\"%s\", "
                  "UTGID=\"%s\", TID=%d, CMDLINE=\"%s\" has to be monitored.",
                  updates,prcs.data[i].cmd,prcs.data[i].utgid,prcs.data[i].tid,
                  prcs.data[i].cmdline);
        }
        if(pthread_mutex_lock(&dsl.mutex))eExit("pthread_mutex_lock()");
        /* Look for the current process in the monitored process list dsl */
        for(ds=dsl.head;ds;ds=ds->next)     /* Loop over monitored processes */
        {
          if((ds->tid==prcs.data[i].tid) &&
             !strcmp(ds->utgid,prcs.data[i].utgid) &&
             !strcmp(ds->cmd,prcs.data[i].cmd))
          {
            break;                                          /* Process found */
          }
        }                                   /* Loop over monitored processes */
        if(ds)    /* Current process found in the monitored process list dsl */
        {
          /*.................................................................*/
          /* Update process data for dynamical SVC */
          ds->updated=1;
          ds->status=1;
          ds->hStatus|=0x1;
          ds->lastUpdateTime=time(NULL);
          localtime_r(&ds->lastUpdateTime,&lastUpdateTimeL);
          strftime(ds->lastUpdateTimeStr,UPDATETIME_LEN+1,"%b %d, %H:%M:%S",
                   &lastUpdateTimeL);
          ds->nlwp=prcs.data[i].nlwp;
          ds->processor=prcs.data[i].processor;
          ds->vm_size=prcs.data[i].vm_size;
          ds->vm_lock=prcs.data[i].vm_lock;
          ds->vm_rss=prcs.data[i].vm_rss;
          ds->vm_data=prcs.data[i].vm_data;
          ds->vm_stack=prcs.data[i].vm_stack;
          ds->vm_exe=prcs.data[i].vm_exe;
          ds->vm_lib=prcs.data[i].vm_lib;
          ds->share=prcs.data[i].share;
          ds->rtprio=prcs.data[i].rtprio;
          ds->nice=prcs.data[i].nice;
          ds->prio=prcs.data[i].prio;
          ds->pcpu=prcs.data[i].pcpu;
          ds->pmem=prcs.data[i].pmem;
          ds->minFltRate=prcs.data[i].minFltRate;
          ds->majFltRate=prcs.data[i].majFltRate;
          if(unlikely(deBug & 0x0400))
          {
            mPrintf(errU,DEBUG,__func__,0,"Frame %d. Monitored task: "
                    "CMD=\"%s\", UTGID=\"%s\", TID=%d, CMDLINE=\"%s\" "
                    "updated.",updates,prcs.data[i].cmd,prcs.data[i].utgid,
                    prcs.data[i].tid,prcs.data[i].cmdline);
          }
          /*.................................................................*/
        }         /* Current process found in the monitored process list dsl */
        else  /* Current process NOT found in the monitored process list dsl */
        {
          /* Create a new node in the monitored process list dsl */
          char fmt[64]="";
          char *utgid=NULL;
          /*.................................................................*/
          /* allocate memory for the new node */
          ds=(dynSvcListEl_t*)malloc(sizeof(dynSvcListEl_t));
          memset(ds,0,sizeof(dynSvcListEl_t));
          /*.................................................................*/
          /* fill-in the new node */
          ds->sumSvcId=0;
          ds->detSvcId=0;
          ds->updated=1;
          ds->status=1;
          ds->hStatus=0x1;
          ds->lastUpdateTime=time(NULL);
          localtime_r(&ds->lastUpdateTime,&lastUpdateTimeL);
          strftime(ds->lastUpdateTimeStr,UPDATETIME_LEN+1,"%b %d, %H:%M:%S",
                   &lastUpdateTimeL);
          ds->tid=prcs.data[i].tid;
          ds->nlwp=prcs.data[i].nlwp;
          ds->processor=prcs.data[i].processor;
          ds->vm_size=prcs.data[i].vm_size;
          ds->vm_lock=prcs.data[i].vm_lock;
          ds->vm_rss=prcs.data[i].vm_rss;
          ds->vm_data=prcs.data[i].vm_data;
          ds->vm_stack=prcs.data[i].vm_stack;
          ds->vm_exe=prcs.data[i].vm_exe;
          ds->vm_lib=prcs.data[i].vm_lib;
          ds->share=prcs.data[i].share;
          ds->rtprio=prcs.data[i].rtprio;
          ds->nice=prcs.data[i].nice;
          ds->prio=prcs.data[i].prio;
          ds->pcpu=prcs.data[i].pcpu;
          ds->pmem=prcs.data[i].pmem;
          ds->minFltRate=prcs.data[i].minFltRate;
          ds->majFltRate=prcs.data[i].majFltRate;
          snprintf(ds->cmd,CMD_LEN+1,"%s",prcs.data[i].cmd);
          snprintf(ds->utgid,UTGID_LEN+1,"%s",prcs.data[i].utgid);
          snprintf(ds->euser,USER_LEN+1,"%s",prcs.data[i].euser);
          ds->cmdline=strndup(prcs.data[i].cmdline,CMD_LINE_LEN);  /* malloc */
          /*.................................................................*/
          /* insert the new node in the dsl list */
          if(dsl.tail)                    /* dsl list has at least 1 element */
          {
            ds->prev=dsl.tail;
            dsl.tail->next=ds;
          }
          else                                   /* dsl list has no elements */
          {
            dsl.head=ds;
          }
          dsl.tail=ds;
          /*.................................................................*/
          mPrintf(errU,INFO,__func__,0,"Start monitoring task: "
                  "CMD=\"%s\", UTGID=\"%s\", TID=%d, CMDLINE=\"%s\".",
                  ds->cmd,ds->utgid,ds->tid,ds->cmdline);
          /*.................................................................*/
          if(!strcmp(ds->utgid,"N/A"))utgid="";
          else utgid=ds->utgid;
          /* add DIM service:                                                */
          /* /FMC/HOST/ps/summary/monitored/processes/CMD{UTGID}TID/data     */
          snprintf(s,DIM_SVC_LEN+1,
                   "%s/summary/monitored/processes/%s{%s}%d/data",
                   srvPath,ds->cmd,utgid,ds->tid);
          snprintf(fmt,64,"I:5;F:2;C:%d;C:%d;C:%d;C",CMD_LEN+1,UTGID_LEN+1,
                   USER_LEN+1);
          ds->sumSvcId=dis_add_service(s,fmt,0,0,spmDataSvcHandler,ds->tid);
          if(unlikely(deBug & 0x0400))
          {
            mPrintf(errU,DEBUG,__func__,0,"Dynamical process monitor DIM SVC "
                    "\"%s\" added.",s);
          }
          /* add DIM service:                                                */
          /* /FMC/HOST/ps/details/monitored/processes/CMD{UTGID}TID/data     */
          snprintf(s,DIM_SVC_LEN+1,
                   "%s/details/monitored/processes/%s{%s}%d/data",
                   srvPath,ds->cmd,utgid,ds->tid);
          snprintf(fmt,64,"I:17;F:4;C:%d;C:%d;C:%d;C:%d;C",UPDATETIME_LEN+1,
                   CMD_LEN+1,UTGID_LEN+1,USER_LEN+1);
          ds->detSvcId=dis_add_service(s,fmt,0,0,dpmDataSvcHandler,ds->tid);
          if(unlikely(deBug & 0x0400))
          {
            mPrintf(errU,DEBUG,__func__,0,"Dynamical process monitor DIM SVC "
                    "\"%s\" added.",s);
          }
          doUpdateDimDns=1;              /* need to call dis_start_serving() */
          /*.................................................................*/
        }     /* current process NOT found in the monitored process list dsl */
        if(pthread_mutex_unlock(&dsl.mutex))eExit("pthread_mutex_unlock()");
      }                        /* if the current process has to be monitored */
      /* ................................................................... */
    }                                                 /* loop over processes */
    /*-----------------------------------------------------------------------*/
    /* If at least one SVC added or removed then update the DIM DNS */
    if(doUpdateDimDns)dis_start_serving(srvPath);
    /*-----------------------------------------------------------------------*/
    /* Update dynamical SVCs */
    for(ds=dsl.head;ds;ds=ds->next)       /* loop over active dynamical SVCs */
    {
      dis_update_service(ds->sumSvcId);
      dis_update_service(ds->detSvcId);
    }                                     /* loop over active dynamical SVCs */
    /*-----------------------------------------------------------------------*/
    /* End management of dynamical DIM services */
    /*-----------------------------------------------------------------------*/
    /* sleep for waitTimeTS.tv_sec seconds, but wake-up immediately if a     */
    /* blocked signal is received, to process it (synchronously)             */
    now=time(NULL);
    if(nextTime>now)waitTimeTS.tv_sec=nextTime-now;
    else waitTimeTS.tv_sec=0;
    waitTimeTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&waitTimeTS);
    if(signo!=-1)
    {
      signalHandler(signo);
      if(unlikely(deBug & 0x0200))
      {
        mPrintf(errU,VERB,__func__,0,"Forced update.");
      }
    }
    else
    {
      if(unlikely(deBug & 0x0200))
      {
        mPrintf(errU,VERB,__func__,0,"Timed update.");
      }
    }
    if(updates%500==0 && updates!=0)
    {
      mPrintf(errU,DEBUG,__func__,0,"%d updates.",updates);
      if(unlikely(deBug & 0x0100))
      {
        mPrintf(errU,VERB,__func__,0,"Summary process data buffer size: %d B, "
                "busy: %d B, free %d B = %d ps.",spsDataBufS,spsDataBufBS,
                spsDataBufS-spsDataBufBS,(spsDataBufS-spsDataBufBS)/ssLineLen);
        mPrintf(errU,VERB,__func__,0,"Details process data buffer size: %d B, "
                "busy: %d B, free %d B = %d ps.",dpsDataBufS,dpsDataBufBS,
                dpsDataBufS-dpsDataBufBS,(dpsDataBufS-dpsDataBufBS)/dsLineLen);
        mPrintf(errU,VERB,__func__,0,"Details task data buffer size: %d B, "
                "busy: %d B, free %d B = %d ps.",dtsDataBufS,dtsDataBufBS,
                dtsDataBufS-dtsDataBufBS,(dtsDataBufS-dtsDataBufBS)/dsLineLen);
        mPrintf(errU,VERB,__func__,0,"longList buffer size: %d B, busy: %d B, "
                "free %d B.",tmLlBufSz,tmLlBufBSz,tmLlBufSz-tmLlBufBSz);
      }
    }
    /*-----------------------------------------------------------------------*/
  }                                                     /* loop over updates */
  /* ======================================================================= */
  return 0;
}                                                                  /* main() */
/* ######################################################################### */
void cmdHandler(long *tag,int *cmnd,int *size)
{
  /*-------------------------------------------------------------------------*/
  char *cmd=NULL;
  /*-------------------------------------------------------------------------*/
  cmd=(char*)cmnd;
  cmd+=strspn(cmd," \t");                             /* skip initial blanks */
  mPrintf(errU,DEBUG,__func__,0,"Received CMD: \"%s\".",cmd);
  if(!strncmp(cmd,"StartMonitoringProcess",strlen("StartMonitoringProcess")))
  {
    cmd+=strlen("StartMonitoringProcess");
    cmd+=strspn(cmd," \t");                                    /* skip blanks */
    startMonitorCmdHandler(tag,(int*)cmd,size);
  }
  else if(!strncmp(cmd,"StopMonitoringProcess",strlen("StopMonitoringProcess")))
  {
    cmd+=strlen("StopMonitoringProcess");
    cmd+=strspn(cmd," \t");                                   /* skip blanks */
    stopMonitorCmdHandler(tag,(int*)cmd,size);
  }
  else
  {
    mPrintf(errU,ERROR,__func__,0,"Unknown command: \"%s\"!",cmd);
  }
  return;
}
/*****************************************************************************/
void startMonitorCmdHandler(long *tag,int *cmnd,int *size)
{
  /*-------------------------------------------------------------------------*/
  /* related to argument parsing */
  char *cmd=NULL;                    /* cmnd with "startMonitoring" prefixed */
  char *argz=NULL;
  size_t argz_len=0;
  char **rArgv=NULL;
  int rArgc=0;
  tkFltr_t *fltr=NULL;
  /* check to avoid duplicates */
  int found=0;
  tkFltr_t *strdFltr=NULL;
  char *fltrS=NULL;
  /* getopt */
  int flag;
  char *shortOptions="t:u:c:C:";               /* short command line options */
  static struct option longOptions[]=           /* long command line options */
  {
    /* standard options */
    {"tid",required_argument,NULL,'t'},
    {"utgid",required_argument,NULL,'u'},
    {"cmd",required_argument,NULL,'c'},
    {"cmdline",required_argument,NULL,'C'},
    {"a1",required_argument,NULL,1},
    {"a2",required_argument,NULL,2},
    {"a3",required_argument,NULL,3},
    {"a4",required_argument,NULL,4},
    {"a5",required_argument,NULL,5},
    {"a6",required_argument,NULL,6},
    {"a7",required_argument,NULL,7},
    {"a8",required_argument,NULL,8},
    {"a9",required_argument,NULL,9},
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  asprintf(&cmd,"startMonitoring %s",(char*)cmnd);
  mPrintf(errU,DEBUG,__func__,0,"Received CMD: \"%s\".",cmd);
  if(strchr(cmd,'\n'))
  {
    mPrintf(errU,ERROR,__func__,0,"Invalid CMD: \"%s\" received! CMD must not "
            "contain the newline character!",cmd);
    FREE_CMD_HANDLER_HEAP;
    printOutFuncNOK;
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* convert the command string in an argz vector */
  if(argz_create_sep(cmd,32,&argz,&argz_len))
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": argz_create_sep(): %s!",cmd,
            strerror(errno));
    FREE_CMD_HANDLER_HEAP;
    printOutFuncNOK;
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* convert the argz vector into argv format */
  rArgc=argz_count(argz,argz_len);
  rArgv=(char**)malloc((rArgc+1)*sizeof(char*));
  argz_extract(argz,argz_len,rArgv);
  /*-------------------------------------------------------------------------*/
  /* allocate room for a new task filter */
  fltr=(tkFltr_t*)malloc(sizeof(tkFltr_t));
  memset(fltr,0,sizeof(tkFltr_t));
  /*-------------------------------------------------------------------------*/
  /* process DIM command arguments filling task filter */
  optind=0;
  opterr=0;
  while((flag=getopt_long(rArgc,rArgv,shortOptions,longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 't': /* -t, --tid */
        fltr->tid=(pid_t)strtoul(optarg,(char**)NULL,0);
        break;
      case 'u': /* -u, --utgid */
        fltr->utgid=strdup(optarg);
        break;
      case 'c': /* -c, --cmd */
        fltr->cmd=strdup(optarg);
        break;
      case 'C': /* -C, --cmdline */
        fltr->args[0]=strdup(optarg);
        break;
      case 1: /* --a1 */
        fltr->args[1]=strdup(optarg);
        break;
      case 2: /* --a2 */
        fltr->args[2]=strdup(optarg);
        break;
      case 3: /* --a3 */
        fltr->args[3]=strdup(optarg);
        break;
      case 4: /* --a4 */
        fltr->args[4]=strdup(optarg);
        break;
      case 5: /* --a5 */
        fltr->args[5]=strdup(optarg);
        break;
      case 6: /* --a6 */
        fltr->args[6]=strdup(optarg);
        break;
      case 7: /* --a7 */
        fltr->args[7]=strdup(optarg);
        break;
      case 8: /* --a8 */
        fltr->args[8]=strdup(optarg);
        break;
      case 9: /* --a9 */
        fltr->args[9]=strdup(optarg);
        break;
      default:
        if(optopt)
        {
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt_long(): invalid "
                  "option \"-%c\"!",cmd,optopt);
        }
        else
        {
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt_long(): invalid "
                  "option \"%s\"!",cmd,rArgv[optind-1]);
        }
        FREE_CMD_HANDLER_HEAP;
        FREE_FLTR;
        printOutFuncNOK;
        return;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* if the filter is already in the fl list then send a message and return */
  if(pthread_mutex_lock(&fl.mutex))eExit("pthread_mutex_lock()");
  for(strdFltr=fl.head,found=0;strdFltr;strdFltr=strdFltr->next)
  {
    if(!fltrCmp(fltr,strdFltr))
    {
      found=1;
      break;
    }
  }
  if(pthread_mutex_unlock(&fl.mutex))eExit("pthread_mutex_unlock()");
  if(found)
  {
    fltrS=filter2str(fltr);
    mPrintf(errU,WARN,__func__,0,"CMD: \"%s\": Filter %s is already in the "
            "task selection expression! No filter added!",cmd,fltrS);
    if(fltrS)
    {
      free(fltrS);
      fltrS=NULL;
    }
    printFl();
    FREE_CMD_HANDLER_HEAP;
    FREE_FLTR;
    printOutFuncNOK;
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* add task filter to fl list */
  if(pthread_mutex_lock(&fl.mutex))eExit("pthread_mutex_lock()");
  if(fl.tail)                                   /* fl has at least 1 element */
  {
    fltr->prev=fl.tail;
    fl.tail->next=fltr;
  }
  else                                                 /* fl has no elements */
  {
    fl.head=fltr;
  }
  fl.tail=fltr;
  if(pthread_mutex_unlock(&fl.mutex))eExit("pthread_mutex_unlock()");
  fltrS=filter2str(fltr);
  mPrintf(errU,INFO,__func__,0,"CMD: \"%s\": Filter %s added to the task "
          "selection expression.",cmd,fltrS);
  if(fltrS)
  {
    free(fltrS);
    fltrS=NULL;
  }
  dis_update_service(fltrSvcId);
  printFl();
  /*-------------------------------------------------------------------------*/
  FREE_CMD_HANDLER_HEAP;
  if(unlikely(deBug & 0x0200))
  {
    mPrintf(errU,VERB,__func__,0,"Forcing a SVC update...");
  }
  if(main_tid)kill(main_tid,SIGHUP);
  printOutFuncOK;
  /*-------------------------------------------------------------------------*/
  return;
}
/*****************************************************************************/
void stopMonitorCmdHandler(long *tag,int *cmnd,int *size)
{
  /*-------------------------------------------------------------------------*/
  /* related to argument parsing */
  char *cmd=NULL;                    /* cmnd with "startMonitoring" prefixed */
  char *argz=NULL;
  size_t argz_len=0;
  char **rArgv=NULL;
  int rArgc=0;
  tkFltr_t *fltr=NULL;
  /* check to avoid duplicates */
  int found=0;
  tkFltr_t *strdFltr=NULL;
  tkFltr_t *next, *prev;
  /* getopt */
  int flag;
  char *shortOptions="t:u:c:C:";               /* short command line options */
  static struct option longOptions[]=           /* long command line options */
  {
    /* standard options */
    {"tid",required_argument,NULL,'t'},
    {"utgid",required_argument,NULL,'u'},
    {"cmd",required_argument,NULL,'c'},
    {"cmdline",required_argument,NULL,'C'},
    {"a1",required_argument,NULL,1},
    {"a2",required_argument,NULL,2},
    {"a3",required_argument,NULL,3},
    {"a4",required_argument,NULL,4},
    {"a5",required_argument,NULL,5},
    {"a6",required_argument,NULL,6},
    {"a7",required_argument,NULL,7},
    {"a8",required_argument,NULL,8},
    {"a9",required_argument,NULL,9},
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  asprintf(&cmd,"stopMonitoring %s",(char*)cmnd);
  mPrintf(errU,DEBUG,__func__,0,"Received CMD: \"%s\".",cmd);
  if(strchr(cmd,'\n'))
  {
    mPrintf(errU,ERROR,__func__,0,"Invalid CMD: \"%s\" received! CMD must not "
            "contain the newline character!",cmd);
    FREE_CMD_HANDLER_HEAP;
    printOutFuncNOK;
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* convert the command string in an argz vector */
  if(argz_create_sep(cmd,32,&argz,&argz_len))
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": argz_create_sep(): %s!",cmd,
            strerror(errno));
    FREE_CMD_HANDLER_HEAP;
    printOutFuncNOK;
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* convert the argz vector into argv format */
  rArgc=argz_count(argz,argz_len);
  rArgv=(char**)malloc((rArgc+1)*sizeof(char*));
  argz_extract(argz,argz_len,rArgv);
  /*-------------------------------------------------------------------------*/
  /* allocate room to store the task filter to suppress */
  fltr=(tkFltr_t*)malloc(sizeof(tkFltr_t));
  memset(fltr,0,sizeof(tkFltr_t));
  /*-------------------------------------------------------------------------*/
  /* process DIM command arguments filling task filter */
  optind=0;
  opterr=0;
  while((flag=getopt_long(rArgc,rArgv,shortOptions,longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 't': /* -t, --tid */
        fltr->tid=(pid_t)strtoul(optarg,(char**)NULL,0);
        break;
      case 'u': /* -u, --utgid */
        fltr->utgid=strdup(optarg);
        break;
      case 'c': /* -c, --cmd */
        fltr->cmd=strdup(optarg);
        break;
      case 'C': /* -C, --cmdline */
        fltr->args[0]=strdup(optarg);
        break;
      case 1: /* --a1 */
        fltr->args[1]=strdup(optarg);
        break;
      case 2: /* --a2 */
        fltr->args[2]=strdup(optarg);
        break;
      case 3: /* --a3 */
        fltr->args[3]=strdup(optarg);
        break;
      case 4: /* --a4 */
        fltr->args[4]=strdup(optarg);
        break;
      case 5: /* --a5 */
        fltr->args[5]=strdup(optarg);
        break;
      case 6: /* --a6 */
        fltr->args[6]=strdup(optarg);
        break;
      case 7: /* --a7 */
        fltr->args[7]=strdup(optarg);
        break;
      case 8: /* --a8 */
        fltr->args[8]=strdup(optarg);
        break;
      case 9: /* --a9 */
        fltr->args[9]=strdup(optarg);
        break;
      default:
        if(optopt)
        {
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt_long(): invalid "
                  "option \"-%c\"!",cmd,optopt);
        }
        else
        {
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt_long(): invalid "
                  "option \"%s\"!",cmd,rArgv[optind-1]);
        }
        FREE_CMD_HANDLER_HEAP;
        FREE_FLTR;
        printOutFuncNOK;
        return;
    }
  }
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&fl.mutex))eExit("pthread_mutex_lock()");
  /* find the filter to suppress in the list */
  for(strdFltr=fl.head,found=0;strdFltr;strdFltr=strdFltr->next)
  {
    if(!fltrCmp(fltr,strdFltr))
    {
      found=1;
      break;
    }
  }
  if(found)
  {
    /* remove task filter from fl list */
    next=strdFltr->next;
    prev=strdFltr->prev;
    if(strdFltr->next)strdFltr->next->prev=prev;
    if(strdFltr->prev)strdFltr->prev->next=next;
    if(!prev)fl.head=next;
    if(!next)fl.tail=prev;
  }
  if(pthread_mutex_unlock(&fl.mutex))eExit("pthread_mutex_unlock()");
  if(found)
  {
    int i;
    char *fltrS=NULL;
    /* free memory */
    if(strdFltr->utgid)free(strdFltr->utgid);
    if(strdFltr->cmd)free(strdFltr->cmd);
    for(i=0;i<10;i++)
    {
      if(strdFltr->args[i])free(strdFltr->args[i]);
    }
    free(strdFltr);
    strdFltr=NULL;
    fltrS=filter2str(fltr);
    mPrintf(errU,INFO,__func__,0,"CMD: \"%s\": Filter %s removed from the "
            "task selection expression.",cmd,fltrS);
    if(fltrS)
    {
      free(fltrS);
      fltrS=NULL;
    }
  }
  else
  {
    char *fltrS=NULL;
    fltrS=filter2str(fltr);
    mPrintf(errU,WARN,__func__,0,"CMD: \"%s\": Filter %s is not in the task "
            "selection expression! No filter removed!",cmd,fltrS);
    if(fltrS)
    {
      free(fltrS);
      fltrS=NULL;
    }
  }
  dis_update_service(fltrSvcId);
  printFl();
  /*-------------------------------------------------------------------------*/
  FREE_CMD_HANDLER_HEAP;
  FREE_FLTR;
  if(unlikely(deBug & 0x0200))
  {
    mPrintf(errU,VERB,__func__,0,"Forcing a SVC update...");
  }
  if(main_tid)kill(main_tid,SIGHUP);
  printOutFuncOK;
  /*-------------------------------------------------------------------------*/
  return;
}
/* ######################################################################### */
/* DIM SVC server handler */
void spsCmdSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)spsCmdBuf;
  *size=spsCmdBufBS;
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void spsCmdlineSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)spsCmdlineBuf;
  *size=spsCmdlineBufBS;
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void spsUserSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)spsUserBuf;
  *size=spsUserBufBS;
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void spsPcpuSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)spsPcpuBuf;
  *size=spsPcpuBufBS*sizeof(float);
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void spsPmemSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)spsPmemBuf;
  *size=spsPmemBufBS*sizeof(float);
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void spsVsizeSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)spsVsizeBuf;
  *size=spsVsizeBufBS*sizeof(uint32_t);
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void spsRssSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)spsRssBuf;
  *size=spsRssBufBS*sizeof(uint32_t);
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void spsTgidSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)spsTgidBuf;
  *size=spsTgidBufBS*sizeof(uint32_t);
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void spsDataSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)spsDataBuf;
  *size=spsDataBufBS;
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void dpsDataSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)dpsDataBuf;
  *size=dpsDataBufBS;
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void dtsDataSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  *address=(int*)dtsDataBuf;
  *size=dtsDataBufBS;
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void flSvcHandler(long *tag,int **address,int *size)
{
  int rv=0;
  static char *msg=NULL;
  static int msgL=0;
  tkFltr_t *fltr=NULL;
  char *p;
  int i;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  msgL=0;
  msg=(char*)realloc(msg,msgL+1);
  msg[0]='\0';
  /*-------------------------------------------------------------------------*/
  fl.timeout.tv_sec=1;
  fl.timeout.tv_nsec=0;
  rv=pthread_mutex_timedlock(&fl.mutex,&fl.timeout);
  if(rv)
  {
    mPrintf(errU,DEBUG,__func__,0,"file: \"%s\", line %d: "
            "pthread_mutex_timedlock(): Error %d: %s!",__FILE__,__LINE__,errno,
            strerror(errno));
    mPrintf(errU,DEBUG,__func__,0,"Skip task filter list SVC update!");
    if(msg)free(msg);
    return;
  }
  /*-------------------------------------------------------------------------*/
  for(fltr=fl.head,i=0;fltr;fltr=fltr->next,i++)        /* loop over filters */
  {
    char *fltrS=NULL;
    /* ..................................................................... */
    if(i>0)
    {
      msgL+=snprintf(NULL,0," || ");
      msg=(char*)realloc(msg,msgL+1);
      p=strchr(msg,'\0');
      sprintf(p," || ");
    }
    /* ..................................................................... */
    fltrS=filter2str(fltr);
    msgL+=snprintf(NULL,0,"%s",fltrS);
    msg=(char*)realloc(msg,msgL+1);
    p=strchr(msg,'\0');
    sprintf(p,"%s",fltrS);
    if(fltrS)
    {
      free(fltrS);
      fltrS=NULL;
    }
    /* ..................................................................... */
  }                                                     /* loop over filters */
  if(pthread_mutex_unlock(&fl.mutex))eExit("pthread_mutex_unlock()");
  /*-------------------------------------------------------------------------*/
  if(!i)
  {
    msgL+=snprintf(NULL,0,"0");
    msg=(char*)realloc(msg,msgL+1);
    p=strchr(msg,'\0');
    sprintf(p,"0");
  }
  /*-------------------------------------------------------------------------*/
  *address=(int*)msg;
  *size=1+strlen(msg);
  /*-------------------------------------------------------------------------*/
  printOutFuncOK;
  return;
}
/*****************************************************************************/
/* summary process monitored data service handler */
void spmDataSvcHandler(long *tag,int **address,int *size)
{
  int rv=0;
  dynSvcListEl_t *ds=NULL;
  static char *dataBuf=NULL;                            /* data buffer       */
  static int dataBufS=0;                                /* dataBuf size      */
  int dataBufBS=0;                                      /* dataBuf busy size */
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  dsl.timeout.tv_sec=1;
  dsl.timeout.tv_nsec=0;
  rv=pthread_mutex_timedlock(&dsl.mutex,&dsl.timeout);
  if(rv)
  {
    mPrintf(errU,DEBUG,__func__,0,"file: \"%s\", line %d: "
            "pthread_mutex_timedlock(): Error %d: %s!",__FILE__,__LINE__,errno,
            strerror(errno));
    mPrintf(errU,DEBUG,__func__,0,"Skip SVC update for tid: %u!",(pid_t)*tag);
    return;
  }
  /*-------------------------------------------------------------------------*/
  if(unlikely(deBug & 0x0400))
  {
    if(pthread_equal(pthread_self(),main_ptid))
    {
      mPrintf(errU,DEBUG,__func__,0,"Executing for task tid=%lu in MAIN "
              "THREAD (MONITORED update, requested by DIM server).",*tag);
    }
    else
    {
      mPrintf(errU,DEBUG,__func__,0,"Executing for task tid=%lu in I/O "
              "THREAD (TIMED update, requested by DIM client).",*tag);
    }
  }
  /*-------------------------------------------------------------------------*/
  for(ds=dsl.head;ds;ds=ds->next)              /* loop over dynamic SVC list */
  {
    if(ds->tid==*tag)
    {
      /* evaluate dataBuf size */
      dataBufBS=2*sizeof(int32_t)+3*sizeof(uint32_t)+2*sizeof(float)+CMD_LEN+1+
                UTGID_LEN+1+USER_LEN+1+strlen(ds->cmdline)+1;
      /* allocate memory for dataBuf */
      if(dataBufS<dataBufBS)
      {
        dataBufS=dataBufBS;
        dataBuf=(char*)realloc(dataBuf,dataBufS);
      }
      memset(dataBuf,0,dataBufS);
      /* fill dataBuf mixed array */
      dataBufBS=0;
      memcpy(dataBuf+dataBufBS,&ds->updated,sizeof(int32_t));
      dataBufBS+=sizeof(int32_t);
      memcpy(dataBuf+dataBufBS,&ds->status,sizeof(int32_t));
      dataBufBS+=sizeof(int32_t);
      memcpy(dataBuf+dataBufBS,&ds->tid,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->vm_size,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->vm_rss,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->pcpu,sizeof(float));
      dataBufBS+=sizeof(float);
      memcpy(dataBuf+dataBufBS,&ds->pmem,sizeof(float));
      dataBufBS+=sizeof(float);
      snprintf(dataBuf+dataBufBS,CMD_LEN+1,"%s",ds->cmd);
      dataBufBS+=CMD_LEN+1;
      snprintf(dataBuf+dataBufBS,UTGID_LEN+1,"%s",ds->utgid);
      dataBufBS+=UTGID_LEN+1;
      snprintf(dataBuf+dataBufBS,USER_LEN+1,"%s",ds->euser);
      dataBufBS+=USER_LEN+1;
      snprintf(dataBuf+dataBufBS,1+strlen(ds->cmdline),"%s",ds->cmdline);
      dataBufBS+=1+strlen(ds->cmdline);
    }
  }                                            /* loop over dynamic SVC list */
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_unlock(&dsl.mutex))eExit("pthread_mutex_unlock()");
  /*-------------------------------------------------------------------------*/
  *address=(int*)dataBuf;
  *size=dataBufBS;
  printOutFuncOK;
  return;
}
/*****************************************************************************/
/* detailed process monitored data service handler */
void dpmDataSvcHandler(long *tag,int **address,int *size)
{
  int rv=0;
  dynSvcListEl_t *ds=NULL;
  static char *dataBuf=NULL;                            /* data buffer       */
  static int dataBufS=0;                                /* dataBuf size      */
  int dataBufBS=0;                                      /* dataBuf busy size */
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  dsl.timeout.tv_sec=1;
  dsl.timeout.tv_nsec=0;
  rv=pthread_mutex_timedlock(&dsl.mutex,&dsl.timeout);
  if(rv)
  {
    mPrintf(errU,DEBUG,__func__,0,"file: \"%s\", line %d: "
            "pthread_mutex_timedlock(): Error %d: %s!",__FILE__,__LINE__,errno,
            strerror(errno));
    mPrintf(errU,DEBUG,__func__,0,"Skip SVC update for tid: %u!",(pid_t)*tag);
    return;
  }
  /*-------------------------------------------------------------------------*/
  if(unlikely(deBug & 0x0400))
  {
    if(pthread_equal(pthread_self(),main_ptid))
    {
      mPrintf(errU,DEBUG,__func__,0,"Executing for task tid=%lu in MAIN "
              "THREAD (MONITORED update, requested by DIM server).",*tag);
    }
    else
    {
      mPrintf(errU,DEBUG,__func__,0,"Executing for task tid=%lu in I/O "
              "THREAD (TIMED update, requested by DIM client).",*tag);
    }
  }
  /*-------------------------------------------------------------------------*/
  for(ds=dsl.head;ds;ds=ds->next)              /* loop over dynamic SVC list */
  {
    if(ds->tid==*tag)
    {
      /* evaluate dataBuf size */
      dataBufBS=3*sizeof(int32_t)+11*sizeof(uint32_t)+3*sizeof(int32_t)+
                4*sizeof(float)+UPDATETIME_LEN+1+CMD_LEN+1+
                UTGID_LEN+1+USER_LEN+1+strlen(ds->cmdline)+1;
      /* allocate memory for dataBuf */
      if(dataBufS<dataBufBS)
      {
        dataBufS=dataBufBS;
        dataBuf=(char*)realloc(dataBuf,dataBufS);
      }
      memset(dataBuf,0,dataBufS);
      /* fill dataBuf mixed array */
      dataBufBS=0;
      memcpy(dataBuf+dataBufBS,&ds->updated,sizeof(int32_t));
      dataBufBS+=sizeof(int32_t);
      memcpy(dataBuf+dataBufBS,&ds->status,sizeof(int32_t));
      dataBufBS+=sizeof(int32_t);
      memcpy(dataBuf+dataBufBS,&ds->hStatus,sizeof(int32_t));
      dataBufBS+=sizeof(int32_t);
      memcpy(dataBuf+dataBufBS,&ds->tid,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->nlwp,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->processor,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->vm_size,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->vm_lock,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->vm_rss,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->vm_data,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->vm_stack,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->vm_exe,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->vm_lib,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->share,sizeof(uint32_t));
      dataBufBS+=sizeof(uint32_t);
      memcpy(dataBuf+dataBufBS,&ds->rtprio,sizeof(int32_t));
      dataBufBS+=sizeof(int32_t);
      memcpy(dataBuf+dataBufBS,&ds->nice,sizeof(int32_t));
      dataBufBS+=sizeof(int32_t);
      memcpy(dataBuf+dataBufBS,&ds->prio,sizeof(int32_t));
      dataBufBS+=sizeof(int32_t);
      memcpy(dataBuf+dataBufBS,&ds->pcpu,sizeof(float));
      dataBufBS+=sizeof(float);
      memcpy(dataBuf+dataBufBS,&ds->pmem,sizeof(float));
      dataBufBS+=sizeof(float);
      memcpy(dataBuf+dataBufBS,&ds->minFltRate,sizeof(float));
      dataBufBS+=sizeof(float);
      memcpy(dataBuf+dataBufBS,&ds->majFltRate,sizeof(float));
      dataBufBS+=sizeof(float);
      snprintf(dataBuf+dataBufBS,UPDATETIME_LEN+1,"%s",ds->lastUpdateTimeStr);
      dataBufBS+=UPDATETIME_LEN+1;
      snprintf(dataBuf+dataBufBS,CMD_LEN+1,"%s",ds->cmd);
      dataBufBS+=CMD_LEN+1;
      snprintf(dataBuf+dataBufBS,UTGID_LEN+1,"%s",ds->utgid);
      dataBufBS+=UTGID_LEN+1;
      snprintf(dataBuf+dataBufBS,USER_LEN+1,"%s",ds->euser);
      dataBufBS+=USER_LEN+1;
      snprintf(dataBuf+dataBufBS,1+strlen(ds->cmdline),"%s",ds->cmdline);
      dataBufBS+=1+strlen(ds->cmdline);
    }
  }                                            /* loop over dynamic SVC list */
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_unlock(&dsl.mutex))eExit("pthread_mutex_unlock()");
  /*-------------------------------------------------------------------------*/
  *address=(int*)dataBuf;
  *size=dataBufBS;
  printOutFuncOK;
  return;
}
/* ######################################################################### */
/* DIM SVC client handler */
/* Executed when the Task Manager updates the process list */
void *tmLlUpdateHandler(long *tag,int *longList,int *longListSz)
{
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  dataReceived=1;
  if(*(char*)longList==-1)        /* Task Manager longList SVC NOT available */
  {
    /*.......................................................................*/
    mPrintf(errU,INFO,__func__,0,"Task Manager SVC \"%s\" not available. "
            "Unable to retirieve UTGIDs of processes with a huge (>4095 B) "
            "environment using SLC kernel < 2.6.9-78.",tmLlSvcPath);
    tmLlBufBSz=0;
    /*.......................................................................*/
  }
  else                                /* Task Manager longList SVC available */
  {
    /*.......................................................................*/
    if(pthread_mutex_lock(&tmLlLock))eExit("pthread_mutex_lock()");
    tmLlBufBSz=*longListSz;
    if(tmLlBufSz<tmLlBufBSz)
    {
      int oldTmLlBufSz;
      oldTmLlBufSz=tmLlBufSz;
      tmLlBufSz=tmLlBufBSz*2;
      tmLlBuf=(char*)realloc(tmLlBuf,tmLlBufSz);
      if(deBug & 0x0100)
      {
        mPrintf(errU|L_STD|L_SYS,VERB,__func__,9, "longList buffer %d B, "
                "required: %d B, Reallocate buffer to %d B.",
                oldTmLlBufSz,tmLlBufBSz,tmLlBufSz);
      }
    }                                           /* if(tmLlBufSz<*longListSz) */
    memset(tmLlBuf,0,tmLlBufSz);
    memcpy(tmLlBuf,(char*)longList,tmLlBufBSz);
    /*.......................................................................*/
    if(pthread_mutex_unlock(&tmLlLock))eExit("pthread_mutex_unlock()");
    /* require service update in next cycle */
    if(unlikely(deBug & 0x0200))
    {
      mPrintf(errU,VERB,__func__,0,"Forcing a SVC update...");
    }
    if(main_tid)kill(main_tid,SIGHUP);
    /*.......................................................................*/
  }                                   /* Task Manager longList SVC available */
  /*-------------------------------------------------------------------------*/
  printOutFuncOK;
  return NULL;
} 
/* ######################################################################### */
/* Utilities */
/*****************************************************************************/
char *tmLlGetUtgid(pid_t tgid)
{
  char *tmLlBufP;
  int tmTgid;
  char *tmCmd,*tmUtgid,*tmLastUpdate,*tmStatus;
  static char utgid[UTGID_LEN+1];
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  *utgid='\0';
  if(!tmLlBufSz)
  {
    return utgid;
    printOutFuncNOK;
  }
  if(pthread_mutex_lock(&tmLlLock))eExit("pthread_mutex_lock()");
  for(tmLlBufP=tmLlBuf;;)                        /* loop over tmLlBuf fields */
  {
    tmTgid=(int)strtol(tmLlBufP,(char**)NULL,0);
    tmLlBufP=1+strchr(tmLlBufP,'\0');
    tmCmd=tmLlBufP;
    tmLlBufP=1+strchr(tmLlBufP,'\0');
    tmUtgid=tmLlBufP;
    tmLlBufP=1+strchr(tmLlBufP,'\0');
    tmLastUpdate=tmLlBufP;
    tmLlBufP=1+strchr(tmLlBufP,'\0');
    tmStatus=tmLlBufP;
    tmLlBufP=1+strchr(tmLlBufP,'\0');
    if(tmTgid==tgid)
    {
      snprintf(utgid,UTGID_LEN+1,"%s",tmUtgid);
      break;
    }
    if(tmLlBufP>=tmLlBuf+tmLlBufBSz)break;
  }               
  if(pthread_mutex_unlock(&tmLlLock))eExit("pthread_mutex_unlock()");
  /*-------------------------------------------------------------------------*/
  printOutFuncOK;
  return utgid;
}
/*****************************************************************************/
/* return 0 if equal, 1 if different */
int fltrCmp(tkFltr_t *f1, tkFltr_t *f2)
{
  int i;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  /* different TID */
  if(f1->tid!=f2->tid)
  {
    printOutFuncOK;
    return 1;
  }
  /* UTGID defined in only one of the two filters */
  if((f1->utgid && !f2->utgid) || (f2->utgid && !f1->utgid))
  {
    printOutFuncOK;
    return 1;
  }
  /* different UTGIDs */
  if(f1->utgid && f2->utgid && strcmp(f1->utgid,f2->utgid))
  {
    printOutFuncOK;
    return 1;
  }
  /* CMD defined in only one of the two filters */
  if((f1->cmd && !f2->cmd) || (f2->cmd && !f1->cmd))
  {
    printOutFuncOK;
    return 1;
  }
  /* different CMDs */
  if(f1->cmd && f2->cmd && strcmp(f1->cmd,f2->cmd))
  {
    printOutFuncOK;
    return 1;
  }
  for(i=0;i<10;i++)
  {
    /* CMDLINE/ARG defined in only one of the two filters */
    if((f1->args[i] && !f2->args[i]) || (f2->args[i] && !f1->args[i]))
    {
      printOutFuncOK;
      return 1;
    }
    /* different CMDLINE/ARG */
    if(f1->args[i] && f2->args[i] && strcmp(f1->args[i],f2->args[i]))
    {
      printOutFuncOK;
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  printOutFuncOK;
  return 0;
}
/*****************************************************************************/
/* return 0 if matches, 1 if doesnt match */
int tkMatch(tkFltr_t *fltr,taskData_t *prcsData)
{
  int i;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  /* compare TIDs */
  if(fltr->tid)
  {
    if(fltr->tid!=prcsData->tid)
    {
      printOutFuncOK;
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* compare UTGIDs */
  if(fltr->utgid)
  {
    if(fnmatch(fltr->utgid,prcsData->utgid,0))
    {
      printOutFuncOK;
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* compare CMDs */
  if(fltr->cmd)
  {
    if(fnmatch(fltr->cmd,prcsData->cmd,0))
    {
      printOutFuncOK;
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* compare CMDLINEs */
  if(fltr->args[0])
  {
    char *cmdline=NULL;
    size_t cmdlineLen=0;
    /* ..................................................................... */
    cmdlineLen=strcspn(prcsData->cmdline," \t");
    cmdline=strndupa(prcsData->cmdline,cmdlineLen);
      /* ................................................................... */
    if(fnmatch(fltr->args[0],cmdline,0))
    {
      printOutFuncOK;
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* compare ARGs */
  for(i=1;i<10;i++)                           /* loop over cmdline arguments */
  {
    if(fltr->args[i])
    {
      int j,l;
      char *arg=NULL;
      size_t argLen=0;
      char *p;
      /* ................................................................... */
      p=prcsData->cmdline;
      for(j=0;j<i;j++)
      {
        l=strcspn(p," \t");
        if(l==strlen(p))                             /* nothing after string */
        {
          printOutFuncOK;
          return 1;
        }
        p+=l;                                                 /* skip string */
        l=strspn(p," \t");
        if(l==strlen(p))                             /* nothing after blanks */
        {
          printOutFuncOK;
          return 1;
        }
        p+=l;                                                 /* skip blanks */
      }
      argLen=strcspn(p," \t");
      arg=strndupa(p,argLen);
      /* ................................................................... */
      if(fnmatch(fltr->args[i],arg,0))
      {
        printOutFuncOK;
        return 1;
      }
    }
  }                                           /* loop over cmdline arguments */
  /*-------------------------------------------------------------------------*/
  printOutFuncOK;
  return 0;
}
/*****************************************************************************/
int tkMatch2(tkFltr_t *fltr,dynSvcListEl_t *ds)
{
  int i;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  /* compare TIDs */
  if(fltr->tid)
  {
    if(fltr->tid!=ds->tid)
    {
      printOutFuncOK;
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* compare UTGIDs */
  if(fltr->utgid)
  {
    if(fnmatch(fltr->utgid,ds->utgid,0))
    {
      printOutFuncOK;
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* compare CMDs */
  if(fltr->cmd)
  {
    if(fnmatch(fltr->cmd,ds->cmd,0))
    {
      printOutFuncOK;
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* compare CMDLINEs */
  if(fltr->args[0])
  {
    char *cmdline=NULL;
    size_t cmdlineLen=0;
    /* ..................................................................... */
    cmdlineLen=strcspn(ds->cmdline," \t");
    cmdline=strndupa(ds->cmdline,cmdlineLen);
      /* ................................................................... */
    if(fnmatch(fltr->args[0],cmdline,0))
    {
      printOutFuncOK;
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* compare ARGs */
  for(i=1;i<10;i++)                           /* loop over cmdline arguments */
  {
    if(fltr->args[i])
    {
      int j,l;
      char *arg=NULL;
      size_t argLen=0;
      char *p;
      /* ................................................................... */
      p=ds->cmdline;
      for(j=0;j<i;j++)
      {
        l=strcspn(p," \t");
        if(l==strlen(p))                             /* nothing after string */
        {
          printOutFuncOK;
          return 1;
        }
        p+=l;                                                 /* skip string */
        l=strspn(p," \t");
        if(l==strlen(p))                             /* nothing after blanks */
        {
          printOutFuncOK;
          return 1;
        }
        p+=l;                                                 /* skip blanks */
      }
      argLen=strcspn(p," \t");
      arg=strndupa(p,argLen);
      /* ................................................................... */
      if(fnmatch(fltr->args[i],arg,0))
      {
        printOutFuncOK;
        return 1;
      }
    }
  }                                           /* loop over cmdline arguments */
  /*-------------------------------------------------------------------------*/
  printOutFuncOK;
  return 0;
}
/*****************************************************************************/
char *filter2str(tkFltr_t *fltr)
{
  int j;
  char *msg=NULL;
  int msgL=0;
  char *p;
  int isFirst=1;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  msgL=snprintf(NULL,0,"(");
  msg=(char*)realloc(msg,msgL+1);
  sprintf(msg,"(");
  /*-------------------------------------------------------------------------*/
  if(fltr->tid)
  {
    msgL+=snprintf(NULL,0,"TID=%d",fltr->tid);
    msg=(char*)realloc(msg,msgL+1);
    p=strchr(msg,'\0');
    sprintf(p,"TID=%d",fltr->tid);
    isFirst=0;
  }
  /*-------------------------------------------------------------------------*/
  if(fltr->cmd)
  {
    if(!isFirst)
    {
      msgL+=snprintf(NULL,0," && ");
      msg=(char*)realloc(msg,msgL+1);
      p=strchr(msg,'\0');
      sprintf(p," && ");
    }
    msgL+=snprintf(NULL,0,"CMD=\"%s\"",fltr->cmd);
    msg=(char*)realloc(msg,msgL+1);
    p=strchr(msg,'\0');
    sprintf(p,"CMD=\"%s\"",fltr->cmd);
    isFirst=0;
  }
  /*-------------------------------------------------------------------------*/
  if(fltr->utgid)
  {
    if(!isFirst)
    {
      msgL+=snprintf(NULL,0," && ");
      msg=(char*)realloc(msg,msgL+1);
      p=strchr(msg,'\0');
      sprintf(p," && ");
    }
    msgL+=snprintf(NULL,0,"UTGID=\"%s\"",fltr->utgid);
    msg=(char*)realloc(msg,msgL+1);
    p=strchr(msg,'\0');
    sprintf(p,"UTGID=\"%s\"",fltr->utgid);
    isFirst=0;
  }
  /*-------------------------------------------------------------------------*/
  if(fltr->args[0])
  {
    if(!isFirst)
    {
      msgL+=snprintf(NULL,0," && ");
      msg=(char*)realloc(msg,msgL+1);
      p=strchr(msg,'\0');
      sprintf(p," && ");
    }
    msgL+=snprintf(NULL,0,"CMDLINE=\"%s\"",fltr->args[0]);
    msg=(char*)realloc(msg,msgL+1);
    p=strchr(msg,'\0');
    sprintf(p,"CMDLINE=\"%s\"",fltr->args[0]);
    isFirst=0;
  }
  /*-------------------------------------------------------------------------*/
  for(j=1;j<10;j++)                                   /* loop over arguments */
  {
    if(fltr->args[j])
    {
      if(!isFirst)
      {
        msgL+=snprintf(NULL,0," && ");
        msg=(char*)realloc(msg,msgL+1);
        p=strchr(msg,'\0');
        sprintf(p," && ");
      }
      msgL+=snprintf(NULL,0,"ARGV[%d]=\"%s\"",j,fltr->args[j]);
      msg=(char*)realloc(msg,msgL+1);
      p=strchr(msg,'\0');
      sprintf(p,"ARGV[%d]=\"%s\"",j,fltr->args[j]);
      isFirst=0;
    }
  }                                                   /* loop over arguments */
  /*-------------------------------------------------------------------------*/
  msgL+=snprintf(NULL,0,")");
  msg=(char*)realloc(msg,msgL+1);
  p=strchr(msg,'\0');
  sprintf(p,")");
  /*-------------------------------------------------------------------------*/
  printOutFuncOK;
  return msg;
}
/*****************************************************************************/
void printFl()
{
  int rv=0;
  tkFltr_t *fltr=NULL;
  char *msg=NULL;
  int msgL=0;
  char *p;
  int i;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  msgL=snprintf(NULL,0,"Task selection expression: ");
  msg=(char*)realloc(msg,msgL+1);
  sprintf(msg,"Task selection expression: ");
  /*-------------------------------------------------------------------------*/
  fl.timeout.tv_sec=1;
  fl.timeout.tv_nsec=0;
  rv=pthread_mutex_timedlock(&fl.mutex,&fl.timeout);
  if(rv)
  {
    mPrintf(errU,DEBUG,__func__,0,"file: \"%s\", line %d: "
            "pthread_mutex_timedlock(): Error %d: %s!",__FILE__,__LINE__,errno,
            strerror(errno));
    mPrintf(errU,DEBUG,__func__,0,"Skip task filter list!");
    if(msg)free(msg);
    return;
  }
  /*-------------------------------------------------------------------------*/
  for(fltr=fl.head,i=0;fltr;fltr=fltr->next,i++)        /* loop over filters */
  {
    char *fltrS=NULL;
    /* ..................................................................... */
    if(i>0)
    {
      msgL+=snprintf(NULL,0," || ");
      msg=(char*)realloc(msg,msgL+1);
      p=strchr(msg,'\0');
      sprintf(p," || ");
    }
    /* ..................................................................... */
    fltrS=filter2str(fltr);
    msgL+=snprintf(NULL,0,"%s",fltrS);
    msg=(char*)realloc(msg,msgL+1);
    p=strchr(msg,'\0');
    sprintf(p,"%s",fltrS);
    if(fltrS)
    {
      free(fltrS);
      fltrS=NULL;
    }
    /* ..................................................................... */
  }                                                     /* loop over filters */
  if(pthread_mutex_unlock(&fl.mutex))eExit("pthread_mutex_unlock()");
  /*-------------------------------------------------------------------------*/
  if(!i)
  {
    msgL+=snprintf(NULL,0,"0 [no task selected]");
    msg=(char*)realloc(msg,msgL+1);
    p=strchr(msg,'\0');
    sprintf(p,"0 [no task selected]");
  }
  msgL+=snprintf(NULL,0,".");
  msg=(char*)realloc(msg,msgL+1);
  p=strchr(msg,'\0');
  sprintf(p,".");
  mPrintf(errU,DEBUG,__func__,0,"%s",msg);
  if(msg)free(msg);
  /*-------------------------------------------------------------------------*/
  printOutFuncOK;
  return;
}
/* ######################################################################### */
/* DIM Error Handlers */
/*****************************************************************************/
void disErrorHandler(int severity,int errorCode,char *message)
{
  printInFunc;
  mPrintf(errU|L_SYS,severity+3,__func__,0,"%s! (error code = %d)",
          message,errorCode);
  if(severity==3)_exit(1);
  printOutFuncOK;
  return;
}
/*****************************************************************************/
void dicErrorHandler(int severity,int errorCode,char *message)
{
  printInFunc;
  mPrintf(errU|L_SYS,severity+3,__func__,0,"%s! (error code = %d)",
          message,errorCode);
  if(severity==3)abort();
  printOutFuncOK;
  return;
}
/* ######################################################################### */
/* Signal Handlers */
/*****************************************************************************/
/* signalHandler() executed synchronously in the main thread. So don't warry */
/* about async-signal-safe.                                                  */
static void signalHandler(int signo)
{
  int i;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  if(signo==SIGHUP)
  {
    if(unlikely(deBug & 0x0200))
    {
      mPrintf(errU|L_SYS,VERB,__func__,0,"Signal %d (%s) caught. "
          "Updating...",signo,sig2msg(signo));
    }
    return;
    printOutFuncOK;
  }
  for(i=0;i<svcNum;i++)dis_remove_service(svcId[i]);
  dis_stop_serving();
  dic_release_service(tmLlSvcID);
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
  printOutFuncOK;
  return;
}
/* ######################################################################### */
void shortUsage(void)
{
  char *shortUsageStr=
"SYNOPSIS\n"
"psSrv [-d] [-N DIM_DNS_NODE] [-l LOGGER] [-u REFRESH_TIME_INTERVAL]\n"
"      [-p PERSISTENCE] [-C CMD_PATTERN] [-U UTGID_PATTERN]\n"
"psSrv -h\n"
"Try \"psSrv -h\" for more information.\n";
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
"psSrv.man\n"
"\n"
"..\n"
"%s"
".hw sensor\\[hy]dictionary\\[hy]file  max\\[hy]threads\\[hy]number \n"
".TH psSrv 8  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC Servers\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis psSrv\\ \\-\n"
"FMC Process Monitor Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis psSrv\n"
".ShortOpt[] d\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] l LOGGER\n"
".ShortOpt[] u REFRESH_TIME_INTERVAL\n"
".ShortOpt[] p PERSISTENCE\n"
".ShortOpt[] C CMD_PATTERN\n"
".ShortOpt[] U UTGID_PATTERN\n"
".EndSynopsis\n"
".Synopsis psSrv\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"The FMC Process Monitor Server, \\fBpsSrv\\fP(8) retrieves information "
"concerning the processes and tasks (lighweight processes) running on the "
"current farm node and publishes them using DIM.\n"
".PP\n"
"The DIM Name Server, looked up by \\fBpsSrv\\fP to register the "
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
"Set verbosity to maximum for debugging. More detailed control on diagnostic "
"verbosity can be achieved by means of the \\fIdeBug\\fP environment variable "
"(see below). Option \\fB-d\\fP is equivalent to \\fIdeBug=0x7ff\\fP.\n"
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
".OptDef u \"\" REFRESH_TIME_INTERVAL (integer)\n"
"Set the data refresh period to \\fIREFRESH_TIME_INTERVAL\\fP seconds. (must "
"be in the range [0,60]s). \\fBDefault\\fP: 20 s.\n"
".\n"
".OptDef p \"\" PERSISTENCE (integer)\n"
"Keep alive the on-demand process monitor DIM services (see below) referring "
"to terminated processes for \\fIPERSISTENCE\\fP service updates after "
"process termination. \\fBDefault\\fP: \\fIPERSISTENCE\\fP = 6.\n"
".\n"
".OptDef C \"\" CMD_PATTERN (string)\n"
"Publish only processes whose command (executable image name), cut at 15 "
"characters, matches the wildcard pattern \\fICMD_PATTERN\\fP.\n"
".\n"
".OptDef U \"\" UTGID_PATTERN (string)\n"
"Publish only processes which have the UTGID set and whose UTGID matches the "
"wildcard pattern \\fIUTGID_PATTERN\\fP.\n"
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
"UTGID of the \\fBpsSrv\\fP(8) process.\n"
".\n"
".TP\n"
".EnvVar deBug \\ (integer,\\ optional)\n"
"Debug mask. Can be set to 0x000...0x7ff. The meaning of the mask bit are:\n"
".PP\n"
".RS\n"
"0x001 print in/out functions;\n"
".br\n"
"0x002 print file2strvec()/file2str() diagnostics;\n"
".br\n"
"0x004 print getTasks() diagnosttics;\n"
".br\n"
"0x008 print incoherence in read data;\n"
".br\n"
"0x010 print print process and task numbers;\n"
".br\n"
"0x020 print process data from sensor;\n"
".br\n"
"0x040 print task data from sensor;\n"
".br\n"
"0x080 print huge environment management;\n"
".br\n"
"0x100 print data buffer allocation operations;\n"
".br\n"
"0x200 print timed/forced updates;\n"
".br\n"
"0x400 print dynamical SVC creation/remotion.\n"
".PP\n"
"\\fBDefault\\fP: \\fIdeBug\\fP = 0.\n"
".RE\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Start the FMC Process Monitor Server, sending diagnostic messages to the "
"default FMC Message Logger:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/psSrv\n"
".PP\n"
"Start the FMC Process Monitor Server, sending diagnostic messages to the "
"default FMC Message Logger and to the system syslog facility:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/psSrv -l 0x5\n"
".PP\n"
"Set the delay time interval between two subsequent refresh to four seconds:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/psSrv -u 4\n"
".PP\n"
"Publish only processes/tasks having the executable image name terminating "
"with \"Srv\":\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/psSrv -C \\[rs]*Srv\n"
".PP\n"
"Publish only processes/tasks having the UTGID \"counter_7\" or \"counter_8\""
":\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/psSrv -U \\[dq]counter_[78]\\[dq]\n"
".PP\n"
"Keep active the dynamical DIM service related to a certain monitored process "
"until the 10th refresh after the process termination:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/psSrv -p 10\n"
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
"the \\fBpsSrv.C\\fP source code of\n"
"the current \\fBpsSrv\\fP(8) instance, as returned by the command\n"
"\"ident /opt/FMC/sbin/psSrv\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/sensor_version\n"
"\\fBOutput format\\fP: \"C\" (NULL-terminated string).\n"
".br\n"
"Publishes a NULL-terminated string containing the RCS revision label of\n"
"the \\fBgetPsFromProc.c\\fP source code used in\n"
"the current \\fBpsSrv\\fP(8) instance, as returned by the command\n"
"\"ident /opt/FMC/lib/libFMCsensors*\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/fmc_version\n"
"\\fBOutput format\\fP: \"C\" (NULL-terminated string).\n"
".br\n"
"Publishes a NULL-terminated string containing the revision label of the FMC\n"
"package which includes the current \\fBpsSrv\\fP(8) executable, in the\n"
"present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/success\n"
"\\fBOutput format\\fP: \"I\" (integer).\n"
".br\n"
"Always publishes the integer 1. This dummy services can be used to check\n"
"whether the current instance of \\fBpsSrv\\fP is up and running.\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/updates\n"
"\\fBOutput format\\fP: \"I\" (integer).\n"
".br\n"
"Publishes an integer which is the number of refreshes of the "
"\\fBpsSrv\\fP(8) server. Useful to understand if the server is running and "
"refreshing data properly.\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/nprocs\n"
"\\fBOutput format\\fP: \"I\" (integer).\n"
".br\n"
"Publishes an integer which is the number of the processes in the last server "
"refresh (a multi-threaded processes contributes to this number with one "
"unit).\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/ntasks\n"
"\\fBOutput format\\fP: \"I\" (integer).\n"
".br\n"
"Publishes an integer which is the number of the tasks (light-weight "
"processes) in the last server refresh (a multi-threaded processes "
"contributes to this number with the number of its threads).\n"
".PP\n"
"\\fBSummary SVCs\\fP\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/labels\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 11 NULL-terminated strings).\n"
".br\n"
"Publishes the labels referring to the data published in the SVC "
"\".../summary/processes/data\":\n"
".br\n"
"\"TGID\" \"UTGID\" \"USER\" \"VSIZE\" \"RSS\" \"STAT\" \"%%CPU\" \"%%MEM\" "
"\"STARTED\" \"CMD\" \"CMDLINE\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/units\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 11 NULL-terminated strings).\n"
".br\n"
"Publishes the units referring to the data published in the SVC "
"\".../summary/processes/data\":\n"
".br\n"
"\"\" \"\" \"\" \"KiB\" \"KiB\" \"\" \"%%\" \"%%\" "
"\"aaa mmm dd HH:MM:SS YYYY\" \"\" \"\".\n"
".br\n"
"where \\fIaaa\\fP is the abbreviated weekday, \\fImmm\\fP is the month as a "
"decimal number [01,12], \\fIdd\\fP is the day of the month as a decimal "
"number [01,31], \\fIHH:MM:SS\\fP is the time and \\fIYYYY\\fP is the year as "
"a decimal number.\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/processes/data\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 11 * nprocs NULL-terminated"
"strings).\n"
".br\n"
"Publishes the summary data, for each process that was running at the time of "
"the last server refresh. The first 11 strings refer to the process with the "
"lowest TGID, the following 11 string refer to the next process in TGID "
"order, and so on. The string sequence, for each process, is the following:\n"
".RS\n"
".TP\n"
"\\fBTGID\\fP (alias PID)\n"
"Thread Group Identifier (the former PID, Process Identifier). The term "
"\\fBprocess\\fP is synonymous of \\fBthread group\\fP.\n"
".TP\n"
"\\fBUTGID\\fP\n"
"FMC User-assigned unique Thread Group Identifier, if available (e.g. if the "
"process has been started by the FMC Task Manager or has the variable UTGID "
"defined in the process environment). Otherwise \"N/A\" is published.\n"
".TP\n"
"\\fBUSER\\fP (alias EUSER, UNAME)\n"
"\\fBEffective\\fP user name. Used for all the security checks. The only user "
"name of the process that normally has any effect.\n"
".TP\n"
"\\fBVSIZE\\fP (alias VSZ, VIRT)\n"
"The total size of the process's memory footprint. This includes the text "
"segment, stack, static variables, data segment, and pages which are shared "
"with other processes.\n"
".TP\n"
"\\fBRSS\\fP (alias RSZ, RES, RESIDENT)\n"
"The kernel's estimate of the resident set size for this process.\n"
".TP\n"
"\\fBSTAT\\fP\n"
"The multi-characters process state of the task. The meaning of the first "
"character is:\n"
".RS\n"
".TP\n"
"\\fBD\\fP\n"
"Uninterruptible sleep (usually I/O);\n"
".TP\n"
"\\fBR\\fP\n"
"Running or runnable (on run queue);\n"
".TP\n"
"\\fBS\\fP\n"
"Interruptible sleep (waiting for an event to complete);\n"
".TP\n"
"\\fBT\\fP\n"
"Stopped, either by a job control signal or because it is being traced;\n"
".TP\n"
"\\fBX\\fP\n"
"Dead (should never be seen);\n"
".TP\n"
"\\fBZ\\fP\n"
"Defunct (\"zombie\") process, terminated but not reaped by its parent.\n"
".RE\n"
".PP\n"
".RS\n"
"The meaning of the following characters is:\n"
".TP\n"
"\\fB<\\fP\n"
"Higher-priority (not nice to other users);\n"
".TP\n"
"\\fBN\\fP\n"
"Lower-priority (nice to other users);\n"
".TP\n"
"\\fBL\\fP\n"
"Has pages locked into memory (for real-time and custom I/O);\n"
".TP\n"
"\\fBs\\fP\n"
"Is a session leader;\n"
".TP\n"
"\\fBl\\fP\n"
"Is multi-threaded (using CLONE_THREAD, like NPTL pthreads does);\n"
".TP\n"
"\\fB+\\fP\n"
"Is in the foreground process group.\n"
".RE\n"
".TP\n"
"\\fB%%CPU\\fP (alias PCPU, CP, C)\n"
"The task's share of the CPU time since the last update, expressed as a "
"percentage of the total CPU time per processor. \\fBMaximum\\fP: 100%% x "
"CPU_core_number.\n"
".TP\n"
"\\fB%%MEM\\fP (alias PMEM)\n"
"The ratio of the process's resident set size to the physical memory on the "
"machine, expressed as a percentage. \\fBMaximum\\fP: 100%%.\n"
".TP\n"
"\\fBSTARTED\\fP (alias STARTTIME, START)\n"
"The process's start time.\n"
".TP\n"
"\\fBCMD\\fP (alias COMM, COMMAND, UCMD, UCOMM)\n"
"Command name, i.e. the basename (w/o path) of the executable image file, "
"without arguments, eventually truncated at 15 characters.\n"
".TP\n"
"\\fBCMDLINE\\fP (alias ARGS, CMD, COMMAND)\n"
"The complete command line with its arguments (up to PATH_MAX = 4096 "
"characters). This string may contain blank characters.\n"
".RE\n"
".PP\n"
"\\fBDetailed SVCs\\fP\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/labels\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 44 NULL-terminated strings).\n"
".br\n"
"Publishes the labels referring to the data published in the SVC "
"\".../details/processes/data\" and \".../details/tasks/data\":\n"
".br\n"
"\"CMD\" \"UTGID\" \"TID\" \"USER\" \"TTY\" \"NLWP\" "
"\"TGID\" \"PPID\" \"PGID\" \"SID\" \"TPGID\" "
"\"FUSER\" \"RUSER\" \"SUSER\" \"GROUP\" \"FGROUP\" \"RGROUP\" \"SGROUP\" "
"\"SCH\" \"RTPRIO\" \"NICE\" \"PRIO\" "
"\"%%CPU\" \"%%MEM\" \"MIN_FLT\" \"MAJ_FLT\" \"STAT\" \"PSR\" "
"\"VSIZE\" \"LOCK\" \"RSS\" \"DATA\" \"STACK\" \"EXE\" \"LIB\" \"SHARE\" "
"\"IGNORED\" \"PENDING\" \"CATCHED\" \"BLOCKED\" "
"\"STARTED\" \"ELAPSED\" \"CPUTIME\" "
"\"CMDLINE\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/units\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 44 NULL-terminated strings).\n"
".br\n"
"Publishes the units referring to the data published in the SVC "
"\".../details/processes/data\" and \".../details/tasks/data\":\n"
".br\n"
"\"\" \"\" \"\" \"\" \"\" \"\" "
"\"\" \"\" \"\" \"\" \"\" "
"\"\" \"\" \"\" \"\" "
"\"\" \"\" \"\" \"\" \"\" \"\" \"\" "
"\"\" \"\" \"\" \"\" "
"\"%%\" \"%%\" \"Hz\" \"Hz\" \"\" \"\" "
"\"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" "
"\"\" \"\" \"\" \"\" "
"\"aaa mmm dd HH:MM:SS YYYY\" \"ddd-HH:MM:SS\" \"ddd-HH:MM:SS\" "
"\"\".\n"
".br\n"
"where \\fIaaa\\fP is the abbreviated weekday, \\fImmm\\fP is the month as a "
"decimal number [01,12], \\fIdd\\fP is the day of the month as a decimal "
"number [01,31], \\fIddd\\fP is the number of the past days, \\fIHH:MM:SS\\fP "
"is the time and \\fIYYYY\\fP is the year as a decimal number.\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/types\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 44 NULL-terminated strings).\n"
".br\n"
"Publishes the types of data (used from the clients to select information "
"from specific categories) referring to the data published in the SVC "
"\".../details/processes/data\" and \".../details/tasks/data\":\n"
".br\n"
"\"basic\" \"basic\" \"basic\" \"basic\" \"basic\" \"basic\" "
"\"ids\" \"ids\" \"ids\" \"ids\" \"ids\" "
"\"owner\" \"owner\" \"owner\" \"owner\" \"owner\" \"owner\" \"owner\" "
"\"sched\" \"sched\" \"sched\" \"sched\" "
"\"stat\" \"stat\" \"stat\" \"stat\" \"stat\" \"stat\" "
"\"size\" \"size\" \"size\" \"size\" \"size\" \"size\" \"size\" \"size\" "
"\"signal\" \"signal\" \"signal\" \"signal\" "
"\"time\" \"time\" \"time\" "
"\"cmdline\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/processes/data\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/tasks/data\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 44 * nprocs NULL-terminated"
"strings).\n"
".br\n"
"Publishes the detailed data, for each \\fBprocess\\fP (\\fBtask\\fP) that "
"was running at the time of the last server refresh. The first 44 strings "
"refer to the process with the lowest TGID, the following 44 string refer to "
"the next process in TGID order, and so on. The string sequence, for each "
"process, is the following:\n"
".RS\n"
".TP\n"
"\\fBCMD\\fP (alias COMM, COMMAND, UCMD, UCOMM)\n"
"Command name, i.e. the basename (w/o path) of the executable image file, "
"without arguments, eventually truncated at 15 characters.\n"
".TP\n"
"\\fBUTGID\\fP\n"
"FMC User-assigned unique Thread Group Identifier, if available (e.g. if the "
"process has been started by the FMC Task Manager or has the variable UTGID "
"defined in the process environment). Otherwise \"N/A\" is published.\n"
".TP\n"
"\\fBTID\\fP (alias LWP, SPID)\n"
"Thread Identifier, aka Light Weight Process Identifier. The \\fBTID\\fP of "
"the thread group leader is equal to the \\fBTGID\\fP (Thread Group "
"Identifier, the old \\fBPID\\fP). In \".../processes/...\" SVC (as opposite "
"to \".../tasks/...\" SVC) \\fBTID\\fP always coincide with \\fBTGID\\fP "
"(\\fBPID\\fP).\n"
".TP\n"
"\\fBUSER\\fP (alias EUSER, UNAME)\n"
"\\fBEffective\\fP user name. Used for all the security checks. The only user "
"name of the process that normally has any effect.\n"
".TP\n"
"\\fBTTY\\fP (alias TT, TNAME)\n"
"The name of the controlling tty (terminal) if available, a question mark "
"(\\fB?\\fP) otherwise.\n"
".TP\n"
"\\fBNLWP\\fP (alias THCNT)\n"
"Number of Lightweight Processes (Threads) in the process (thread group).\n"
".TP\n"
"\\fBTGID\\fP (alias PID)\n"
"Thread Group Identifier (the former PID, Process Identifier). The term "
"\\fBprocess\\fP is synonymous of \\fBthread group\\fP.\n"
".TP\n"
"\\fBPPID\\fP\n"
"Parent Process Identifier, i.e. the Thread Group Identifier of the parent "
"process.\n"
".TP\n"
"\\fBPGID\\fP (alias PGRP)\n"
"Process Group Identifierr, i.e. the Thread Group Identifier of the process "
"group leader. Every process is member of a unique process group. The "
"processes of one pipeline form a single process group. Among the process "
"groups in a session, at most one can be the \\fBforeground process group\\fP "
"of that session. The tty (teminal) input and tty signals (signals generated "
"by Ctrl-C, Ctrl-Z, etc.) go to processes in this foreground process group. "
"All process groups in a session that are not foreground process group are "
"\\fBbackground process groups\\fP.\n"
".TP\n"
"\\fBSID\\fP (alias SESS, SESSION)\n"
"Session Identifier, i.e. the Thread Group Identifier of the session leader. "
"Every process group is in a unique session. Every session may have a "
"controlling tty (terminal), that is also the controlling tty of each one of "
"its member processes. A session is often set up by a login process.\n"
".TP\n"
"\\fBTPGID\\fP\n"
"Terminal Process Group Identifier, i.e. the Process Group Identifier of the "
"process which currently owns the tty (terminal) that the process is "
"connected to, or -1 if the process is not connected to a tty.\n"
".TP\n"
"\\fBFUSER\\fP\n"
"\\fBFilesystem access\\fP user name. Used only in file-system access. Can be "
"set to any of the current effective, saved or real user names. Processes "
"with \"root\" as effective user name can set their filesystem access user "
"name to an arbitrary value. Used, e.g., by the Linux's user-space NFS "
"server.\n"
".TP\n"
"\\fBRUSER\\fP\n"
"\\fBReal\\fP user name. Checked only when a process attempts to change its "
"effective user name. Any process may change its effective user name to the "
"same value as either its saved or real user name. Only processes with "
"\"root\" as effective user name may change their effective user name to an "
"arbitrary value. Used, e.g., by \"ftpd\" and \"passwd\" programs.\n"
".TP\n"
"\\fBSUSER\\fP\n"
"\\fBSaved\\fP user name. Checked only when a process attempts to change its "
"effective user name. Any process may change its effective user name to the "
"same value as either its saved or real user name. Only processes with "
"\"root\" as effective user name may change their effective user name to an "
"arbitrary value. Used, e.g., by \"ftpd\" and \"passwd\" programs.\n"
".TP\n"
"\\fBGROUP\\fP (alias EGROUP)\n"
"\\fBEffective\\fP group name. Used for all the security checks. The only "
"group name of the process that normally has any effect.\n"
".TP\n"
"\\fBFGROUP\\fP\n"
"\\fBFilesystem access\\fP group name. Used only in file-system access. Can "
"be set to any of the current effective, saved or real group names. Processes "
"with \"root\" as effective group name can set their filesystem access group "
"name to an arbitrary value. Used, e.g., by the Linux's user-space NFS "
"server.\n"
".TP\n"
"\\fBRGROUP\\fP\n"
"\\fBReal\\fP group name. Checked only when a process attempts to change its "
"effective group name. Any process may change its effective group name to the "
"same value as either its saved or real group name. Only processes with "
"\"root\" as effective group name may change their effective group name to an "
"arbitrary value. Used, e.g., by \"ftpd\" and \"passwd\" programs.\n"
".TP\n"
"\\fBSGROUP\\fP\n"
"\\fBSaved\\fP group name. Checked only when a process attempts to change its "
"effective group name. Any process may change its effective group name to the "
"same value as either its saved or real group name. Only processes with "
"\"root\" as effective group name may change their effective group name to an "
"arbitrary value. Used, e.g., by \"ftpd\" and \"passwd\" programs.\n"
".TP\n"
"\\fBSCH\\fP (alias CLS, POL)\n"
"Scheduling class of the process. Possible values of this fields are:\n"
".RS\n"
".TP\n"
"\\fBTS\\fP\n"
"\\fISCHED_OTHER\\fP, the default Linux time-sharing scheduler, with a "
"dynamic priority based on the nice level.\n"
".TP\n"
"\\fBFF\\fP\n"
"\\fISCHED_FIFO\\fP, the static-priority Linux real-time FIFO scheduler, "
"without time slicing.\n"
".TP\n"
"\\fBRR\\fP\n"
"\\fISCHED_RR\\fP, the static-priority Linux real-time Round-Robin "
"scheduler.\n"
".TP\n"
"\\fBN/A\\fP\n"
"Not reported.\n"
".TP\n"
"\\fB?\\fP\n"
"Unknown value.\n"
".RE\n"
".TP\n"
"\\fBRTPRIO\\fP\n"
"Static (real-time) priority of the process. Only value 0 is allowed for TS "
"processes. For FF and RR real-time processes, allowed values are in the "
"range 1...99 (0 is the least favorable priority, 99 is the most favorable "
"priority).\n"
".TP\n"
"\\fBNICE\\fP (alias NI)\n"
"The nice level of the process, used by the TS Linux scheduler to compute the "
"dynamic priority. Allowed values for nice level are in the range -20...19 "
"(-20 corresponds to the most favorable scheduling; 19 corresponds to the "
"least favorable scheduling).\n"
".TP\n"
"\\fBPRIO\\fP (alias PRI, PR)\n"
"Kernel scheduling priority. Possible values are in the range -100...39 (-100 "
"is the most favorable priority, 39 is the least favorable priority). TS "
"processes have PRIO in the range 0...39, FF and RR processes have PRIO in "
"the range -100...-1.\n"
".TP\n"
"\\fB%%CPU\\fP (alias PCPU, CP, C)\n"
"The task's share of the CPU time since the last update, expressed as a "
"percentage of the total CPU time per processor. \\fBMaximum\\fP: 100%% x "
"CPU_core_number.\n"
".TP\n"
"\\fB%%MEM\\fP (alias PMEM)\n"
"The ratio of the process's resident set size to the physical memory on the "
"machine, expressed as a percentage. \\fBMaximum\\fP: 100%%.\n"
".TP\n"
"\\fBMIN_FLT\\fP\n"
"The rate (page faults per second) of page faults the process has made which "
"have not required loading a memory page from disk.\n"
".TP\n"
"\\fBMAJ_FLT\\fP\n"
"The rate (page faults per second) of page faults the process has made which "
"have required loading a memory page from disk.\n"
".TP\n"
"\\fBSTAT\\fP (alias S)\n"
"The multi-characters process state of the task. The meaning of the first "
"character is:\n"
".RS\n"
".TP\n"
"\\fBD\\fP\n"
"Uninterruptible sleep (usually I/O);\n"
".TP\n"
"\\fBR\\fP\n"
"Running or runnable (on run queue);\n"
".TP\n"
"\\fBS\\fP\n"
"Interruptible sleep (waiting for an event to complete);\n"
".TP\n"
"\\fBT\\fP\n"
"Stopped, either by a job control signal or because it is being traced;\n"
".TP\n"
"\\fBX\\fP\n"
"Dead (should never be seen);\n"
".TP\n"
"\\fBZ\\fP\n"
"Defunct (\"zombie\") process, terminated but not reaped by its parent.\n"
".RE\n"
".PP\n"
".RS\n"
"The meaning of the following characters is:\n"
".TP\n"
"\\fB<\\fP\n"
"Higher-priority (not nice to other users);\n"
".TP\n"
"\\fBN\\fP\n"
"Lower-priority (nice to other users);\n"
".TP\n"
"\\fBL\\fP\n"
"Has pages locked into memory (for real-time and custom I/O);\n"
".TP\n"
"\\fBs\\fP\n"
"Is a session leader;\n"
".TP\n"
"\\fBl\\fP\n"
"Is multi-threaded (using CLONE_THREAD, like NPTL pthreads does);\n"
".TP\n"
"\\fB+\\fP\n"
"Is in the foreground process group.\n"
".RE\n"
".TP\n"
"\\fBPSR\\fP (alias CPUID, P)\n"
"The processor that process is currently assigned to. It is useful to check "
"the operation of process-to-CPU affinity setting by the FMC Task Manager.\n"
".TP\n"
"\\fBVSIZE\\fP (alias VSZ, VIRT)\n"
"The total size of the process's memory footprint. This includes the text "
"segment, stack, static variables, data segment, and pages which are shared "
"with other processes.\n"
".TP\n"
"\\fBLOCK\\fP\n"
"The amount of the process's memory which is currently locked by the kernel. "
"Locked memory cannot be swapped out.\n"
".TP\n"
"\\fBRSS\\fP (alias RSZ, RES, RESIDENT)\n"
"The kernel's estimate of the resident set size for this process.\n"
".TP\n"
"\\fBDATA\\fP\n"
"The amount of memory used for data by the process. It includes static "
"variables and the data segment, but excludes the stack.\n"
".TP\n"
"\\fBSTACK\\fP\n"
"The amount of memory consumed by the process's stack.\n"
".TP\n"
"\\fBEXE\\fP\n"
"The size of the process's executable pages, excluding shared pages.\n"
".TP\n"
"\\fBLIB\\fP\n"
"The size of the shared memory pages mapped into the process's address space. "
"This excludes pages shared using System V style IPC.\n"
".TP\n"
"\\fBSHARE\\fP (alias SHRD, SHR)\n"
"The size of the pages of shared (mmap-ed) memory.\n"
".TP\n"
"\\fBIGNORED\\fP (alias SIGIGNORE)\n"
"The 64-bit mask of the ignored signals.\n"
".TP\n"
"\\fBPENDING\\fP (alias SIGPEND)\n"
"The 64-bit mask of the pending signals.\n"
".TP\n"
"\\fBCATCHED\\fP (alias SIGCATCH, CAUGHT)\n"
"The 64-bit mask of the caught signals.\n"
".TP\n"
"\\fBBLOCKED\\fP (alias SIGBLOCKED)\n"
"The 64-bit mask of the blocked signals.\n"
".TP\n"
"\\fBSTARTED\\fP (alias STARTTIME, START)\n"
"The process's start time.\n"
".TP\n"
"\\fBELAPSED\\fP (alias ELAPSEDTIME)\n"
"The time elapsed since the process was started.\n"
".TP\n"
"\\fBCPUTIME\\fP (alias TIME)\n"
"The process's cumulative CPU time.\n"
".TP\n"
"\\fBCMDLINE\\fP (alias ARGS, CMD, COMMAND)\n"
"The complete command line with its arguments (up to PATH_MAX = 4096 "
"characters). This string may contain blank characters.\n"
".RE\n"
".TP\n"
"\\fBSettings SVCs\\fP\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/monitored/properties\n"
"\\fBOutput format\\fP: \"C\" (one NULL-terminated string).\n"
".br\n"
"Publishes a string containing the \\fIfilter\\fP used to select the "
"processes to be monitored by means of a dedicated DIM SVC. In the string "
"\"\\fB||\\fP\" means logical \\fIOR\\fP and \"\\fB&&\\fP\" means logical "
"\\fIAND\\fP. The filter can be changed by means od the "
"\\fBstartMonitoring\\fP and \\fBstopMonitoring\\fP DIM CMDs (see below).\n"
".br\n"
"Example:\n"
".br\n"
"(CMD=\"httpd\") || (UTGID=\"counter_0\") || (CMD=\"*Srv\" && "
"UTGID=\"*Srv_u\") || (CMDLINE=\"/usr/bin/top\" && ARGV[1]=\"-d\" && "
"ARGV[2]=\"1\")\n"
".PP\n"
"\\fBOn-demand SVCs\\fP\n"
".PP\n"
"These services are added as soon as one or more processes, matching the "
"filter published in \".../monitored/properties\" (see above), are started "
"and removed after \\fIPERSISTENCE\\fP refreshes of the \\fBpsSrv\\fP(8) "
"server after the termination of such processes.  The filter can be changed "
"by means od the \\fBstartMonitoring\\fP and \\fBstopMonitoring\\fP DIM CMDs "
"(see below).\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/monitored/labels\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 11 NULL-terminated strings).\n"
".br\n"
"Publishes the labels referring to the summary data published in the SVC "
"\".../summary/monitored/processes/\\fICMD\\fP{\\fIUTGID\\fP}\\fITID\\fP/data"
"\":\n"
".br\n"
"\"UPDATED\" \"STATUS\" \"TID\" \"VSIZE\" \"RSS\" \"%%CPU\" \"%%MEM\" "
"\"CMD\" \"UTGID\" \"USER\" \"CMDLINE\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/monitored/units\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 11 NULL-terminated strings).\n"
".br\n"
"Publishes the units referring to the summary data published in the SVC "
"\".../summary/monitored/processes/\\fICMD\\fP{\\fIUTGID\\fP}\\fITID\\fP/data"
"\":\n"
".br\n"
"\"\" \"\" \"\" \"KiB\" \"KiB\" \"%%\" \"%%\" "
"\"\" \"\" \"\" \"\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/summary/monitored/processes/\\fICMD\\fP"
"{\\fIUTGID\\fP}\\fITID\\fP/data\n"
"\\fBOutput format\\fP: \"I:5;F:2;C:16;C:256;C:20;C\".\n"
".br\n"
"Publishes the summary data of the process having \\fICMD\\fP as command name "
"(basename, w/o path, of the executable image file, without arguments, "
"eventually truncated at 15 characters), \\fIUTGID\\fP as User-assigned "
"unique Thread Group Identifier and \\fITID\\fP as Thread/Thread-group "
"identifier. The data sequence is the following:\n"
".RS\n"
".TP\n"
"\\fBUPDATED\\fP [int32_t]\n"
"1 if service has been updated in the last \\fBpsSrv\\fP(8) server refresh, 0 "
"otherwise.\n"
".TP\n"
"\\fBSTATUS\\fP [int32_t]\n"
"1 if the process was still running in the last \\fBpsSrv\\fP(8) server "
"refresh, 0 otherwise.\n"
".TP\n"
"\\fBTID\\fP (alias LWP, SPID) [uint32_t]\n"
"Thread Identifier, aka Light Weight Process Identifier. In this service "
"(being a process SVC) \\fBTID\\fP always coincide with \\fBTGID\\fP "
"(Thread Group Identifier, the old \\fBPID\\fP).\n"
".TP\n"
"\\fBVSIZE\\fP (alias VSZ, VIRT) [uint32_t]\n"
"The total size of the process's memory footprint. This includes the text "
"segment, stack, static variables, data segment, and pages which are shared "
"with other processes.\n"
".TP\n"
"\\fBRSS\\fP (alias RSZ, RES, RESIDENT) [uint32_t]\n"
"The kernel's estimate of the resident set size for this process.\n"
".TP\n"
"\\fB%%CPU\\fP (alias PCPU, CP, C) [float]\n"
"The task's share of the CPU time since the last update, expressed as a "
"percentage of the total CPU time per processor. \\fBMaximum\\fP: 100%% x "
"CPU_core_number.\n"
".TP\n"
"\\fB%%MEM\\fP (alias PMEM) [float]\n"
"The ratio of the process's resident set size to the physical memory on the "
"machine, expressed as a percentage. \\fBMaximum\\fP: 100%%.\n"
".TP\n"
"\\fBCMD\\fP (alias COMM, COMMAND, UCMD, UCOMM) [char[16]]\n"
"Command name, i.e. the basename (w/o path) of the executable image file, "
"without arguments, eventually truncated at 15 characters.\n"
".TP\n"
"\\fBUTGID\\fP [char[256]]\n"
"FMC User-assigned unique Thread Group Identifier, if available (e.g. if the "
"process has been started by the FMC Task Manager or has the variable UTGID "
"defined in the process environment). Otherwise \"N/A\" is published.\n"
".TP\n"
"\\fBUSER\\fP (alias EUSER, UNAME) [char[20]]\n"
"\\fBEffective\\fP user name. Used for all the security checks. The only user "
"name of the process that normally has any effect.\n"
".TP\n"
"\\fBCMDLINE\\fP (alias ARGS, CMD, COMMAND) [char*]\n"
"The complete command line with its arguments (up to PATH_MAX = 4096 "
"characters). This string may contain blank characters.\n"
".RE\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/monitored/labels\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 26 NULL-terminated strings).\n"
".br\n"
"Publishes the labels referring to the detailed data published in the SVC "
"\".../details/monitored/processes/\\fICMD\\fP{\\fIUTGID\\fP}\\fITID\\fP/data"
"\":\n"
".br\n"
"\"UPDATED\" \"STATUS\" \"HSTATUS\" \"TID\" \"NLWP\" \"PSR\" \"VSIZE\" "
"\"LOCK\" \"RSS\" \"DATA\" \"STACK\" \"EXE\" \"LIB\" \"SHARE\" \"RTPRIO\" "
"\"NICE\" \"PRIO\" \"%%CPU\" \"%%MEM\" \"MIN_FLT\" \"MAJ_FLT\" "
"\"LAST_UPDATE\" \"CMD\" \"UTGID\" \"USER\" \"CMDLINE\".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/monitored/units\n"
"\\fBOutput format\\fP: \"C\" (a sequence of 26 NULL-terminated strings).\n"
".br\n"
"Publishes the units referring to the detailed data published in the SVC "
"\".../details/monitored/processes/\\fICMD\\fP{\\fIUTGID\\fP}\\fITID\\fP/data"
"\":\n"
".br\n"
"\"\" \"\" \"\" \"\" \"\" \"\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" \"KiB\" "
"\"KiB\" \"KiB\" \"KiB\" \"\" \"\" \"\" \"%%\" \"%%\" \"Hz\" \"Hz\" "
"\"bbb dd, HH:MM:SS\" \"\" \"\" \"\" \"\".\n"
".br\n"
"where \\fIbbb\\fP is the abbreviated month name, \\fIdd\\fP is the day of "
"the month as a decimal number [01,31] and \\fIHH:MM:SS\\fP is the time.\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/details/monitored/processes/\\fICMD\\fP"
"{\\fIUTGID\\fP}\\fITID\\fP/data\n"
"\\fBOutput format\\fP: \"I:17;F:4;C:13;C:16;C:256;C:20;C\".\n"
".br\n"
"Publishes the detailed data of the process having \\fICMD\\fP as command "
"name (basename, w/o path, of the executable image file, without arguments, "
"eventually truncated at 15 characters), \\fIUTGID\\fP as User-assigned "
"unique Thread Group Identifier and \\fITID\\fP as Thread/Thread-group "
"identifier. The data sequence is the following:\n"
".RS\n"
".TP\n"
"\\fBUPDATED\\fP [int32_t]\n"
"1 if service has been updated in the last \\fBpsSrv\\fP(8) server refresh, 0 "
"otherwise.\n"
".TP\n"
"\\fBSTATUS\\fP [int32_t]\n"
"1 if the process was still running in the last \\fBpsSrv\\fP(8) server "
"refresh, 0 otherwise.\n"
".TP\n"
"\\fBHSTATUS\\fP [uint32_t]\n"
"Historical status. The least significant \\fIPERSISTENCE\\fP bits of this "
"32-bit integer is the status of the monitored process in the last "
"\\fIPERSISTENCE\\fP refreshes of the \\fBpsSrv\\fP(8) server (the least "
"significant bit corresponds to the most recent refresh, the most significant "
"bit corresponds to the oldest refresh). Each bit is 1 if the process was "
"retrieved in the process table, 0 if the process was not retrieved in the "
"process table.\n"
".TP\n"
"\\fBTID\\fP (alias LWP, SPID) [uint32_t]\n"
"Thread Identifier, aka Light Weight Process Identifier. In this service "
"(being a process SVC) \\fBTID\\fP always coincide with \\fBTGID\\fP "
"(Thread Group Identifier, the old \\fBPID\\fP).\n"
".TP\n"
"\\fBNLWP\\fP (alias THCNT) [uint32_t]\n"
"Number of Lightweight Processes (Threads) in the process (thread group).\n"
".TP\n"
"\\fBPSR\\fP (alias CPUID, P) [uint32_t]\n"
"The processor that process is currently assigned to. It is useful to check "
"the operation of process-to-CPU affinity setting by the FMC Task Manager.\n"
".TP\n"
"\\fBVSIZE\\fP (alias VSZ, VIRT) [uint32_t]\n"
"The total size of the process's memory footprint. This includes the text "
"segment, stack, static variables, data segment, and pages which are shared "
"with other processes.\n"
".TP\n"
"\\fBLOCK\\fP [uint32_t]\n"
"The amount of the process's memory which is currently locked by the kernel. "
"Locked memory cannot be swapped out.\n"
".TP\n"
"\\fBRSS\\fP (alias RSZ, RES, RESIDENT) [uint32_t]\n"
"The kernel's estimate of the resident set size for this process.\n"
".TP\n"
"\\fBDATA\\fP [uint32_t]\n"
"The amount of memory used for data by the process. It includes static "
"variables and the data segment, but excludes the stack.\n"
".TP\n"
"\\fBSTACK\\fP [uint32_t]\n"
"The amount of memory consumed by the process's stack.\n"
".TP\n"
"\\fBEXE\\fP [uint32_t]\n"
"The size of the process's executable pages, excluding shared pages.\n"
".TP\n"
"\\fBLIB\\fP [uint32_t]\n"
"The size of the shared memory pages mapped into the process's address space. "
"This excludes pages shared using System V style IPC.\n"
".TP\n"
"\\fBSHARE\\fP (alias SHRD, SHR) [uint32_t]\n"
"The size of the pages of shared (mmap-ed) memory.\n"
".TP\n"
"\\fBRTPRIO\\fP [uint32_t]\n"
"Static (real-time) priority of the process. Only value 0 is allowed for TS "
"processes. For FF and RR real-time processes, allowed values are in the "
"range 1...99 (0 is the least favorable priority, 99 is the most favorable "
"priority).\n"
".TP\n"
"\\fBNICE\\fP (alias NI) [uint32_t]\n"
"The nice level of the process, used by the TS Linux scheduler to compute the "
"dynamic priority. Allowed values for nice level are in the range -20...19 "
"(-20 corresponds to the most favorable scheduling; 19 corresponds to the "
"least favorable scheduling).\n"
".TP\n"
"\\fBPRIO\\fP (alias PRI, PR) [uint32_t]\n"
"Kernel scheduling priority. Possible values are in the range -100...39 (-100 "
"is the most favorable priority, 39 is the least favorable priority). TS "
"processes have PRIO in the range 0...39, FF and RR processes have PRIO in "
"the range -100...-1.\n"
".TP\n"
"\\fB%%CPU\\fP (alias PCPU, CP, C) [float]\n"
"The task's share of the CPU time since the last update, expressed as a "
"percentage of the total CPU time per processor. \\fBMaximum\\fP: 100%% x "
"CPU_core_number.\n"
".TP\n"
"\\fB%%MEM\\fP (alias PMEM) [float]\n"
"The ratio of the process's resident set size to the physical memory on the "
"machine, expressed as a percentage. \\fBMaximum\\fP: 100%%.\n"
".TP\n"
"\\fBMIN_FLT\\fP [float]\n"
"The rate (page faults per second) of page faults the process has made which "
"have not required loading a memory page from disk.\n"
".TP\n"
"\\fBMAJ_FLT\\fP [float]\n"
"The rate (page faults per second) of page faults the process has made which "
"have required loading a memory page from disk.\n"
".TP\n"
"\\fBLAST_UPDATE\\fP [char[13]]\n"
"The date and time of the last refresh of the \\fBpsSrv\\fP(8) server.\n"
".TP\n"
"\\fBCMD\\fP (alias COMM, COMMAND, UCMD, UCOMM) [char[16]]\n"
"Command name, i.e. the basename (w/o path) of the executable image file, "
"without arguments, eventually truncated at 15 characters.\n"
".TP\n"
"\\fBUTGID\\fP [char[256]]\n"
"FMC User-assigned unique Thread Group Identifier, if available (e.g. if the "
"process has been started by the FMC Task Manager or has the variable UTGID "
"defined in the process environment). Otherwise \"N/A\" is published.\n"
".TP\n"
"\\fBUSER\\fP (alias EUSER, UNAME) [char[20]]\n"
"\\fBEffective\\fP user name. Used for all the security checks. The only user "
"name of the process that normally has any effect.\n"
".TP\n"
"\\fBCMDLINE\\fP (alias ARGS, CMD, COMMAND) [char*]\n"
"The complete command line with its arguments (up to PATH_MAX = 4096 "
"characters). This string may contain blank characters.\n"
".RE\n"
".\n"
".\n"
".SS Published DIM Commands:\n"
".\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/startMonitoring\n"
"\\fBInput format\\fP: \"C\" (a sequence of 26 NULL-terminated strings).\n"
".br\n"
"Accepts, as argument, a NULL-terminated string containing a command-line to "
"add a new \\fIterm\\fP to the \\fIfilter expression\\fP used to select the "
"processes to be monitored by means of a dedicated DIM SVC. Different options "
"specified within the same \\fBstartMonitoring\\fP command are put in logical "
"\\fIAND\\fP among them, while options specified in different "
"\\fBstartMonitoring\\fP commands are put in logical \\fIOR\\fP among them. "
"The current \\fIfilter expression\\fP can be viewed by means of the "
"\".../monitored/properties\" DIM SVC (see above).\n"
".IP\n"
".B DIM Command Synopsis\n"
".IP\n"
".ad l\n"
"[\\fB-t\\fP\\ |\\ \\fB--tid\\fP\\ \\fITID\\fP]\n"
"[\\fB-u\\fP\\ |\\ \\fB--utgid\\fP\\ \\fIUTGID_PATTERN\\fP]\n"
"[\\fB-c\\fP\\ |\\ \\fB--cmd\\fP\\ \\fICMD_PATTERN\\fP]\n"
"[\\fB-C\\fP\\ |\\ \\fB--cmdline\\fP\\ \\fICMDLINE_PATTERN\\fP]\n"
"[\\fB--a1\\fP\\ \\fIARG1_PATTERN\\fP]\n"
"[\\fB--a2\\fP\\ \\fIARG2_PATTERN\\fP]\n"
"[\\fB--a3\\fP\\ \\fIARG3_PATTERN\\fP]\n"
"[\\fB--a4\\fP\\ \\fIARG4_PATTERN\\fP]\n"
"[\\fB--a5\\fP\\ \\fIARG5_PATTERN\\fP]\n"
"[\\fB--a6\\fP\\ \\fIARG6_PATTERN\\fP]\n"
"[\\fB--a7\\fP\\ \\fIARG7_PATTERN\\fP]\n"
"[\\fB--a8\\fP\\ \\fIARG8_PATTERN\\fP]\n"
"[\\fB--a9\\fP\\ \\fIARG9_PATTERN\\fP]\n"
".ad b\n"
".PP\n"
".RS\n"
".B DIM Command Options\n"
".OptDef t tid TID (unsigned integer)\n"
"Select the process which has \\fITID\\fP as Thread Identifier.\n"
".OptDef u utgid UTGID_PATTERN (string)\n"
"Select the process(es) whose UTGID matches the wildcard pattern "
"\\fIUTGID_PATTERN\\fP.\n"
".OptDef c cmd CMD_PATTERN (string)\n"
"Select the process(es) whose command name (basename, w/o path, of the "
"executable image file, without arguments, eventually truncated at 15 "
"characters) matches the wildcard pattern \\fICMD_PATTERN\\fP.\n"
".OptDef C cmdline CMDLINE_PATTERN (string)\n"
"Select the process(es) whose command line (complete name of the executable "
"image file, including path but excluding arguments, up to PATH_MAX = 4096 "
"characters) matches the wildcard pattern \\fICMDLINE_PATTERN\\fP.\n"
".OptDef \"\" a\\fIn\\fP ARGn_PATTERN (string)\n"
"Select the process(es) whose \\fIn\\fP-th argument on the command-line "
"matches the wildcard pattern \\fIARGn_PATTERN\\fP (the digit \\fIn\\fP "
"ranges in 1..9).\n"
".RE\n"
".PP\n"
".RS\n"
".B DIM Command Examples:\n"
".PP\n"
"\"-c psSrv\"\n"
".br\n"
"\"-t 20152\"\n"
".br\n"
"\"-u *Srv_u -c *Srv\"\n"
".br\n"
"\"-C /opt/FMC/sbin/tmSrv --a1 -l --a2 1 --a3 -p --a4 2\"\n"
".RE\n"
".TP\n"
SVC_HEAD"/\\fIHOSTNAME\\fP/"SRV_NAME"/stopMonitoring\n"
"\\fBInput format\\fP: \"C\" (a sequence of 26 NULL-terminated strings).\n"
".br\n"
"Accepts, as argument, a NULL-terminated string containing a command-line to "
"remove a \\fIterm\\fP from the \\fIfilter expression\\fP used to select the "
"processes to be monitored by means of a dedicated DIM SVC.\n"
".br\n"
"The command to remove a certain filter must have \\fIexactly\\fP the same "
"options used before to add the filter iteself. E.g., if a filter was added "
"by means of the string \"-c *Srv -u *Srv_u\" sent to the "
"\\fBstartMonitoring\\fP CMD, it can be removed by sending the same string "
"\"-c *Srv -u *Srv_u\" to the CMD \\fBstopMonitoring\\fP.\n"
".br\n"
"The current \\fIfilter expression\\fP can be viewed by means of the "
"\".../monitored/properties\" DIM SVC (see above).\n"
".IP\n"
".B DIM Command Synopsis\n"
".IP\n"
".ad l\n"
"[\\fB-t\\fP\\ |\\ \\fB--tid\\fP\\ \\fITID\\fP]\n"
"[\\fB-u\\fP\\ |\\ \\fB--utgid\\fP\\ \\fIUTGID_PATTERN\\fP]\n"
"[\\fB-c\\fP\\ |\\ \\fB--cmd\\fP\\ \\fICMD_PATTERN\\fP]\n"
"[\\fB-C\\fP\\ |\\ \\fB--cmdline\\fP\\ \\fICMDLINE_PATTERN\\fP]\n"
"[\\fB--a1\\fP\\ \\fIARG1_PATTERN\\fP]\n"
"[\\fB--a2\\fP\\ \\fIARG2_PATTERN\\fP]\n"
"[\\fB--a3\\fP\\ \\fIARG3_PATTERN\\fP]\n"
"[\\fB--a4\\fP\\ \\fIARG4_PATTERN\\fP]\n"
"[\\fB--a5\\fP\\ \\fIARG5_PATTERN\\fP]\n"
"[\\fB--a6\\fP\\ \\fIARG6_PATTERN\\fP]\n"
"[\\fB--a7\\fP\\ \\fIARG7_PATTERN\\fP]\n"
"[\\fB--a8\\fP\\ \\fIARG8_PATTERN\\fP]\n"
"[\\fB--a9\\fP\\ \\fIARG9_PATTERN\\fP]\n"
".ad b\n"
".PP\n"
".RS\n"
".B DIM Command Options\n"
".OptDef t tid TID (unsigned integer)\n"
"Deselect the process which has \\fITID\\fP as Thread Identifier.\n"
".OptDef u utgid UTGID_PATTERN (string)\n"
"Deselect the process(es) whose UTGID matches the wildcard pattern "
"\\fIUTGID_PATTERN\\fP.\n"
".OptDef c cmd CMD_PATTERN (string)\n"
"Deselect the process(es) whose command name (basename, w/o path, of the "
"executable image file, without arguments, eventually truncated at 15 "
"characters) matches the wildcard pattern \\fICMD_PATTERN\\fP.\n"
".OptDef C cmdline CMDLINE_PATTERN (string)\n"
"Deselect the process(es) whose command line (complete name of the executable "
"image file, including path but excluding arguments, up to PATH_MAX = 4096 "
"characters) matches the wildcard pattern \\fICMDLINE_PATTERN\\fP.\n"
".OptDef \"\" a\\fIn\\fP ARGn_PATTERN (string)\n"
"Deselect the process(es) whose \\fIn\\fP-th argument on the command-line "
"matches the wildcard pattern \\fIARGn_PATTERN\\fP (the digit \\fIn\\fP "
"ranges in 1..9).\n"
".RE\n"
".PP\n"
".RS\n"
".B DIM Command Examples:\n"
".PP\n"
"\"-c psSrv\"\n"
".br\n"
"\"-t 20152\"\n"
".br\n"
"\"-u *Srv_u -c *Srv\"\n"
".br\n"
"\"-C /opt/FMC/sbin/tmSrv --a1 -l --a2 1 --a3 -p --a4 2\"\n"
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
".BR \\%%psViewer (1),\n"
".BR \\%%psMonitor (1),\n"
".BR \\%%psSetProperties (1).\n"
".br\n"
".BR \\%%tmSrv (8),\n"
".BR \\%%tmStart (1),\n"
".BR \\%%tmKill (1),\n"
".BR \\%%tmStop (1).\n"
".br\n"
".BR \\%%ps (1),\n"
".BR \\%%top (1),\n"
".BR \\%%proc (5).\n"
".br\n"
".BR \\%%/proc/TGID/status ,\n"
".BR \\%%/proc/TGID/stat ,\n"
".BR \\%%/proc/TGID/statm .\n"
".br\n"
".BR \\%%/proc/TGID/task/TID/status ,\n"
".BR \\%%/proc/TGID/task/TID/stat ,\n"
".BR \\%%/proc/TGID/task/TID/statm .\n"
".br\n"
".BR \\%%/usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt .\n"
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
