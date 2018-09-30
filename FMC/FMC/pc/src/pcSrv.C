/*****************************************************************************/
/*
 * $Log: pcSrv.C,v $
 * Revision 3.28  2012/12/13 16:21:24  galli
 * Minor changes in man page references
 *
 * Revision 3.26  2012/12/04 12:45:11  galli
 * Support for oomScoreAdj
 *
 * Revision 3.25  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 3.24  2009/10/03 15:21:02  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 3.21  2008/10/09 09:37:58  galli
 * sig2msg() moved to fmcSigUtils.c
 *
 * Revision 3.16  2008/10/03 15:07:42  galli
 * DIM_CONF_FILE_NAME and DIM_DNS_SRV_LIST got from fmcPar.h
 * "process_controller" parametrized in SRV_NAME
 *
 * Revision 3.13  2008/09/25 15:33:01  galli
 * check the username and the groupname got from the command line
 *
 * Revision 3.12  2008/09/25 14:26:00  galli
 * stop all started processes when terminates on SIGTERM or SIGINT
 * print the update counter every ~4 h
 *
 * Revision 3.11  2008/09/25 12:53:54  galli
 * Rewritten from scratch in C++
 * Can dynamically link Task Managers started later
 *
 * Revision 2.13  2008/09/02 10:25:25  galli
 * minor changes
 *
 * Revision 2.12  2008/07/07 12:36:11  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 * service names starts with /FMC
 *
 * Revision 2.10  2008/03/06 08:13:32  galli
 * -N option changed in -M in add CMD
 *
 * Revision 2.6  2007/10/26 11:57:00  galli
 * usage() prints FMC version
 * publishes service fmc_version
 *
 * Revision 2.5  2007/10/26 10:37:12  galli
 * in non-authenticated mode accepts also authenticated commands
 *
 * Revision 2.2  2007/10/01 12:03:51  galli
 * authentication for commands pcAdd and pcRm
 * send authenticated command tmStart and tmStop to tmSrv
 * read access control list from file /etc/pcSrv.allow
 * command-line switch --no-authentication for compatibility with version < 2.0
 *
 * Revision 1.30  2007/08/22 14:19:38  galli
 * minor changes
 *
 * Revision 1.29  2007/08/16 08:18:51  galli
 * manage groups in addition to users
 * added -g group DIM CMD switch
 *
 * Revision 1.28  2007/08/14 13:25:51  galli
 * compatible with libFMCutils v 2.0
 * usage() changed
 *
 * Revision 1.27  2006/10/20 10:07:11  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 1.26  2006/10/18 12:31:05  galli
 * uses the __func__ magic variable
 *
 * Revision 1.25  2006/09/06 07:20:58  galli
 * print command in error messages
 *
 * Revision 1.24  2006/09/05 09:20:44  galli
 * options: -O outFIFO, -E errFIFO e -A (noDrop) added in add CMD
 *
 * Revision 1.23  2006/08/21 08:45:19  galli
 * -m option is repeatable in add and rm DIM CMDs
 * bug fixed in call to dic_info_service() on x86_64 architecture
 * node specified in logged messages
 *
 * Revision 1.22  2006/08/16 13:18:16  galli
 * Using -s cmdline switch, can read an initialization file for processes
 * to be started at pcSrv start-up.
 * Dead-lock fixed.
 * Double process respawn reduced.
 *
 * Revision 1.21  2006/08/10 14:59:16  galli
 * process command line options -a (affinity mask), -M, -X and -K (respawn
 * control)
 *
 * Revision 1.20  2006/08/10 09:36:14  galli
 * Read configuration (list of nodes to be controlled) either from DIM
 * Configuration Manager or (is specified by -c command line option) from
 * a configuration file.
 *
 * Revision 1.19  2006/08/10 08:02:52  galli
 * bug fixed in schPsRem() (seg fault removing process)
 * schPsAdd() requires a process refresh, to speed up process start-up
 *
 * Revision 1.18  2006/02/09 12:08:53  galli
 * x86_64 compatibility problems fixed
 *
 * Revision 1.17  2005/05/26 14:29:22  galli
 * added ll and lss services
 *
 * Revision 1.16  2005/05/26 00:06:54  galli
 * first working version
 *
 * Revision 1.1  2005/05/10 13:26:13  galli
 * Initial revision
 */
/*****************************************************************************/
/* Respawn control:                                                          */
/* - If a process is started more than maxStartN times in checkPeriod        */
/*   seconds, then the process restart is disabled for disPeriod seconds.    */
/* - The parameters maxStartN, checkPeriod and disPeriod are                 */
/*   process-specific and are read from XML configuration file.              */
/* - Default values are: maxStartN=10, checkPeriod=120s and disPeriod=300s.  */
/* - If maxStartN=-1, then respawn control is excluded, i.e. process can be  */
/*   restarted indefinitely.                                                 */
/* - If disPeriod=-1, then the process-restart for the process, once         */
/*   disabled, is never re-enabled.                                          */
/*****************************************************************************/
/* Published DIM CMD:                                                        */
/*   /FMC/<CRTL_PC_HOSTNAME>/<SRV_NAME>/add                                  */
/*     [-m hostnamePattern...]                                               */
/*     [-c][-D NAME=value...]                                                */
/*     [-s Scheduler][-p nice_level][-r rt_priority][-a cpu_num...]          */
/*     [-d][-n user_name][-n group_name] -u utgid [-w wd]                    */
/*     [-o][-e][-O outFIFO][-E errFIFO][-A]                                  */
/*     [-M maxStartN][-K checkPeriod][-X disPeriod]                          */
/*     path [arg...]                                                         */
/*   /FMC/<CRTL_PC_HOSTNAME>/<SRV_NAME>/rm                                   */
/*     [-m hostnamePattern...][-s signo][-d s9delay] utgidPattern            */
/*****************************************************************************/
/* Published DIM SVC:                                                        */
/*   /FMC/<CRTL_PC_HOSTNAME>/<SRV_NAME>/ls                                   */
/*   /FMC/<CRTL_PC_HOSTNAME>/<SRV_NAME>/ll                                   */
/*   /FMC/<CRTL_PC_HOSTNAME>/<SRV_NAME>/lss                                  */
/*   /FMC/<CRTL_PC_HOSTNAME>/<SRV_NAME>/server_version                       */
/*   /FMC/<CRTL_PC_HOSTNAME>/<SRV_NAME>/fmc_version                          */
/*   /FMC/<CRTL_PC_HOSTNAME>/<SRV_NAME>/success                              */
/*****************************************************************************/
/* Contacted DIM SVC:                                                        */
/*   /FMC/<NODE_HOSTNAME>/task_manager/list                                  */
/*****************************************************************************/
/* Contacted DIM CMD:                                                        */
/*   /FMC/<NODE_HOSTNAME>/task_manager/start                                 */
/*   /FMC/<NODE_HOSTNAME>/task_manager/stop                                  */
/*****************************************************************************/
/* ls SVC output sample:                                                     */
/*                                                                           */
/* 0  lhcbsrv:2:counter_1,counter_0.                                         */
/* 1  lhcbcn2:1:counter_0.                                                   */
/*****************************************************************************/
/* ll SVC output sample:                                                     */
/*                                                                           */
/* 0  lhcbsrv:2:counter_1(wd="/opt/FMC/tests",path="./counter",clearEnv,LD_L */
/* IBRARY_PATH=/opt/dim/linux:/opt/FMC/lib,DIM_DNS_NODE=lhcbsrv.bo.infn.it,d */
/* aemon,SCHED_FIFO,nice=0,rtprio=1,affMask=0x00000007,user="online",rdrStde */
/* rr,rdrStdout,maxStartN=20,disPeriod=60s,checkPeriod=30s),counter_0(wd="/o */
/* pt/FMC/tests",path="./counter",clearEnv,LD_LIBRARY_PATH=/opt/dim/linux:/o */
/* pt/FMC/lib,DIM_DNS_NODE=lhcbsrv.bo.infn.it,daemon,SCHED_FIFO,nice=0,rtpri */
/* o=1,affMask=0x00000007,user="online",rdrStderr,rdrStdout,maxStartN=20,dis */
/* Period=60s,checkPeriod=30s).                                              */
/* 1  lhcbcn2:1:counter_0(wd="/opt/FMC/tests",path="./counter",clearEnv,LD_L */
/* IBRARY_PATH=/opt/dim/linux:/opt/FMC/lib,DIM_DNS_NODE=lhcbsrv.bo.infn.it,d */
/* aemon,SCHED_FIFO,nice=0,rtprio=1,affMask=0x00000007,user="online",rdrStde */
/* rr,rdrStdout,maxStartN=20,disPeriod=60s,checkPeriod=30s).                 */
/*****************************************************************************/
/* lss SVC output sample:                                                    */
/*                                                                           */
/* 0  lhcbsrv:3:counter_2[disabled,(re)start=10:Aug21-121059,Aug21-121058,Au */
/* g21-121057,Aug21-121056,Aug21-121055,Aug21-121054,Aug21-121053,Aug21-1210 */
/* 52,Aug21-121419),counter_1[(re)start=1:Aug21-121259),counter_0[(re)start= */
/* 2:Aug21-121059,Aug21-121419].                                             */
/* 1  lhcbcn2:1:counter_0[(re)start=1:Aug21-121059].                         */
/* 2  lhcbcn3:2:counter_0[(re)start=0:(no start)],counter_1[(re)start=0:(no  */
/* start)].                                                                  */
/*****************************************************************************/
#include <string>
#include <vector>
#include <list>
/* ------------------------------------------------------------------------- */
#include <stdio.h>
#include <unistd.h>                                               /* pause() */
#include <getopt.h>                                         /* getopt_long() */
#include <sys/wait.h>         /* waitpid(), WIFEXITED(), WEXITSTATUS(), etc. */
#include <pwd.h>                           /* uid_t, getpwnam(3) getpwuid(3) */
#include <grp.h>                           /* gid_t, getgrnam(3) getgrgid(3) */
#include <sys/types.h>                                            /* open(2) */
#include <sys/stat.h>                                             /* open(2) */
#include <fcntl.h>                                                /* open(2) */
#include <unistd.h>                                     /* read(2), write(2) */
#include <sys/utsname.h>                         /* uname(2), struct utsname */
/* ------------------------------------------------------------------------- */
/* DIM */
#include <dis.hxx>
#include <dic.hxx>
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcSrvNameUtils.h"                               /* getSrvPrefix() */
#include "fmcMsgUtils.h"           /* dfltLoggerOpen(), mPrintf(), rPrintf() */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "fmcSigUtils.h"                                        /* sig2msg() */
/* ------------------------------------------------------------------------- */
/* include */
#include "fmcRef.h"                                  /* FMC_URLS FMC_AUTHORS */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcMacro.h"                                             /* eExit() */
#include "fmcVersion.h"                                       /* FMC version */
#include "fmcGroffMacro.h"                             /* macro for man page */
#include "fmcDate.h"                                     /* compilation date */
/* ------------------------------------------------------------------------- */
/* in pc/include */
#include "PcProcess.IC"
#include "PcNode.IC"
#include "PcCluster.IC"
#include "PcAcl.IC"
#include "PcLsSvc.IC"
#include "PcLlSvc.IC"
#include "PcLssSvc.IC"
#include "PcAddCmd.IC"
#include "PcRmCmd.IC"
#include "PcDimErrorHandler.IC"
#include "PcTmSeeker.IC"
#include "pcSrv.H"                                  /* hard-coded parameters */
/* ************************************************************************* */
/* function prototype */
/* ************************************************************************* */
void usage(int mode);
void shortUsage(void);
static void signalHandler(int signo);
/*****************************************************************************/
/* globals */
/*****************************************************************************/
int errU=L_DIM;                                                /* error unit */
int deBug=0;                                                   /* debug mask */
/* max number of (re)start times kept for each process for lss/pcLss service */
size_t startTimeMaxSize=DFLT_START_TIME_MAX_SIZE;
/* process controller objects (1 instance only) */
PcAcl acl;                         /* access control list data and functions */
PcCluster cluster;                                    /* main data structure */
/* DIM SVC/CMD object (1 instance only) pointers */
PcLsSvc *lsSvc=NULL;                /* object created once and never deleted */
PcLlSvc *llSvc=NULL;                /* object created once and never deleted */
PcLssSvc *lssSvc=NULL;              /* object created once and never deleted */
PcAddCmd *addCmd=NULL;              /* object created once and never deleted */
PcRmCmd *rmCmd=NULL;                /* object created once and never deleted */
PcDimErrorHandler errorHandler;/* instantiate dim srv and clnt error handler */
/* constant string parameters */
string confFileName(CONF_FILE_NAME);                   /* configuration file */
string aclFileName(ACL_FILE_NAME);               /* Access Control List file */
string initFileName(INIT_FILE_NAME);                  /* Initialization file */
string dfltUserName(DFLT_USER_NAME);                         /* default user */
string dfltGroupName(DFLT_GROUP_NAME);                      /* default group */
int success=1;
static char rcsid[]="$Id: pcSrv.C,v 3.28 2012/12/13 16:21:24 galli Exp galli $";
char *rcsidP=rcsid;
/*****************************************************************************/
int main(int argc,char **argv)
{
  int i=0;
  char *p;
  const char *dimConfFile=DIM_CONF_FILE_NAME;
  char *dimDnsNode=NULL;
  char *sP;
  int flag=0;
  char *deBugS=NULL;
  string srvName(SRV_NAME);
  string srvPath;
  sigset_t signalMask;
  int signo;
  char mesg[MSG_LEN]="";
  char *mesgP=NULL;
  struct timespec updatePeriod;
  uid_t dfltUid=DFLT_UID;
  gid_t dfltGid=DFLT_GID;
  struct passwd *pw;
  struct group *gr;
  char *endptr=NULL;
  /* kernel version */
  static struct utsname osBuf;
  char *kernelVersion=NULL;
  /* oom adjust */
  int pcOomAdjVal;
  int pcOomScoreAdjVal;
  char pcOomAdjStr[8]="";                                        /* [-17,15] */
  char pcOomScoreAdjStr[8]="";                               /* [-1000,1000] */
  /* getopt */
  static struct option longOptions[]=
  {
    {"no-authentication",no_argument,NULL,0},
    {"starttime-max-size",required_argument,NULL,1},
    {"logger",required_argument,NULL,'l'},
    {"conf-file",required_argument,NULL,'c'},
    {"init-file",required_argument,NULL,'s'},
    {"dflt_user",required_argument,NULL,'u'},
    {"dflt_group",required_argument,NULL,'g'},
    {"help",optional_argument,NULL,'h'},
    {"dim_dns_node",required_argument,NULL,'N'},
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsidP,':')+2;
  for(i=0,sP=rcsidP;i<5;i++)sP=1+strchr(sP,' ');
  *(sP-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* syslog version */
  mPrintf(L_SYS,INFO,__func__,0,"Starting FMC Process Controller Server... "
          "Using: \"%s\", \"FMC-%s\".",rcsidP,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* process command-line arguments */
  opterr=0;
  while((flag=getopt_long(argc,argv,"+h::l:c:s:N:u:g:",longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 0: /* --no-authentication */
        acl.off();
        break;
      case 1: /* --starttime-max-size */
        startTimeMaxSize=(size_t)strtol(optarg,(char**)NULL,0);
        break;
      case 'c': /* -c, --conf-file */
        confFileName=string(optarg);
        break;
      case 's': /* -s, --init-file */
        initFileName=string(optarg);
        break;
      case 'l': /* -l, --logger */
        errU=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'N': /* -N, --dim_dns_node */
        dimDnsNode=optarg;
        break;
      case 'u': /* -u, --dflt_user */
        dfltUserName=string(optarg);
        break;
      case 'g': /* -g, --dflt_group */
        dfltGroupName=string(optarg);
        break;
      case 'h': /* -h, -hh, --help */
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        mPrintf(L_STD|L_SYS,FATAL,__func__,0,"getopt(): invalid option "
                "\"%s\"!",argv[optind-1]);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* check non-option arguments */
  if(optind<argc)
  {
    mesgP=mesg;
    sprintf(mesgP,"Unrequired non-option command-line arguments: ");
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
  /*-------------------------------------------------------------------------*/
  /* open error logger */
  if(errU&L_DIM)
  {
    if(dfltLoggerOpen(10,0,ERROR,DEBUG,0)==-1)
    {
      mPrintf(L_STD|L_SYS,FATAL,__func__,0,"Can't initialize Message Logger! "
              "Exiting...");
      exit(1);
    }
  }
  mPrintf(errU,INFO,__func__,0,"Starting FMC Process Controller Server... "
          "Using: \"%s\", \"FMC-%s\".",rcsidP,FMC_VERSION);
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
      dimDnsNode=getDimDnsNode(dimConfFile,0,1);
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
  /* check default user and group */
  /* get the UID and the GID from username */
  /* try to convert the -u option argument to UID number */
  dfltUid=(uid_t)strtol(dfltUserName.c_str(),&endptr,0);
  if((dfltUid==0 && (errno==EINVAL || errno==ERANGE)) || endptr[0]!=0)
  {
    errno=0;
    /* get the UID from the username */
    pw=getpwnam(dfltUserName.c_str());
    if(!pw)
    {
      if(!errno)
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getpwnam(\"%s\") "
                 "failed: %s!",dfltUserName.c_str(),"The given name or uid "
                 "was not found");
      else
         mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getpwnam(\"%s\") "
                 "failed: %s!",dfltUserName.c_str(),strerror(errno));
      shortUsage();
      exit(1);
    }
    dfltUid=pw->pw_uid;                                               /* UID */
    dfltGid=pw->pw_gid;                           /* default GID of user UID */
  }
  if(dfltGroupName!="nobody")                   /* if -g cmdline option used */
  {
    /* get the GID from groupname */
    /* try to convert the -g option argument to GID number */
    dfltGid=(gid_t)strtol(dfltGroupName.c_str(),&endptr,0);
    if((dfltGid==0 && (errno==EINVAL || errno==ERANGE)) || endptr[0]!=0)
    {
      errno=0;
      /* get the GID from the groupname */
      gr=getgrnam(dfltGroupName.c_str());
      if(!gr)
      {
        if(!errno)
           mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getgrnam(\"%s\") "
                   "failed: %s!",dfltGroupName.c_str(),"The given group or "
                   "gid was not found");
        else
           mPrintf(L_STD|L_SYS|errU,FATAL,__func__,0,"getgrnam(\"%s\") "
                   "failed: %s!",dfltGroupName.c_str(),strerror(errno));
        shortUsage();
        exit(1);
      }
      dfltGid=gr->gr_gid;                  /* GID from the -g cmdline option */
    }
  }
  if(dfltUid!=99)
  {
    struct passwd *newPw=NULL;
    errno=0;
    /* get the username from the UID */
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
    else
    {
      dfltUserName=newPw->pw_name;
    }
  }
  if(dfltGid!=99)
  {
    struct group *newGr=NULL;
    errno=0;
    /* get the groupname from the GID */
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
    else
    {
      dfltGroupName=newGr->gr_name;
    }
  }
  mPrintf(errU,INFO,__func__,0,"Default user: \"%s\" (%d); Default group: "
          "\"%s\" (%d).",dfltUserName.c_str(),dfltUid,dfltGroupName.c_str(),
          dfltGid);
  /*-------------------------------------------------------------------------*/
  /* get kernel version */
  if(uname(&osBuf))
  {
    mPrintf(errU,ERROR,__func__,0,"uname(): %s",strerror(errno));
    kernelVersion=(char*)"N/A";
  }
  else
  {
    kernelVersion=osBuf.release;
  }
  /*-------------------------------------------------------------------------*/
  /* Control the behavior of the OOM killer on pcSrv process */
  if(!access(PC_OOM_SCORE_ADJ_INODE,W_OK))               /* kernel >= 2.6.36 */
  {
    int pcOomScoreAdjFD=-1;
    pcOomScoreAdjVal=PC_OOM_SCORE_ADJ;
    snprintf(pcOomScoreAdjStr,8,"%d",pcOomScoreAdjVal);
    /* write value */
    pcOomScoreAdjFD=open(PC_OOM_SCORE_ADJ_INODE,O_WRONLY);
    if(pcOomScoreAdjFD==-1)
    {
      mPrintf(errU,WARN,__func__,0,"Cannot set oom_score_adj for the Process "
              "Controller: open(2): %s!",strerror(errno));
    }
    else
    {
      int wB=-1;
      wB=write(pcOomScoreAdjFD,pcOomScoreAdjStr,strlen(pcOomScoreAdjStr));
      if(wB==-1)
      {
        mPrintf(errU,WARN,__func__,0,"Cannot set oom_score_adj for the "
                "Process Controller: write(2): %s!",strerror(errno));
      }
      else
      {
        mPrintf(errU,DEBUG,__func__,0,"Process Controller oom_score_adj set "
                "to %s.",pcOomScoreAdjStr);
      }
      close(pcOomScoreAdjFD);
    }
    /* read written value */
    memset(pcOomScoreAdjStr,0,8);
    pcOomScoreAdjFD=open(PC_OOM_SCORE_ADJ_INODE,O_RDONLY);
    read(pcOomScoreAdjFD,pcOomScoreAdjStr,8);
    close(pcOomScoreAdjFD);
    p=strchr(pcOomScoreAdjStr,'\n');
    if(p)*p='\0';
  }
  else if(!access(PC_OOM_ADJ_INODE,W_OK))                 /* kernel < 2.6.36 */
  {
    int pcOomAdjFD=-1;
    pcOomAdjVal=PC_OOM_ADJ;
    snprintf(pcOomAdjStr,8,"%d",pcOomAdjVal);
    /* write value */
    pcOomAdjFD=open(PC_OOM_ADJ_INODE,O_WRONLY);
    if(pcOomAdjFD==-1)
    {
      mPrintf(errU,WARN,__func__,0,"Cannot set oom_adj for the Process "
              "Controller: open(2): %s!",strerror(errno));
    }
    else
    {
      int wB=-1;
      wB=write(pcOomAdjFD,pcOomAdjStr,strlen(pcOomAdjStr));
      if(wB==-1)
      {
        mPrintf(errU,WARN,__func__,0,"Cannot set oom_adj for the Process "
                "Controller: write(2): %s!",strerror(errno));
      }
      else
      {
        mPrintf(errU,DEBUG,__func__,0,"Process Controller oom_adj set to %s.",
                pcOomAdjStr);
      }
      close(pcOomAdjFD);
    }
    /* read written value */
    memset(pcOomAdjStr,0,8);
    pcOomAdjFD=open(PC_OOM_ADJ_INODE,O_RDONLY);
    read(pcOomAdjFD,pcOomAdjStr,8);
    close(pcOomAdjFD);
    p=strchr(pcOomAdjStr,'\n');
    if(p)*p='\0';
  }
  else
  {
    mPrintf(errU,WARN,__func__,0,"OOM Killer Score Adjustment not foreseen "
            "in this OS version (%s)!",kernelVersion);
  }
  /*-------------------------------------------------------------------------*/
  /* block SIGINT & SIGTERM, to be handled synchronously by sigtimedwait() */
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigaddset(&signalMask,SIGHUP);
  if(pthread_sigmask(SIG_BLOCK,&signalMask,NULL))eExit("pthread_sigmask()");
  /*-------------------------------------------------------------------------*/
  /* Start DIM. Here threads becomes 3 */
  dim_init();
  /*-------------------------------------------------------------------------*/
  /* read environment variables */
  deBugS=getenv("deBug");
  if(deBugS)deBug=(int)strtol(deBugS,(char**)NULL,0);
  mPrintf(errU,DEBUG,__func__,0,"Debug mask = %#x.",deBug);
  /*-------------------------------------------------------------------------*/
  /* Read the list of the nodes to be controlled from the file confFileName  */
  cluster.readNodes(confFileName);
  cluster.printNodes(INFO,"Controlled nodes: ",", ",".\n");
  /*-------------------------------------------------------------------------*/
  /* Read from the file initFileName a list of processes to be started at    */
  /* pcSrv start-up and add them to the scheduled process list               */
  cluster.readInit(initFileName);
  //cluster.printInit(INFO,"Process(es) added at start-up: ",", ",".\n");
  /*-------------------------------------------------------------------------*/
  /* read the access control list */
  acl.read(aclFileName);
  acl.print(INFO,"Allowed user@nodes pairs: ",", ",".\n");
  /*-------------------------------------------------------------------------*/
  /* define server path */
  srvPath=string(getSrvPrefix())+"/"+srvName;
  /*-------------------------------------------------------------------------*/
  /* install server SVCs and CMDs */
  lsSvc=new PcLsSvc(srvPath+"/ls");                         /* never deleted */
  llSvc=new PcLlSvc(srvPath+"/ll");                         /* never deleted */
  lssSvc=new PcLssSvc(srvPath+"/lss");                      /* never deleted */
  addCmd=new PcAddCmd(srvPath+"/add");                      /* never deleted */
  rmCmd=new PcRmCmd(srvPath+"/rm");                         /* never deleted */
  /*-------------------------------------------------------------------------*/
  /* install minor SVCs */
  DimService successSvc((srvPath+"/success").c_str(),success); 
  DimService fmcVersionSvc((srvPath+"/fmc_version").c_str(),
                           (char*)FMC_VERSION); 
  DimService versionSvc((srvPath+"/server_version").c_str(),rcsidP); 
  /*-------------------------------------------------------------------------*/
  /* start DIM server */
  DimServer::start(srvPath.c_str()); 
  /*-------------------------------------------------------------------------*/
  /* manage Task Manager rise-ups */
  PcTmSeeker tmSeeker(DIM_DNS_SRV_LIST);
  /*-------------------------------------------------------------------------*/
  for(;;)                                                       /* main loop */
  {
    updatePeriod.tv_sec=14400;                                        /* 4 h */
    updatePeriod.tv_nsec=0;
    //signo=sigwaitinfo(&signalMask,NULL);
    signo=sigtimedwait(&signalMask,NULL,&updatePeriod);
    if(signo==SIGTERM||signo==SIGINT)signalHandler(signo);
    cluster.printUpdateCnt(DEBUG);                   /* print update counter */
  }
  return 0;
}
/*****************************************************************************/
/* signalHandler() executed synchronously in the main thread. So don't warry */
/* about async-signal-safe.                                                  */
static void signalHandler(int signo)
{
  if(signo==SIGTERM || signo==SIGINT)
  {
    mPrintf(errU|L_SYS,WARN,__func__,0,"Signal %u (%s) caught. Sending stop "
            "command to all the processes started by this Process Controller "
            "instance...",signo,sig2msg(signo));
    /* stop all the controlled processes */
    vector<string> nodePttn;
    nodePttn.push_back("*");
    cluster.rmSchPs("*",nodePttn,15,4,"pcRm *");
    mPrintf(errU|L_SYS,WARN,__func__,0,"Signal %u (%s) caught. Stop command "
            "sent to all the processes started by this Process Controller "
            "instance. Now exiting...",signo,sig2msg(signo));
    _exit(0);
  }
  else
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"Signal %u (%s) caught. "
            "Exiting...",signo,sig2msg(signo));
    _exit(signo);
  }
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"pcSrv [-N | --dim_dns_node DIM_DNS_NODE] [-l | --logger LOGGER]\n"
"      [--no-authentication] [--start-time-max-size MAX_SIZE]\n"
"      [-u | --dflt_user DEFAULT_USER] [-g | --dflt_group DEFAULT_GROUP]\n"
"      [-c | --conf-file CONF_FILE] [-s | --init-file INIT_FILE]\n"
"pcSrv { -h | --help }\n"
"Try \"pcSrv -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  mPrintf(L_DIM|L_STD|L_SYS,INFO,__func__,0,"\n%s",shortUsageStr);
  exit(1);
}
/*****************************************************************************/
void usage(int mode)
{
  FILE *fpOut;
  int status;
  const char *formatter;
  /*-------------------------------------------------------------------------*/
  if(!mode)formatter="/usr/bin/groff -man -Tascii | /usr/bin/less -iRs";
  else formatter="/bin/cat";
  fpOut=popen(formatter,"w");
  fprintf(fpOut,
".ig\n"
"pcSrv.man\n"
"\n"
"..\n"
"%s"
".hw WILDCARD_PATTERN USER_NAME \n"
".TH pcSrv 8  %s \"FMC-%s\" \"FMC Servers\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis pcSrv\\ \\-\n"
"FMC Process Controller Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis pcSrv\n"
".DoubleOpt[] N dim\\[ru]dns\\[ru]node DIM_DNS_NODE\n"
".DoubleOpt[] l logger LOGGER\n"
".LongOpt[] no\\[hy]authentication\n"
".LongOpt[] start\\[hy]time\\[hy]max\\[hy]size MAX_SIZE\n"
".DoubleOpt[] u dflt_user DEFAULT_USER\n"
".DoubleOpt[] g dflt_group DEFAULT_GROUP\n"
".DoubleOpt[] c conf-file CONF_FILE\n"
".DoubleOpt[] s init-file INIT_FILE\n"
".EndSynopsis\n"
".sp\n"
".Synopsis pcSrv\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".SH DESCRIPTION\n"
"The FMC \\fBProcess Controller\\fP is a tool in charge of \\fIkeeping a "
"dynamically manageable list of applications up and running\\fP on the farm "
"nodes.\n"
".PP\n"
"The Process Controller Server (\\fBpcSrv\\fP(8)) tipically runs on a few\n"
"control PCs (each one watching ~200 farm nodes) and performs its task by\n"
"maintaining the \\fIlist of the scheduled applications\\fP for each\n"
"controlled farm node and by interacting with the FMC Task Manager\n"
"Servers (\\fBtmSrv\\fP(8)) running on each farm node to start processes,\n"
"to obtain the notification of the process termination, to re-start the\n"
"untimely terminated processes (\"respawn\"), and to stop processes.\n"
".PP\n"
"The list of the controlled farm nodes is read from the Process Controller "
"configuration file \\fICONF_FILE\\fP (by default: \"/etc/pcSrv.conf\").\n"
".PP\n"
"At runtime, processes can be added to the list for one or more nodes by\n"
"means of the \\fBpcAdd\\fP(3) command and removed from the list for one or\n"
"more nodes by means of the \\fBpcRm\\fP(3) command. The commands\n"
"\\fBpcLs\\fP(3), \\fBpcLl\\fP(3) and \\fBpcLss\\fP(3) provide several\n"
"information about the controlled processes.\n"
".PP\n"
"At \\fBpcSrv\\fP start-up the file \\fIINIT_FILE\\fP (by default:\n"
"\"/etc/pcSrv.init\") is read and the \\fBpcAdd\\fP(3) commands contained in\n"
"the file are executed.\n"
".PP\n"
"By default the Process Controller implements a rhosts-like authentication:\n"
"only the couples user@host which match at least one of the patterns\n"
"user_pattern@host_pattern listed in the file \"/etc/pcSrv.allow\" are\n"
"allowed to issue \\fBpcAdd\\fP(3) and \\fBpcRm\\fP(3) commands to the\n"
"Process Controller Server. In turn the \\fBtmStart\\fP(3) and\n"
"\\fBtmStop\\fP(3) commands sent by the Process Controller Server to the\n"
"Task Manager Servers include by default an authentication string\n"
"containing, as host, the hostname of the PC running \\fBpcSrv\\fP and, as\n"
"user, the real user ID of the \\fBpcSrv\\fP process. The authentication can\n"
"be disabled by means of the \\fB--no-authentication\\fP command-line\n"
"switch: in this case all the users from all the hosts are allowed to send\n"
"send \\fBpcAdd\\fP(3) and \\fBpcRm\\fP(3) commands to the Process\n"
"Controller Server and the \\fBtmStart\\fP(3) and \\fBtmStop\\fP(3) commands\n"
"sent by the Process Controller Server to the Task Manager Servers do not\n"
"contain the authentication string.\n"
".PP\n"
"The DIM Name Server, looked up by \\fBpcSrv\\fP to register the provided\n"
"DIM services, can be chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\"\\fB"DIM_CONF_FILE_NAME"\\fP\".\n"
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
".OptDef \"\" no-authentication\n"
"Do not check the permission of the user and host which sent the command.\n"
"\\fBDefault\\fP: do check the permission.\n"
".\n"
".OptDef \"\" starttime-max-size MAX_SIZE (integer)\n"
"Set the maximum number of (re)start times for each process kept stored in a\n"
"buffer (to be shown by means of the \\fBpcLss\\fP(3) command) to\n"
"\\fIMAX_SIZE\\fP.\n"
"\\fBDefault\\fP: %d.\n"
".\n"
".OptDef u dflt_user DEFAULT_USER (string)\n"
"Assign the started processes, by default, to the user \\fIDEFAULT_USER\\fP.\n"
"\\fBDefault\\fP: \\fIDEFAULT_USER\\fP\\~=\\~\"nobody\".\n"
".\n"
".OptDef u dflt_group DEFAULT_GROUP (string)\n"
"Assign the started processes, by default, to the group\n"
"\\fIDEFAULT_GROUP\\fP.\n"
"\\fBDefault\\fP: \\fIDEFAULT_GROUP\\fP\\~=\\~\"nobody\".\n"
".\n"
".OptDef c conf-file CONF_FILE (string)\n"
"Read the configuration (i.e. the list of the nodes to be controlled by\n"
"this Process Controller) from the file \\fICONF_FILE\\fP.\n"
"\\fBDefault\\fP: \\fICONF_FILE\\fP\\~=\\~\"/etc/pcSrv.conf\".\n"
".\n"
".OptDef s init-file INIT_FILE (string)\n"
"Read from file \\fIINIT_FILE\\fP a list of commands to add scheduled\n"
"processes to the list, as soon as the Process Controller is ready.\n"
"\\fBDefault\\fP: \\fIINIT_FILE\\fP\\~=\\~\"/etc/pcSrv.init\".\n"
"Anyhow processes can be added later by using the \\fBpcAdd\\fP(3) command.\n"
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
".EnvVar deBug \\ (integer,\\ optional)\n"
"Debug mask. Can be set to 0x00...0xff. The meaning of the mask bit are:\n"
".RS\n"
".TP\n"
"0x01:\nprint PcTmSeeker diagnostics;\n"
".TP\n"
"0x02:\nprint PcTmWatcher diagnostics;\n"
".TP\n"
"0x04:\nprint PcAcl diagnostics;\n"
".TP\n"
"0x08:\nprint PcProcess diagnostics;\n"
".TP\n"
"0x10:\nprint PcNode diagnostics;\n"
".TP\n"
"0x20:\nprint PcCluster diagnostics;\n"
".PP\n"
"\\fBDefault\\fP: \\fIdeBug\\fP = 0.\n"
".RE\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Start the FMC Process Controller Server without authentication, defining the "
"user \"online\" as the \\fIDEFAULT_USER\\fP and the group \"onliners\" as "
"the \\fIDEFAULT_GROUP\\fP, and sending messages to the default FMC Message "
"Logger:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/pcSrv -u online -g onliners --no-auth\n"
".PP\n"
"Start the FMC Process Controller Server without authentication, defining the "
"user \"online\" as the \\fIDEFAULT_USER\\fP and the default group of the "
"user \"online\" as the \\fIDEFAULT_GROUP\\fP, and sending messages to the "
"default FMC Message Logger:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/pcSrv -u online --no-auth\n"
".\n"
".\n"
".SH FILES\n"
".\n"
".I /etc/pcSrv.conf\n"
".br\n"
".I /etc/pcSrv.init\n"
".br\n"
".I /etc/pcSrv.allow\n"
".\n"
".\n"
".SS Configuration File\n"
".I /etc/pcSrv.conf\n"
".PP\n"
".B Description:\n"
".br\n"
"The file \\fICONF_FILE\\fP (by default \"/etc/pcSrv.conf\") must contain the "
"list of the hostnames of the nodes controlled by the Process Controller "
"Server running on this node.\n"
".PP\n"
".B File format:\n"
".br\n"
"One hostname per record. Records are separated using '\\[rs]n' (new-line, "
"line-feed, LF, ASCII 0x0a). Blank records as well as comment records (i.e. "
"records starting with the '#' character) are skipped.\n"
".PP\n"
".B Record format:\n"
".br\n"
"hostname (without domain).\n"
".PP\n"
".B Record Examples:\n"
".br\n"
"farm0101\n"
".br\n"
"farm0102\n"
".br\n"
"farm0103\n"
".\n"
".\n"
".SS Initalization File\n"
".I /etc/pcSrv.init\n"
".PP\n"
".B Description:\n"
".br\n"
"The file \\fIINIT_FILE\\fP (by default \"/etc/pcSrv.init\") must contains "
"the list of the processes to be started at the start-up of the Process "
"Controller Server. Further processes can be added at runtime to the list by "
"means of the \\fBpcAdd\\fP(3) command. Processes can be removed from the "
"list by means of the \\fBpcRm\\fP(3) command.\n"
".PP\n"
".B File format:\n"
".br\n"
"One process entry per record. Records are separated using '\\[rs]n' "
"(new-line, line-feed, LF, ASCII 0x0a). Blank records as well as comment "
"records (i.e. records starting with the '#' character) are skipped.\n"
".PP\n"
".B Record format:\n"
".br\n"
"The same format of the \\fBpcAdd\\fP(3) command (excepting \\fB-v\\fP, "
"\\fB-N\\fP, \\fB-C\\fP and \\fB--no-authentication\\fP options), i.e.:\n"
".PP\n"
".ad l\n"
"[\\fB-m\\fP \\fINODE_PATTERN...\\fP]\n"
"[\\fB-c\\fP]\n"
"[\\fB-D\\fP\\ \\fINAME=value...\\fP]\n"
"[\\fB-s\\fP\\ \\fISCHEDULER\\fP]\n"
"[\\fB-p\\fP\\ \\fINICE_LEVEL\\fP]\n"
"[\\fB-r\\fP\\ \\fIRT_PRIORITY\\fP]\n"
"[\\fB-a\\fP\\ \\fICPU_NUM...\\fP]\n"
"[\\fB-d\\fP]\n"
"[\\fB-n\\fP\\ \\fIUSER_NAME\\fP]\n"
"[\\fB-g\\fP\\ \\fIGROUP_NAME\\fP]\n"
"[\\fB-w\\fP\\ \\fIWD\\fP]\n"
"[\\fB-o\\fP]\n"
"[\\fB-e\\fP]\n"
"[\\fB-O\\fP\\ \\fIOUT_FIFO\\fP]\n"
"[\\fB-E\\fP\\ \\fIERR_FIFO\\fP]\n"
"[\\fB-A\\fP]\n"
"[\\fB-M\\fP\\ \\fIMAX_START_N\\fP]\n"
"[\\fB-K\\fP\\ \\fICHECK_PERIOD\\fP]\n"
"[\\fB-X\\fP\\ \\fIDIS_PERIOD\\fP]\n"
"\\fB-u\\fP\\ \\fIUTGID\\fP\n"
"\\fIPATH\\fP\n"
"[\\fIARG...\\fP]\n"
".ad b\n"
".PP\n"
".B Parameter Expansion:\n"
".br\n"
"The string \"${RUN_NODE}\" is replaced by the hostname of the node on which "
"the process is started, in the fields: \\fIUTGID\\fP, \\fIPATH\\fP, "
"\\fIARG\\fP, \\fIWD\\fP, \\fINAME=value\\fP, \\fIOUT_FIFO\\fP and "
"\\fIERR_FIFO\\fP.\n"
".PP\n"
".B Record Examples:\n"
".br\n"
"-m * -e -o -c -u memSrv_u /opt/FMC/sbin/memSrv -l 1\n"
".br\n"
"-m * -e -o -c -u fsSrv_u -n root /opt/FMC/sbin/fsSrv -l 1 -a\n"
".br\n"
"-m * -c -u gaudiLog /opt/FMC/sbin/logSrv -S 100 -p /tmp/logGaudi.fifo -s "
"gaudi\n"
".\n"
".\n"
".SS Access Control File\n"
".I /etc/pcSrv.allow\n"
".PP\n"
".B Description:\n"
".br\n"
"The file \\fI/etc/pcSrv.allow\\fP must contain the list of the couples of\n"
"patterns \\fBuser_pattern@hostname_pattern\\fP allowed to issue commands\n"
"to the Process Controller Server running on the current node.\n"
".PP\n"
".B File format:\n"
".br\n"
"One couple \\fBuser_pattern@hostname_pattern\\fP per record. Records are "
"separated using '\\[rs]n' (new-line, line-feed, LF, ASCII 0x0a). Blank "
"records as well as comment records (i.e. records starting with the '#' "
"character) are skipped.\n"
".PP\n"
".B Record format:\n"
".br\n"
"\\fBuser_pattern@hostname_pattern\\fP.\n"
".br\n"
"user_pattern and host_pattern are separated by the '@' (at) character.\n"
".PP\n"
".B Fields description:\n"
".TP\n"
".B user_pattern\n"
"User name wildcard pattern (see glob(7)).\n"
".TP\n"
".B host_pattern\n"
"Host name wildcard pattern (see glob(7)).\n"
".\n"
".PP\n"
".B Record Examples:\n"
".br\n"
"online@farm0101.mydomain\n"
".br\n"
"*@farm0101.mydomain\n"
".br\n"
"online@*\n"
".br\n"
"*@*\n"
".br\n"
"online@farm01??.mydomain\n"
".br\n"
"online@farm01?[0-9].mydomain\n"
".br\n"
"online@farm01[0-9][0-9].mydomain\n"
".\n"
".\n"
".SH DIM INFORMATION\n"
".\n"
".SS DIM Server Name\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/"SRV_NAME"\n"
"where \\fIHOST_NAME\\fP is the host name of the current PC, as returned by\n"
"the command \"hostname -s\", but written in upper case characters.\n"
".\n"
".\n"
".SS Published DIM Services\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/ls (string)\n"
"Publishes a NULL-terminated string containing the list of the scheduled "
"UTGIDs, node by node, with the format:\n"
".IP\n"
"host_1:N_1:utgid_1,utgid_2,...,utgid_N_1.\\[rs]n\n"
".br\n"
"host_2:N_2:utgid_1,utgid_2,...,utgid_N_2.\\[rs]n\n"
".br\n"
"\\[md]\\[md]\\[md]\n"
".IP\n"
"Example:\n"
".IP\n"
"farm0101:2:counter_1,counter_0.\\[rs]n\n"
".br\n"
"farm0102:1:counter_0.\\[rs]n\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/ll (string)\n"
"Publishes a NULL-terminated string containing the list of the scheduled "
"UTGIDs, node by node, with the start-up details (s_1,d_2,...) between "
"parentheses, with the format:\n"
".IP\n"
"host_1:N_1:utgid_1(d_1,d_2,...),...,utgid_N_1(d_1,d_2,...).\\[rs]n\n"
".br\n"
"host_2:N_2:utgid_1(d_1,d_2,...),...,utgid_N_2(d_1,d_2,...).\\[rs]n\n"
".br\n"
"\\[md]\\[md]\\[md]\n"
".IP\n"
"Example:\n"
".IP\n"
".ad l\n"
"farm0101:2:\\:counter_1\\:(\\:wd=\"/opt/FMC/tests\",\\:path=\"./counter\","
"\\:clearEnv,\\:LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib,\\:"
"DIM_DNS_NODE=lhcbsrv.bo.infn.it\\:,daemon,\\:SCHED_FIFO,\\:nice=0,\\:"
"rtprio=1,\\:affMask=0x00000007,\\:user=\"online\",\\:rdrStderr,\\:"
"rdrStdout,\\:maxStartN=20,\\:disPeriod=60s,\\:checkPeriod=30s),\\:"
"counter_0\\:(\\:wd=\"/opt/FMC/tests\",\\:path=\"./counter\",\\:clearEnv,\\:"
"LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib,\\:"
"DIM_DNS_NODE=lhcbsrv.bo.infn.it,daemon,\\:SCHED_OTHER,\\:nice=-12,\\:"
"rtprio=0,\\:affMask=0x00000007,\\:user=\"online\",\\:rdrStderr,\\:"
"rdrStdout,\\:maxStartN=20,\\:disPeriod=60s,\\:checkPeriod=30s).\\[rs]n\n"
".br\n"
"farm0102:1:counter_0\\:(\\:wd=\"/opt/FMC/tests\",\\:path=\"./counter\",\\:"
"clearEnv,\\:LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib,\\:"
"DIM_DNS_NODE=lhcbsrv.bo.infn.it,daemon,\\:SCHED_OTHER,\\:nice=-12,\\:"
"rtprio=0,\\:affMask=0x00000007,\\:user=\"online\",\\:rdrStderr,\\:"
"rdrStdout,\\:maxStartN=20,\\:disPeriod=60s,\\:checkPeriod=30s).\\[rs]n\n"
".ad b\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/lss (string)\n"
"Publishes a NULL-terminated string containing the list of the scheduled "
"UTGIDs, node by node, with the possible \"disabled\" state and the (re)start times [t_1,t_2,...] between "
"squared brackets, with the format:\n"
".IP\n"
".ad l\n"
"host_1:N_1:utgid_1[(re)start=t_1,t_2,...],\\:...,\\:utgid_N_1\\:"
"[(re)start\\:=\\:t_1,\\:t_2,\\:...].\\[rs]n\n"
".br\n"
"host_2:N_2:utgid_1[disabled,(re)start=t_1,t_2,...],\\:...,\\:utgid_N_2\\:"
"[(re)start\\:=\\:t_1,\\:t_2,\\:...].\\[rs]n\n"
".br\n"
"\\[md]\\[md]\\[md]\n"
".ad b\n"
".IP\n"
"Example:\n"
".IP\n"
".ad l\n"
"farm0101:2:counter_1[disabled,(re)start=3:Aug21-12:10:59.867288,\\:"
"Aug21-12:10:58.867369,\\:Aug21-12:10:57.867647],\\:counter_0\\:"
"[(re)start=\\:1:\\:Aug21-12:12:59.865047].\\[rs]n\n"
".br\n"
"farm0102:1:counter_0[(re)start=1:Aug21-12:10:59.867797].\\[rs]n\n"
".br\n"
"farm0103:2:counter_0[(re)start=0:(no start)],\\:counter_1\\:[(re)start=0:\\:"
"(nostart)].\\[rs]n\n"
".ad b\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/server_version (string)\n"
"Publishes a NULL-terminated string containing the RCS revision label of\n"
"the \\fBpcSrv.C\\fP source code of\n"
"the current \\fBpcSrv\\fP instance, as returned by the command\n"
"\"ident /opt/FMC/sbin/pcSrv\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/fmc_version (string)\n"
"Publishes a NULL-terminated string containing the revision label of the FMC\n"
"package which includes the current \\fBpcSrv\\fP executable, in the\n"
"present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/success (int)\n"
"Always publishes the integer 1. This dummy services can be used to check\n"
"whether the current instance of \\fBpcSrv\\fP is up and running.\n"
".\n"
".\n"
".SS Published DIM Commands:\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/add\n"
"Accepts, as argument, a NULL-terminated string containing the \\fIPATH\\fP "
"to the executable image to be added to the list, the command line arguments "
"(\\fIARG\\fP), and the Start and Respawn Control options, in the format:\n"
".IP\n"
".ad l\n"
"[\\fB-m\\fP \\fINODE_PATTERN...\\fP]\n"
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
"[\\fB-w\\fP\\ \\fIWD\\fP]\n"
"[\\fB-o\\fP]\n"
"[\\fB-e\\fP]\n"
"[\\fB-O\\fP\\ \\fIOUT_FIFO\\fP]\n"
"[\\fB-E\\fP\\ \\fIERR_FIFO\\fP]\n"
"[\\fB-A\\fP]\n"
"[\\fB-M\\fP\\ \\fIMAX_START_N\\fP]\n"
"[\\fB-K\\fP\\ \\fICHECK_PERIOD\\fP]\n"
"[\\fB-X\\fP\\ \\fIDIS_PERIOD\\fP]\n"
"\\fB-u\\fP\\ \\fIUTGID\\fP\n"
"\\fIPATH\\fP\n"
"[\\fIARG...\\fP]\n"
".ad b\n"
".PP\n"
".RS\n"
".B DIM Command Options\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"Add the process \\fIPATH\\fP to the list for the nodes (among those "
"controlled by the current \\fBpcSrv\\fP instance) whose hostname matches the "
"wildcard pattern \\fINODE_PATTERN\\fP (\\fBdefault\\fP: add process to the "
"list for all the nodes controlled by the current \\fBpcSrv\\fP instance).\n"
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
"priority process, or it calls \\fBsched_yield\\fP(2).\n"
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
".OptDef M \"\" MAX_START_N (integer)\n"
".OptDef+ K \"\" CHECK_PERIOD (integer)\n"
".OptDef+ X \"\" DIS_PERIOD (integer)\n"
"Respawn control parameters. If a process is (re)started more than\n"
"\\fIMAX_START_N\\fP times in \\fICHECK_PERIOD\\fP seconds, then the\n"
"process respawn is disabled for \\fIDIS_PERIOD\\fP seconds. \\fBDefault\\fP\n"
"values are: \\fIMAX_START_N\\fP\\~=\\~10,\n"
"\\fICHECK_PERIOD\\fP\\~=\\~120\\~s and\n"
"\\fIDIS_PERIOD\\fP\\~=\\~300\\~s. If \\fIMAX_START_N\\fP\\~=\\~-1, then\n"
"the respawn control is excluded, i.e. process can be restarted\n"
"indefinitely. If \\fIDIS_PERIOD\\fP\\~=\\~-1, then the process restart\n"
"for the process, once disabled, is never re-enabled.\n"
".\n"
".OptDef u \"\" UTGID (string, mandatory)\n"
"Set the process \\fButgid\\fP (User assigned unique Thread Group\n"
"Identifier) to \\fIUTGID\\fP.\n"
".\n"
".TP\n"
"\\fIPATH\\fP\n"
"The path to the executable image to be started\n"
".\n"
".TP\n"
"[\\fIARGS\\fP]\n"
"The path to the executable image to be started\n"
".PP\n"
".B Parameter Expansion:\n"
".br\n"
"The string \"${RUN_NODE}\" is replaced by the hostname of the node on which "
"the process is started, in the fields: \\fIUTGID\\fP, \\fIPATH\\fP, "
"\\fIARG\\fP, \\fIWD\\fP, \\fINAME=value\\fP, \\fIOUT_FIFO\\fP and "
"\\fIERR_FIFO\\fP.\n"
".\n"
".RE\n"
".\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/rm\n"
"Accepts, as argument, a NULL-terminated string containing the Stop options "
"and the UTGID wildcard pattern of the process to be removed from the list "
"and stopped, in the format:\n"
".IP\n"
".ad l\n"
"[\\fB-m\\fP \\fINODE_PATTERN...\\fP]\n"
"[\\fB-s\\fP\\ \\fISIGNAL\\fP]\n"
"[\\fB-d\\fP\\ \\fIKILL_9_DELAY\\fP]\n"
"\\fIUTGID_PATTERN\\fP\n"
".ad b\n"
".PP\n"
".RS\n"
".B DIM Command Options\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"Remove the process(es) which match the pattern \\fIUTGID_PATTERN\\fP from "
"the list for the nodes (among those controlled by the current \\fBpcSrv\\fP "
"instance) whose hostname matches the wildcard pattern \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: remove process(es) from the list for all the nodes "
"controlled by the current \\fBpcSrv\\fP instance).\n"
".\n"
".OptDef s \"\" SIGNAL (integer)\n"
"Send the signal \\fISIGNAL\\fP as the first signal to stop the process(es)\n"
"(\\fBdefault\\fP: signal 15, \\fBSIGTERM\\fP).\n"
".\n"
".OptDef d \"\" KILL_9_DELAY (integer)\n"
"Wait \\fIKILL_9_DELAY\\fP seconds before sending the second signal (which\n"
"is always the signal 9, \\fBSIGKILL\\fP). \\fBDefault\\fP: 4 s.\n"
".\n"
".TP\n"
"\\fIUTGID_PATTERN\\fP\n"
"The UTGID wildcard pattern of the processes to be removed from the list and "
"stopped.\n"
".\n"
".RE\n"
"%s"                                                             /* FMC_URLS */
"%s"                                                          /* FMC_AUTHORS */
".\n"
".\n"
".SH VERSION\n"
".\n"
"Source:  %s\n"
".PP\n"
"Package: FMC-%s\n"
".\n"
".\n"
".SH SEE ALSO:\n"
".BR \\%%pcAdd (1),\n"
".BR \\%%pcRm (1),\n"
".BR \\%%pcLs (1),\n"
".BR \\%%pcLl (1),\n"
".BR \\%%pcLss (1).\n"
".br\n"
".BR \\%%tmSrv (8),\n"
".BR \\%%tmStart (1),\n"
".BR \\%%tmStop (1),\n"
".BR \\%%tmLs (1).\n"
".br\n"
".BR \\%%cmSrv (8),\n"
".BR \\%%cmSet (1),\n"
".BR \\%%cmUnset (1),\n"
".BR \\%%cmGet (1).\n"
".br\n"
".BR \\%%init (8),\n"
".BR \\%%inittab (5).\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro,FMC_DATE,FMC_VERSION
,DFLT_START_TIME_MAX_SIZE
,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD,rcsidP,SVC_HEAD,FMC_VERSION
,SVC_HEAD
,SVC_HEAD,SVC_HEAD
,FMC_URLS,FMC_AUTHORS
,rcsidP,FMC_VERSION
       );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/*****************************************************************************/
