/* ************************************************************************* */
/* tmSrv.c - Task Manager Server                                             */
/* ************************************************************************* */
/*
 * Revision 5.4  2012/08/27 12:25:12  galli
 * Publishes in longList the oom_score
 *
 * Revision 5.1  2011/11/28 16:22:20  galli
 * Can set the oom_adj or oom_score_adj of started processes.
 *
 * Revision 5.0  2011/11/28 09:42:16  galli
 * Set oom_adj to -17 or oom_score_adj to -1000.
 *
 * Revision 4.27  2009/09/16 13:46:47  galli
 * Removed workaround for obsolete Linux kernel 2.4
 * SIGCHLD handled directly in the main loop
 *
 * Revision 4.26  2009/09/16 13:19:55  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 4.21  2008/11/28 17:27:55  galli
 * bug fixed
 *
 * Revision 4.18  2008/11/28 16:16:20  galli
 * Constant MAX_PID substituted by variable maxPid read at runtime
 * from /proc/sys/kernel/pid_max.
 *
 * Revision 4.16  2008/11/11 21:24:29  galli
 * added option --no-ptrace-workaround to be used with kernels with the
 * patch Don-t-truncate-proc-PID-environ-at-4096-characters.
 *
 * Revision 4.9  2008/09/01 15:03:20  galli
 * groff manual
 *
 * Revision 4.3  2008/07/02 11:15:32  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 4.2  2008/05/02 15:29:35  galli
 * use function sig2msg()
 *
 * Revision 4.1  2008/04/30 14:47:20  galli
 * does not need libprocps no more
 *
 * Revision 3.7  2008/04/29 15:35:27  galli
 * - process list update in new function procListUpdate()
 * - now shortListSvcHandler() and longListSvcHandler() only compose
 *   dim service buffer
 *
 * Revision 3.6  2008/04/29 11:39:34  galli
 * - setuid/setgid at startup
 * - new cmdline options -U and -G
 * - cmdline option -d renamed -u
 *
 * Revision 3.1  2007/12/07 11:52:05  galli
 * Cmd-line option --defunct-persistence PERSISTENCE added
 *
 * Revision 2.23  2007/12/06 14:11:01  galli
 * SVC longList added
 *
 * Revision 2.22  2007/12/05 16:30:25  galli
 * Keeps a list of processes (included processes terminated within a timeout)
 *
 * Revision 2.20  2007/11/28 10:52:27  galli
 * Back to only one version of findUtgid() and findTgid()
 *
 * Revision 2.19  2007/11/27 15:26:19  galli
 * 2 different findTgid functions (findTgidProcEnviron and findTgidLargeEnv)
 *
 * Revision 2.18  2007/11/23 14:52:41  galli
 * Added hidden cmd-line option --ptrace-always for test
 * SIGCHLD handler installed with SA_NOCLDSTOP.
 * Handle case sigtimedwait interruped by an unblocked signal
 * List update required only once for waitpid() bunch and not if STOPPED
 *
 * Revision 2.17  2007/11/14 15:02:59  galli
 * usage() update
 *
 * Revision 2.16  2007/11/13 14:41:32  galli
 * hard-coded constant moved to tmSrv.h
 * 2 different findUtgid functions (findUtgidProcEnviron and findUtgidPtrace)
 * --huge-environment command-line switch to choose findUtgidPtrace
 * few hard-coded limits removed (SRVC_BUF_LEN, USER_NAME_LEN, GROUP_NAME_LEN,
 * FIFO_NAME_LEN)
 *
 * Revision 2.13  2007/10/26 10:23:00  galli
 * in non-authenticated mode accepts also authenticated commands
 *
 * Revision 2.11  2007/10/26 09:40:07  galli
 * bug fixed
 *
 * Revision 2.10  2007/10/26 09:36:16  galli
 * prints FMC version in starting messages
 * publishes fmc_version
 *
 * Revision 2.9  2007/10/23 15:44:00  galli
 * usage() prints FMC version
 *
 * Revision 2.6  2007/09/30 20:26:56  galli
 * bug fixed
 *
 * Revision 2.5  2007/09/29 22:37:21  galli
 * authentication for command tmStart, tmKill, tmStop
 * read access control list from file /etc/tmSrv.allow
 * command-line switch --no-authentication for compatibility with version < 2.0
 *
 * Revision 1.54  2007/08/14 14:36:39  galli
 * compatible with libFMCutils v 2
 * usage() changed
 *
 * Revision 1.53  2007/06/28 13:25:28  galli
 * usage() updated
 *
 * Revision 1.52  2007/06/28 12:54:09  galli
 * added -g command-line option to set default group
 * added -g startCmdHandler() option to set group
 * if group not set uses the default group of user
 *
 * Revision 1.50  2007/06/28 11:45:34  galli
 * added commented lines for benchmark
 *
 * Revision 1.48  2006/10/20 09:23:49  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 1.47  2006/10/18 12:42:58  galli
 * uses the __func__ magic variable
 *
 * Revision 1.46  2006/09/07 08:39:31  galli
 * check if starting process path exists and is executable
 *
 * Revision 1.45  2006/09/05 09:30:46  galli
 * comments added
 *
 * Revision 1.44  2006/08/25 13:48:34  galli
 * -O outFIFO, -E errFIFO, -A command line options added to startCmdHandler()
 *
 * Revision 1.43  2006/08/23 13:49:18  galli
 * Uses sigaction() instead of signal() to install SIGCHLD signal handlers
 *
 * Revision 1.42  2006/08/23 13:40:27  galli
 * Read updatePeriod variable from environment.
 * To cope with POSIX non conformity of Linux kernel 2.4 install a
 * signal handler for SIGCHLD which send a SIGUSR1 to the main thread.
 *
 * Revision 1.41  2006/08/22 08:58:05  galli
 * read deBug value from environment
 * processes signals synchronously
 * uses SIGHUP signal to request a list update
 *
 * Revision 1.40  2006/02/09 11:23:10  galli
 * x86_64 compatibility problems fixed
 *
 * Revision 1.39  2005/06/22 10:28:45  galli
 * -a command line options added to startCmdHandler()
 * to set a bit in process-to-cpu affinity mask
 *
 * Revision 1.37  2004/12/21 00:01:09  galli
 * Added command-line options: --schedpol, --mainprio, --ioprio & --timerprio
 *
 * Revision 1.36  2004/12/19 00:05:47  galli
 * memory leak fixed on variable cmd
 *
 * Revision 1.34  2004/12/17 15:04:31  galli
 * memory leak debugging version
 *
 * Revision 1.33  2004/12/14 09:43:11  galli
 * sigChldHandler() tasks moved to main()
 *
 * Revision 1.32  2004/12/13 00:38:45  galli
 * Uses sigaction() instead of signal() to install signal handlers
 * Uses nanosleep() instead of dtq_sleep()
 * Runs finishOffPs() in different threads
 * Added functions: doProcListUpdate(), highPrioProcListUpdate(),
 * lowPrioProcListUpdate()
 * INFO severity for processes died due to SIG: INT, TERM, ALRM and KILL.
 *
 * Revision 1.30  2004/12/05 22:35:15  galli
 * Signal handler sigChldHandler() added to print to DIM logger either the
 * exit status or the signal received of the dead processes.
 * Can redirect the stdout & stderr of started process to the DIM logger.
 *
 * Revision 1.28  2004/11/21 19:46:59  galli
 * actuator_version DIM service added
 *
 * Revision 1.27  2004/11/21 10:59:02  galli
 * new service flag -D to set additional environment variables
 * memory leak fixed in argz
 *
 * Revision 1.25  2004/11/09 09:22:59  galli
 * DIM error handler added
 *
 * Revision 1.21  2004/10/28 00:29:06  galli
 * -p and -d command line options added to main() (permission and default user)
 * -s, -p, -n & -r command line options added to startCmdHandler()
 * to set scheduler, nice level, real-time priority and username
 *
 * Revision 1.1  2004/10/11 00:44:14  galli
 * Initial revision
 */
/* ************************************************************************* */
/* REMIND:                                                                   */
/* 1) can't use pthread_cond_signal() to notify changes because we have to   */
/*    catch SIGCHLD anyhow. So we have to use pthread_kill(SIGHUP).          */
/* ************************************************************************* */
/* headers */
#define _GNU_SOURCE                                            /* asprintf() */
#include <stdio.h>
#include <unistd.h>                                               /* read(2) */
#include <getopt.h>                                         /* getopt_long() */
#include <string.h>                                              /* strdup() */
#include <errno.h>                                                  /* errno */
#include <argz.h>           /* argz_create_sep(), argz_len(), argz_extract() */
#include <syslog.h>                                              /* syslog() */
#include <stdlib.h>                                   /* strtol(), strtoul() */
#include <signal.h>                                              /* signal() */
#include <pwd.h>                                               /* getpwnam() */
#include <grp.h>                                               /* getgrnam() */
#include <sys/types.h>                     /* getpwnam(), waitpid(), open(2) */
#include <sys/wait.h>         /* waitpid(), WIFEXITED(), WEXITSTATUS(), etc. */
#include <sys/stat.h>                                   /* lstat(2), open(2) */
#include <fcntl.h>                                                /* open(2) */
#include <envz.h>                                              /* envz_add() */
#include <pthread.h>          /* pthread_create(), pthread_attr_init(), etc. */
#include <fnmatch.h>                                           /* fnmatch(3) */
#include <stdint.h>                        /* SIZE_MAX, limit of size_t type */
#include <sys/utsname.h>                         /* uname(2), struct utsname */
#include <limits.h>                                         /* HOST_NAME_MAX */
/* ------------------------------------------------------------------------- */
#include <dis.h>                  /* dis_add_cmnd(), dis_add_service(), etc. */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcRef.h"                                  /* FMC_URLS FMC_AUTHORS */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcMacro.h"     /* eExit(), likely(), unlikely(), printOutFuncOK() */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
#include "fmcDate.h"                                     /* compilation date */
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcSrvNameUtils.h"                               /* getSrvPrefix() */
#include "fmcMsgUtils.h"           /* dfltLoggerOpen(), mPrintf(), rPrintf() */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "fmcSigUtils.h"                                        /* sig2msg() */
/* ------------------------------------------------------------------------- */
/* in tm/include */
#define _SERVER 1
#include "tmSrv.h"                                    /* hard-coded constant */
#include "tmUtils.h"      /* startPs(), killPs(), findUtgid(), freePssinfo() */
/* ************************************************************************* */
//#define BENCHMARK
#ifdef BENCHMARK
  #include <sys/timeb.h>
#endif
/* ************************************************************************* */
/* macro to free-up memory m-allocated by startCmdHandler() */
#define FREE_START_CMD_HANDLER_HEAP \
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
if(envz) \
{ \
  free(envz); \
  envz=NULL; \
  envz_len=0; \
}
/* ------------------------------------------------------------------------- */
/* macro to free-up memory m-allocated by killCmdHandler() */
#define FREE_KILL_CMD_HANDLER_HEAP \
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
}
/* ------------------------------------------------------------------------- */
/* macro to free-up memory m-allocated by stopCmdHandler() (except td) */
#define FREE_STOP_CMD_HANDLER_HEAP \
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
}
/* ------------------------------------------------------------------------- */
/* macro to free-up m-allocated td strcture */
#define FREE_FINISH_OFF_PS_HEAP \
if(td) \
{ \
  free(td); \
  td=NULL; \
}
/* ************************************************************************* */
/* structure definition */
typedef struct finishOffPsData                                  /* 264 bytes */
{
  char utgidPattern[UTGID_LEN+1];
  int s9delay;
  int sentSigno;
}finishOffPsData_t;
/* ------------------------------------------------------------------------- */
typedef struct accessControlList
{
  int entryN;
  char **hostNameList;
  char **userNameList;
}accessControlList_t;
/* ************************************************************************* */
/* global variables */
/* max PID */
int maxPid;
/* max size of /proc/<pid>/environ files */
size_t procPidEnvSz=DFLT_PROC_PID_ENV_SZ;
/* maximum number of characters read from procfs files in one shot */
size_t procMaxReadChars=DFLT_PROC_MAX_READ_CHARS;
/* mutex */
pthread_mutex_t openprocLock=PTHREAD_MUTEX_INITIALIZER;
/* constants throughout the whole process                                    */
char nodeName[HOST_NAME_MAX+1]="";
int deBug=0;                                                   /* debug mask */
int errU=L_DIM;                                                /* error unit */
int updatePeriod=20;              /* update period if nothing happens (20 s) */
char *kernelVersion=NULL;
/* passed from main() to other functions */
int authenticated=1;
int startCmdID=0;
int killCmdID=0;
int stopCmdID=0;
int shortListSvcID=0;
int longListSvcID=0;
int versionSvcID=0;
int aVersionSvcID=0;
int fmcVersionSvcID=0;
int successSvcID=0;
/* passed from main() to startCmdHandler() */
char *dfltUserName="nobody";
char *dfltGroupName="nobody";
uid_t dfltUid=99;
gid_t dfltGid=99;
int permission=0;
/* other */
int success=1;
int tmSchedPol=2;
int tmMainPrio=80;
int tmIoPrio=93;                  /* passed from main() to startCmdHandler() */
int tmTimerPrio=10;
char *ss[3]={"SCHED_OTHER","SCHED_FIFO","SCHED_RR"};
static char rcsid[]="$Id: tmSrv.c,v 5.7 2012/12/13 15:30:54 galli Exp galli $";
char *aRcsid;
char *rcsidP=NULL;
char *aRcsidP=NULL;
pthread_t main_ptid;                 /* thread identifier of the main thread */
pid_t main_tgid;                                  /* tgid of the main thread */
/* reused */
/* variables related with threads created by stopCmdHandler()                */
pthread_t tid;
pthread_attr_t t_attr;
/* access control list */
accessControlList_t acl;
/* process linked list */
procList_t psList={PTHREAD_MUTEX_INITIALIZER,NULL,NULL};
int defunctPersistence=300;
/* ************************************************************************* */
/* function prototype */
/* DIM command handlers */
void startCmdHandler(long *tag,int *cmnd,int *size);
void killCmdHandler(long *tag,int *cmnd,int *size);
void stopCmdHandler(long *tag,int *cmnd,int *size);
/* DIM service handler */
void shortListSvcHandler(long *tag,int **address,int *size);
void longListSvcHandler(long *tag,int **address,int *size);
/* DIM error handler */
void disErrorHandler(int severity,int errorCode,char *message);
/* process list updater */
void procListUpdate(void);
/* new thread start-routine */
void *finishOffPs(void *data);
/* signal handlers */
static void signalHandler(int signo);
/* functions related with list service update */
void doProcListUpdate(long tag);
void highPrioProcListUpdate(void);
void lowPrioProcListUpdate(int delay);
/* functions related to the access control list */
accessControlList_t parseACL(char *aclString);
accessControlList_t readACL(char *aclFileName);
/* other functions */
void usage(int mode);
void shortUsage(void);
/* ************************************************************************* */
int main(int argc,char **argv)
{
  int i;
  char *p;
  char *dimDnsNode=NULL;
  char *deBugS=NULL;
  char *updatePeriodS=NULL;
  char svcPath[DIM_SVC_LEN+1]="";
  char srvPath[DIM_SRV_LEN+1]="";
  struct passwd *pw;
  struct group *gr;
  int signo;
  sigset_t signalMask;
  unsigned long long pU=0,tU=0;
  char *mesgP=NULL;
  struct timespec updatePeriodTS={0,0};
  int minTmMainPrio=0;
  int maxTmMainPrio=0;
  int minTmIoPrio=0;
  int maxTmIoPrio=0;
  int minTmTimerPrio=0;
  int maxTmTimerPrio=0;
  char mesg[MSG_LEN+1]="";
  char *tmUserName="nobody";
  char *tmGroupName="nobody";
  uid_t tmUid=99;
  gid_t tmGid=99;
  char *endptr=NULL;
  /* kernel version */
  static struct utsname osBuf;
  /* oom adjust */
  int oomAdjVal;
  int oomScoreAdjVal;
  char oomAdjStr[8]="";                                          /* [-17,15] */
  char oomScoreAdjStr[8]="";                                 /* [-1000,1000] */
  /* getopt */
  int flag=0;
  static struct option longOptions[]=
  {
    {"schedpol",required_argument,NULL,0},
    {"mainprio",required_argument,NULL,1},
    {"ioprio",required_argument,NULL,2},
    {"timerprio",required_argument,NULL,3},
    {"no-authentication",no_argument,NULL,4},
    {"defunct-persistence",required_argument,NULL,5},
    {"no-ptrace-workaround",no_argument,NULL,6},
    {"always-ptrace-workaround",no_argument,NULL,7},
    {"logger",required_argument,NULL,'l'},
    {"permission",required_argument,NULL,'p'},
    {"tm_user",required_argument,NULL,'U'},
    {"tm_group",required_argument,NULL,'G'},
    {"dflt_user",required_argument,NULL,'u'},
    {"dflt_group",required_argument,NULL,'g'},
    {"help",optional_argument,NULL,'h'},
    {"dim_dns_node",required_argument,NULL,'N'},
    {NULL,0,NULL,0}
  };
  /* ----------------------------------------------------------------------- */
  aRcsid=getActuatorVersion();
  /* ----------------------------------------------------------------------- */
  /* Strip rcsid and aRcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  aRcsidP=strchr(aRcsid,':')+2;
  for(i=0,p=aRcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /* ----------------------------------------------------------------------- */
  /* syslog version */
  mPrintf(L_SYS,INFO,__func__,0,"Starting FMC Task Manager... Using: \"%s\", "
          "\"%s\", \"FMC-%s\".",rcsidP,aRcsidP,FMC_VERSION);
  /* ----------------------------------------------------------------------- */
  /* get maximum PID in running kernel */
  maxPid=getMaxPid();
  /* ----------------------------------------------------------------------- */
  /* process command-line arguments */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt_long(argc,argv,"l:p:u:g:U:G:h::N:",longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 0:   /* --schedpol */
        tmSchedPol=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 1:   /* --mainprio */
        tmMainPrio=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 2:   /* --ioprio */
        tmIoPrio=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 3:   /* --timerprio */
        tmTimerPrio=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 4:   /* --no-authentication */
        authenticated=0;
        break;
      case 5:   /* --defunct-persistence */
        defunctPersistence=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 6:   /* --no-ptrace-workaround */
        procPidEnvSz=SIZE_MAX;
        procMaxReadChars=131072;
        break;
      case 7:   /* --always-ptrace-workaround */
        procPidEnvSz=0;
        break;
      case 'l': /* -l, --logger */
        errU=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'p': /* -p, --permission */
        permission=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'u': /* -u, --dflt_user */
        dfltUserName=optarg;
        break;
      case 'g': /* -g, --dflt_group */
        dfltGroupName=optarg;
        break;
      case 'U': /* -U, --tm_user */
        tmUserName=optarg;
        break;
      case 'G': /* -G, --tm_group */
        tmGroupName=optarg;
        break;
      case 'N': /* -N, --dim_dns_node */
        dimDnsNode=optarg;
        break;
      case 'h': /* -h, -hh, --help */
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        mPrintf(L_STD|L_SYS,FATAL,__func__,0,"getopt_long(): invalid "
                "option \"%s\"!",argv[optind-1]);
        shortUsage();
    }
  }
  /* ----------------------------------------------------------------------- */
  if(optind<argc)
  {
    mesgP=mesg;
    sprintf(mesgP,"Invalid non-option command-line arguments: ");
    mesgP=strchr(mesgP,'\0');
    while(optind<argc)
    {
      sprintf(mesgP,"\"%s\" ",argv[optind++]);
      mesgP=strchr(mesgP,'\0');
    }
    sprintf(mesgP,".");
    mPrintf(L_STD|L_SYS,FATAL,__func__,0,mesg);
    shortUsage();
  }
  if(argc-optind!=0)shortUsage();
  /* ----------------------------------------------------------------------- */
  /* check command line options */
  if(errU<0||errU>7)
  {
    mPrintf(L_STD|L_SYS,FATAL,__func__,0,"Logger type %d not allowed. Logger "
            "type must be in the range 0..7! Exiting...",errU);
    shortUsage();
    exit(1);
  }
  /* ----------------------------------------------------------------------- */
  /* open error logger */
  if(errU&L_DIM)
  {
    if(dfltLoggerOpen(10,0,ERROR,DEBUG,0)==-1)
    {
      mPrintf(L_STD|L_SYS,FATAL,__func__,0,"Can't initialize error logger! "
              "Exiting...");
      exit(1);
    }
  }
  mPrintf(errU,INFO,__func__,0,"Starting FMC Task Manager... Using: \"%s\", "
          "\"%s\", \"FMC-%s\".",rcsidP,aRcsidP,FMC_VERSION);
  /* ----------------------------------------------------------------------- */
  /* get hostname */
  if(!gethostname(nodeName,HOST_NAME_MAX+1)<0)
  {
    mPrintf(errU,FATAL,__func__,0,"gethostname(): %s.",strerror(errno));
    exit(1);
  }
  p=strchr(nodeName,'.');
  if(p)*p='\0';
  /* ----------------------------------------------------------------------- */
  /* get kernel version */
  if(uname(&osBuf))
  {
    mPrintf(errU,ERROR,__func__,0,"uname(): %s",strerror(errno));
    kernelVersion="N/A";
  }
  else
  {
    kernelVersion=osBuf.release;
  }
  /* ----------------------------------------------------------------------- */
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
  /* ----------------------------------------------------------------------- */
  /* check command line options (continued) */
  if(permission<0||permission>2)
  {
    mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"Permission %d not allowed. "
            "Permission must be in the range 0..2! Exiting...",permission);
    shortUsage();
    exit(1);
  }
  /* ----------------------------------------------------------------------- */
  /* set tmSrv user and group */
  if(strcmp(tmUserName,"nobody"))
  {
    tmUid=(uid_t)strtol(tmUserName,&endptr,0);
    if((tmUid==0 && (errno==EINVAL || errno==ERANGE)) || endptr[0]!=0)
    {
      errno=0;
      pw=getpwnam(tmUserName);
      if(!pw)
      {
        if(!errno)
           mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getpwnam(\"%s\") "
                   "failed: %s!",tmUserName,"The given name or uid was not "
                   "found");
        else
           mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getpwnam(\"%s\") "
                   "failed: %s!",tmUserName,strerror(errno));
        shortUsage();
        exit(1);
      }
      tmUid=pw->pw_uid;
      tmGid=pw->pw_gid;
    }
  }
  if(strcmp(tmGroupName,"nobody"))
  {
    tmGid=(gid_t)strtol(tmGroupName,&endptr,0);
    if((tmGid==0 && (errno==EINVAL || errno==ERANGE)) || endptr[0]!=0)
    {
      errno=0;
      gr=getgrnam(tmGroupName);
      if(!gr)
      {
        if(!errno)
           mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getgrnam(\"%s\") "
                   "failed: %s!",tmGroupName,"The given group or gid was not "
                   "found");
        else
           mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getgrnam(\"%s\") "
                   "failed: %s!",tmGroupName,strerror(errno));
        shortUsage();
        exit(1);
      }
      tmGid=gr->gr_gid;
    }
  }
  if(tmUid!=99)
  {
    struct passwd *newPw=NULL;
    errno=0;
    newPw=getpwuid(tmUid);
    if(!newPw)
    {
      if(!errno)
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getpwuid(%d) failed: %s!",
                 tmUid,"The given name or uid was not found");
      else
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getpwuid(%d) failed: %s!",
                 tmUid,strerror(errno));
      shortUsage();
      exit(1);
    }
    mPrintf(errU,DEBUG,__func__,0,"Trying to set the Task Manager UID to %d "
            "(user: \"%s\")...",tmUid,newPw->pw_name);
    if(setuid(tmUid)==-1)
    {
       mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"setuid(%d=\"%s\") failed: "
               "%s!",tmUid,newPw->pw_name,strerror(errno));
      shortUsage();
      exit(1);
    }
    mPrintf(errU,DEBUG,__func__,0,"Task Manager UID set to %d (user: %s).",
            getuid(),newPw->pw_name);
  }
  if(tmGid!=99)
  {
    struct group *newGr=NULL;
    errno=0;
    newGr=getgrgid(tmGid);
    if(!newGr)
    {
      if(!errno)
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getgrgid(%d) failed: %s!",
                 tmGid,"The given group or gid was not found");
      else
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getgrgid(%d) failed: %s!",
                 tmGid,strerror(errno));
      shortUsage();
      exit(1);
    }
    mPrintf(errU,DEBUG,__func__,0,"Trying to set the Task Manager GID to %d "
            "(group: \"%s\")...",tmGid,newGr->gr_name);
    if(setgid(tmGid)==-1)
    {
       mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"setgid(%d=\"%s\") failed: "
               "%s!",tmGid,newGr->gr_name,strerror(errno));
      shortUsage();
      exit(1);
    }
    mPrintf(errU,DEBUG,__func__,0,"Task Manager GID set to %d (group: %s).",
            getgid(),newGr->gr_name);
  }
  /* ----------------------------------------------------------------------- */
  /* set default user and group */
  dfltUid=(uid_t)strtol(dfltUserName,&endptr,0);
  if((dfltUid==0 && (errno==EINVAL || errno==ERANGE)) || endptr[0]!=0)
  {
    errno=0;
    pw=getpwnam(dfltUserName);
    if(!pw)
    {
      if(!errno)
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getpwnam(\"%s\") "
                 "failed: %s!",dfltUserName,"The given name or uid was not "
                 "found");
      else
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getpwnam(\"%s\") "
                 "failed: %s!",dfltUserName,strerror(errno));
      shortUsage();
      exit(1);
    }
    dfltUid=pw->pw_uid;
    dfltGid=pw->pw_gid;
  }
  if(strcmp(dfltGroupName,"nobody"))
  {
    dfltGid=(gid_t)strtol(dfltGroupName,&endptr,0);
    if((dfltGid==0 && (errno==EINVAL || errno==ERANGE)) || endptr[0]!=0)
    {
      errno=0;
      gr=getgrnam(dfltGroupName);
      if(!gr)
      {
        if(!errno)
           mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getgrnam(\"%s\") "
                   "failed: %s!",dfltGroupName,"The given group or gid was "
                   "not found");
        else
           mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getgrnam(\"%s\") "
                   "failed: %s!",dfltGroupName,strerror(errno));
        shortUsage();
        exit(1);
      }
      dfltGid=gr->gr_gid;
    }
  }
  if(dfltUid!=99)
  {
    struct passwd *newPw=NULL;
    errno=0;
    newPw=getpwuid(dfltUid);
    if(!newPw)
    {
      if(!errno)
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getpwuid(%d) failed: %s!",
                 dfltUid,"The given name or uid was not found");
      else
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getpwuid(%d) failed: %s!",
                 dfltUid,strerror(errno));
      shortUsage();
      exit(1);
    }
  }
  if(dfltGid!=99)
  {
    struct group *newGr=NULL;
    errno=0;
    newGr=getgrgid(dfltGid);
    if(!newGr)
    {
      if(!errno)
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getgrgid(%d) failed: %s!",
                 dfltGid,"The given group or gid was not found");
      else
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getgrgid(%d) failed: %s!",
                 dfltGid,strerror(errno));
      shortUsage();
      exit(1);
    }
  }
  /* ----------------------------------------------------------------------- */
  /* check command line options (continued) */
  if(tmSchedPol<0||tmSchedPol>2)
  {
    mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"Scheduling policy %d not "
            "allowed. Scheduling policy must be in the range 0..2! Exiting...",
            tmSchedPol);
    shortUsage();
    exit(1);
  }
  if(tmSchedPol==0)                                           /* SCHED_OTHER */
  {
    minTmMainPrio=0;
    maxTmMainPrio=0;
    minTmIoPrio=0;
    maxTmIoPrio=0;
    minTmTimerPrio=0;
    maxTmTimerPrio=0;
  }
  else                                             /* SCHED_FIFO or SCHED_RR */
  {
    minTmMainPrio=1;
    maxTmMainPrio=99;
    minTmIoPrio=1;
    maxTmIoPrio=99;
    minTmTimerPrio=1;
    maxTmTimerPrio=99;
  }
  if(tmMainPrio<minTmMainPrio||tmMainPrio>maxTmMainPrio)
  {
    mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"Main thread priority %d not "
            "allowed for scheduler %d (%s). Main thread priority for "
            "scheduler %d (%s) must be in the range %d..%d! Exiting...",
            tmMainPrio,tmSchedPol,ss[tmSchedPol],tmSchedPol,ss[tmSchedPol],
            minTmMainPrio,maxTmMainPrio);
    shortUsage();
    exit(1);
  }
  if(tmIoPrio<minTmIoPrio||tmIoPrio>maxTmIoPrio)
  {
    mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"I/O thread priority %d not "
            "allowed for scheduler %d (%s). I/O thread priority for scheduler "
            "%d (%s) must be in the range %d..%d! Exiting...",tmIoPrio,
            tmSchedPol,ss[tmSchedPol],tmSchedPol,ss[tmSchedPol],minTmIoPrio,
            maxTmIoPrio);
    shortUsage();
    exit(1);
  }
  if(tmTimerPrio<minTmTimerPrio||tmTimerPrio>maxTmTimerPrio)
  {
    mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"Timer thread priority %d not "
            "allowed for scheduler %d (%s). Timer thread priority for "
            "scheduler %d (%s) must be in the range %d..%d! Exiting...",
            tmTimerPrio,tmSchedPol,ss[tmSchedPol],tmSchedPol,ss[tmSchedPol],
            minTmTimerPrio,maxTmTimerPrio);
    shortUsage();
    exit(1);
  }
  if(defunctPersistence<1)
  {
    mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"Defunct persistence %d s not "
            "allowed. Defunct persistence must be >0! Exiting...",
            defunctPersistence);
    shortUsage();
    exit(1);
  }
  /* ----------------------------------------------------------------------- */
  /* read environment variables */
  deBugS=getenv("deBug");
  if(deBugS)deBug=(int)strtol(deBugS,(char**)NULL,0);
  updatePeriodS=getenv("updatePeriod");
  if(updatePeriodS)updatePeriod=atoi(updatePeriodS);
  /* ----------------------------------------------------------------------- */
  /* Control the behavior of the OOM killer */
  if(!access(OOM_SCORE_ADJ,W_OK))                        /* kernel >= 2.6.36 */
  {
    int oomScoreAdjFD=-1;
    oomScoreAdjVal=TM_OOM_SCORE_ADJ;
    snprintf(oomScoreAdjStr,8,"%d",oomScoreAdjVal);
    /* write value */
    oomScoreAdjFD=open(OOM_SCORE_ADJ,O_WRONLY);
    if(oomScoreAdjFD==-1)
    {
      mPrintf(errU,WARN,__func__,0,"Cannot set oom_score_adj for the Task "
              "Manager: open(2): %s!",strerror(errno));
    }
    else
    {
      int wB=-1;
      wB=write(oomScoreAdjFD,oomScoreAdjStr,strlen(oomScoreAdjStr));
      if(wB==-1)
      {
        mPrintf(errU,WARN,__func__,0,"Cannot set oom_score_adj for the Task "
                "Manager: write(2): %s!",strerror(errno));
      }
      else
      {
        mPrintf(errU,DEBUG,__func__,0,"Task Manager oom_score_adj set to %s.",
                oomScoreAdjStr);
      }
      close(oomScoreAdjFD);
    }
    /* read written value */
    memset(oomScoreAdjStr,0,8);
    oomScoreAdjFD=open(OOM_SCORE_ADJ,O_RDONLY);
    read(oomScoreAdjFD,oomScoreAdjStr,8);
    close(oomScoreAdjFD);
    p=strchr(oomScoreAdjStr,'\n');
    if(p)*p='\0';
  }
  else if(!access(OOM_ADJ,W_OK))                          /* kernel < 2.6.36 */
  {
    int oomAdjFD=-1;
    oomAdjVal=TM_OOM_ADJ;
    snprintf(oomAdjStr,8,"%d",oomAdjVal);
    /* write value */
    oomAdjFD=open(OOM_ADJ,O_WRONLY);
    if(oomAdjFD==-1)
    {
      mPrintf(errU,WARN,__func__,0,"Cannot set oom_adj for the Task Manager: "
              "open(2): %s!",strerror(errno));
    }
    else
    {
      int wB=-1;
      wB=write(oomAdjFD,oomAdjStr,strlen(oomAdjStr));
      if(wB==-1)
      {
        mPrintf(errU,WARN,__func__,0,"Cannot set oom_adj for the Task "
                "Manager: write(2): %s!",strerror(errno));
      }
      else
      {
        mPrintf(errU,DEBUG,__func__,0,"Task Manager oom_adj set to %s.",
                oomAdjStr);
      }
      close(oomAdjFD);
    }
    /* read written value */
    memset(oomAdjStr,0,8);
    oomAdjFD=open(OOM_ADJ,O_RDONLY);
    read(oomAdjFD,oomAdjStr,8);
    close(oomAdjFD);
    p=strchr(oomAdjStr,'\n');
    if(p)*p='\0';
  }
  else
  {
    mPrintf(errU,WARN,__func__,0,"OOM Killer Score Adjustment not foreseen "
            "in this OS version (%s)!",kernelVersion);
  }
  /* ----------------------------------------------------------------------- */
  /* start-up printout */
  mPrintf(errU,INFO,__func__,0,"Scheduling policy:     %2d (%s).",tmSchedPol,
          ss[tmSchedPol]);
  mPrintf(errU,INFO,__func__,0,"Main thread priority:  %2d.",tmMainPrio);
  mPrintf(errU,INFO,__func__,0,"I/O thread priority:   %2d.",tmIoPrio);
  mPrintf(errU,INFO,__func__,0,"Timer thread priority: %2d.",tmTimerPrio);
  mPrintf(errU,INFO,__func__,0,"Task Manager user:     %s.",
          getpwuid(tmUid)->pw_name);
  mPrintf(errU,INFO,__func__,0,"Task Manager group:    %s.",
          getgrgid(tmGid)->gr_name);
  mPrintf(errU,INFO,__func__,0,"Default user:          %s.",
          getpwuid(dfltUid)->pw_name);
  mPrintf(errU,INFO,__func__,0,"Default group:         %s.",
          getgrgid(dfltGid)->gr_name);
  mPrintf(errU,INFO,__func__,0,"Update period:         %d s.",updatePeriod);
  mPrintf(errU,INFO,__func__,0,"Defunct persistence:   %d s.",
          defunctPersistence);
  mPrintf(errU,INFO,__func__,0,"Debug mask:            %#x.",deBug);
  mPrintf(errU,INFO,__func__,0,"Kernel version:        %s.",kernelVersion);
  mPrintf(errU,INFO,__func__,0,"Maximum PID:           %d.",maxPid);
  if(*oomScoreAdjStr)
    mPrintf(errU,INFO,__func__,0,"oom_score_adj:         %s.",oomScoreAdjStr);
  else if(*oomAdjStr)
    mPrintf(errU,INFO,__func__,0,"oom_adj:               %s.",oomAdjStr);
  else
    mPrintf(errU,INFO,__func__,0,"oom_adj/oom_score_adj  %s.","N/A");
  if(errU&L_DIM)mPrintf(errU,INFO,__func__,0,"Messages sent to DIM Logger.");
  if(errU&L_STD)mPrintf(errU,INFO,__func__,0,"Messages sent to stderr.");
  if(errU&L_SYS)mPrintf(errU,INFO,__func__,0,"Messages sent to syslog.");
  if(authenticated)
  {
    mPrintf(errU,INFO,__func__,0,"Client authentication: required.");
  }
  else
  {
    mPrintf(errU,INFO,__func__,0,"Client authentication: not required.");
    mPrintf(errU,WARN,__func__,0,"Client AUTHENTICATION NOT required! ALL "
            "the users from ALL the hosts are allowed to send command to the "
            "Task Manager Server running on this node! Start tmSrv without "
            "the \"--no-authentication\" command-line switch and use an "
            "authenticated client to avoid this.");
  }
  if(procPidEnvSz==DFLT_PROC_PID_ENV_SZ)   /* proc_pid_env patch not applied */
  {
    mPrintf(errU,WARN,__func__,0,"The FMC Task Manager Server uses the "
            "\"ptrace workaround\" to retrieve the UTGID of processes having "
            "an environment larger than %d B. This makes the system less "
            "efficient and incompatible with the use of gdb, ltrace and "
            "strace!",procPidEnvSz);
  }
  else if(procPidEnvSz==0)                       /* ptrace workaround forced */
  {
    mPrintf(errU,WARN,__func__,0,"The FMC Task Manager Server always uses the "
            "\"ptrace workaround\" to retrieve the UTGID of any process. This "
            "makes the system less efficient and incompatible with the use of "
            "gdb, ltrace and strace!");
  }
  else if(procPidEnvSz==SIZE_MAX) /* proc_pid_env patch applied in runn krnl */
  {
    mPrintf(errU,WARN,__func__,0,"The FMC Task Manager never uses the "
            "\"ptrace workaround\" to retrieve the UTGID of the processes. "
            "This improves system performances.");
  }
  /* ----------------------------------------------------------------------- */
  /* define server name */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),SRV_NAME);
  /* ----------------------------------------------------------------------- */
  /* Block SIGINT, SIGTERM, SIGCHLD and SIGHUP, to be handled synchronously  */
  /* by sigtimedwait().                                                      */
  /* Signals must be blocked before the dim_init() call, which creates 2 new */
  /* threads, to keep the signals blocked in all the 3 threads.              */
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigaddset(&signalMask,SIGCHLD);
  sigaddset(&signalMask,SIGHUP);
  if(pthread_sigmask(SIG_BLOCK,&signalMask,NULL))eExit("pthread_sigmask()");
  /* ----------------------------------------------------------------------- */
  /* Start DIM. Here threads becomes 3 */
  dim_init();
  /* ----------------------------------------------------------------------- */
  /* define service name */
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/start",getSrvPrefix(),SRV_NAME);
  startCmdID=dis_add_cmnd(svcPath,"C",startCmdHandler,1);
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/kill",getSrvPrefix(),SRV_NAME);
  killCmdID=dis_add_cmnd(svcPath,"C",killCmdHandler,1);
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/stop",getSrvPrefix(),SRV_NAME);
  stopCmdID=dis_add_cmnd(svcPath,"C",stopCmdHandler,1);
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/list",getSrvPrefix(),SRV_NAME);
  shortListSvcID=dis_add_service(svcPath,"C",0,0,shortListSvcHandler,1);
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/longList",getSrvPrefix(),SRV_NAME);
  longListSvcID=dis_add_service(svcPath,"C",0,0,longListSvcHandler,1);
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/server_version",getSrvPrefix(),
           SRV_NAME);
  versionSvcID=dis_add_service(svcPath,"C",rcsidP,1+strlen(rcsidP),0,0);
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/actuator_version",getSrvPrefix(),
           SRV_NAME);
  aVersionSvcID=dis_add_service(svcPath,"C",aRcsidP,1+strlen(aRcsidP),0,0);
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/fmc_version",getSrvPrefix(),SRV_NAME);
  fmcVersionSvcID=dis_add_service(svcPath,"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/success",getSrvPrefix(),SRV_NAME);
  successSvcID=dis_add_service(svcPath,"I",&success,sizeof(int),0,0);
  /* ----------------------------------------------------------------------- */
  /* define DIM server error handler */
  dis_add_error_handler(disErrorHandler);
  /* ----------------------------------------------------------------------- */
  /* read access control list */
  if(authenticated)acl=readACL(ACL_FILE_NAME);
  /* ----------------------------------------------------------------------- */
  /* update process list */
  /* (before dis_start_serving() to avoid empty list at start-up) */
  procListUpdate();
  /* ----------------------------------------------------------------------- */
  /* Start DIM server */
  dis_start_serving(srvPath);
  /* ----------------------------------------------------------------------- */
  /* save the thread identifier of the main thread, to send it a SIGHUP      */
  /* using pthread_kill().                                                   */
  main_ptid=pthread_self();
  /* save the tid of the main thread, to avoid doing ptrace() on the         */
  /* current process (see tmUtils.c, findUtgid(), findTgid())                */
  main_tgid=getpid();
  /* ----------------------------------------------------------------------- */
  /* set Task Manager scheduler and priority */
  dim_set_scheduler_class(tmSchedPol);
  dim_set_priority(1,tmMainPrio);                            /* main thread  */
  dim_set_priority(2,tmIoPrio);                              /* IO thread    */
  dim_set_priority(3,tmTimerPrio);                           /* timer thread */
  /* ----------------------------------------------------------------------- */
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC Task Manager started. Using "
          "\"%s\", \"%s\", \"FMC-%s\". TGID=%d, srvPath=\"%s\"",rcsidP,aRcsidP,
          FMC_VERSION,getpid(),srvPath);
  /* ----------------------------------------------------------------------- */
  dis_update_service(versionSvcID);
  dis_update_service(aVersionSvcID);
  dis_update_service(fmcVersionSvcID);
  dis_update_service(successSvcID);
  dis_update_service(shortListSvcID);
  dis_update_service(longListSvcID);
  /* ----------------------------------------------------------------------- */
  /* main control loop                                                       */
  for(pU=0,tU=0;;)
  {
    /* sleep for updatePeriod seconds, but wake-up immediately if a blocked  */
    /* signal is received, to process it (synchronously)                     */
    /* blocked signals are SIGINT, SIGTERM, SIGCHLD and SIGHUP               */
    /* SIGHUP is sent by command handlers to require a list update           */
    updatePeriodTS.tv_sec=updatePeriod;
    updatePeriodTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&updatePeriodTS);
    if(deBug&0x1)
    {
      if(signo!=-1)
      {
        mPrintf(errU,DEBUG,__func__,0,"signo=%d (%s) received.",signo,
                sig2msg(signo));
      }
    }
    /* ..................................................................... */
    /* sigtimedwait interruped by an unblocked signal */
    if(signo==-1 && errno==EINTR)
    {
      continue;
    }
    /* ..................................................................... */
    /* sigtimedwait(2) timeout; no signal received thoughout updatePeriodTS */
    else if(signo==-1 && errno==EAGAIN)
    {
      if(deBug&0x1)
      {
        mPrintf(errU,DEBUG,__func__,0,"Periodic list service update");
      }
      lowPrioProcListUpdate(0);                /* update list in timer thread */
      pU++;
      if((pU+tU)%500==0 && (pU+tU)!=0)
        mPrintf(errU,DEBUG,__func__,0,"%llu updates: %llu periodic, %llu "
                "triggered.",pU+tU,pU,tU);
    }
    /* ..................................................................... */
    /* 1 or more terminated process(es) */
    else if(signo==SIGCHLD)
    {
      pid_t pid=-1;
      int status=-1;
      int listUpdateRequired=0;
      for(;;)
      {
        /* do not call waitpid() during ptrace(2) work */
        pthread_mutex_lock(&openprocLock);
        pid=waitpid(-1,&status,WNOHANG);
        pthread_mutex_unlock(&openprocLock);
        if(deBug&0x1)mPrintf(errU,DEBUG,__func__,0,"pid=%d collected.",pid);
        if(pid>0)
        {
#ifdef BENCHMARK
          /* for benchmark only */
          struct timeb now;
          double nowF=0.0;
          ftime(&now);
          nowF=(double)now.time+(double)now.millitm/1000.0;
          mPrintf(errU,WARN,__func__,0,"stopTime=%f",nowF);
#endif
          /* set process as terminated in psList and send a message */
          pLsetTerm(&psList,pid,status);
          if(WIFEXITED(status) || WIFSIGNALED(status))listUpdateRequired=1;
        }
        else break;
      }
      /* require list update in next cycle */
      if(deBug&0x1)mPrintf(errU,DEBUG,__func__,0,"listUpdateRequired=%d.",
                           listUpdateRequired);
      if(listUpdateRequired)pthread_kill(main_ptid,SIGHUP);
    }
    /* ..................................................................... */
    /* list update required */
    else if(signo==SIGHUP)
    {
      if(deBug&0x1)
        mPrintf(errU,DEBUG,__func__,0,"Triggered list service update");
      highPrioProcListUpdate();
      //lowPrioProcListUpdate(0);
      lowPrioProcListUpdate(1);
      tU++;
      if((pU+tU)%500==0 && (pU+tU)!=0)
        mPrintf(errU,DEBUG,__func__,0,"%llu updates: %llu periodic, %llu "
                "triggered.",pU+tU,pU,tU);
    }
    /* ..................................................................... */
    /* Task Manager Termination Required */
    else if(signo==SIGTERM||signo==SIGINT)
    {
      signalHandler(signo);
    }
    /* ..................................................................... */
    else
    {
      mPrintf(errU,WARN,__func__,0,"Signal %d (%s) caught. Continueing...",
              signo,sig2msg(signo));
    }
    /* ..................................................................... */
  }
  /* ----------------------------------------------------------------------- */
}
/* ************************************************************************* */
/* E.g.: cmnd="-d -u myps -c -w /bin ps -e -f"                               */
/* ************************************************************************* */
void startCmdHandler(long *tag,int *cmnd,int *size)
{
  int i=0;
  char *cmd=NULL;      /* cmnd - authentication string + "tmStart" prepended */
  /* related to argument parsing */
  char *argz=NULL;
  size_t argz_len=0;
  char *envz=NULL;
  size_t envz_len=0;
  char **rArgv=NULL;
  int rArgc=0;
  int flag;
  /* start parameters */
  int asDaemon,doClearenv,doRedirectStdout,doRedirectStderr,noDrop;
  char utgid[UTGID_LEN+1]="";
  char wd[WD_LEN+1]="";
  struct passwd *pw;
  struct group *gr;
  char *userName="";
  char *groupName="";
  char *outFIFO=NULL;
  char *errFIFO=NULL;
  uid_t uid=99;
  gid_t gid=99;
  int scheduler=SCHED_OTHER;
  int niceLevel=0;
  int rtPriority=0;
  cpu_set_t cpuSet;
  char *complPath=NULL;              /* complete path (wd + executable path) */
  int complPathLen=0;
  int oomScoreAdj;
  /* related to authentication */
  char *cmndP=(char*)cmnd;      /* then moved past the authentication string */
  int authStringLen=0;
  char authString[AUTH_STR_LEN+1]="";
  char *authUser=NULL;
  char *authHost=NULL;
  /* DIM command options */
  char *dimCmdOpts="+cD:s:p:r:a:k:n:g:du:w:oeO:E:A";
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  CPU_ZERO(&cpuSet);
  /* ----------------------------------------------------------------------- */
  if(*tag==1)
  {
    /* --------------------------------------------------------------------- */
    uid=dfltUid;
    gid=dfltGid;
    /* --------------------------------------------------------------------- */
    /* check authentication */
    if(authenticated)
    {
      int authorized;
      char *p;
      int i;
      cmndP=cmndP+strspn(cmndP," \t");                        /* skip blanks */
      authStringLen=strcspn(cmndP," \t");
      if(authStringLen>sizeof(authString)-1)authStringLen=sizeof(authString)-1;
      snprintf(authString,authStringLen+1,"%s",cmndP);
      cmndP=cmndP+strcspn(cmndP," \t");        /* skip authentication string */
      cmndP=cmndP+strspn(cmndP," \t");                        /* skip blanks */
      p=strchr(authString,'\f');
      if(!p)
      {
        mPrintf(errU,ERROR,__func__,0,"CMD: \"tmStart %s\": FORMAT error! "
                "Received authentication string \"%s\" has not the foreseen "
                "format \"user@host\"!",cmndP,authString);
        printOutFuncNOK;
        return;
      }
      *p='\0';
      authHost=p+1;
      authUser=authString;
      if(deBug&0x4)
        mPrintf(errU,VERB,__func__,0,"Received authentication string: "
                "\"%s@%s\".",authUser,authHost);
      if(authUser[0]=='\0'||authHost[0]=='\0')
      {
        mPrintf(errU,ERROR,__func__,0,"CMD: \"tmStart %s\": FORMAT error! "
                "Received authentication string \"%s@%s\" has not the "
                "foreseen format \"user@host\"!",cmndP,authUser,authHost);
        printOutFuncNOK;
        return;
      }
      for(i=0,authorized=0;i<acl.entryN;i++)
      {
        if(!fnmatch(acl.userNameList[i],authUser,0) &&
           !fnmatch(acl.hostNameList[i],authHost,0))
        {
          authorized=1;
          break;
        }
      }
      if(!authorized)
      {
         mPrintf(errU,ERROR,__func__,0,"CMD: \"tmStart %s\" REFUSED! User "
                 "\"%s\" at host \"%s\" is NOT AUTHORIZED to send commands "
                 "to the Task Manager running on this node!",cmndP,authUser,
                 authHost);
        printOutFuncNOK;
        return;
      }
    }                                                    /* if authenticated */
    else                                             /* if not authenticated */
    {
      if(strchr(cmndP,'\f'))                  /* authentication string found */
      {
        /* skip authentication string */
        cmndP=cmndP+strspn(cmndP," \t");                      /* skip blanks */
        cmndP=cmndP+strcspn(cmndP," \t");      /* skip authentication string */
        cmndP=cmndP+strspn(cmndP," \t");                      /* skip blanks */
      }                                    /* if authentication string found */
    }                                                /* if not authenticated */
    /* --------------------------------------------------------------------- */
    /* prepend a dummy string to command for getopt() */
    asprintf(&cmd,"tmStart %s",cmndP);
    /* --------------------------------------------------------------------- */
    if(authenticated)
    {
      mPrintf(errU,DEBUG,__func__,0,"Received CMD: \"%s\" from user: "
              "\"%s\" at host: \"%s\".",(char*)cmd,authUser,authHost);
    }
    else                                                   /* !authenticated */
    {
      mPrintf(errU,DEBUG,__func__,0,"Received CMD: \"%s\".",(char*)cmd);
    }
    /* --------------------------------------------------------------------- */
    /* convert the command string in an argz vector */
    if(argz_create_sep(cmd,32,&argz,&argz_len))
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": argz_create_sep(): %s!",cmd,
              strerror(errno));
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* --------------------------------------------------------------------- */
    /* convert the argz vector into argv format */
    rArgc=argz_count(argz,argz_len);
    rArgv=(char**)malloc((rArgc+1)*sizeof(char*));
    argz_extract(argz,argz_len,rArgv);
    /* --------------------------------------------------------------------- */
    /* default values */
    asDaemon=0;
    doClearenv=0;
    doRedirectStdout=0;
    doRedirectStderr=0;
    noDrop=0;
    oomScoreAdj=0;
    /* --------------------------------------------------------------------- */
    /* process DIM command options */
    optind=0;
    opterr=0;                /* do not print default error message to stderr */
    while((flag=getopt(rArgc,rArgv,dimCmdOpts))!=EOF)
    {
      switch(flag)
      {
        /* ................................................................. */
        /* Process Environment Options */
        case 'c':
          doClearenv=1;
          break;
        case 'D':
          envz_add(&envz,&envz_len,optarg,NULL);
          break;
        /* ................................................................. */
        /* Process Scheduling Options */
        case 's':
          scheduler=(int)strtol(optarg,(char**)NULL,0);
          break;
        case 'p':
          niceLevel=(int)strtol(optarg,(char**)NULL,0);
          break;
        case 'r':
          rtPriority=(int)strtol(optarg,(char**)NULL,0);
          break;
        case 'a':
          CPU_SET((int)strtol(optarg,(char**)NULL,0), &cpuSet);
          break;
        /* ................................................................. */
        /* Process OOM-killer Options */
        case 'k':
          oomScoreAdj=(int)strtol(optarg,(char**)NULL,0);
          break;
        /* ................................................................. */
        /* Process Owner Options */
        case 'n':
          userName=optarg;
          break;
        case 'g':
          groupName=optarg;
          break;
        /* ................................................................. */
        /* Process Output Redirection Options */
        case 'o':
          doRedirectStdout=1;
          break;
        case 'e':
          doRedirectStderr=1;
          break;
        case 'O':
          doRedirectStdout=1;
          outFIFO=optarg;
          break;
        case 'E':
          doRedirectStderr=1;
          errFIFO=optarg;
          break;
        case 'A':
          noDrop=1;
          break;
        /* ................................................................. */
        /* Other Process Options */
        case 'd':
          asDaemon=1;
          break;
        case 'u':
          snprintf(utgid,UTGID_LEN+1,"%s",optarg);
          break;
        case 'w':
          snprintf(wd,WD_LEN+1,"%s",optarg);
          break;
        /* ................................................................. */
        case '?':
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): "
                  "Invalid option \"-%c\"!",cmd,optopt);
          FREE_START_CMD_HANDLER_HEAP;
          pthread_kill(main_ptid,SIGHUP);             /* require list update */
          printOutFuncNOK;
          return;
        /* ................................................................. */
        default:
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): "
                  "Error: %d!",cmd,optopt);
          FREE_START_CMD_HANDLER_HEAP;
          pthread_kill(main_ptid,SIGHUP);             /* require list update */
          printOutFuncNOK;
          return;
        /* ................................................................. */
      }
    }
    /* --------------------------------------------------------------------- */
    /* check command line arguments */
    if(optind==rArgc)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Missing program path!",cmd);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* check if path is executable */
    if(wd[0]=='\0')
    {
      complPathLen=strlen(rArgv[optind]);
      complPath=(char*)realloc(complPath,1+complPathLen);
      strcpy(complPath,rArgv[optind]);
    }
    else
    {
      complPathLen=strlen(wd)+1+strlen(rArgv[optind]);
      complPath=(char*)realloc(complPath,1+complPathLen);
      sprintf(complPath,"%s/%s",wd,rArgv[optind]);
    }
    if(access(complPath,X_OK)==-1)                          /* access denied */
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Can't execute path: \"%s\": "
              "%s!",cmd,complPath,strerror(errno));
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* --------------------------------------------------------------------- */
    /* check command line options */
    if(CPU_COUNT(&cpuSet) == 0)
    {
      if(deBug&0x8)
      {
        mPrintf(errU,DEBUG,__func__,0,"CMD: \"%s\": No affinity option "
                "specified. Setting affinity mask to (all processor allowed).",cmd);
      }
      for(i=0;i<CPU_SETSIZE;i++)
      {
         CPU_SET(i, &cpuSet);
      }
    }
    if(scheduler<SCHED_OTHER || scheduler>SCHED_RR)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Unknown scheduler %d! Only "
              "values SCHED_OTHER=0, SCHED_FIFO=1 and SCHED_RR=2 are allowed.",
              cmd,scheduler);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if(scheduler==SCHED_OTHER && rtPriority!=0)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Real-time priority %d not "
              "allowed for SCHED_OTHER time sharing scheduler! Only 0 "
              "real-time priority allowed for SCHED_OTHER time sharing "
              "scheduler!",cmd,rtPriority);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if((scheduler==SCHED_FIFO||scheduler==SCHED_RR)&&
       (rtPriority<1||rtPriority>99))
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Real-time priority %d not "
              "allowed for %s real-time scheduler! Only 1..99 real-time "
              "priority allowed for %s real-rime scheduler!",cmd,rtPriority,
              ss[scheduler],ss[scheduler]);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if((scheduler==SCHED_FIFO||scheduler==SCHED_RR)&&
       !(rtPriority<tmIoPrio))
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Task Manager cannot start a "
              "process with real-time priority %d, which is greater than or "
              "equal to the real-time priority of the Task Manager's I/O "
              "thread priority (%d)!",cmd,rtPriority,tmIoPrio);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if(niceLevel<-20 || niceLevel>19)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Nice level %d not allowed! "
              "Only values -20..19 allowed for nice level!",cmd,niceLevel);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if(oomScoreAdj<-999 || oomScoreAdj>1000)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": OOM Score Adjustment %d not "
              "allowed! Only values -999..1000 allowed for OOM Score "
              "Adjustment!",cmd,oomScoreAdj);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* --------------------------------------------------------------------- */
    /* set user and group */
    if(strcmp(userName,""))                            /* username specified */
    {
      pw=getpwnam(userName);
      if(pw==NULL)               /* user does not exist or getpwnam(3) error */
      {
        if(errno==ENOMEM||errno==EIO||errno==EINTR||errno==EMFILE||
           errno==ENFILE)
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getpwnam(%s): %s!",cmd,
                  userName,strerror(errno));
        else
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Username \"%s\" is not "
                  "defined on node: \"%s\"!",cmd,userName,nodeName);
        FREE_START_CMD_HANDLER_HEAP;
        pthread_kill(main_ptid,SIGHUP);               /* require list update */
        printOutFuncNOK;
        return;
      }
      else
      {
        uid=pw->pw_uid;
        gid=pw->pw_gid;
      }
    }
    if(strcmp(groupName,""))                         /* group name specified */
    {
      gr=getgrnam(groupName);
      if(gr==NULL)              /* group does not exist or getgrnam(3) error */
      {
        if(errno==ENOMEM||errno==EIO||errno==EINTR||errno==EMFILE||
           errno==ENFILE)
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getgrnam(%s): %s!",cmd,
                  groupName,strerror(errno));
        else
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Group name \"%s\" is "
                  "not defined on node: \"%s\"!",cmd,groupName,nodeName);
        FREE_START_CMD_HANDLER_HEAP;
        pthread_kill(main_ptid,SIGHUP);               /* require list update */
        printOutFuncNOK;
        return;
      }
      else
      {
        gid=gr->gr_gid;
      }
    }
    /* --------------------------------------------------------------------- */
    /* check user and group */
    if(permission==0 && uid!=dfltUid)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": User \"%s\" is not allowed "
              "because tmSrv has been started with \"-p 0\" option! Only "
              "user \"%s\" is allowed!",cmd,userName,dfltUserName);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if(permission==1 && uid==0)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": User \"%s\" is not allowed "
              "because tmSrv has been started with \"-p 1\" option! Only "
              "users different from \"%s\" are allowed!",cmd,userName,
              userName);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if(permission==0 && gid!=dfltGid)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Group \"%s\" is not allowed "
              "because tmSrv has been started with \"-p 0\" option! Only "
              "group \"%s\" is allowed!",cmd,groupName,dfltGroupName);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if(permission==1 && gid==0)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Group \"%s\" is not allowed "
              "because tmSrv has been started with \"-p 1\" option! Only "
              "groups different from \"%s\" are allowed!",cmd,groupName,
              groupName);
      FREE_START_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* --------------------------------------------------------------------- */
    startPs(asDaemon,doClearenv,doRedirectStdout,doRedirectStderr,outFIFO,
            errFIFO,noDrop,uid,gid,scheduler,niceLevel,rtPriority,&cpuSet,
            oomScoreAdj,utgid,wd,rArgv[optind],rArgv+optind,envz,envz_len);
    /* --------------------------------------------------------------------- */
    FREE_START_CMD_HANDLER_HEAP;
    /* --------------------------------------------------------------------- */
    /* update list service */
    pthread_kill(main_ptid,SIGHUP);                   /* require list update */
    /* --------------------------------------------------------------------- */
  }
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
/* E.g.: cmnd="-s 2 contatore*"                                              */
/* ************************************************************************* */
void killCmdHandler(long *tag,int *cmnd,int *size)
{
  char *cmd=NULL;
  /* related to argument parsing */
  char *argz=NULL;
  size_t argz_len=0;
  char **rArgv=NULL;
  int rArgc=0;
  int flag;
  /* kill parameters */
  unsigned signo=15;                                      /* default SIGTERM */
  /* related to authentication */
  char *cmndP=(char*)cmnd;      /* then moved past the authentication string */
  int authStringLen=0;
  char authString[AUTH_STR_LEN+1]="";
  char *authUser=NULL;
  char *authHost=NULL;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  if(*tag==1)
  {
    /* --------------------------------------------------------------------- */
    /* check authentication */
    if(authenticated)
    {
      int authorized;
      char *p;
      int i;
      cmndP=cmndP+strspn(cmndP," \t");                        /* skip blanks */
      authStringLen=strcspn(cmndP," \t");
      if(authStringLen>sizeof(authString)-1)authStringLen=sizeof(authString)-1;
      snprintf(authString,authStringLen+1,"%s",cmndP);
      cmndP=cmndP+strcspn(cmndP," \t");        /* skip authentication string */
      cmndP=cmndP+strspn(cmndP," \t");                        /* skip blanks */
      p=strchr(authString,'\f');
      if(!p)
      {
        mPrintf(errU,ERROR,__func__,0,"CMD: \"tmKill %s\": FORMAT error! "
                "Received authentication string \"%s\" has not the foreseen "
                "format \"user@host\"!",cmndP,authString);
        printOutFuncNOK;
        return;
      }
      *p='\0';
      authHost=p+1;
      authUser=authString;
      if(deBug&0x4)
        mPrintf(errU,VERB,__func__,0,"Received authentication string: "
                "\"%s@%s\".",authUser,authHost);
      if(authUser[0]=='\0'||authHost[0]=='\0')
      {
        mPrintf(errU,ERROR,__func__,0,"CMD: \"tmKill %s\": FORMAT error! "
                "Received authentication string \"%s@%s\" has not the "
                "foreseen format \"user@host\"!",cmndP,authUser,authHost);
        printOutFuncNOK;
        return;
      }
      for(i=0,authorized=0;i<acl.entryN;i++)
      {
        if(!fnmatch(acl.userNameList[i],authUser,0) &&
           !fnmatch(acl.hostNameList[i],authHost,0))
        {
          authorized=1;
          break;
        }
      }
      if(!authorized)
      {
         mPrintf(errU,ERROR,__func__,0,"CMD: \"tmKill %s\" REFUSED! User "
                 "\"%s\" at host \"%s\" is NOT AUTHORIZED to send commands "
                 "to the Task Manager running on this node!",cmndP,authUser,
                 authHost);
        printOutFuncNOK;
        return;
      }
    }                                                    /* if authenticated */
    else                                             /* if not authenticated */
    {
      if(strchr(cmndP,'\f'))                  /* authentication string found */
      {
        /* skip authentication string */
        cmndP=cmndP+strspn(cmndP," \t");                      /* skip blanks */
        cmndP=cmndP+strcspn(cmndP," \t");      /* skip authentication string */
        cmndP=cmndP+strspn(cmndP," \t");                      /* skip blanks */
      }                                    /* if authentication string found */
    }                                                /* if not authenticated */
    /* --------------------------------------------------------------------- */
    /* prepend a dummy string to command for getopt() */
    asprintf(&cmd,"tmKill %s",(char*)cmndP);
    /* --------------------------------------------------------------------- */
    if(authenticated)
    {
      mPrintf(errU,DEBUG,__func__,0,"Received CMD: \"%s\" from user: "
              "\"%s\" at host: \"%s\".",(char*)cmd,authUser,authHost);
    }
    else                                                   /* !authenticated */
    {
      mPrintf(errU,DEBUG,__func__,0,"Received CMD: \"%s\".",(char*)cmd);
    }
    /* --------------------------------------------------------------------- */
    /* convert the command string in an argz vector */
    if(argz_create_sep(cmd,32,&argz,&argz_len))
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": argz_create_sep(): %s!",cmd,
              strerror(errno));
      FREE_KILL_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* --------------------------------------------------------------------- */
    /* convert the argz vector into argv format */
    rArgc=argz_count(argz,argz_len);
    rArgv=(char**)malloc((rArgc+1)*sizeof(char*));
    argz_extract(argz,argz_len,rArgv);
    /* --------------------------------------------------------------------- */
    /* default values */
    signo=15;                                             /* default SIGTERM */
    /* --------------------------------------------------------------------- */
    /* process DIM command arguments */
    optind=0;
    opterr=0;
    while((flag=getopt(rArgc,rArgv,"+s:"))!=EOF)
    {
      switch(flag)
      {
        case 's':
          signo=(unsigned)strtoul(optarg,(char**)NULL,0);
          break;
        default:
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid "
                  "option \"-%c\"!",cmd,optopt);
          FREE_KILL_CMD_HANDLER_HEAP;
          pthread_kill(main_ptid,SIGHUP);             /* require list update */
          printOutFuncNOK;
          return;
      }
    }
    /* --------------------------------------------------------------------- */
    /* check command line arguments */
    if(optind==rArgc)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Missing UTGID pattern!",cmd);
      FREE_KILL_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* --------------------------------------------------------------------- */
    killPs(rArgv[optind],signo);
    /* --------------------------------------------------------------------- */
    FREE_KILL_CMD_HANDLER_HEAP;
    /* --------------------------------------------------------------------- */
    /* update list service */
    pthread_kill(main_ptid,SIGHUP);                   /* require list update */
    /* --------------------------------------------------------------------- */
    printOutFuncOK;
    return;
  }
}
/* ************************************************************************* */
/* E.g.: cmnd="-s 2 -d 4 contatore*"                                         */
/* ************************************************************************* */
void stopCmdHandler(long *tag,int *cmnd,int *size)
{
  char *cmd=NULL;
  /* related to argument parsing */
  char *argz=NULL;
  size_t argz_len=0;
  char **rArgv=NULL;
  int rArgc=0;
  int flag;
  /* stop parameters */
  unsigned signo=15;                                      /* default SIGTERM */
  unsigned s9delay=1;
  /* data for new thread */
  finishOffPsData_t *td=NULL;
  /* related to authentication */
  char *cmndP=(char*)cmnd;      /* then moved past the authentication string */
  int authStringLen=0;
  char authString[AUTH_STR_LEN+1]="";
  char *authUser=NULL;
  char *authHost=NULL;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  if(*tag==1)
  {
    /* --------------------------------------------------------------------- */
    /* check authentication */
    if(authenticated)
    {
      int authorized;
      char *p;
      int i;
      cmndP=cmndP+strspn(cmndP," \t");                        /* skip blanks */
      authStringLen=strcspn(cmndP," \t");
      if(authStringLen>sizeof(authString)-1)authStringLen=sizeof(authString)-1;
      snprintf(authString,authStringLen+1,"%s",cmndP);
      cmndP=cmndP+strcspn(cmndP," \t");        /* skip authentication string */
      cmndP=cmndP+strspn(cmndP," \t");                        /* skip blanks */
      p=strchr(authString,'\f');
      if(!p)
      {
        mPrintf(errU,ERROR,__func__,0,"CMD: \"tmStop %s\": FORMAT error! "
                "Received authentication string \"%s\" has not the foreseen "
                "format \"user@host\"!",cmndP,authString);
        printOutFuncNOK;
        return;
      }
      *p='\0';
      authHost=p+1;
      authUser=authString;
      if(deBug&0x4)
        mPrintf(errU,VERB,__func__,0,"Received authentication string: "
                "\"%s@%s\".",authUser,authHost);
      if(authUser[0]=='\0'||authHost[0]=='\0')
      {
        mPrintf(errU,ERROR,__func__,0,"CMD: \"tmStop %s\": FORMAT error! "
                "Received authentication string \"%s@%s\" has not the "
                "foreseen format \"user@host\"!",cmndP,authUser,authHost);
        printOutFuncNOK;
        return;
      }
      for(i=0,authorized=0;i<acl.entryN;i++)
      {
        if(!fnmatch(acl.userNameList[i],authUser,0) &&
           !fnmatch(acl.hostNameList[i],authHost,0))
        {
          authorized=1;
          break;
        }
      }
      if(!authorized)
      {
         mPrintf(errU,ERROR,__func__,0,"CMD: \"tmStop %s\" REFUSED! User "
                 "\"%s\" at host \"%s\" is NOT AUTHORIZED to send commands "
                 "to the Task Manager running on this node!",cmndP,authUser,
                 authHost);
        printOutFuncNOK;
        return;
      }
    }                                                    /* if authenticated */
    else                                             /* if not authenticated */
    {
      if(strchr(cmndP,'\f'))                  /* authentication string found */
      {
        /* skip authentication string */
        cmndP=cmndP+strspn(cmndP," \t");                      /* skip blanks */
        cmndP=cmndP+strcspn(cmndP," \t");      /* skip authentication string */
        cmndP=cmndP+strspn(cmndP," \t");                      /* skip blanks */
      }                                    /* if authentication string found */
    }                                                /* if not authenticated */
    /* --------------------------------------------------------------------- */
    /* prepend a dummy string to command for getopt() */
    asprintf(&cmd,"tmStop %s",(char*)cmndP);
    /* --------------------------------------------------------------------- */
    if(authenticated)
    {
      mPrintf(errU,DEBUG,__func__,0,"Received CMD: \"%s\" from user: "
              "\"%s\" at host: \"%s\".",(char*)cmd,authUser,authHost);
    }
    else                                                   /* !authenticated */
    {
      mPrintf(errU,DEBUG,__func__,0,"Received CMD: \"%s\".",(char*)cmd);
    }
    /* --------------------------------------------------------------------- */
    /* convert the command string in an argz vector */
    if(argz_create_sep(cmd,32,&argz,&argz_len))
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": argz_create_sep(): %s!",cmd,
              strerror(errno));
      FREE_STOP_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* --------------------------------------------------------------------- */
    /* convert the argz vector into argv format */
    rArgc=argz_count(argz,argz_len);
    rArgv=(char**)malloc((rArgc+1)*sizeof(char*));
    argz_extract(argz,argz_len,rArgv);
    /* --------------------------------------------------------------------- */
    /* default values */
    signo=15;                                             /* default SIGTERM */
    /* --------------------------------------------------------------------- */
    /* process DIM command arguments */
    optind=0;
    opterr=0;
    while((flag=getopt(rArgc,rArgv,"+s:d:"))!=EOF)
    {
      switch(flag)
      {
        case 'd':
          s9delay=(unsigned)strtoul(optarg,(char**)NULL,0);;
          break;
        case 's':
          signo=(unsigned)strtoul(optarg,(char**)NULL,0);;
          break;
        default:
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid "
                  "option \"-%c\"!",cmd,optopt);
          FREE_STOP_CMD_HANDLER_HEAP;
          pthread_kill(main_ptid,SIGHUP);             /* require list update */
          printOutFuncNOK;
          return;
      }
    }
    /* --------------------------------------------------------------------- */
    /* check command line arguments */
    if(optind==rArgc)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": Missing UTGID pattern!",cmd);
      FREE_STOP_CMD_HANDLER_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* --------------------------------------------------------------------- */
    killPs(rArgv[optind],signo);
    /* --------------------------------------------------------------------- */
    /* update list service */
    pthread_kill(main_ptid,SIGHUP);                   /* require list update */
    /* --------------------------------------------------------------------- */
    /* set-up a new thread for finishOffPs() */
    /* thread argument */
    td=(finishOffPsData_t*)malloc(sizeof(finishOffPsData_t));
    snprintf(td->utgidPattern,sizeof(td->utgidPattern),"%s",rArgv[optind]);
    if(!memchr(td->utgidPattern,0,sizeof(td->utgidPattern)))
      td->utgidPattern[sizeof(td->utgidPattern)-1]='\0';
    td->s9delay=s9delay;
    td->sentSigno=signo;
    FREE_STOP_CMD_HANDLER_HEAP;    /* free-up m-allocated memory (except td) */
    /* thread attributes */
    if(pthread_attr_init(&t_attr)==-1)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": pthread_attr_init(): %s!",
              cmd,strerror(errno));
      FREE_FINISH_OFF_PS_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if(pthread_attr_setinheritsched(&t_attr,PTHREAD_INHERIT_SCHED)==-1)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": "
              "pthread_attr_setinheritsched(): %s!",cmd,strerror(errno));
      FREE_FINISH_OFF_PS_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if(pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_DETACHED)==-1)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": "
              "pthread_attr_setdetachstate(): %s!",cmd,strerror(errno));
      FREE_FINISH_OFF_PS_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* create the new thread for finishOffPs() */
    if(pthread_create(&tid,&t_attr,finishOffPs,td)==-1)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": pthread_create(): %s!",cmd,
              strerror(errno));
      FREE_FINISH_OFF_PS_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    if(pthread_detach(tid)==-1)
    {
      mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": pthread_detach(): %s!",cmd,
              strerror(errno));
      FREE_FINISH_OFF_PS_HEAP;
      pthread_kill(main_ptid,SIGHUP);                 /* require list update */
      printOutFuncNOK;
      return;
    }
    /* --------------------------------------------------------------------- */
    pthread_kill(main_ptid,SIGHUP);                   /* require list update */
    printOutFuncOK;
    return;
  }
}
/* ************************************************************************* */
/* Finisches off processes which are still alive after the first kill.       */
/* Deferred-call by stopCmdHandler() running in a new thread.               */
/* ************************************************************************* */
void *finishOffPs(void *data)
{
  struct timespec delay={1,0};
  finishOffPsData_t *td=NULL;
  pssinfo pss={0,NULL};
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  pthread_detach(pthread_self());
  /* ----------------------------------------------------------------------- */
  td=(finishOffPsData_t*)data;
  /* ----------------------------------------------------------------------- */
  /* sleep for s9delay seconds */
  delay.tv_sec=td->s9delay;
  nanosleep(&delay,NULL);                /* POSIX.4 call: sleep w/out signal */
  /* ----------------------------------------------------------------------- */
  pss=findUtgid(td->utgidPattern);
  if(pss.psN==0)
  {
    pthread_kill(main_ptid,SIGHUP);                   /* require list update */
    freePssinfo(&pss);
    FREE_FINISH_OFF_PS_HEAP;
    printOutFuncNOK;
    return NULL;
  }
  mPrintf(errU,WARN,__func__,0,"%u process(es) still alive after signal %u "
          "(%s)! Sending signal %u (%s) to them...",pss.psN,td->sentSigno,
          sig2msg(td->sentSigno),9,sig2msg(9));
  freePssinfo(&pss);
  /* ----------------------------------------------------------------------- */
  killPs(td->utgidPattern,SIGKILL);
  delay.tv_sec=1;
  nanosleep(&delay,NULL);                /* POSIX.4 call: sleep w/out signal */
  /* ----------------------------------------------------------------------- */
  pss=findUtgid(td->utgidPattern);
  if(pss.psN==0)
  {
    pthread_kill(main_ptid,SIGHUP);                   /* require list update */
    freePssinfo(&pss);
    FREE_FINISH_OFF_PS_HEAP;
    printOutFuncNOK;
    return NULL;
  }
  mPrintf(errU,ERROR,__func__,0,"%u process(es) still alive after signal %u "
          "(%s)!",pss.psN,9,sig2msg(9));
  pthread_kill(main_ptid,SIGHUP);                     /* require list update */
  freePssinfo(&pss);
  FREE_FINISH_OFF_PS_HEAP;
  printOutFuncOK;
  pthread_exit(NULL);
}
/* ************************************************************************* */
void doProcListUpdate(long tag)
{
  printInFunc;
  procListUpdate();
  dis_update_service(shortListSvcID);
  dis_update_service(longListSvcID);
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
void highPrioProcListUpdate(void)
{
  printInFunc;
  doProcListUpdate(1);
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
void lowPrioProcListUpdate(int delay)
{
  printInFunc;
  dtq_start_timer(delay,doProcListUpdate,1);
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
void procListUpdate(void)
{
  pssinfo pss={0,NULL};
  int i;
  procListEl_t oldPs;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  if(deBug&0x1)mPrintf(errU,DEBUG,__func__,0,"Updating UTGID list");
  /* ----------------------------------------------------------------------- */
  /* get UTGID list */
  pss=findUtgid("*");
  /* update process list */
  for(i=0;i<pss.psN;i++)
  {
    /* add or replace element in psList */
    oldPs=pLadd(&psList,pss.ps[i].tgid,strdup(pss.ps[i].cmd),
                strdup(pss.ps[i].utgid),pss.ps[i].oomScore);
    if(oldPs.tgid!=-1)                               /* old element replaced */
    {
      if(oldPs.cmd)free(oldPs.cmd);
      if(oldPs.utgid)free(oldPs.utgid);
    }
  }
  /* set disappeared processes in psList as terminated */
  pLautoSetTerm(&psList,updatePeriod);
  /* remove from psList procs terminated since more than defunctPersistence */
  for(;;)
  {
    oldPs=pLrmTerm(&psList,defunctPersistence);
    if(oldPs.tgid==-1)break;
    if(deBug&0x40)
    {
      char *cmd=oldPs.cmd;
      char *utgid=oldPs.utgid;
      struct tm lastUpdateL;
      char lastUpdateS[13];                                /* Oct12-134923\0 */
      if(!cmd)cmd="unknown";
      if(!utgid)utgid="unknown";
      localtime_r(&oldPs.lastUpdate,&lastUpdateL);
      strftime(lastUpdateS,13,"%b%d-%H%M%S",&lastUpdateL);
      mPrintf(errU,VERB,__func__,0,"Removed timeouted element: "
              "(%d,\"%s\",\"%s\",\"%s\")",oldPs.tgid,cmd,utgid,
              lastUpdateS);
    }
    if(oldPs.cmd)free(oldPs.cmd);
    if(oldPs.utgid)free(oldPs.utgid);
  }
  if(deBug&0x40)
  {
    mPrintf(errU,DEBUG,__func__,0,"List length: %d",pLlen(&psList));
    pLprint(&psList,VERB);
  }
  /* ----------------------------------------------------------------------- */
  freePssinfo(&pss);
  /* ----------------------------------------------------------------------- */
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
void shortListSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  if(*tag==1)
  {
    /* get the psList process list */
    pLlsSvc(&psList,(char**)address,size);
  }
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
void longListSvcHandler(long *tag,int **address,int *size)
{
  printInFunc;
  if(*tag==1)
  {
    /* get the psList process list */
    pLllSvc(&psList,(char**)address,size);
  }
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
void disErrorHandler(int severity,int errorCode,char *message)
{
  printInFunc;
  mPrintf(errU|L_SYS,severity+3,__func__,0,"%s! (error code = %d)",
          message,errorCode);
  if(severity==3)abort();
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
/* signalHandler() executed synchronously in the main thread. So don't warry */
/* about async-signal-safe.                                                  */
static void signalHandler(int signo)
{
  printInFunc;
  /* ----------------------------------------------------------------------- */
  dis_stop_serving();
  dis_remove_service(startCmdID);
  dis_remove_service(killCmdID);
  dis_remove_service(stopCmdID);
  dis_remove_service(shortListSvcID);
  dis_remove_service(longListSvcID);
  dis_remove_service(versionSvcID);
  /* ----------------------------------------------------------------------- */
  if(signo==SIGTERM || signo==SIGINT)
  {
    mPrintf(errU|L_SYS,WARN,__func__,0,"Signal %d (%s) caught. "
            "Exiting...",signo,sig2msg(signo));
    printOutFuncOK;
    _exit(0);
  }
  else
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"Signal %d (%s) caught. "
            "Exiting...",signo,sig2msg(signo));
    printOutFuncOK;
    _exit(signo);
  }
}
/* ************************************************************************* */
accessControlList_t parseACL(char *aclString)
{
  /* returned Access Control List structure */
  accessControlList_t acl={0,NULL,NULL};
  /*  Access Control List string and its length */
  char *s=NULL;
  int sl=0;
  /*  Access Control List item and its length */
  char *itemS=NULL;
  size_t itemSL;
  /*  username and its length */
  char *userS=NULL;
  size_t userSL;
  /*  hostname and its length */
  char *hostS=NULL;
  size_t hostSL;
  /* counters */
  int i;
  /* pointers */
  char *sp,*itemSP;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  if(aclString==0||aclString[0]=='\0')
  {
    printOutFuncOK;
    return acl;
  }
  s=strdup(aclString);
  sl=strlen(s);
  for(i=0;i<sl;i++)if(s[i]=='\r')s[i]=' ';         /* replace \r with spaces */
  for(i=0;i<sl;i++)if(s[i]=='\n')s[i]=';';              /* replace \n with ; */
  if(s[sl-1]=='\n' || s[sl-1]==';')s[sl-1]=' ';  /* suppress last empty line */
  /* ----------------------------------------------------------------------- */
  /* split ACL string in records */
  for(sp=s,i=0;;i++)
  {
    /* ..................................................................... */
    itemSL=strcspn(sp,";");
    itemS=(char*)realloc(itemS,itemSL+1);
    snprintf(itemS,itemSL+1,"%s",sp);
    itemSP=itemS+strspn(itemS," \t");                         /* skip blanks */
    if(deBug&0x4)
    {
      mPrintf(errU,VERB,__func__,0,"ACL record %3d: \"%s\".",i,itemS);
    }
    if(itemSP[0]=='\0')                                  /* skip blank lines */
    {
      if(deBug&0x4)
      {
        mPrintf(errU,VERB,__func__,0,"ACL record %d skipped (blank record).",i);
      }
    }
    else if(itemSP[0]=='#')                            /* skip comment lines */
    {
      if(deBug&0x4)
      {
        mPrintf(errU,VERB,__func__,0,"ACL record %d: \"%s\" skipped (comment "
                "record).",i,itemS);
      }
    }
    else if(!strchr(itemSP,'@'))
    {
      mPrintf(errU,ERROR,__func__,0,"ACL record %d: \"%s\" skipped (has not "
              "the foreseen format user_pattern@host_pattern)!",i,itemS);
    }
    else
    {
      /* split ACL record in 2 fields */
      userSL=strcspn(itemSP,"@ ");
      userS=(char*)realloc(userS,userSL+1);
      snprintf(userS,userSL+1,"%s",itemSP);
      itemSP=1+strchr(itemSP,'@');
      itemSP=itemSP+strspn(itemSP," \t");                     /* skip blanks */
      hostSL=strcspn(itemSP," ;");
      hostS=(char*)realloc(hostS,hostSL+1);
      snprintf(hostS,hostSL+1,"%s",itemSP);
      if(!userSL || !hostSL)
      {
        mPrintf(errU,ERROR,__func__,0,"ACL record %d: \"%s\" skipped (has "
                "not the foreseen format user_pattern@host_pattern)!",i,
                itemS);
      }
      else
      {
        acl.entryN++;
        acl.userNameList=(char**)realloc(acl.userNameList,
                                         acl.entryN*sizeof(char*));
        acl.hostNameList=(char**)realloc(acl.hostNameList,
                                         acl.entryN*sizeof(char*));
        acl.userNameList[acl.entryN-1]=strdup(userS);
        acl.hostNameList[acl.entryN-1]=strdup(hostS);
      }
    }
    /* ..................................................................... */
    sp+=(itemSL+1);
    if(sp>s+sl)break;
    /* ..................................................................... */
  }
  /* ----------------------------------------------------------------------- */
  printOutFuncOK;
  return acl;
}
/* ************************************************************************* */
accessControlList_t readACL(char *aclFileName)
{
  /* file descriptors */
  int aclFileFD=-1;
  /* file state */
  struct stat aclFileStat;
  /* string containing the access control list to parse */
  char *aclString=NULL;
  ssize_t aclStringLen=0;
  size_t aclStringBufLen=0;
  /* returned access control list */
  accessControlList_t acl={0,NULL,NULL};
  /* counters */
  int i;
  /* pointers */
  char *p;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  mPrintf(errU,DEBUG,__func__,0,"Trying to read Access Control List file: "
          "\"%s\"...",aclFileName);
  /* check acl file  accessibility */
  if(access(aclFileName,R_OK)==-1)
  {
    mPrintf(errU,FATAL,__func__,0,"Access Control List file: \"%s\" not "
            "accessible. access(2) error: %s!",aclFileName,strerror(errno));
    exit(1);
  }
  /* get configuration file size */
  if(lstat(aclFileName,&aclFileStat)<0)
  {
    mPrintf(errU,FATAL,__func__,0,"Can't get info about file: \"%s\". "
            "stat(2) error: %s!",aclFileName,strerror(errno));
    exit(1);
  }
  if(deBug&0x4)
  {
    mPrintf(errU,VERB,__func__,0,"Size of file \"%s\": %d B.",aclFileName,
            (int)aclFileStat.st_size);
  }
  /* allocate buffer to store aclFileName */
  aclStringBufLen=aclFileStat.st_size*1.1;
  aclString=(char*)malloc(aclStringBufLen*sizeof(char));
  /* read aclFileName */
  aclFileFD=open(aclFileName,O_RDONLY);
  aclStringLen=read(aclFileFD,aclString,aclStringBufLen);
  close(aclFileFD);
  if(aclStringLen<aclFileStat.st_size)
  {
    mPrintf(errU,FATAL,__func__,0,"Problem reading Access Control List file "
            "\"%s\": %d B read out of %d B!",aclFileName,aclStringLen,
            aclFileStat.st_size);
    exit(1);
  }
  for(p=aclString;*p;p++)if(*p=='\r')*p=' ';
  for(p=aclString;*p;p++)if(*p=='\n')*p=';';
  if(deBug&0x4)
  {
    mPrintf(errU,VERB,__func__,0,"Access Control List string retieved from "
            "the file \"%s\": \"%s\".",aclFileName,aclString);
  }
  /* parse aclFileName */
  acl=parseACL(aclString);
  if(aclString)free(aclString);
  mPrintf(errU,DEBUG,__func__,0,"Access Control List read from file: \"%s\".",
          aclFileName);
  if(!acl.entryN)
  {
    mPrintf(errU,FATAL,__func__,0,"No user_pattern@host_pattern configured in "
            "file: \"%s\"! The Task Manager Server terminates since no client "
            "would be allowed to send command!",aclFileName);
    exit(1);
  }
  /* print configuration */
  mPrintf(errU,DEBUG,__func__,0,"Access Control List entries: %d.",acl.entryN);
  for(i=0;i<acl.entryN;i++)
  {
    mPrintf(errU,DEBUG,__func__,0,"ACL entry # %3d: \"%s@%s\"",i,
            acl.userNameList[i],acl.hostNameList[i]);
  }
  printOutFuncOK;
  return acl;
}
/* ************************************************************************* */
void shortUsage(void)
{
  char *shortUsageStr=
"SYNOPSIS\n"
"tmSrv [-N | --dim_dns_node DIM_DNS_NODE] [-l | --logger LOGGER]\n"
"      [-p | --permission PERMISSION] [--no-authentication]\n"
"      [-u | --dflt_user DEFAULT_USER] [-g | --dflt_group DEFAULT_GROUP]\n"
"      [-U | --tm_user TMSRV_USER] [-G | --tm_group TMSRV_GROUP]\n"
"      [--schedpol SCHEDULING_POLICY] [--mainprio MAIN_THR_PRIORITY]\n"
"      [--ioprio I/O_THR_PRIORITY] [--timerprio TIMER_THR_PRIORITY]\n"
"      [--defunct-persistence PERSISTENCE]\n"
"tmSrv { -h | --help }\n"
"Try \"tmSrv -h\" for more information.\n";
  /* ----------------------------------------------------------------------- */
  //fprintf(stderr,"\n%s\n",shortUsageStr);
  mPrintf(L_DIM|L_STD|L_SYS,INFO,__func__,0,"\n%s",shortUsageStr);
  exit(1);
}

/* ************************************************************************* */
void usage(int mode)
{
  FILE *fpOut;
  int status;
  char *formatter;
  /* ----------------------------------------------------------------------- */
  if(!mode)formatter="/usr/bin/groff -man -Tascii | /usr/bin/less -iRs";
  else formatter="/bin/cat";
  fpOut=popen(formatter,"w");
  fprintf(fpOut,
".ig\n"
"tmSrv.man\n"
"\n"
"..\n"
"%s"
".hw WILDCARD_PATTERN INTERVAL /tmp/logGaudi.fifo logGetProperties\n"
".TH tmSrv 8  %s \"FMC-%s\" \"FMC Servers\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis tmSrv\\ \\-\n"
"FMC Task Manager Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis tmSrv\n"
".DoubleOpt[] N dim\\[ru]dns\\[ru]node DIM_DNS_NODE\n"
".DoubleOpt[] l logger LOGGER\n"
".DoubleOpt[] p permission PERMISSION\n"
".LongOpt[] no\\[hy]authentication\n"
".LongOpt[] no\\[hy]ptrace\\[hy]workaround\n"
".LongOpt[] always\\[hy]ptrace\\[hy]workaround\n"
".DoubleOpt[] u dflt_user DEFAULT_USER\n"
".DoubleOpt[] g dflt_group DEFAULT_GROUP\n"
".DoubleOpt[] U tm_user TMSRV_USER\n"
".DoubleOpt[] G tm_group TMSRV_GROUP\n"
".LongOpt[] schedpol SCHEDULING_POLICY\n"
".LongOpt[] mainprio MAIN_THR_PRIORITY\n"
".LongOpt[] ioprio I/O_THR_PRIORITY\n"
".LongOpt[] timerprio TIMER_THR_PRIORITY\n"
".LongOpt[] defunct-persistence PERSISTENCE\n"
".EndSynopsis\n"
".sp\n"
".Synopsis tmSrv\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".SH DESCRIPTION\n"
"The FMC Task Manager Server (\\fBtmSrv\\fP) is a FMC server which can\n"
"\\fBstart\\fP or \\fBstop\\fP a process on the current node or send a\n"
"\\fBsignal\\fP to a process running on the current node at a\n"
"\\fBDIM command\\fP. Furthermore \\fBtmSrv\\fP publishes two\n"
"\\fBDIM services\\fP which \\fBlist\\fP the processes started by the\n"
"FMC Task Manager Server on the current node.\n"
".PP\n"
"A process started by \\fBtmSrv\\fP is assigned, by \\fBdefault\\fP, to\n"
"the \\fBuser\\fP \\fIDEFAULT_USER\\fP and to the \\fBgroup\\fP\n"
"\\fIDEFAULT_GROUP\\fP, chosen by means of the\n"
"\\fB-u\\fP \\fIDEFAULT_USER\\fP and \\fB-g\\fP \\fIDEFAULT_GROUP\\fP\n"
"command line options. The default FMC Task Manager policy is to allow\n"
"to assign a started process only to this user and to this group. The policy\n"
"can be made looser by means of the \\fB-p\\fP \\fIPERMISSION\\fP\n"
"command line option, allowing to assign a started process to an arbitrary\n"
"user excepting the user \"root\" (\\fB-p\\fP 1) or to an arbitrary user\n"
"including the user \"root\" (\\fB-p\\fP 2).\n"
".PP\n"
"The process \\fBtmSrv\\fP itself can be assigned to a user and a group\n"
"which are different from the user and the group which actually started the\n"
"process  \\fBtmSrv\\fP, by means of the command-line options\n"
"\\fB-U\\fP \\fITMSRV_USER\\fP and \\fB-G\\fP \\fITMSRV_GROUP\\fP.\n"
".PP\n"
"A process started by \\fBtmSrv\\fP is managed, by default, by the Linux\n"
"default time-sharing scheduler (SCHED_OTHER), with dynamic priority, based\n"
"on the nice level. \\fBtmSrv\\fP is also able to assign a started process\n"
"to a \\fBreal-time\\fP scheduler (SCHED_FIFO or SCHED_RR). This choice is\n"
"allowed only if \\fBtmSrv\\fP too is managed by a\n"
"real-time scheduler and if the static priority of \\fBtmSrv\\fP is higher\n"
"than the static priority of the started process. The command line\n"
"options \\fB--schedpol\\fP \\fISCHEDULING_POLICY\\fP,\n"
"\\fB--mainprio\\fP \\fIMAIN_THR_PRIORITY\\fP,\n"
"\\fB--ioprio\\fP \\fII/O_THR_PRIORITY\\fP, and\n"
"\\fB--timerprio\\fP \\fITIMER_THR_PRIORITY\\fP allow to choose the\n"
"scheduler and the static priority of the threads of the FMC Task Manager\n"
"Server.\n"
".PP\n"
"By default the FMC Task Manager implements a rhosts-like authentication:\n"
"only the couples user@host which match at least one of the patterns\n"
"user_pattern@host_pattern listed in the file \"/etc/tmSrv.allow\" are\n"
"allowed to issue commands to the Task Manager. The authentication can\n"
"be disabled by means of the \\fB--no-authentication\\fP command-line\n"
"switch.\n"
".PP\n"
"The DIM Name Server, looked up by \\fBtmSrv\\fP to register the provided\n"
"DIM services, can be chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\"\\fB/etc/sysconfig/dim\\fP\".\n"
".RE\n"
".\n"
".\n"
".SH OPTIONS\n"
".\n"
".OptDef h help\n"
"Get basic usage help from the command line.\n"
".\n"
".OptDef N dim_dns_node DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef l logger LOGGER (integer)\n"
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
".OptDef u dflt_user DEFAULT_USER (string)\n"
"Assign the started processes, by default, to the user\n"
"\\fIDEFAULT_USER\\fP. If \\fIPERMISSION\\fP is greater then 0 (i.e.\n"
"\\fIPERMISSION\\fP is equal either to 1 or 2), by means of the DIM\n"
"command option \\fB-n\\fP \\fIUSER_NAME\\fP (see \\fBPublished DIM\n"
"Commands\\fP below), a process can also be assigned to a user\n"
"different from \\fIDEFAULT_USER\\fP.\n"
"\\fBDefault\\fP: \\fIDEFAULT_USER\\fP\\~=\\~\"nobody\".\n"
".\n"
".OptDef u dflt_group DEFAULT_GROUP (string)\n"
"Assign the started processes, by default, to the group\n"
"\\fIDEFAULT_GROUP\\fP. If \\fIPERMISSION\\fP is greater then 0 (i.e.\n"
"\\fIPERMISSION\\fP is equal either to 1 or 2), by means of the DIM\n"
"command option \\fB-g\\fP \\fIGROUP_NAME\\fP (see \\fBPublished DIM\n"
"Commands\\fP below), a process can also be assigned to a group\n"
"different from \\fIDEFAULT_GROUP\\fP.\n"
"\\fBDefault\\fP: \\fIDEFAULT_GROUP\\fP\\~=\\~\"nobody\".\n"
".\n"
".OptDef U tm_user TMSRV_USER (string)\n"
"Assign the \\fBtmSrv\\fP process to the user \\fITMSRV_USER\\fP, by\n"
"invoking \\fBsetuid\\fP(2).\n"
".OptDef G tm_group TMSRV_GROUP (string)\n"
"Assign the \\fBtmSrv\\fP process to the group \\fITMSRV_GROUP\\fP, by\n"
"invoking \\fBsetgid\\fP(2).\n"
".OptDef p permission PERMISSION (integer in the range 0..2)\n"
"Set the policy followed by the Task Manager Server in allowing to assign\n"
"the started processes to users and groups. The meaning of the\n"
"\\fIPERMISSION\\fP parameter is the following:\n"
".RS\n"
".TP\n"
"0\nthe processes started by the FMC Task Manager can only be assigned to\n"
"the user \\fIDEFAULT_USER\\fP and to the group \\fIDEFAULT_GROUP\\fP.\n"
".TP\n"
"1\nthe processes started by the FMC Task Manager can be assigned also to a\n"
"user different from \\fIDEFAULT_USER\\fP (by means of the DIM command\n"
"option \\fB-n\\fP \\fIUSER_NAME\\fP, see \\fBPublished DIM Commands\\fP\n"
"below) and to a group different from \\fIDEFAULT_GROUP\\fP (by means of\n"
"the DIM command option \\fB-g\\fP \\fIGROUP_NAME\\fP, see \\fBPublished\n"
"DIM Commands\\fP below) but not to the user \"root\" and not to the\n"
"group \"root\";\n"
".TP\n"
"2\nthe processes started by the FMC Task Manager can be assigned also to a\n"
"user different from \\fIDEFAULT_USER\\fP (by means of the DIM command\n"
"option \\fB-n\\fP \\fIUSER_NAME\\fP, see \\fBPublished DIM Commands\\fP\n"
"below) and to a group different from \\fIDEFAULT_GROUP\\fP (by means of\n"
"the DIM command option \\fB-g\\fP \\fIGROUP_NAME\\fP, see \\fBPublished\n"
"DIM Commands\\fP below) including the user \"root\" and the\n"
"group \"root\".\n"
".PP\n"
"\\fBDefault\\fP: \\fIPERMISSION\\fP = 0.\n"
".RE\n"
".\n"
".OptDef \"\" schedpol SCHEDULING_POLICY (integer in the range 0..2)\n"
"Run the FMC Task Manager Server using the scheduling policy\n"
"\\fISCHEDULING_POLICY\\fP. Allowed values for \\fISCHEDULING_POLICY\\fP\n"
"are:\n"
".RS\n"
".TP\n"
"0 (SCHED_OTHER)\n"
"The  default Linux time-sharing scheduler, with a dynamic priority based\n"
"on the NICE_LEVEL.\n"
".TP\n"
"1 (SCHED_FIFO)\n"
"The static-priority Linux real-time fifo scheduler, without  time slicing.\n"
"A SCHED_FIFO process runs until either it is blocked by an I/O request, it\n"
"is preempted by a higher priority process, or it calls sched_yield(2).\n"
".TP\n"
"2 (SCHED_RR)\n"
"The static-priority Linux real-time Round-Robin scheduler. It differs\n"
"from SCHED_FIFO because each process is only allowed to run for a maximum\n"
"time quantum. If a SCHED_RR process has been running for a time period\n"
"equal to or longer than the time quantum, it will be put at the end of\n"
"the list for its priority.\n"
".PP\n"
"\\fBDefault\\fP: \\fISCHEDULING_POLICY\\fP = 2 (SCHED_RR).\n"
".RE\n"
".\n"
".OptDef \"\" mainprio MAIN_THR_PRIORITY (integer in the range 0..99)\n"
"Run the FMC Task Manager Server with the priority of the \\fBmain thread\\fP\n"
"(the thread which runs the main control loop that\n"
"schedules list service updates and cleans-up \"zombies\")\n"
"equal to \\fIMAIN_THR_PRIORITY\\fP. Allowed values: only 0 for\n"
"SCHED_OTHER scheduling policy, 1..99 for SCHED_FIFO and\n"
"SCHED_RR scheduling policy. \\fBDefault\\fP value: 80.\n"
".\n"
".OptDef \"\" ioprio I/O_THR_PRIORITY (integer in the range 0..99)\n"
"Run the FMC Task Manager Server with the priority of the \\fBI/O thread\\fP\n"
"(the thread which manages DIM commands and services)\n"
"equal to \\fII/O_THR_PRIORITY\\fP. Allowed values: only 0 for\n"
"SCHED_OTHER scheduling policy, 1..99 for SCHED_FIFO and\n"
"SCHED_RR scheduling policy. \\fBDefault\\fP value: 93.\n"
".\n"
".OptDef \"\" timerprio TIMER_THR_PRIORITY (integer in the range 0..99)\n"
"Run the FMC Task Manager Server with the priority of the \\fBtimer\n"
"thread\\fP (the thread which manages timers and delayed\n"
"executions) equal to \\fITIMER_THR_PRIORITY\\fP. Allowed values:\n"
"only 0 for SCHED_OTHER scheduling policy, 1..99 for SCHED_FIFO\n"
"and SCHED_RR scheduling policy. \\fBDefault\\fP value: 10.\n" 
".\n"
".OptDef \"\" no-authentication\n"
"Do not check the permission of the user and host which sent the command.\n"
"\\fBDefault\\fP: do check the permission.\n"
".\n"
".OptDef \"\" no-ptrace-workaround\n"
"Never use the \"\\fBptrace\\fP(2) workaround\" to retrieve the UTGID of "
"processes even if the process environment is larger than 4096 B. This option "
"improves the performances on systems running a Linux kernel with the patch "
"\"Don-t-truncate-proc-PID-environ-at-4096-characters\" installed. This "
"option must \\fInever\\fP be used if this patch is not installed in the "
"running kernel. \\fBDefault\\fP: use the \"\\fBptrace\\fP(2) workaround\" "
"when the /proc/\\fIPID\\fP/environ file has the size equal to 4096 B. "
"\\fBN.B.\\fP: the system V start-up script provided with the FMC package "
"(\\fI/etc/rc.d/init.d/fmc\\fP) makes a test to find out if the patch is "
"installed in the running kernel and, in case, starts \\fBtmSrv\\fP(8) with "
"the current option.\n"
".\n"
".OptDef \"\" always-ptrace-workaround\n"
"Always use the \"\\fBptrace\\fP(2) workaround\" to retrieve the UTGID of "
"processes. This option must never be used except for tests. "
"\\fBDefault\\fP: use the \"\\fBptrace\\fP(2) workaround\" "
"when the /proc/\\fIPID\\fP/environ file has the size equal to 4096 B.\n"
".\n"
".OptDef \"\" defunct-persistence PERSISTENCE (integer)\n"
"Set the persistence time of terminated processes in \\fBlongList\\fP\n"
"DIM Service (accessed, i.e., by the \\fBtmLl\\fP(1) command-line command)\n"
"to \\fIPERSISTENCE\\fP seconds. \\fBDefault\\fP: 300 s, "
"\\fBminimum\\fP: 1 s.\n"
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
"Must include the path to the libraries \"libdim\" and \"libFMCutils\".\n"
".\n"
".TP\n"
".EnvVar updatePeriod \\ (integer,\\ optional)\n"
"Period of the main control loop in seconds if no new processes are started\n"
"and no started processes are terminated. \\fBDefault\\fP value: 20 s.\n"
".\n"
".TP\n"
".EnvVar deBug \\ (integer,\\ optional)\n"
"Debug mask. Can be set to 0x00...0xff. The meaning of the mask bit are:\n"
".RS\n"
".TP\n"
"0x01:\nprint updates;\n"
".TP\n"
"0x02:\nprint findUtgid()/findTgid() diagnostics;\n"
".TP\n"
"0x04:\nprint authentication;\n"
".TP\n"
"0x08:\nprint function start;\n"
".TP\n"
"0x10:\nprint list buffer size;\n"
".TP\n"
"0x20:\nprint ptrace(2) diagnostics;\n"
".TP\n"
"0x40:\nprint process list diagnostics;\n"
".TP\n"
"0x80:\nprint detailed findUtgid() diagnostics.\n"
".PP\n"
"\\fBDefault\\fP: \\fIdeBug\\fP = 0.\n"
".RE\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Start the FMC Task Manager Server without authentication, as user\n"
"\"root\", defining the user \"online\" as the \\fIDEFAULT_USER\\fP, and\n"
"allowing to start processes as any users, included the user \"root\", and\n"
"sending messages to the default FMC Message Logger:\n"
".\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/tmSrv -p 2 -u online -U root --no-auth\n"
".\n"
".\n"
".SH FILES\n"
".\n"
".SS Access Control File\n"
".I /etc/tmSrv.allow\n"
".\n"
".SS Description\n"
"The file \\fI/etc/tmSrv.allow\\fP must contain the list of the couples of\n"
"patterns \\fBuser_pattern@hostname_pattern\\fP allowed to issue commands\n"
"to the Task Manager running on the current node.\n"
".\n"
".SS File Format\n"
"One couple \\fBuser_pattern@hostname_pattern\\fP per record. Records\n"
"are separated using '\\[rs]n' (new-line, line-feed, LF, ASCII  0x0a).\n"
"Blank records as well as comment records (i.e. records\n"
"starting with the '#' character) are skipped.\n"
".\n"
".SS Record Format\n"
"\\fBuser_pattern@hostname_pattern\\fP. \\fBuser_pattern\\fP and\n"
"\\fBhost_pattern\\fP are separated by the '@' (at) character.\n"
".\n"
".SS Fields description\n"
".TP\n"
".B user_pattern\n"
"User name wildcard pattern (see glob(7)).\n"
".TP\n"
".B host_pattern\n"
"Host name wildcard pattern (see glob(7)).\n"
".\n"
".SS Record Examples\n"
"online@pc01.mydomain\n"
".br\n"
"*@pc01.mydomain\n"
".br\n"
"online@*\n"
".br\n"
"*@*\n"
".br\n"
"online@pc??.mydomain\n"
".br\n"
"online@pc?[0-9].mydomain\n"
".br\n"
"online@pc[0-9][0-9].mydomain\n"
".\n"
".\n"
".SH DIM INFORMATION\n"
".\n"
".SS DIM Server Name\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/task_manager\n"
"where \\fIHOST_NAME\\fP is the host name of the current PC, as returned by\n"
"the command \"hostname -s\", but written in upper case characters.\n"
".\n"
".\n"
".SS Published DIM Services\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/task_manager/list\n"
"Publishes a sequence of NULL-terminated strings containing the list of the\n"
"UTGIDs of the processes started by the FMC Task Manager running on the\n"
"current PC.\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/task_manager/longList\n"
"Publishes a sequence of NULL-terminated strings containing the list of the\n"
"4-tuples (TGID, CMD, UTGID, STATUS) of the processes started by the FMC\n"
"Task Manager running on the current PC.\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/task_manager/success\n"
"Always publishes the integer 1. This dummy services can be used to check\n"
"whether the current instance of \\fBtmSrv\\fP is up and running.\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/task_manager/server_version\n"
"Publishes a NULL-terminated string containing the RCS revision label of\n"
"the \\fBtmSrv.c\\fP source code of\n"
"the current \\fBtmSrv\\fP instance, as returned by the command\n"
"\"ident /opt/FMC/sbin/tmSrv\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/task_manager/actuator_version\n"
"Publishes a NULL-terminated string containing the RCS revision label of\n"
"the \\fBtmUtils.c\\fP source code of\n"
"the current \\fBtmSrv\\fP instance, as returned by the command\n"
"\"ident /opt/FMC/sbin/tmSrv\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/task_manager/fmc_version\n"
"Publishes a NULL-terminated string containing the revision label of the FMC\n"
"package which includes the current \\fBtmSrv\\fP executable, in the\n"
"present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".\n"
".SS Published DIM Commands\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/task_manager/start\n"
"Accepts, as argument, a NULL-terminated string containing the \\fIPATH\\fP\n"
"to the executable image to be started by the current \\fBtmSrv\\fP, the\n"
"command line arguments (\\fIARG\\fP), and the Start Options, in the format:\n"
".IP\n"
".ad l\n"
"[\\fB-c\\fP]\n"
"[\\fB-D\\fP\\ \\fINAME=value...\\fP]\n"
"[\\fB-s\\fP\\ \\fISCHEDULER\\fP]\n"
"[\\fB-p\\fP\\ \\fINICE_LEVEL\\fP]\n"
"[\\fB-r\\fP\\ \\fIRT_PRIORITY\\fP]\n"
"[\\fB-a\\fP\\ \\fICPU_NUM...\\fP]\n"
"[\\fB-k\\fP\\ \\fIOOM_SCORE_ADJ\\fP]\n"
"[\\fB-d\\fP]\n"
"[\\fB-n\\fP\\ \\fIUSER_NAME\\fP]\n"
"[\\fB-g\\fP\\ \\fIGROUP_NAME\\fP]\n"
"[\\fB-u\\fP\\ \\fIUTGID\\fP]\n"
"[\\fB-w\\fP\\ \\fIWD\\fP]\n"
"[\\fB-o\\fP]\n"
"[\\fB-e\\fP]\n"
"[\\fB-O\\fP\\ \\fIOUT_FIFO\\fP]\n"
"[\\fB-E\\fP\\ \\fIERR_FIFO\\fP]\n"
"[\\fB-A\\fP]\n"
"\\fIPATH\\fP\n"
"[\\fIARG...\\fP]\n"
".ad b\n"
".PP\n"
".RS\n"
".B DIM Command Options\n"
".\n"
".OptDef c \"\"\n"
"Clear the \\fBenvironment\\fP inherited by the Task Manager.\n"
"\\fBDefault\\fP: inherit the Task Manager environment.\n"
".\n"
".OptDef D \"\" NAME=VALUE (string, repeatable)\n"
"Set the \\fBenvironment\\fP variable \\fINAME\\fP to the value\n"
"\\fIVALUE\\fP.\n"
".\n"
".OptDef s \"\" SCHEDULER (integer in the range 0..2)\n"
"Set the process scheduler to \\fISCHEDULER\\fP, which can be: \n"
".RS\n"
".TP\n"
"\\fB0\\fP\\~(\\fBSCHED_OTHER\\fP)\n"
"The default Linux \\fBtime-sharing\\fP scheduler, with a\n"
"\\fBdynamic priority\\fP based on the \\fINICE_LEVEL\\fP.\n"
".TP\n"
"\\fB1\\fP\\~(\\fBSCHED_FIFO\\fP)\n"
"The \\fBstatic-priority\\fP Linux \\fBreal-time\\fP \\fBfifo\\fP scheduler,\n"
"without time slicing. A \\fBSCHED_FIFO\\fP process runs until either it is\n"
"blocked by an I/O request, it is preempted by a higher "
"priority process, or it calls sched_yield(2).\n"
".TP\n"
"\\fB2\\fP\\~(\\fBSCHED_RR\\fP)\n"
"The \\fBstatic-priority\\fP Linux \\fBreal-time Round-Robin\\fP scheduler.\n"
"It differs from \\fBSCHED_FIFO\\fP because each process is only allowed to\n"
"run for a maximum time quantum. If a \\fBSCHED_RR\\fP process has been\n"
"running for a time period equal to or longer than the time quantum, it\n"
"will be put at the end of the list for its priority.\n"
".PP\n"
"\\fBDefault\\fP: 0 (SCHED_OTHER).\n"
".RE\n"
".\n"
".OptDef p \"\" NICE_LEVEL (integer in the range -20..19)\n"
"If \\fISCHEDULER\\fP=\\fBSCHED_OTHER\\fP, set the \\fBnice level\\fP of the\n"
"process to \\fINICE_LEVEL\\fP. The \\fBnice level\\fP is used by the\n"
"\\fBSCHED_OTHER\\fP time-sharing Linux scheduler to compute the\n"
"\\fBdynamic priority\\fP. Allowed values for \\fINICE_LEVEL\\fP are in the\n"
"range -20..19 (-20 corresponds to the most favorable scheduling; 19\n"
"corresponds to the least favorable scheduling). The \\fBnice level\\fP\n"
"can be lowered by the Task Manager even for processes which run as a user\n"
"different from root.\n"
".\n"
".OptDef r \"\" RT_PRIORITY (integer in the range 0..99)\n"
"Set the \\fBstatic\\fP (real-time) \\fBpriority\\fP of the process to\n"
"\\fIRT_PRIORITY\\fP. Only value 0 (zero) for \\fIRT_PRIORITY\\fP is\n"
"allowed for scheduler \\fBSCHED_OTHER\\fP. For \\fBSCHED_FIFO\\fP and\n"
"\\fBSCHED_RR\\fP real-time schedulers, allowed values are in the range\n"
"1..99 (1 is the least favorable priority, 99 is the most favorable\n"
"priority).\n"
".\n"
".OptDef a \"\" CPU_NUM (integer, repeatable)\n"
"Add the CPU number \\fICPU_NUM\\fP to the \\fBprocess-to-CPU affinity\n"
"mask\\fP. More than one of these options can be present in the same command\n"
"to add more than one CPU to the affinity mask. Started process is allowed\n"
"to run only on the CPUs specified in the affinity mask. Omitting this\n"
"option, process is allowed to run on any CPU of the node. Allowed\n"
"\\fICPU_NUM\\fP depend on the PC architecture\n"
".\n"
".OptDef k \"\" OOM_SCORE_ADJ (integer in the range -999..1000)\n"
"Set the \\fBOut Of Memory Killer Adjust Parameter\\fP of the started process "
"(\\fBoom_score_adj\\fP) to \\fIOOM_SCORE_ADJ\\fP. If the kernel version is "
"less than 2.6.36 then the value is converted in the range -16..15 for the "
"\\fBoom_adj\\fP parameter.  The process to be killed in an out-of-memory "
"situation is selected based on its badness score. The badness score is "
"reflected in /proc/<pid>/oom_score. This value is determined on the basis "
"that the system loses the minimum amount of work done, recovers a large "
"amount of memory, doesn't kill any innocent process eating tons of memory, "
"and kills the minimum number of processes (if possible limited to one). The "
"badness score is computed using the original memory size of the process, its "
"CPU time (utime + stime), the run time (uptime - start time) and its "
"oom_adj/oom_score_adj value. The \\fBhigher\\fP the parameter "
"\\fIOOM_SCORE_ADJ\\fP, \\fBmore likely\\fP the process is to be "
"\\fBkilled\\fP by \\fBOOM-killer\\fP. The value \\fIOOM_SCORE_ADJ\\fP=-1000 "
"is reserved for the Task Manager process and is not allowed for the started "
"processes.\n"
".\n"
".OptDef d \"\"\n"
"Start the process as \\fBdaemon\\fP: the process \\fBumask\\fP (user\n"
"file-creation mask) is changed into 0 (zero) and the program is run in a\n"
"new \\fBsession\\fP as \\fBprocess group leader\\fP.\n"
".\n"
".OptDef n \"\" USER_NAME (string)\n"
"Set the \\fBeffective UID\\fP (User IDentifier), the \\fBreal UID\\fP and\n"
"the \\fBsaved UID\\fP of the process to the \\fBUID\\fP corresponding to\n"
"the user \\fIUSER_NAME\\fP.\n"
".\n"
".OptDef g \"\" GROUP_NAME (string)\n"
"Set the \\fBeffective GID\\fP (Group IDentifier), the \\fBreal GID\\fP and\n"
"the \\fBsaved GID\\fP of the process to the \\fBGID\\fP corresponding to\n"
"the group \\fIGROUP_NAME\\fP.\n"
".\n"
".OptDef u \"\" UTGID (string)\n"
"Set the process \\fButgid\\fP (User assigned unique Thread Group\n"
"Identifier) to \\fIUTGID\\fP.\n"
".\n"
".OptDef w \"\" WD (string)\n"
"Set the process \\fBworking directory\\fP to \\fIWD\\fP. File open by the\n"
"process without path specification are sought by the process in this\n"
"directory. Process \\fBrelative file path\\fP start from this directory.\n"
".\n"
".OptDef e \"\"\n"
"Redirect the process \\fBstderr\\fP (standard error stream) to the default\n"
"FMC Message Logger. Omitting \\fB-e\\fP or \\fB-E\\fP \\fIERR_FIFO\\fP\n"
"options, the standard error stream is thrown in /dev/null.\n"
".\n"
".OptDef o \"\"\n"
"Redirect the process \\fBstdout\\fP (standard output stream) to the default\n"
"FMC Message Logger. Omitting \\fB-o\\fP or \\fB-O\\fP \\fIOUT_FIFO\\fP\n"
"options, the standard output stream is thrown in /dev/null.\n"
".\n"
".OptDef E \"\" ERR_FIFO (string)\n"
"Redirect the process \\fBstderr\\fP to the FMC Message Logger which uses\n"
"the FIFO (named pipe) \\fIERR_FIFO\\fP. Omitting \\fB-e\\fP or \\fB-E\\fP\n"
"\\fIERR_FIFO\\fP options, the standard error stream is thrown in\n"
"/dev/null.\n"
".\n"
".OptDef O \"\" OUT_FIFO (string)\n"
"Redirect the process \\fBstdout\\fP to the FMC Message Logger which uses\n"
"the FIFO (named pipe) \\fIOUT_FIFO\\fP. Omitting \\fB-o\\fP or \\fB-O\\fP\n"
"\\fIOUT_FIFO\\fP options, the standard output stream is thrown in\n"
"/dev/null.\n"
".\n"
".OptDef A \"\"\n"
"In conjunction with \\fB-E\\fP \\fIERR_FIFO\\fP and\n"
"\\fB-O\\fP \\fIOUT_FIFO\\fP options, this option forces the \\fBno-drop\\fP\n"
"policy for the FIFO. \\fBDefault\\fP: the \\fBcongestion-proof\\fP policy\n"
"is used for the FIFO.\n"
".\n"
".TP\n"
"\\fIPATH\\fP\n"
"The path to the executable image to be started\n"
".\n"
".TP\n"
"[\\fIARGS\\fP]\n"
"The path to the executable image to be started\n"
".\n"
".RE\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/task_manager/kill\n"
"Accepts, as argument, a NULL-terminated string containing the Kill\n"
"Options and the UTGID wildcard pattern of the process to be signaled by\n"
"the current \\fBtmSrv\\fP, in the format:\n"
".IP\n"
".ad l\n"
"[\\fB-s\\fP\\ \\fISIGNAL\\fP]\n"
"\\fIUTGID_PATTERN\\fP\n"
".ad b\n"
".PP\n"
".RS\n"
".B DIM Command Options\n"
".\n"
".OptDef s \"\" SIGNAL (integer)\n"
"Send the signal \\fISIGNAL\\fP to the process(es) (\\fBdefault\\fP:\n"
"signal 15, \\fBSIGTERM\\fP).\n"
".\n"
".TP\n"
"\\fIUTGID_PATTERN\\fP\n"
"The UTGID wildcard pattern of the processes to be signaled.\n"
".\n"
".RE\n"
".\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/task_manager/stop\n"
"Accepts, as argument, a NULL-terminated string containing the Stop\n"
"Options and the UTGID wildcard pattern of the process to be stopped by\n"
"the current \\fBtmSrv\\fP, in the format:\n"
".IP\n"
".ad l\n"
"[\\fB-s\\fP\\ \\fISIGNAL\\fP]\n"
"[\\fB-d\\fP\\ \\fIKILL_9_DELAY\\fP]\n"
"\\fIUTGID_PATTERN\\fP\n"
".ad b\n"
".PP\n"
".RS\n"
".B DIM Command Options\n"
".\n"
".OptDef s \"\" SIGNAL (integer)\n"
"Send the signal \\fISIGNAL\\fP as the first signal to the process(es)\n"
"(\\fBdefault\\fP: signal 15, \\fBSIGTERM\\fP).\n"
".\n"
".OptDef d \"\" KILL_9_DELAY (integer)\n"
"Wait \\fIKILL_9_DELAY\\fP seconds before sending the second signal (which\n"
"is always the signal 9, \\fBSIGKILL\\fP). \\fBDefault\\fP: 1 s.\n"
".\n"
".TP\n"
"\\fIUTGID_PATTERN\\fP\n"
"The UTGID wildcard pattern of the processes to be stopped.\n"
".\n"
".RE\n"
"%s"
"%s"
".\n"
".\n"
".SH VERSION\n"
".\n"
"Source:  %s\n"
".PP\n"
"Source:  %s\n"
".PP\n"
"Package: FMC-%s\n"
".\n"
".\n"
".SH SEE ALSO\n"
".ad l\n"
".BR \\%%tmStart (1),\n"
".BR \\%%tmKill (1),\n"
".BR \\%%tmStop (1),\n"
".BR \\%%tmLs (1),\n"
".BR \\%%tmLl (1),\n"
".BR \\%%logSrv (8),\n"
".BR \\%%pcSrv (8).\n"
".br\n"
".BR \\%%nice (1),\n"
".BR \\%%chdir (2),\n"
".BR \\%%dup2 (2),\n"
".BR \\%%fork (2),\n"
".BR \\%%kill (2),\n"
".BR \\%%ptrace (2),\n"
".BR \\%%sched_setaffinity (2),\n"
".BR \\%%sched_setscheduler (2),\n"
".BR \\%%sched_yield (2),\n"
".BR \\%%setgid (2),\n"
".BR \\%%setpriority (2),\n"
".BR \\%%setsid (2),\n"
".BR \\%%setuid (2),\n"
".BR \\%%umask (2),\n"
".BR \\%%waitpid (2),\n"
".BR \\%%argz_add (3),\n"
".BR \\%%clearenv (3),\n"
".BR \\%%execv (3),\n"
".BR \\%%fnmatch (3),\n"
".BR \\%%setenv (3),\n"
".BR \\%%glob (7),\n"
".BR \\%%signal (7).\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro,FMC_DATE,FMC_VERSION
,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD
,rcsidP
,SVC_HEAD,aRcsidP
,SVC_HEAD,FMC_VERSION
,SVC_HEAD,SVC_HEAD,SVC_HEAD
,FMC_URLS,FMC_AUTHORS
,rcsidP,aRcsidP,FMC_VERSION
       );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/* ************************************************************************* */
