/* ************************************************************************* */
/*
 * $Log: tmUtils.c,v $
 * Revision 5.3  2012/08/29 08:28:22  galli
 * publishes "N/A" if oom score not available
 *
 * Revision 5.2  2012/08/27 12:26:07  galli
 * Publishes in longList the oom_score
 *
 * Revision 5.1  2011/11/29 12:17:03  galli
 * Minor changes.
 *
 * Revision 5.0  2011/11/28 16:27:12  galli
 * Can set the oom_adj or oom_score_adj of started processes.
 *
 * Revision 3.12  2009/09/16 13:48:11  galli
 * minor changes
 *
 * Revision 3.11  2009/09/16 12:51:02  galli
 * minor changes and bug fixes
 *
 * Revision 3.10  2009/01/22 10:16:10  galli
 * do not depend on kernel-devel headers
 *
 * Revision 3.9  2008/12/02 11:23:54  galli
 * minor changes
 *
 * Revision 3.8  2008/11/28 16:16:20  galli
 * Constant MAX_PID substituted by variable maxPid read at runtime
 * from /proc/sys/kernel/pid_max.
 *
 * Revision 3.7  2008/11/11 21:23:06  galli
 * The threshold for the ptrace(2) workaround is now a variable
 * The size of chunks read from procfs is now a variable
 *
 * Revision 3.6  2008/10/09 09:39:31  galli
 * sig2msg() moved to fmcSigUtils.c
 *
 * Revision 3.5  2008/08/13 11:11:09  galli
 * Bug fixed. Occurred for processes with a huge env (>4096 B) w/o UTGID
 * To these proceesses was incorrectly attributed, in listing,
 * the UTGID of the last scanned process with huge env and UTGID set.
 *
 * Revision 3.4  2008/05/05 12:31:22  galli
 * minor bug fixed
 *
 * Revision 3.2  2008/05/02 15:28:56  galli
 * added function sig2msg()
 *
 * Revision 3.1  2008/04/30 14:47:54  galli
 * does not need libprocps no more
 *
 * Revision 3.0  2008/04/30 12:53:14  galli
 * memory leak fixed in pLadd()
 *
 * Revision 2.8  2008/04/29 15:29:58  galli
 * - pLsvc() renamed pLllSvc()
 * - pLlsSvc() added
 *
 * Revision 2.7  2008/04/29 12:08:02  galli
 * - do not use procps no more
 * - directly use readdir_r instead
 *
 * Revision 2.4  2008/04/15 10:51:34  galli
 * Niko Neufeld patch as a workaround to a findUtgid bug
 *
 * Revision 2.3  2007/12/14 09:46:01  galli
 * minor changes
 *
 * Revision 2.1  2007/12/10 13:30:32  galli
 * Fast normally-terminating pss are no more confused with still-born ones.
 * Clean-up.
 *
 * Revision 1.55  2007/12/07 11:04:35  galli
 * Cut CMD at 256 characters instead of 16
 *
 * Revision 1.54  2007/12/06 14:09:50  galli
 * added function pLllSvc() to provide longList SVC
 *
 * Revision 1.53  2007/12/05 16:28:31  galli
 * added functions pLadd(), pLsetTerm(), pLrm(), pLrmTerm(), pLautoSetTerm(),
 * pLpop(), pLfind(), pLlen(), pLprint() to keep a list of processes (included
 * processes terminated within a timeout)
 *
 * Revision 1.51  2007/11/28 11:18:20  galli
 * bug fixed in printing start messages
 *
 * Revision 1.50  2007/11/28 10:53:27  galli
 * Back to only one version of findUtgid() and findTgid()
 *
 * Revision 1.47  2007/11/27 15:24:20  galli
 * 2 different findTgid functions (findTgidProcEnviron and findTgidLargeEnv)
 *
 * Revision 1.46  2007/11/23 14:48:10  galli
 * Added utility functions file2str(), getStaStkFromStatus()
 * and getStaStkFromStat().
 * Added function findUtgidLargeEnv()
 * Modified function getUtgidFromTgid()
 *
 * Revision 1.45  2007/11/14 15:06:07  galli
 * hard-coded constant moved to tmSrv.h
 * few hard-coded limits removed (SRVC_BUF_LEN, USER_NAME_LEN, GROUP_NAME_LEN,
 * FIFO_NAME_LEN)
 * 2 different findUtgid functions (findUtgidProcEnviron and findUtgidPtrace)
 * added function getUtgidFromTgid()
 *
 * Revision 1.43  2007/11/11 22:45:15  galli
 * back to version 1.41
 *
 * Revision 1.42  2007/11/11 22:40:15  galli
 * builds the new environment from scratch. Uses execve(2)
 *
 * Revision 1.41  2007/10/01 13:08:38  galli
 * minor changes
 *
 * Revision 1.40  2007/08/14 13:46:16  galli
 * compatible with libFMCutils v 2
 *
 * Revision 1.39  2007/06/28 13:03:33  galli
 * set the gid of the started process
 *
 * Revision 1.38  2006/10/20 09:24:47  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 1.37  2006/10/18 12:50:35  galli
 * uses the __func__ magic variable
 *
 * Revision 1.36  2006/09/29 09:28:48  galli
 * use fcntl() instead of close()+open()
 *
 * Revision 1.35  2006/09/27 15:06:45  galli
 * bug fixed in fifoInit()
 *
 * Revision 1.34  2006/08/25 13:40:51  galli
 * Function fifoInit() added.
 * Parameters: outFIFO, errFIFO and noDrop added in startPs() call.
 * Can redirect stdout/stderr of started processes to specific FIFOs
 * either in no-drop or in congestion-free mode.
 *
 * Revision 1.33  2006/08/22 09:08:18  galli
 * some DEBUG messages moved to higher debug level
 * unmask signal in started processes (works with some signals blocked)
 *
 * Revision 1.32  2005/08/29 11:39:07  galli
 * escape_command() calls modified to comply with libproc-3.2.5
 *
 * Revision 1.31  2005/06/22 10:27:52  galli
 * Set process-to-cpu affinity
 *
 * Revision 1.29  2005/01/06 22:52:34  galli
 * added findTgid() function
 * more responsive for still-born processes
 * always close file descriptors before starting a process
 *
 * Revision 1.27  2005/01/01 23:14:51  galli
 * bug fixed in findUtgid()
 * cleanup
 *
 * Revision 1.25  2004/12/13 00:45:51  galli
 * Modified openProcFlags to speed-up process retrieving.
 * Function startPs() waits until the UTGID is readable by findUtgid().
 * Function findUtgid() protected by a mutex
 * Bug fix and speed-up of genUtgid()
 *
 * Revision 1.23  2004/12/05 21:59:56  galli
 * sigChldHandler() moved to tmSrv
 * Can redirect new process stdout & stderr to DIM logger
 *
 * Revision 1.19  2004/11/21 19:47:30  galli
 * getActuatorVersion() function added
 *
 * Revision 1.18  2004/11/21 10:38:37  galli
 * startPs() can set additional environment variables to the new process
 *
 * Revision 1.17  2004/11/05 01:20:12  galli
 * Close out all the file descriptors
 *
 * Revision 1.16  2004/11/05 00:11:41  galli
 * Reopen the standard file descriptors on /dev/null
 *
 * Revision 1.15  2004/10/28 22:47:47  galli
 * startPs() can set uid, scheduler, niceLevel and real-time priority
 *
 * Revision 1.13  2004/10/15 16:52:43  galli
 * findUtgid() finds only for processes (thread groups) not also threads
 *
 * Revision 1.9  2004/10/13 11:14:13  galli
 * findUtgid() returns a structure containing tid, tgid and utgid of all
 * the found processes
 * killPs uses findUtgid
 *
 * Revision 1.7  2004/10/12 14:48:40  galli
 * killPs() send one kill and take signal # as argument
 *
 * Revision 1.6  2004/10/11 09:41:21  galli
 * Argument doClearenv added to startPs
 *
 * Revision 1.3  2004/09/21 00:12:18  galli
 * Added signal handler for SIGCLD to waitpid
 * Close out the standard file descriptors
 * Do setsid
 *
 * Revision 1.2  2004/09/20 00:03:42  galli
 * First working version
 *
 * Revision 1.1  2004/09/19 22:50:27  galli
 * Initial revision
 * */
/* ************************************************************************* */
#define _GNU_SOURCE
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <sys/types.h>/* fork(2), kill(2), opendir(3), readdir_r(3), open(2) */
#include <unistd.h>                            /* fork(2), execv(3), read(2) */
#include <signal.h>                                               /* kill(2) */
#include <errno.h>                                                  /* errno */
#include <string.h>                                           /* strerror(3) */
#include <stdlib.h>                      /* setenv(3), malloc(3), strtoul(3) */
#include <syslog.h>                                             /* syslog(2) */
#include <sys/stat.h>                                   /* umask(2), open(2) */
#include <fnmatch.h>                                           /* fnmatch(3) */
#include <time.h>                    /* time(2), localtime_r(3), strftime(3) */
#include <sched.h>                                  /* sched_setscheduler(2) */
#include <sys/time.h>                                      /* setpriority(2) */
#include <sys/resource.h>                                  /* setpriority(2) */
#include <fcntl.h>                            /* O_RDONLY, O_WRONLY, open(2) */
#include <argz.h>
#include <pthread.h>
#include <asm/unistd.h>                            /* __NR_sched_setaffinity */
#include <sys/ptrace.h>                                         /* ptrace(2) */
#include <sys/user.h>                                    /* user_regs_struct */
#include <sys/wait.h>         /* waitpid(2), WIFEXITED(), WEXITSTATUS(), ... */
#include <pwd.h>                                              /* getpwuid(3) */
#include <grp.h>                                              /* getgrgid(3) */
#include <dirent.h>                              /* opendir(3), readdir_r(3) */
#include <stddef.h>                                      /* offsetof() macro */
#include <limits.h>                                         /* HOST_NAME_MAX */
#include <sched.h>                                      /* sched_setaffinity */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcPar.h"       /* UTGID_LEN, DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcMacro.h"     /* eExit(), likely(), unlikely(), printOutFuncOK() */
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcMsgUtils.h"           /* dfltLoggerOpen(), mPrintf(), rPrintf() */
#include "fmcSigUtils.h"                                        /* sig2msg() */
/* ------------------------------------------------------------------------- */
/* in tm/include */
#define _SERVER 1
#include "tmSrv.h"                                    /* hard-coded constant */
#include "tmUtils.h"      /* startPs(), killPs(), findUtgid(), freePssinfo() */
/* ************************************************************************* */
/* external global */
extern int deBug;                                             /* debug level */
extern int errU;                                               /* error unit */
extern size_t procPidEnvSz;         /* max size of /proc/<pid>/environ files */
extern size_t procMaxReadChars; /* max char num read from procfs in one shot */
extern pid_t main_tgid;                           /* tgid of the main thread */
extern pthread_t main_ptid;          /* thread identifier of the main thread */
extern char nodeName[HOST_NAME_MAX+1];                          /* host name */
extern procList_t psList;
extern int maxPid;                                            /* maximum PID */
extern char *kernelVersion;
/* ************************************************************************* */
/* globals */
extern pthread_mutex_t openprocLock;
static char rcsid[]="$Id: tmUtils.c,v 5.3 2012/08/29 08:28:22 galli Exp galli $";
/* ************************************************************************* */
/* static function prototype */
static int file2str(const char *path,char *buf,int bufSz);
static char **file2strvec(const char* path);
static char getStateFromStat(char *stat);
static char getStateFromStatus(char *status);
static int getOomScore(char *oomScoreBuf);
static char *getCmdFromStatus(char *status);
static unsigned long getStaStkFromStatus(char *status);
static unsigned long getStaStkFromStat(char *stat);
void *realloc1(void *oldp,size_t size);
/* ************************************************************************* */
/* startPs() - Starts a new process with:                                    */
/*             wd:               as working directory.                       */
/*             path:             as path, absolute or relative to wd.        */
/*             argv:             as argument list.                           */
/*             envz:             as environment envz list.                   */
/*             envz_len:         as environment envz list lenght.            */
/*             utgid:            as UTGID.                                   */
/*             uid:              as user identifier.                         */
/*             gid:              as group identifier.                        */
/*             scheduler:        as scheduler type.                          */
/*             niceLevel:        as nice level.                              */
/*             rtPriority:       as static (real-time) priority.             */
/*             affMask:          as affinity mask                            */
/*                               0x00000001 cpu 0                            */
/*                               0x00000002 cpu 1                            */
/*                               0x00000003 cpu 0 or 1                       */
/*                               0x00000004 cpu 2                            */
/*                               0x00000005 cpu 0 or 2                       */
/*                               0x00000006 cpu 1 or 2                       */
/*             oomScoreAdj:      as oom killer score adjustment.             */
/*             asDaemon:         if true starts the process as daemon.       */
/*             doClearenv:       if true does not inherit task manager env.  */
/*             doRedirectStdout: if true redirects stdour to DIM logger FIFO.*/
/*             outFIFO:          if specified, the filename of the FIFO to   */
/*                               which redirect the stdout.                  */
/*             doRedirectStderr: if true redirects stderr to DIM logger FIFO.*/
/*             errFIFO:          if specified, the filename of the FIFO to   */
/*                               which redirect the stderr.                  */
/*             noDrop:           if true open the specific FIFOs in no-drop  */
/*                               mode; otherway open them in congestion-free */
/*                               mode.                                       */
/* ************************************************************************* */
int startPs(int asDaemon,int doClearenv,int doRedirectStdout,
            int doRedirectStderr,char *outFIFO,char *errFIFO,int noDrop,
            uid_t uid,gid_t gid,int scheduler,int niceLevel,int rtPriority,
            cpu_set_t* cpuSet,int oomScoreAdj, char *utgid,char *wd,
            char *path,char **argv,char *envz,int envz_len)
{
  int i;
  static int isFirstCall=1;
  pid_t tgid;
  char msg[MSG_LEN+1]="";
  char *msgP=NULL;
  size_t msgRL;
  char *eP=NULL;
  pssinfo pss={0,NULL};
  struct sched_param sp;
  char *ss[3]={"SCHED_OTHER","SCHED_FIFO","SCHED_RR"};
  int logFD=-1;
  int outFD=-1;
  int errFD=-1;
  struct timespec delay={0,100000000};                             /* 1/10 s */
  sigset_t psSignalMask;
  procListEl_t oldPs;
  char *cmd;
  unsigned long affMask=0x0;
  /* ----------------------------------------------------------------------- */
  if(isFirstCall)
  {
    /* syslog actuator version */
    mPrintf(errU|L_SYS,INFO,__func__,0,"Using: %s",rcsid);
    isFirstCall=0;
  }
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  if(!strcmp(wd,""))wd="/";
  if(!strcmp(utgid,""))utgid=genUtgid(path);
  /* ----------------------------------------------------------------------- */
  /* send "Starting process" message */
  msgP=msg;
  msgRL=MSG_LEN+1-9-(msgP-msg);                            /* 9: "... )...\0 */
  snprintf(msg,msgRL,"Starting process (path=\"%s\" wd=\"%s\" UTGID=\"%s\"",
           path,wd,utgid);
  msgP=strchr(msgP,'\0');
  msgRL=MSG_LEN+1-9-(msgP-msg);
  for(i=1;argv[i];i++)
  {
    snprintf(msgP,msgRL," argv[%d]=\"%s\"",i,argv[i]);
    msgP=strchr(msgP,'\0');
    msgRL=MSG_LEN+1-9-(msgP-msg);
  }
  if(asDaemon)
  {
    snprintf(msgP,msgRL," asDaemon");
    msgP=strchr(msgP,'\0');
    msgRL=MSG_LEN+1-9-(msgP-msg);
  }
  if(doClearenv)
  {
    snprintf(msgP,msgRL," doClearenv");
    msgP=strchr(msgP,'\0');
    msgRL=MSG_LEN+1-9-(msgP-msg);
  }
  if(doRedirectStdout)
  {
    snprintf(msgP,msgRL," doRedirectStdout");
    msgP=strchr(msgP,'\0');
    msgRL=MSG_LEN+1-9-(msgP-msg);
  }
  if(doRedirectStderr)
  {
    snprintf(msgP,msgRL," doRedirectStderr");
    msgP=strchr(msgP,'\0');
    msgRL=MSG_LEN+1-9-(msgP-msg);
  }
  if(outFIFO)
  {
    snprintf(msgP,msgRL," outFIFO=\"%s\"",outFIFO);
    msgP=strchr(msgP,'\0');
    msgRL=MSG_LEN+1-9-(msgP-msg);
  }
  if(errFIFO)
  {
    snprintf(msgP,msgRL," errFIFO=\"%s\"",errFIFO);
    msgP=strchr(msgP,'\0');
    msgRL=MSG_LEN+1-9-(msgP-msg);
  }
  if(noDrop)
  {
    snprintf(msgP,msgRL," no-drop");
    msgP=strchr(msgP,'\0');
    msgRL=MSG_LEN+1-9-(msgP-msg);
  }
  if(scheduler<0||scheduler>2)
    snprintf(msgP,msgRL," user=\"%s\" group=\"%s\" sched=%d rtprio=%d nice=%d",
             getpwuid(uid)->pw_name,getgrgid(gid)->gr_name,scheduler,
             rtPriority,niceLevel);
  else
    snprintf(msgP,msgRL," user=\"%s\" group=\"%s\" sched=%s rtprio=%d nice=%d",
             getpwuid(uid)->pw_name,getgrgid(gid)->gr_name,ss[scheduler],
             rtPriority,niceLevel);
  msgP=strchr(msgP,'\0');
  msgRL=MSG_LEN+1-9-(msgP-msg);
  for(i=0; i < CPU_SETSIZE && i < sizeof(unsigned long); ++i)
  {
     if (CPU_ISSET(i, cpuSet)) affMask |= 1 << i;
  }
  snprintf(msgP,msgRL," affMask=%#8.8lx",affMask);
  msgP=strchr(msgP,'\0');
  msgRL=MSG_LEN+1-9-(msgP-msg);
  snprintf(msgP,msgRL," oom_score_adj=%d",oomScoreAdj);
  msgP=strchr(msgP,'\0');
  msgRL=MSG_LEN+1-9-(msgP-msg);
  for(eP=argz_next(envz,envz_len,NULL);eP;eP=argz_next(envz,envz_len,eP))
  {
    char *p;
    p=strchr(eP,'=');
    if(p)
    {
      snprintf(msgP,MIN(3+p-eP,msgRL)," %s",eP);
      msgP=strchr(msgP,'\0');
      msgRL=MSG_LEN+1-9-(msgP-msg);
      snprintf(msgP,msgRL,"\"%s\"",p+1);
      msgP=strchr(msgP,'\0');
      msgRL=MSG_LEN+1-9-(msgP-msg);
    }
  }
  if(msgRL<2)
  {
    sprintf(msgP,"... ");
    msgP=strchr(msgP,'\0');
  }
  sprintf(msgP,")...");
  mPrintf(errU,DEBUG,__func__,0,"%s",msg);
  /* ----------------------------------------------------------------------- */
  pss=findUtgid(utgid);
  if(pss.psN!=0)
  {
    mPrintf(errU,ERROR,__func__,0,"A process with UTGID=\"%s\" already exists "
            "(tgid=%d)! Can't start a new process with the same UTGID!",
            pss.ps[0].utgid,pss.ps[0].tgid);
    freePssinfo(&pss);
    printOutFuncNOK;
    return 1;
  }
  freePssinfo(&pss);
  /* ----------------------------------------------------------------------- */
  /* open out/err FIFOs, if required */
  if(outFIFO)                                        /* specific output FIFO */
  {
    outFD=loggerOpen(outFIFO,noDrop,errU,ERROR);
    if(outFD==-1)
    {
      char fifoBn[1+strlen(outFIFO)];
      char *p;
      strcpy(fifoBn,outFIFO);
      p=strrchr(fifoBn,'.');
      if(p)*p='\0';
      p=strrchr(fifoBn,'/');
      if(p)p++;
      else p=fifoBn;
      mPrintf(errU,FATAL,__func__,0,"Cannot open the FIFO: \"%s\" for the "
              "stdout of the process \"%s\" (UTGID=\"%s\")!",outFIFO,path,
              utgid);
      mPrintf(errU,FATAL,__func__,0,"Hint: run \"/opt/FMC/sbin/logSrv -p %s "
              "-s %s\" on the node \"%s\" to open the FIFO and \"/opt/FMC/bin/"
              "logViewer -s /\\*/%s/log\" to get the messages!",outFIFO,p,
              nodeName,p);
      printOutFuncNOK;
      return 1;
    }
    mPrintf(errU,INFO,__func__,0,"FIFO: \"%s\" opened for stdout of process: "
            "\"%s\".",outFIFO,utgid);
  }
  if(errFIFO)                                         /* specific error FIFO */
  {
    errFD=loggerOpen(errFIFO,noDrop,errU,ERROR);
    if(errFD==-1)
    {
      char fifoBn[1+strlen(errFIFO)];
      char *p;
      strcpy(fifoBn,errFIFO);
      p=strrchr(fifoBn,'.');
      if(p)*p='\0';
      p=strrchr(fifoBn,'/');
      if(p)p++;
      else p=fifoBn;
      mPrintf(errU,FATAL,__func__,0,"Cannot open the FIFO: \"%s\" for the "
              "stderr of the process \"%s\" (UTGID=\"%s\")!",errFIFO,path,
              utgid);
      mPrintf(errU,FATAL,__func__,0,"Hint: run \"/opt/FMC/sbin/logSrv -p %s "
              "-s %s\" on the node \"%s\" to open the FIFO and \"/opt/FMC/bin/"
              "logViewer -s /\\*/%s/log\" to get the messages!",errFIFO,p,
              nodeName,p);
      printOutFuncNOK;
      return 1;
    }
    mPrintf(errU,INFO,__func__,0,"FIFO: \"%s\" opened for stderr of process: "
            "\"%s\".",errFIFO,utgid);
  }
  /* ----------------------------------------------------------------------- */
  switch(tgid=fork())
  {
    /* ..................................................................... */
    case -1:                                                        /* error */
      mPrintf(errU,ERROR,__func__,0,"fork(): %s!",strerror(errno));
      printOutFuncNOK;
      return 1;
    /* ..................................................................... */
    case 0:                                                 /* child process */
      if(asDaemon)
      {
        /* reset umask */
        umask(0);
        /* create a new session */
        if(setsid()<0)
        {
          mPrintf(errU,ERROR,__func__,0,"setsid(): %s!",strerror(errno));
          exit(1);
        }
        signal(SIGCHLD,SIG_IGN);
      }
      else
      {
        signal(SIGCHLD,SIG_DFL);
      }
      /* change working directory to wd */
      if(chdir(wd)==-1)
      {
        mPrintf(errU,ERROR,__func__,0,"chdir(%s): %s!",wd,strerror(errno));
        exit(1);
      }
      logFD=getDfltFifoFD();
      /* Close out all the file descriptors */
      for(i=getdtablesize();i>=0;--i)
        if((i!=logFD && i!=outFD && i!=errFD)||
           !(doRedirectStdout||doRedirectStderr))
          close(i);
      /* Open the standard file descriptors on /dev/null                     */
      /* or on the DIM logger FIFO                                           */
      dup2(open("/dev/null",O_RDONLY),STDIN_FILENO);
      if(doRedirectStdout)
      {
        if(outFD>-1)dup2(outFD,STDOUT_FILENO);       /* specific output FIFO */
        else if(logFD>-1)dup2(logFD,STDOUT_FILENO);   /* default output FIFO */
        else dup2(open("/dev/null",O_WRONLY),STDOUT_FILENO);  /* drop output */
      }
      else
      {
        dup2(open("/dev/null",O_WRONLY),STDOUT_FILENO);       /* drop output */
      }
      if(doRedirectStderr)
      {
        if(errFD>-1)dup2(errFD,STDERR_FILENO);        /* specific error FIFO */
        else if(logFD>-1)dup2(logFD,STDERR_FILENO);    /* default error FIFO */
        else dup2(open("/dev/null",O_WRONLY),STDERR_FILENO);   /* drop error */
      }
      else
      {
        dup2(open("/dev/null",O_WRONLY),STDERR_FILENO);        /* drop error */
      }
      /* clear child environment */
      if(doClearenv)clearenv();
      /* set child environment */
      for(eP=argz_next(envz,envz_len,NULL);eP;eP=argz_next(envz,envz_len,eP))
        putenv(eP);
      setenv("UTGID",utgid,1);
      setenv("PWD",getcwd(NULL,0),1);
      /* set process-to-cpu affinity */
      if(sched_setaffinity(getpid(),sizeof(cpu_set_t), cpuSet)==-1)
      {
        mPrintf(errU,WARN,__func__,0,"sched_setaffinity(%#0lx): %s!",affMask,
                strerror(errno));
      }
      /* set scheduler and rtPriority */
      if(scheduler<SCHED_OTHER || scheduler>SCHED_RR)
      {
        mPrintf(errU,WARN,__func__,0,"Unknown scheduler %d! Only values "
                "SCHED_OTHER=0, SCHED_FIFO=1 and SCHED_RR=2 are allowed. "
                "Setting scheduler to SCHED_OTHER...",scheduler);
        scheduler=SCHED_OTHER;
      }
      if(scheduler==SCHED_OTHER && rtPriority!=0)
      {
        mPrintf(errU,WARN,__func__,0,"%d real-time priority not allowed for "
                "SCHED_OTHER time sharing scheduler! Only 0 real-time "
                "priority allowed for SCHED_OTHER time sharing scheduler. "
                "Setting real-time priority to 0...",rtPriority);
        rtPriority=0;
      }
      if((scheduler==SCHED_FIFO||scheduler==SCHED_RR)&&
         (rtPriority<1||rtPriority>99))
      {
        mPrintf(errU,WARN,__func__,0,"%d real-time priority not allowed for %s "
                "real-time scheduler! Only 1..99 real-time priority allowed "
                "for %s real-rime scheduler. Setting real-time priority to "
                "1...",rtPriority,ss[scheduler],ss[scheduler]);
        rtPriority=1;
      }
      sp.sched_priority=rtPriority;
      if(sched_setscheduler(0,scheduler,&sp)<0)
      {
        mPrintf(errU,WARN,__func__,0,"sched_setscheduler(0,%s,%d): %s!",
                ss[scheduler],rtPriority,strerror(errno));
      }
      /* set niceLevel */
      if(niceLevel<-20 || niceLevel>19)
      {
        mPrintf(errU,WARN,__func__,0,"%d nice level not allowed. Only values "
                "-20..19 allowed for nice level. Setting nice level to 0...",
                niceLevel);
        niceLevel=0;
      }
      if(setpriority(PRIO_PROCESS,getpid(),niceLevel)<0)
      {
        mPrintf(errU,WARN,__func__,0,"setpriority(%d): %s",niceLevel,
                strerror(errno));
      }
      /* set gid */
      if(setgid(gid)==-1)
      {
        mPrintf(errU,WARN,__func__,0,"setgid(%d): %s",gid,strerror(errno));
      }
      /* set uid */
      if(setuid(uid)==-1)
      {
        mPrintf(errU,WARN,__func__,0,"setuid(%d): %s",uid,strerror(errno));
      }
      /* unmask signals */
      sigemptyset(&psSignalMask);
      if(pthread_sigmask(SIG_SETMASK,&psSignalMask,NULL))
        eExit("pthread_sigmask()");
      /* replaces child image */
      if(execv(path,argv)==-1)                 /* if success does not return */
      {
        mPrintf(errU,ERROR,__func__,0,"execv(%s) (pwd=\"%s\"): %s!",path,wd,
                strerror(errno));
        exit(1);
      }
      break;                                            /* end child process */
    /* ..................................................................... */
    default:                                               /* parent process */
      /* close specific FIFO file descriptors opened for the child */
      if(outFD!=-1)close(outFD);
      if(errFD!=-1)close(errFD);
      /* update process list */
      cmd=strrchr(path,'/');
      if(cmd)cmd++;
      else cmd=path;
      oldPs=pLadd(&psList,tgid,strdup(cmd),strdup(utgid),0);
      if(oldPs.tgid!=-1)                             /* old element replaced */
      {
        if(oldPs.cmd)free(oldPs.cmd);
        if(oldPs.utgid)free(oldPs.utgid);
      }
      if(deBug&0x40)
      {
        mPrintf(errU,DEBUG,__func__,0,"List length: %d",pLlen(&psList));
        pLprint(&psList,VERB);
      }
      /* wait 4 s for the child to appear in the process list */
      for(i=0;i<40;i++)
      {
        pss=findTgid(tgid);
        if(pss.psN==0)     /* still-born process or fast terminating process */
        {
          int j;
          procListEl_t *sbEl=NULL;
          for(j=0;j<10;j++)               /* wait for the psList to be ready */
          {
            /* try to get the exit status from psList */
            sbEl=pLfind(&psList,tgid);
            if(sbEl && !sbEl->running)
            {
              if(sbEl->termStatus)                    /* exclude exit code 0 */
              {
                mPrintf(errU,FATAL,__func__,0,"Process: TGID=%d, CMD=\"%s\", "
                        "UTGID=\"%s\" still-born!",tgid,cmd,utgid);
              }
              break;
            }
            delay.tv_sec=0;
            delay.tv_nsec=100000000;                               /* 1/10 s */
            nanosleep(&delay,NULL);      /* POSIX.4 call: sleep w/out signal */
          }                               /* wait for the psList to be ready */
          break;
        }                  /* still-born process or fast terminating process */
        if(pss.ps[0].utgid && !strncmp(pss.ps[0].utgid,utgid,UTGID_LEN+1))
        {
          /* set oom killer adjustment */
          setOomScoreAdj(oomScoreAdj,utgid,tgid);
          mPrintf(errU,INFO,__func__,0,"Process: TGID=%d, CMD=\"%s\", "
                  "UTGID=\"%s\" started.",tgid,cmd,utgid);
          freePssinfo(&pss);
          break;
        }
        /* utgid still not set */
        freePssinfo(&pss);
        delay.tv_sec=0;
        delay.tv_nsec=100000000;                                   /* 1/10 s */
        nanosleep(&delay,NULL);          /* POSIX.4 call: sleep w/out signal */
      }
      if(deBug&0x02 && i<40)
      {
        mPrintf(errU,VERB,__func__,0,"UTGID found after %d retries.",i);
      }
      if(i==40)
      {
        mPrintf(errU,ERROR,__func__,0,"Process: UTGID=\"%s\", TGID=%d not "
                "found within 4 seconds!",utgid,tgid);
      }
      break;
    /* ..................................................................... */
  }
  /* ----------------------------------------------------------------------- */
  printOutFuncOK;
  return 0;
}
/* ************************************************************************* */
/* killPs() - Sends the signal sig to the processes whose UTGID pattern      */
/*            matches the POSIX.2 wildcard pattern utgidPattern              */
/* ************************************************************************* */
int killPs(char *utgidPattern,int sig)
{
  int i;
  int found=0;
  char msg[MSG_LEN+1]="";
  char *msgP=NULL;
  size_t msgRL;
  pssinfo pss={0,NULL};
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  /* find process list */
  pss=findUtgid(utgidPattern);
  if(pss.psN==1)
  {
    mPrintf(errU,DEBUG,__func__,0,"Sending signal %u (%s) to process: "
            "UTGID=\"%s\"...",sig,sig2msg(sig),pss.ps[0].utgid);
  }
  else if(pss.psN>1)
  {
    msgP=msg;
    msgRL=MSG_LEN+1-9-(msgP-msg);                          /* 9: "... )...\0 */
    snprintf(msgP,msgRL,"Sending signal %u (%s) to processes: UTGID=(",
             sig,sig2msg(sig));
    msgP=strchr(msgP,'\0');
    msgRL=MSG_LEN+1-9-(msgP-msg);
    for(i=0,found=0;i<pss.psN;i++)
    {
      snprintf(msgP,msgRL,"\"%s\"",pss.ps[i].utgid);
      msgP=strchr(msgP,'\0');
      msgRL=MSG_LEN+1-9-(msgP-msg);
      if(i<pss.psN-1)
      {
        snprintf(msgP,msgRL,", ");
        msgP=strchr(msgP,'\0');
        msgRL=MSG_LEN+1-9-(msgP-msg);
      }
    }
    if(msgRL<2)
    {
      sprintf(msgP,"... ");
      msgP=strchr(msgP,'\0');
    }
    sprintf(msgP,")...");
    mPrintf(errU,DEBUG,__func__,0,"%s",msg);
  }
  /* ----------------------------------------------------------------------- */
  /* send signals */
  for(i=0,found=0;i<pss.psN;i++)
  {
    if(kill(pss.ps[i].tgid,sig)==-1)
    {
      mPrintf(errU,ERROR,__func__,0,"kill(signal=%d, UTGID=\"%s\", tgid=%d): "
              "%s!",sig,pss.ps[i].utgid,pss.ps[i].tgid,strerror(errno));
    }
    else found++;
  }
  /* ----------------------------------------------------------------------- */
  if(found==0)
  {
    mPrintf(errU,WARN,__func__,0,"No process found whose UTGID matches "
            "pattern: \"%s\"!",utgidPattern);
    printOutFuncOK;
    return WARN;
  }
  else if(found==1)
  {
    mPrintf(errU,DEBUG,__func__,0,"Signal %u (%s) sent to 1 process.",sig,
            sig2msg(sig));
  }
  else
  {
    mPrintf(errU,DEBUG,__func__,0,"Signal %u (%s) sent to %u processes.",
            sig,sig2msg(sig),found);
  }
  /* ----------------------------------------------------------------------- */
  freePssinfo(&pss);
  /* ----------------------------------------------------------------------- */
  printOutFuncOK;
  return OK;
}
/* ************************************************************************* */
/* findUtgid() - Finds the list of processes whose UTGID pattern matches the */
/*               POSIX.2 wildcard pattern utgidPattern.                      */
/* RETURN VALUE                                                              */
/*   malloc-ated pssinfo structure (to be freed using freePssinfo()).        */
/* INTERNALS                                                                 */
/* - Try first to use /proc/<TGID>/environ. If the pseudo-file is completely */
/*   filled (procPidEnvSz B) then uses the ptrace(2) system call.            */
/* - In the latter case the target process is stopped for a small while by a */
/*   SIGSTOP and then is continued by a SIGCONT.                             */
/* Incompatible with other processes using ptrace (gdb, strace, ltrace, ...) */
/* - To always use /proc/<TGID>/environ set procPidEnvSz in tmSrv.h to a     */
/*   large value.                                                            */
/* - To always use ptrace(2) set procPidEnvSz in tmSrv.h to -1               */
/* ************************************************************************* */
pssinfo findUtgid(char *utgidPattern)
{
  /* ----------------------------------------------------------------------- */
  const char *procPath="/proc";
  char psPath[NAME_MAX+16]="";
  char cmdPath[NAME_MAX+16]="";
  char envPath[NAME_MAX+16]="";
  char statPath[NAME_MAX+16]="";
  char statusPath[NAME_MAX+16]="";
  char oomScorePath[NAME_MAX+16]="";
  union
  {
    struct dirent d;
    char b[offsetof(struct dirent,d_name)+NAME_MAX+1];
  }u;
  struct dirent *procEntry=&u.d;
  struct dirent *result;
  DIR *dir=NULL;
  char procState=0;
  int status;
  pid_t tgid;
  char **cmdLine;
  char *cmd;
  char baseCmd[CMD_LEN+1];
  char **procEnv;
  int oomScore;
  char sbuf[1024];                             /* buffer for stat and status */
  char readUtgid[UTGID_LEN+1];
  size_t envSz;
  pssinfo pss={0,NULL};
  int nP;
  int wch;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  pthread_mutex_lock(&openprocLock);
  /* ----------------------------------------------------------------------- */
  dir=opendir(procPath);
  if(!dir)
  {
    mPrintf(errU,ERROR,__func__,0,"Unable to open directory \"%s\": %s!",
            procPath,strerror(errno));
    pthread_mutex_unlock(&openprocLock);
    printOutFuncNOK;
    return pss;
  }
  /* ----------------------------------------------------------------------- */
  for(nP=0;nP<=maxPid;nP++)     /* loop over procs; hopefully exits at break */
  {
    char *p;
    char **pp;
    /* ..................................................................... */
    errno=0;
    status=readdir_r(dir,procEntry,&result);          /* get directory entry */
    if(unlikely(status))
    {
      mPrintf(errU,ERROR,__func__,0,"Unable to read directory \"%s\": error "
              "%d (%s)!\n",procPath,status,strerror(status));
      break;
    }
    if(unlikely(!result))                                /* End of directory */
    {
      break;                                              /* hoped loop exit */
    }
    if(unlikely(unlikely(!procEntry)||unlikely(!procEntry->d_name)))
    {
      if(!procEntry)mPrintf(errU,ERROR,__func__,0,"procEntry=NULL");
      else if(!procEntry->d_name)
        mPrintf(errU,ERROR,__func__,0,"procEntry->d_name=NULL");
      break;
    }
    if(unlikely(unlikely(*procEntry->d_name<='0')||
                unlikely(*procEntry->d_name>'9')))      /* not process entry */
    {
      continue;
    }
    /* ..................................................................... */
    /* get tgid */
    tgid=strtoul(procEntry->d_name,NULL,10);
    /* ..................................................................... */
    sprintf(psPath,"/proc/%s",procEntry->d_name);
    /* ..................................................................... */
    /* get procState */
    procState='U';
    sprintf(statPath,"%s/%s",psPath,"stat");
    sprintf(statusPath,"%s/%s",psPath,"status");
    /* try to get status from /proc/<pid>/stat */
    if(likely(file2str(statPath,sbuf,sizeof(sbuf))!=-1))
    {
      procState=getStateFromStat(sbuf);
    }
    if(unlikely(procState=='U'))  /* procState not found in /proc/<pid>/stat */
    {
      /* try to get status from /proc/<pid>/status */
      if(likely(file2str(statusPath,sbuf,sizeof(sbuf))!=-1))
      {
        procState=getStateFromStatus(sbuf);
      }
    }
    /* ..................................................................... */
    /* get OOM score */
    oomScore=INT_MIN;
    sprintf(oomScorePath,"%s/%s",psPath,"oom_score");
    if(likely(file2str(oomScorePath,sbuf,sizeof(sbuf))!=-1))
    {
      oomScore=getOomScore(sbuf);
    }
    /* ..................................................................... */
    /* get CMD, either from /proc/<pid>/cmdline or from /proc/<pid>/status */
    cmdLine=NULL;
    cmd=NULL;
    sprintf(cmdPath,"%s/%s",psPath,"cmdline");
    cmdLine=file2strvec(cmdPath);
    if(unlikely(!cmdLine))                        /* kernel thread or zombie */
    {
      if(likely(file2str(statusPath,sbuf,sizeof(sbuf))!=-1))
      {
        snprintf(baseCmd,CMD_LEN+1,"%s",getCmdFromStatus(sbuf));
      }
      if(unlikely(deBug&0x80))
      {
        if(procState=='Z')
        {
          mPrintf(errU,VERB,__func__,0,"ps: %d, TGID: %d, CMD: \"%s\": zombie "
                  "process.",nP,tgid,baseCmd);
        }
        else
        {
          mPrintf(errU,VERB,__func__,0,"ps: %d, TGID: %d, CMD: \"%s\": "
                  "probably a kernel thread.",nP,tgid,baseCmd);
        }
      }
    }                                             /* kernel thread or zombie */
    else                                 /* neither kernel thread nor zombie */
    {
      cmd=*cmdLine;
      p=strrchr(cmd,'/');
      if(p)
      {
        wch=snprintf(baseCmd,CMD_LEN+1,"%s",p+1);
        if(unlikely(wch>CMD_LEN))
        {
          mPrintf(errU,ERROR,__func__,0,"CMD truncated (\"%s\" -> \"%s\"). "
                  "Increase CMD_LEN variable in tmSrv.h!",p+1,baseCmd);
        }
      }
      else
      {
        wch=snprintf(baseCmd,CMD_LEN+1,"%s",cmd);
        if(unlikely(wch>CMD_LEN))
        {
          mPrintf(errU,ERROR,__func__,0,"CMD truncated (\"%s\" -> \"%s\"). "
                  "Increase CMD_LEN variable in tmSrv.h!",cmd,baseCmd);
        }
      }
    }                                    /* neither kernel thread nor zombie */
    /* ..................................................................... */
    /* get UTGID and /proc/<pid>/environ size */
    sprintf(envPath,"%s/%s",psPath,"environ");
    procEnv=file2strvec(envPath);
    *readUtgid='\0';
    envSz=0;
    if(likely(procEnv))
    {
      int utgidFound=0;
      for(pp=procEnv;*pp;pp++)
      {
        envSz+=1+strlen(*pp);
        if(likely(!utgidFound))
        {
          if(unlikely(!strncmp(*pp,"UTGID=",6)))
          {
            wch=snprintf(readUtgid,UTGID_LEN+1,"%s",1+strchr(*pp,'='));
            if(unlikely(wch>UTGID_LEN))
            {
              mPrintf(errU,ERROR,__func__,0,"UTGID truncated (\"%s\" -> "
                      "\"%s\"). Increase UTGID_LEN variable in fmcPar.h!",
                      1+strchr(*pp,'='),readUtgid);
            }
            utgidFound=1;
          }
        }
      }
      envSz--;
    }
    /* ..................................................................... */
    /* Here are available:                                                   */
    /* pid_t tgid                                                            */
    /* char procState                                                        */
    /* char baseCmd[CMD_LEN+1]                                               */
    /* size_t envSz                                                          */
    /* char readUtgid[UTGID_LEN+1]                                           */
    /* int oomScore                                                          */
    /* ..................................................................... */
    if(unlikely(deBug&0x80))
    {
      mPrintf(errU,VERB,__func__,0,"ps: %d, tgid: %d.",nP,tgid);
    }
    /* ..................................................................... */
    /* huge environment: use ptrace(2) to get UTGID */
    if(unlikely(envSz>=procPidEnvSz))
    {
      if(unlikely(deBug&(0x02|0x20)))
      {
        mPrintf(errU,INFO,__func__,0,"Process TGID=%d, CMD=\"%s\" has a huge "
                "environment (>%d B). Using ptrace(2) to retrieve its UTGID"
                "...",tgid,baseCmd,procPidEnvSz);
      }
      *readUtgid='\0';                           /* UTGID could be truncated */
      if(unlikely(tgid==main_tgid)) /* current process (don't use ptrace(2)) */
      {
        char *cp;
        cp=getenv("UTGID");
        if(likely(cp))                     /* UTGID of current process found */
        {
          wch=snprintf(readUtgid,UTGID_LEN+1,"%s",cp);
          if(unlikely(wch>UTGID_LEN))
          {
            mPrintf(errU,ERROR,__func__,0,"UTGID truncated (\"%s\" -> \"%s\"). "
                    "Increase UTGID_LEN variable in tmSrv.h!",cp,readUtgid);
          }
        }
      }                             /* current process (don't use ptrace(2)) */
      else                            /* not current process (use ptrace(2)) */
      {
        char *p;
        /* send CONT signal to stopped processes to avoid ptrace lock */
        if(unlikely(procState=='T'))
        {
          mPrintf(errU,WARN,__func__,0,"Sending signal %u (%s) to stopped "
                  "process %d.",SIGCONT,sig2msg(SIGCONT),tgid);
          kill(tgid,SIGCONT);
        }
        p=getUtgidFromTgid(tgid,10);
        wch=snprintf(readUtgid,UTGID_LEN+1,"%s",p);
        if(unlikely(wch>UTGID_LEN))
        {
          mPrintf(errU,ERROR,__func__,0,"UTGID truncated (\"%s\" -> \"%s\"). "
                  "Increase UTGID_LEN variable in tmSrv.h!",p,readUtgid);
        }
      }                               /* not current process (use ptrace(2)) */
    }                        /* huge environment: use ptrace(2) to get UTGID */
    /* ..................................................................... */
    if(unlikely(deBug&0x80))
    {
      if(*readUtgid)
      {
        mPrintf(errU,VERB,__func__,0,"ps: %d, TGID: %d, CMD: \"%s\", UTGID: "
                "\"%s\".",nP,tgid,baseCmd,readUtgid);
      }
      else
      {
        mPrintf(errU,VERB,__func__,0,"ps: %d, TGID: %d, CMD: \"%s\", UTGID: "
                "\"%s\".",nP,tgid,baseCmd,"N/A");
      }
    }
    /* ..................................................................... */
    if(*readUtgid)
    {
      if(!fnmatch(utgidPattern,readUtgid,0))
      {
        pss.psN++;
        pss.ps=(psinfo*)realloc(pss.ps,pss.psN*sizeof(psinfo));
        pss.ps[pss.psN-1].tgid=tgid;
        pss.ps[pss.psN-1].cmd=strdup(baseCmd);
        pss.ps[pss.psN-1].utgid=strdup(readUtgid);
        pss.ps[pss.psN-1].oomScore=oomScore;
      }
    }
    /* ..................................................................... */
    /* free memory allocated by file2strvec() */
    if(likely(cmdLine))free((void*)*cmdLine);
    if(likely(procEnv))free((void*)*procEnv);
    /* ..................................................................... */
  }                                                   /* loop over processes */
  closedir(dir);
  /* ----------------------------------------------------------------------- */
  pthread_mutex_unlock(&openprocLock);
  /* ----------------------------------------------------------------------- */
  if(nP>=maxPid)
  {
    mPrintf(errU|L_SYS,ERROR,__func__,9,"Maximum number of processes (%d) "
            "reached! The current running Linux KERNEL is apparently BROKEN "
            "in the procfs readdir() wrapper: proc_pid_readdir() in source "
            "file \"linux-2.6.9/fs/proc/base.c\"! To fix the problem install "
            "a Linux SLC Kernel >= 2.6.9-55 or apply the Kernel patch "
            "\"linux-2.6.9-proc-readdir.patch\"!",maxPid);
  }
  if(deBug&0x02)
  {
    mPrintf(errU,DEBUG,__func__,9,"%d processes scanned. %d processes found "
            "whose UTGID matches pattern: \"%s\". Returning...",nP,pss.psN,
            utgidPattern);
  }
  /* ----------------------------------------------------------------------- */
  printOutFuncOK;
  return pss;
}
/* ************************************************************************* */
/* findTgid() - Finds the process if exists with TGID tgid                   */
/* To get the UTGID of a found process, try first to use                     */
/* /proc/<TGID>/environ. If the pseudo-file is completely filled             */
/* (procPidEnvSz B) then uses the ptrace(2) system call.                     */
/* In the latter case the target process is stopped for a small while by a   */
/* SIGSTOP and then is continued by a SIGCONT.                               */
/* Incompatible with other processes using ptrace (gdb, strace, ltrace, ...) */
/* ************************************************************************* */
pssinfo findTgid(pid_t tgid)
{
  /* ----------------------------------------------------------------------- */
  const char *procPath="/proc";
  char psPath[NAME_MAX+16]="";
  char cmdPath[NAME_MAX+16]="";
  char envPath[NAME_MAX+16]="";
  char statPath[NAME_MAX+16]="";
  char statusPath[NAME_MAX+16]="";
  char oomScorePath[NAME_MAX+16]="";
  char procState=0;
  static char sbuf[1024];	               /* buffer for stat and status */
  char **cmdLine=NULL;
  char *cmd;
  char baseCmd[CMD_LEN+1];
  char **procEnv=NULL;
  int oomScore;
  size_t envSz;
  char **pp;
  char *p;
  char readUtgid[UTGID_LEN+1];
  pssinfo pss={0,NULL};
  int wch;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  pthread_mutex_lock(&openprocLock);
  /* ----------------------------------------------------------------------- */
  /* compose paths */
  sprintf(psPath,"%s/%u",procPath,tgid);
  sprintf(cmdPath,"%s/%s",psPath,"cmdline");
  sprintf(envPath,"%s/%s",psPath,"environ");
  sprintf(statPath,"%s/%s",psPath,"stat");
  sprintf(statusPath,"%s/%s",psPath,"status");
  /* ----------------------------------------------------------------------- */
  /* check whether process tgid exists */
  if(unlikely(unlikely(access(statPath,F_OK)==-1) &&
              unlikely(access(statusPath,F_OK)==-1)))
  {
    if(unlikely(deBug&0x02))
    {
      mPrintf(errU,DEBUG,__func__,0,"Process TGID=%d does not exist!",tgid);
    }
    pthread_mutex_unlock(&openprocLock);
    printOutFuncOK;
    return pss;
  }
  /* ----------------------------------------------------------------------- */
  /* allocate pss memory */
  pss.psN++;
  pss.ps=(psinfo*)realloc(pss.ps,pss.psN*sizeof(psinfo));
  pss.ps[pss.psN-1].tgid=tgid;
  pss.ps[pss.psN-1].cmd=NULL;
  pss.ps[pss.psN-1].utgid=NULL;
  /* ----------------------------------------------------------------------- */
  /* get procState */
  procState='U';
  /* try to get status from /proc/<pid>/stat */
  if(likely(file2str(statPath,sbuf,sizeof(sbuf))!=-1))
  {
    procState=getStateFromStat(sbuf);
  }
  if(unlikely(procState=='U'))    /* procState not found in /proc/<pid>/stat */
  {
    /* try to get status from /proc/<pid>/status */
    if(likely(file2str(statusPath,sbuf,sizeof(sbuf))!=-1))
    {
      procState=getStateFromStatus(sbuf);
    }
  }
  /* ----------------------------------------------------------------------- */
  /* get OOM score */
  oomScore=INT_MIN;
  sprintf(oomScorePath,"%s/%s",psPath,"oom_score");
  if(likely(file2str(oomScorePath,sbuf,sizeof(sbuf))!=-1))
  {
    oomScore=getOomScore(sbuf);
  }
  pss.ps[pss.psN-1].oomScore=oomScore;
  /* ----------------------------------------------------------------------- */
  /* get CMD */
  cmdLine=NULL;
  cmdLine=file2strvec(cmdPath);
  if(unlikely(!cmdLine))                                    /* kernel thread */
  {
    if(unlikely(deBug&0x80))
    {
      mPrintf(errU,VERB,__func__,0,"ps tgid: %d. No CMD. Probably a "
              "kernel thread.",tgid);
    }
    pthread_mutex_unlock(&openprocLock);
    printOutFuncOK;
    return pss;
  }                                                         /* kernel thread */
  cmd=*cmdLine;
  p=strrchr(cmd,'/');
  if(p)
  {
    wch=snprintf(baseCmd,CMD_LEN+1,"%s",p+1);
    if(unlikely(wch>CMD_LEN))
    {
      mPrintf(errU,ERROR,__func__,0,"CMD truncated (\"%s\" -> \"%s\"). "
              "Increase CMD_LEN variable in tmSrv.h!",p+1,baseCmd);
    }
  }
  else
  {
    wch=snprintf(baseCmd,CMD_LEN+1,"%s",cmd);
    if(unlikely(wch>CMD_LEN))
    {
      mPrintf(errU,ERROR,__func__,0,"CMD truncated (\"%s\" -> \"%s\"). "
              "Increase CMD_LEN variable in tmSrv.h!",cmd,baseCmd);
    }
  }
  pss.ps[pss.psN-1].cmd=strdup(baseCmd);
  /* ----------------------------------------------------------------------- */
  /* get UTGID and /proc/<pid>/environ size */
  procEnv=file2strvec(envPath);
  *readUtgid='\0';
  envSz=0;
  if(likely(procEnv))
  {
    int utgidFound=0;
    for(pp=procEnv;*pp;pp++)
    {
      envSz+=1+strlen(*pp);
      if(likely(!utgidFound))
      {
        if(unlikely(!strncmp(*pp,"UTGID=",6)))
        {
          wch=snprintf(readUtgid,UTGID_LEN+1,"%s",1+strchr(*pp,'='));
          if(unlikely(wch>UTGID_LEN))
          {
            mPrintf(errU,ERROR,__func__,0,"UTGID truncated (\"%s\" -> "
                    "\"%s\"). Increase UTGID_LEN variable in tmSrv.h!",
                    1+strchr(*pp,'='),readUtgid);
          }
          utgidFound=1;
        }
      }
    }
    envSz--;
  }
  /* ----------------------------------------------------------------------- */
  /* Here are available:                                                     */
  /* pid_t tgid                                                              */
  /* char procState                                                          */
  /* char baseCmd[CMD_LEN+1]                                                 */
  /* size_t envSz                                                            */
  /* char readUtgid[UTGID_LEN+1]                                             */
  /* int oomScore                                                            */
  /* ----------------------------------------------------------------------- */
  /* huge environment: use ptrace(2) to get UTGID */
  if(unlikely(envSz>=procPidEnvSz))
  {
    if(unlikely(deBug&(0x02|0x20)))
    {
      mPrintf(errU,INFO,__func__,0,"Process TGID=%d, CMD=\"%s\" has a huge "
              "environment (>%d B). Using ptrace(2) to retrieve its UTGID"
              "...",tgid,baseCmd,procPidEnvSz);
    }
    *readUtgid='\0';                             /* UTGID could be truncated */
    if(unlikely(tgid==main_tgid))   /* current process (don't use ptrace(2)) */
    {
      char *cp;
      cp=getenv("UTGID");
      if(likely(cp))                       /* UTGID of current process found */
      {
        wch=snprintf(readUtgid,UTGID_LEN+1,"%s",cp);
        if(unlikely(wch>UTGID_LEN))
        {
          mPrintf(errU,ERROR,__func__,0,"UTGID truncated (\"%s\" -> \"%s\"). "
                  "Increase UTGID_LEN variable in tmSrv.h!",cp,readUtgid);
        }
      }
    }                               /* current process (don't use ptrace(2)) */
    else                              /* not current process (use ptrace(2)) */
    {
      /* send CONT signal to stopped processes to avoid ptrace lock */
      if(unlikely(procState=='T'))
      {
        mPrintf(errU,WARN,__func__,0,"Sending signal %u (%s) to stopped "
                "process %d.",SIGCONT,sig2msg(SIGCONT),tgid);
        kill(tgid,SIGCONT);
      }
      p=getUtgidFromTgid(tgid,10);
      wch=snprintf(readUtgid,UTGID_LEN+1,"%s",p);
      if(unlikely(wch>UTGID_LEN))
      {
        mPrintf(errU,ERROR,__func__,0,"UTGID truncated (\"%s\" -> \"%s\"). "
                "Increase UTGID_LEN variable in tmSrv.h!",p,readUtgid);
      }
    }                                 /* not current process (use ptrace(2)) */
  }                          /* huge environment: use ptrace(2) to get UTGID */
  /* ----------------------------------------------------------------------- */
  if(*readUtgid)
  {
    pss.ps[pss.psN-1].utgid=strdup(readUtgid);
  }
  /* ----------------------------------------------------------------------- */
  /* free memory allocated by file2strvec() */
  if(likely(cmdLine))free((void*)*cmdLine);
  if(likely(procEnv))free((void*)*procEnv);
  /* ----------------------------------------------------------------------- */
  if(deBug&0x02)
  {
    mPrintf(errU,DEBUG,__func__,0,"%d processes found with TGID: %d.",pss.psN,
            tgid);
  }
  /* ----------------------------------------------------------------------- */
  pthread_mutex_unlock(&openprocLock);
  printOutFuncOK;
  return pss;
}
/* ************************************************************************* */
/* freePssinfo - Frees the memory m-allocated (through strdup())             */
/*               by findUtgid()                                              */
/* ************************************************************************* */
void freePssinfo(pssinfo *pss)
{
  int i;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  if(pss->psN==0 && pss->ps==NULL)
  {
    printOutFuncOK;
    return;
  }
  for(i=0;i<pss->psN;i++)
  {
    if(pss->ps[i].cmd)free(pss->ps[i].cmd);
    if(pss->ps[i].utgid)free(pss->ps[i].utgid);
  }
  if(pss->ps)free(pss->ps);
  pss->psN=0;
  pss->ps=NULL;
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
/* genUtgid() - Generates an UTGID of the form <cmd>_<instanceNumber>.       */
/* ************************************************************************* */
char *genUtgid(char *path)
{
  static char utgid[UTGID_LEN+1];
  int i;
  pssinfo pss={0,NULL};
  int j;
  static char findPattern[257];
  /* ----------------------------------------------------------------------- */
  printInFunc;
  snprintf(findPattern,257,"%s*",basename(path));
  pss=findUtgid(findPattern);
  for(i=0;;i++)
  {
    snprintf(utgid,UTGID_LEN+1,"%s_%d",basename(path),i);
    for(j=0;j<pss.psN;j++)
    {
      if(!strncmp(utgid,pss.ps[j].utgid,UTGID_LEN+1))break;
    }
    if(j==pss.psN)break;
  }
  freePssinfo(&pss);
  /* ----------------------------------------------------------------------- */
  printOutFuncOK;
  return utgid;
}
/* ************************************************************************* */
/* char *getUtgidFromTgid(pid_t tgid,int tryN)                               *
 *   Get the UTGID of the process <tgid>. If it fails to attach the process  *
 *   <tgid>, retry tryN-1 times.                                             *
 *   Uses ptrace(2)                                                          *
 * RETURN VALUE:                                                             *
 *   If UTGID is found:     a pointer to a static character array of length  *
 *                          UTGID_LEN+1 containing the UTGID                 *
 *   If UTGID is not found: a null string                                    *
 *   On error:              a null string                                    */
/* ************************************************************************* */
char *getUtgidFromTgid(pid_t tgid,int tryN)
{
  int i,j;
  char *p;
  int tryC;
  unsigned long addr=0,addr2=0;
  long tArgc;
  long tEnvP;
  long rv=0;
  int wch=0;
  char ch;
  char envLine[ENV_LINE_SZ];
  static char utgid[UTGID_LEN+1];
  int tWS;                            /* word size of traced process in byte */
  unsigned long tWMask;                       /* word mask of traced process */
#if defined __x86_64__
  struct user_regs_struct regs;
#endif
  int status;
  pid_t tgidR;
  char buf[1024];
  char statusPathName[128];
  char statPathName[128];
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  /* reset static variable utgid (bug fix of Revision 3.5) */
  *utgid='\0';
  /* ----------------------------------------------------------------------- */
  /* check tgid */
  if(tgid<1||tgid>maxPid)
  {
    mPrintf(errU,ERROR,__func__,0,"Invalid TGID=%d!",tgid);
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }
  else if(tgid==1)               /* ptrace(2) cannot attach the init process */
  {
    if(deBug&0x20)
    {
      mPrintf(errU,VERB,__func__,0,"Cannot access the process: \"/sbin/init\" "
              "(TGID=%d)!",tgid);
    }
    *utgid='\0';
    printOutFuncOK;
    return utgid;
  }
  else if(tgid==getpid())           /* current process (don't use ptrace(2)) */
  {
    p=getenv("UTGID");
    if(!p)                             /* UTGID of current process not found */
    {
      *utgid='\0';
      printOutFuncOK;
      return utgid;
    }
    else                                   /* UTGID of current process found */
    {
      wch=snprintf(utgid,UTGID_LEN+1,"%s",p);
      if(unlikely(wch>UTGID_LEN))
      {
        mPrintf(errU,ERROR,__func__,0,"UTGID truncated (\"%s\" -> \"%s\"). "
                "Increase UTGID_LEN variable in tmSrv.h!",p,utgid);
      }
      printOutFuncOK;
      return utgid;
    }
  }                                 /* current process (don't use ptrace(2)) */
  /* ----------------------------------------------------------------------- */
  /* set stat and status path */
  sprintf(statusPathName,"/proc/%d/status",tgid);
  sprintf(statPathName,"/proc/%d/stat",tgid);
  /* ----------------------------------------------------------------------- */
  /* check whether process exists */
  if(access(statusPathName,F_OK)==-1 && access(statPathName,F_OK)==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"Process TGID=%d does not exist!",tgid);
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }
  /* ----------------------------------------------------------------------- */
  /* Get the start stack address */
  if(file2str(statusPathName,buf,sizeof(buf))!=-1)
  {
    addr=getStaStkFromStatus(buf);
  }
  if(!addr && file2str(statPathName,buf,sizeof(buf))!=-1)
  {
    addr=getStaStkFromStat(buf);
  }
  if(!addr)
  {
    if(deBug&0x20)mPrintf(errU,VERB,__func__,0,"Start stack pointer not "
                          "available for process TGID=%d!",tgid);
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }
  /* ----------------------------------------------------------------------- */
  /* attach process */
  /* tryC trials of attaching (process may already be being traced) */
  for(tryC=0;tryC<tryN;tryC++)
  {
    struct timespec delay={0,1000000};                            /* 0.001 s */
    rv=ptrace(PTRACE_ATTACH,tgid,NULL,NULL);
    if(!rv || errno!=EPERM)break;
    mPrintf(errU,VERB,__func__,0,"The attempt %d/%d to attach the process "
            "TGID=%d (to get its UTGID using ptrace(2)) failed (is the "
            "process already being traced by another process?): "
            "ptrace(PTRACE_ATTACH): %s!",tryC+1,tryN,tgid,strerror(errno));
    if(tryC<tryN-1)
    {
      nanosleep(&delay,NULL);
      delay.tv_sec*=2;
      delay.tv_nsec*=2;
      if(delay.tv_nsec>999999999)
      {
        delay.tv_sec+=1;
        delay.tv_nsec-=1000000000;
      }
    }
  }
  if(rv)
  {
    if(errno==EPERM)
    {
      mPrintf(errU,ERROR,__func__,0,"The last attempt (%d/%d) to attach the "
              "process TGID=%d (to get its UTGID using ptrace(2)) failed (is "
              "the process already being traced by another process?): "
              "ptrace(PTRACE_ATTACH): %s!",tryC,tryN,tgid,strerror(errno));
    }
    else
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot attach process TGID=%d (to get "
              "its UTGID using ptrace(2)): ptrace(PTRACE_ATTACH): %s!",tgid,
              strerror(errno));
    }
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }
  else if(tryC)
  {
    mPrintf(errU,WARN,__func__,0,"The process TGID=%d was successfully "
            "attached after %d/%d failed attempts (was it being traced by "
            "another process?)!",tgid,tryC,tryN);
  }
  /* wait for the process <tgid> to be stopped */
  tgidR=waitpid(tgid,&status,0);
  if(tgidR!=tgid)
  {
    mPrintf(errU,ERROR,__func__,0,"waitpid() returned %d instead of TGID=%d!",
            tgidR,tgid);
    if(ptrace(PTRACE_DETACH,tgid,NULL,NULL))
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot detach process TGID=%d: "
              "ptrace(PTRACE_DETACH): %s!",tgid,strerror(errno));
    }
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }
  /* Here we are sure that the process has been stopped or terminated */
  if(!WIFSTOPPED(status))
  {
    if(deBug&0x20)
    {
      mPrintf(errU,WARN,__func__,0,"waitpid(): The process TGID=%d is not in "
              "a stopped state after the ptrace(PTRACE_ATTACH) call!",tgid);
    }
    /* set process as terminated in psList and send a message */
    pLsetTerm(&psList,tgid,status);
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }                                               /* if(!WIFSTOPPED(status)) */
  else if(WSTOPSIG(status)==SIGTRAP)
  {
    mPrintf(errU,WARN,__func__,0,"waitpid(): The process TGID=%d was "
            "stopped on signal %d (%s), instead of signal %u (%s). "
            "Was is exec-ing another code? Detaching it without have read "
            "the UTGID!",tgid,WSTOPSIG(status),sig2msg(WSTOPSIG(status)),
            SIGSTOP,sig2msg(SIGSTOP));
    if(ptrace(PTRACE_DETACH,tgid,NULL,NULL))
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot detach process TGID=%d: "
              "ptrace(PTRACE_DETACH): %s!",tgid,strerror(errno));
    }
    kill(tgid,SIGCONT);
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }                                         /* if(WSTOPSIG(status)==SIGTRAP) */
  else if(deBug&0x20)
  {
    mPrintf(errU,VERB,__func__,0,"waitpid(): The process TGID=%d was "
            "succesfully stopped (to get its UTGID using ptrace(2)) on "
            "signal %d (%s).",tgid,WSTOPSIG(status),
            sig2msg(WSTOPSIG(status)));
  }
  /* Here we are sure that the process has been stopped by a SIGSTOP */
  /* ----------------------------------------------------------------------- */
  /* Set tWS (word size in byte) and tWMask (word mask)                      *
   *   tWS = 4 for i386/i686 (32 bit)                                        *
   *   tWS = 4 for x86_64 in compatibility mode (32 bit)                     *
   *   tWS = 8 for x86_64 in long mode (64 bit)                              *
   * The mode is obtained by checking the CS register value.                 *
   * On x86-64 linux CS is:                                                  *
   *      0x33    for long mode (64 bit)                                     *
   *      0x23    for compatibility mode (32 bit)                            */
#if defined __x86_64__
  if(ptrace(PTRACE_GETREGS,tgid,NULL,&regs))
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot get the content of the registers "
            "for process TGID=%d, to get the x86_64 mode (long/64 or "
            "compatibility/32): ptrace(PTRACE_GETREGS): %s!",tgid,
            strerror(errno));
    if(ptrace(PTRACE_DETACH,tgid,NULL,NULL))
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot detach process TGID=%d: "
              "ptrace(PTRACE_DETACH): %s!",tgid,strerror(errno));
    }
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }
  if(regs.cs==0x33)
  {
    tWS=8;
    tWMask=0xffffffffffffffffUL;
  }
  else if(regs.cs==0x23)
  {
    tWS=4;
    tWMask=0x00000000ffffffffUL;
  }
  else
  {
    mPrintf(errU,ERROR,__func__,0,"Unrecognized value of the CS register "
            "(CS=%lx) for process TGID=%d!",regs.cs,tgid);
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }
#elif defined __i386__
  tWS=4;
  tWMask=0xffffffffUL;
#endif
  if(deBug&0x20)
  {
    //mPrintf(errU,VERB,__func__,0,"TGID=%d: word size=%d B",tgid,tWS);
  }
  /* ----------------------------------------------------------------------- */
  /* Get the start stack address again (now it will not change since the     */
  /* process is stopped)                                                     */
  if(file2str(statusPathName,buf,sizeof(buf))!=-1)
  {
    addr=getStaStkFromStatus(buf);
  }
  if(!addr && file2str(statPathName,buf,sizeof(buf))!=-1)
  {
    addr=getStaStkFromStat(buf);
  }
  if(!addr)
  {
    if(deBug&0x20)mPrintf(errU,VERB,__func__,0,"Start stack pointer not "
                          "available for process TGID=%d!",tgid);
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }
  /* ----------------------------------------------------------------------- */
  /* get the command-line arguments number */
  tArgc=ptrace(PTRACE_PEEKDATA,tgid,(void*)addr,NULL);
  if(tArgc==-1 && errno)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot peek data of process TGID=%d at "
            "address %#lx: ptrace(PTRACE_PEEKDATA): %s!",tgid,addr,
            strerror(errno));
    if(ptrace(PTRACE_DETACH,tgid,NULL,NULL))
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot detach process TGID=%d: "
              "ptrace(PTRACE_DETACH): %s!",tgid,strerror(errno));
    }
    *utgid='\0';
    printOutFuncNOK;
    return utgid;
  }
  tArgc&=tWMask;
  /* ----------------------------------------------------------------------- */
  /* skip command-line arguments */
  for(i=0;i<tArgc+1;i++)addr+=tWS;
  /* ----------------------------------------------------------------------- */
  for(i=0;;i++)                             /* loop over environment strings */
  {
    int k;
    addr+=tWS;
    tEnvP=ptrace(PTRACE_PEEKDATA,tgid,(void*)addr,NULL);
    if(tEnvP==-1 && errno)
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot peek data of process TGID=%d at "
              "address %#lx: ptrace(PTRACE_PEEKDATA): %s!",tgid,addr,
              strerror(errno));
      if(ptrace(PTRACE_DETACH,tgid,NULL,NULL))
      {
        mPrintf(errU,ERROR,__func__,0,"Cannot detach process TGID=%d: "
                "ptrace(PTRACE_DETACH): %s!",tgid,strerror(errno));
      }
      *utgid='\0';
      printOutFuncNOK;
      return utgid;
    }
    tEnvP&=tWMask;
    //if(deBug&0x20)mPrintf(errU,VERB,__func__,0,"envP[%d]=%#lx",i,tEnvP);
    if(tEnvP)
    {
      memset(envLine,0,ENV_LINE_SZ);
      for(addr2=tEnvP,k=0;;addr2+=sizeof(long))    /* loop over chars groups */
      {
        rv=ptrace(PTRACE_PEEKDATA,tgid,(void*)addr2,NULL);
        if(rv==-1 && errno)
        {
          mPrintf(errU,ERROR,__func__,0,"Cannot peek data of process TGID=%d "
                  "at address %#lx: ptrace(PTRACE_PEEKDATA): %s!",tgid,addr,
                  strerror(errno));
          if(ptrace(PTRACE_DETACH,tgid,NULL,NULL))
          {
            mPrintf(errU,ERROR,__func__,0,"Cannot detach process TGID=%d: "
                    "ptrace(PTRACE_DETACH): %s!",tgid,strerror(errno));
          }
          *utgid='\0';
          printOutFuncNOK;
          return utgid;
        }
        for(j=0;j<sizeof(long);j++,k++)              /* loop over characters */
        {
          ch=(char)(rv&0xff);
          if(!ch)goto endLoop1;
          rv>>=8;
          if(k<ENV_LINE_SZ-1)envLine[k]=ch;
        }                                            /* loop over characters */
        if(strncmp(envLine,"UTGID=",MIN(k,strlen("UTGID="))))goto endLoop1;
      }                                        /* loop over character groups */
      endLoop1:;
      //if(deBug&0x20)mPrintf(errU,VERB,__func__,0,"env[%d]=\"%s\"",i,envLine);
      if(!strncmp(envLine,"UTGID=",strlen("UTGID=")))
      {
        wch=snprintf(utgid,UTGID_LEN+1,"%s",envLine+strlen("UTGID="));
        if(unlikely(wch>UTGID_LEN))
        {
          mPrintf(errU,ERROR,__func__,0,"UTGID truncated (\"%s\" -> \"%s\"). "
                  "Increase UTGID_LEN variable in tmSrv.h!",
                  envLine+strlen("UTGID="),utgid);
        }
        goto endLoop0;
      }
    }
    if(tEnvP==0)break;
  }                                         /* loop over environment strings */
  endLoop0:;
  /* ----------------------------------------------------------------------- */
  /* detach process */
  if(ptrace(PTRACE_DETACH,tgid,NULL,NULL))
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot detach process TGID=%d: "
            "ptrace(PTRACE_DETACH): %s!",tgid,strerror(errno));
  }
  /* ----------------------------------------------------------------------- */
  printOutFuncOK;
  return utgid;
}
/* ************************************************************************* */
static int file2str(const char *path,char *buf,int bufSz)
{
  int fd,readChars;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  fd=open(path,O_RDONLY,0);
  if(unlikely(fd==-1))
  {
    if(unlikely(deBug&0x80))
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s!",
              path,strerror(errno));
    }
    printOutFuncNOK;
    return -1;
  }
  readChars=read(fd,buf,bufSz-1);
  close(fd);
  if(unlikely(readChars<=0))
  {
    mPrintf(errU,ERROR,__func__,0,"Error reading file \"%s\": "
            "%u characters read!",path,readChars);
    printOutFuncNOK;
    return -1;
  }
  buf[readChars]='\0';
  printOutFuncOK;
  return readChars;
}
/* ************************************************************************* */
static char **file2strvec(const char* path)
{
  /* Read maximum procMaxReadChars bytes at a time */
  /* Linux kernel 2.6.9-67 writes max 1024*3=3072 Bytes at a time */
  /* Linux kernel 2.6.9-78 writes max 121862 Bytes at a time */
  static char *sRBuf;                               /* buffer for one read() */
  static int isFirstTime=1;
  char *tRBuf=NULL;                                     /* buffer for a file */
  int tRBufSz=0;                                            /* size of tRBuf */
  char *p;
  char *endbuf;
  char **q;
  char **ret;
  int fd;
  ssize_t n;
  int c;
  int align;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  if(unlikely(isFirstTime))
  {
    sRBuf=(char*)malloc(procMaxReadChars);
    mPrintf(errU,DEBUG,__func__,0,"%zu Bytes buffer allocated to read from "
            "the procfs.",procMaxReadChars);
    isFirstTime=0;
  }
  /* ----------------------------------------------------------------------- */
  /* read the whole file into a memory buffer, allocating memory as we go */
  fd=open(path,O_RDONLY,0);
  if(unlikely(fd==-1))
  {
    if(unlikely(deBug&0x80))
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s!",
              path,strerror(errno));
    }
    printOutFuncNOK;
    return NULL;
  }
  /* ----------------------------------------------------------------------- */
  for(;;)
  {
    /* ..................................................................... */
    /* I expect n<=3072 or n<=121862 in kernel 2.6.9-78 */
    n=read(fd,sRBuf,sizeof sRBuf); 
    if(unlikely(deBug&0x80))
    {
       mPrintf(errU,VERB,__func__,0,"File: \"%s\": read(): %zu Bytes read.",
               path,n);
    }
    /* ..................................................................... */
    if(unlikely(n<0))                                          /* read error */
    {
      if(errno==EINTR)                            /* Interrupted system call */
      {
        if(unlikely(deBug&0x80))
        {
          mPrintf(errU,WARN,__func__,0,"read(%s): %s! Retrying...",path,
                  strerror(errno));
        }
        continue;
      }
      else                                              /* severe read error */
      {
        if(unlikely(deBug&0x80))
        {
          mPrintf(errU,ERROR,__func__,0,"Error reading file \"%s\": %s!",path,
                  strerror(errno));
        }
        close(fd);
        if(tRBuf)free(tRBuf);
        printOutFuncNOK;
        return NULL;
      }                                              /* if severe read error */
    }                                                       /* if read error */
    /* ..................................................................... */
    if(!n)                                  /* zero Bytes read the last time */
    {
      if(unlikely(!tRBuf))                          /* zero Total Bytes read */
      {
        if(unlikely(deBug&0x80))
        {
           mPrintf(errU,WARN,__func__,0,"Zero Bytes read in file: \"%s\"!",
                   path);
        }
        close(fd);
        if(tRBuf)free(tRBuf);
        printOutFuncOK;
        return NULL;
      }
      else                                                            /* EOF */
      {
        if(unlikely(deBug&0x80))
        {
          mPrintf(errU,WARN,__func__,0,"Last read character: (ASCII "
                  "%u).",tRBuf[tRBufSz-1]);
        }
        if(tRBuf[tRBufSz-1]!=0)                          /* truncated string */
        {
          tRBuf=realloc1(tRBuf,tRBufSz+1);/* allocate 1 B memory more for \0 */
          tRBuf[tRBufSz]='\0';
          tRBufSz++;
          if(unlikely(deBug&0x80))
          {
            mPrintf(errU,WARN,__func__,0,"File \"%s\" truncated. NULL "
                    "terminator added.",path);
            mPrintf(errU,DEBUG,__func__,0,"File: \"%s\": Total %zu Bytes "
                    "read.",path,tRBufSz);
          }
        }
        break;
      }
    }                                    /* if zero Bytes read the last time */
    /* ..................................................................... */
    tRBuf=realloc1(tRBuf,tRBufSz+n);                 /* allocate more memory */
    memcpy(tRBuf+tRBufSz,sRBuf,n);                    /* copy buffer into it */
    tRBufSz+=n;                                  /* increment total byte ctr */
    if(unlikely(deBug&0x80))
    {
      mPrintf(errU,DEBUG,__func__,0,"File: \"%s\": Total %zu Bytes "
              "read.",path,tRBufSz);
    }
  }                                                               /* for(;;) */
  /* ----------------------------------------------------------------------- */
  close(fd);
  /* ----------------------------------------------------------------------- */
  endbuf=tRBuf+tRBufSz;
  align=(sizeof(char*)-1)-((tRBufSz+sizeof(char*)-1)&(sizeof(char*)-1));
  /* one string pointer for each string */
  for(c=0,p=tRBuf;p<endbuf;p++)if(!*p)c+=sizeof(char*);
  c+=sizeof(char*);    /* one extra string pointer for the last NULL pointer */
  /* make room for pointers at the end of tRBuf */
  tRBuf=realloc1(tRBuf,tRBufSz+c+align);
  endbuf=tRBuf+tRBufSz;
  q=ret=(char**)(endbuf+align);              /* use free(*ret) to deallocate */
  *q++=p=tRBuf;                             /* point pointers to the strings */
  endbuf--;                                     /* do not traverse final NUL */
  /* '\0' char implies that next string starts at the next character */
  while(++p<endbuf)if(!*p)*q++=p+1;
  *q=0;                                      /* NULL pointer list terminator */
  printOutFuncOK;
  return ret;
}
/* ************************************************************************* */
static int getOomScore(char *oomScoreBuf)
{
  char *endptr;
  int oomScore;
  /* ----------------------------------------------------------------------- */
  oomScore=(int)strtol(oomScoreBuf,&endptr,10);
  return oomScore;
}
/* ************************************************************************* */
static char getStateFromStat(char *stat)
{
  char *p;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  p=strrchr(stat,')');
  if(unlikely(!p))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process state from "
            "/proc/<pid>/stat: \"%s\"!",stat);
    printOutFuncNOK;
    return 'U';
  }
  p+=2;
  if(unlikely(!strchr("SRDTWXZ",*p)))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process state from "
            "/proc/<pid>/stat: \"%s\"!",stat);
    printOutFuncNOK;
    return 'U';
  }
  printOutFuncOK;
  return *p;
}
/* ************************************************************************* */
static char getStateFromStatus(char *status)
{
  char *p;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  p=strstr(status,"State:");
  if(unlikely(!p))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process state from "
            "/proc/<pid>/status: \"%s\"!",status);
    printOutFuncNOK;
    return 'U';
  }
  p+=strlen("State:");
  p+=strspn(p," \t");
  if(unlikely(!strchr("SRDTWXZ",*p)))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process state from "
            "/proc/<pid>/status: \"%s\"!",status);
    printOutFuncNOK;
    return 'U';
  }
  printOutFuncOK;
  return *p;
}
/* ************************************************************************* */
static char *getCmdFromStatus(char *status)
{
  char *p;
  static char cmd[CMD_LEN+1];
  int cmdLen=0;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  *cmd='\0';
  p=strstr(status,"Name:");
  if(unlikely(!p))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process name from "
            "/proc/<pid>/status: \"%s\"!",status);
    printOutFuncNOK;
    return cmd;
  }
  p+=strlen("Name:");
  p+=strspn(p," \t");
  cmdLen=strcspn(p," \t\n");
  if(unlikely(!cmdLen))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process name from "
            "/proc/<pid>/status: \"%s\"!",status);
    printOutFuncNOK;
    return cmd;
  }
  snprintf(cmd,MIN(CMD_LEN+1,cmdLen+1),"%s",p);
  printOutFuncOK;
  return cmd;
}
/* ************************************************************************* */
static unsigned long getStaStkFromStat(char *stat)
{
  char *p;
  unsigned long staStk;
  int rv;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  p=strrchr(stat,')');
  if(unlikely(!p))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process Start Stack from "
            "/proc/<pid>/stat: \"%s\"!",stat);
    printOutFuncNOK;
    return 0;
  }
  p+=2;
  rv=sscanf(p,"%*c %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s "
            "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lu",&staStk);
  if(unlikely(rv!=1))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process Start Stack from "
            "/proc/<pid>/stat: \"%s\". Not enough items!",stat);
    printOutFuncNOK;
    return 0;
  }
  printOutFuncOK;
  return staStk;
}
/* ************************************************************************* */
static unsigned long getStaStkFromStatus(char *status)
{
  char *p;
  unsigned long staStk;
  char *endptr=NULL;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  p=strstr(status,"StaStk:");
  if(unlikely(!p))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process Start Stack from "
            "/proc/<pid>/status: string \"StaStk:\" not found!");
    printOutFuncNOK;
    return 0;
  }
  p+=strlen("StaStk:");
  staStk=strtoul(++p,&endptr,16);
  if(unlikely(unlikely(staStk==0) && unlikely(errno==EINVAL || errno==ERANGE)))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process Start Stack from "
            "/proc/<pid>/status: %s!",strerror(errno));
    return 0;
  }
  endptr+=strspn(endptr," \t");
  if(unlikely(strncmp(endptr,"kB",2)))
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading process Start Stack from "
            "/proc/<pid>/status: string \"kB\" not found!");
    return 0;
  }
  printOutFuncOK;
  return staStk;
}
/* ************************************************************************* */
void *realloc1(void *oldp,size_t size)
{
  void *p;
  /* ----------------------------------------------------------------------- */
  if(size==0)size++;
  p=realloc(oldp,size);
  if(!p)
  {
    mPrintf(errU,FATAL,__func__,0,"realloc1: realloc(%zu) failed: %s!",size,
            strerror(errno));
    exit(1);
  }
  return(p);
}
/* ************************************************************************* */
/* getActuatorVersion() - Returns the RCS identifier of this file.           */
/* ************************************************************************* */
char *getActuatorVersion()
{
  printInFunc;
  printOutFuncOK;
  return rcsid;
}
/* ************************************************************************* */
/* get maximum PID in running kernel                                         */
/* ************************************************************************* */
int getMaxPid(void)
{
  char sbuf[1024];
  int pm=131072;         /* this must be larger then any possible real value */
  /* ----------------------------------------------------------------------- */
  if(file2str(PID_MAX_FILE,sbuf,sizeof(sbuf))!=-1)
  {
    pm=(int)strtol(sbuf,NULL,0);
  }
  return pm;
}
/* ************************************************************************* */
/* functions to manage the process linked list                               */
/* ************************************************************************* */
/* pLadd()                                                                   */
/* Add one element at the end of the list or replace an existing element.    */
/* If new element added then return {-1,NULL,NULL,0,0,0,0,NULL,NULL}.        */
/* If old element replaced return the old element with char* values not      */
/* changed set to NULL.                                                      */
/*  N.B.: element can change, i.e. due to an execve() which changes cmd      */
/*  N.B.: if cmd and utgid returned are not NULL, they must be removed by    */
/*        the calling function.                                              */
/* ************************************************************************* */
procListEl_t pLadd(procList_t *list,pid_t tgid,char *cmd,char *utgid,
                   int oomScore)
{
  procListEl_t *el=NULL;
  /* procListEl_t: tgid,cmd,utgid,running,termStatus,lastUpdate,prev,next */
  procListEl_t oldEl={-1,NULL,NULL,0,0,0,0,NULL,NULL};
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  /* ----------------------------------------------------------------------- */
  /* try to find the tgid in the list */
  for(el=list->head;el;el=el->next)if(el->tgid==tgid)break;
  /* ----------------------------------------------------------------------- */
  if(el)                                         /* tgid already in the list */
  {
    oldEl.tgid=el->tgid;
    if(!el->cmd || strcmp(cmd,el->cmd))  /* CMD was not defined or different */
    {
      oldEl.cmd=el->cmd;
      el->cmd=cmd;
    }
    else
    {
      if(cmd)free(cmd);                  /* not used. We have to suppress it */
    }
    if(!el->utgid || strcmp(utgid,el->utgid))   /* UTGID was not def or diff */
    {
      oldEl.utgid=el->utgid;
      el->utgid=utgid;
    }
    else
    {
      if(utgid)free(utgid);              /* not used. We have to suppress it */
    }
    el->lastUpdate=time(NULL);
    oldEl.lastUpdate=el->lastUpdate;
    el->oomScore=oomScore;
    oldEl.oomScore=el->oomScore;
    if(!el->running)           /* this could happen for still-born processes */
    {
      char termStatusS[64];
      int severity=FATAL;
      if(WIFEXITED(el->termStatus))
      {
        if(WEXITSTATUS(el->termStatus)==0)severity=WARN;
        snprintf(termStatusS,64,"terminated with exit code %d",
                 WEXITSTATUS(el->termStatus));
      }
      else if(WIFSIGNALED(el->termStatus))
      {
        if(WTERMSIG(el->termStatus)==SIGINT ||
           WTERMSIG(el->termStatus)==SIGTERM)severity=WARN;
        snprintf(termStatusS,64,"terminated on signal %d (%s)",
                 WTERMSIG(el->termStatus),
                 sig2msg(WTERMSIG(el->termStatus)));
      }
      else if(WIFSTOPPED(el->termStatus))
      {
        severity=WARN;
        snprintf(termStatusS,64,"stopped on signal %d (%s)",
                 WSTOPSIG(el->termStatus),
                 sig2msg(WSTOPSIG(el->termStatus)));
      }
      else
      {
        snprintf(termStatusS,64,"terminated");
      }
      mPrintf(errU,severity,__func__,0,"Process TGID=%d, UTGID=\"%s\", "
              "CMD=\"%s\" %s!",el->tgid,el->utgid,el->cmd,termStatusS);
    }
  }
  else                                           /* tgid not yet in the list */
  {
    /* create new node */
    el=(procListEl_t*)malloc(sizeof(procListEl_t));
    /* fill-in new node */
    el->tgid=tgid;
    el->cmd=cmd;
    el->utgid=utgid;
    el->running=1;
    el->oomScore=oomScore;
    el->termStatus=-1;
    el->lastUpdate=time(NULL);
    /* set links */
    el->prev=NULL;
    el->next=NULL;
    if(list->tail)                            /* list has at least 1 element */
    {
      el->prev=list->tail;
      list->tail->next=el;
    }
    else                                             /* list has no elements */
    {
      list->head=el;
    }
    list->tail=el;
  }                                        /* if tgid is not yet in the list */
  /* ----------------------------------------------------------------------- */
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  printOutFuncOK;
  return oldEl;
}
/* ************************************************************************* */
/* set a process as terminated */
void pLsetTerm(procList_t *list,pid_t tgid,int termStatus)
{
  procListEl_t *el=NULL;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  /* ----------------------------------------------------------------------- */
  /* find tgid in the list */
  for(el=list->head;el;el=el->next)if(el->tgid==tgid)break;
  /* ----------------------------------------------------------------------- */
  if(el)                                         /* tgid already in the list */
  {
    char termStatusS[64];
    int severity=FATAL;
    el->lastUpdate=time(NULL);
    el->running=0;
    el->termStatus=termStatus;
    if(WIFEXITED(el->termStatus))
    {
      if(WEXITSTATUS(el->termStatus)==0)severity=INFO;
      snprintf(termStatusS,64,"terminated with exit code: %d",
               WEXITSTATUS(el->termStatus));
    }
    else if(WIFSIGNALED(el->termStatus))
    {
      if(WTERMSIG(el->termStatus)==SIGINT ||
         WTERMSIG(el->termStatus)==SIGTERM)severity=WARN;
      snprintf(termStatusS,64,"terminated on signal: %d (%s)",
               WTERMSIG(el->termStatus),sig2msg(WTERMSIG(el->termStatus)));
    }
    else if(WIFSTOPPED(el->termStatus))
    {
      severity=WARN;
      snprintf(termStatusS,64,"stopped on signal: %d (%s)",
               WSTOPSIG(el->termStatus),sig2msg(WSTOPSIG(el->termStatus)));
    }
    else
    {
      snprintf(termStatusS,64,"terminated");
    }
    if(severity<=INFO)
    {
      mPrintf(errU,severity,__func__,0,"waitpid(): Process TGID=%d, "
              "CMD=\"%s\", UTGID=\"%s\" %s.",el->tgid,el->cmd,el->utgid,
              termStatusS);
    }
    else
    {
      mPrintf(errU,severity,__func__,0,"waitpid(): Process TGID=%d, "
              "CMD=\"%s\", UTGID=\"%s\" %s!",el->tgid,el->cmd,el->utgid,
              termStatusS);
    }
  }
  else   /* tgid not in the list. This could happen for still-born processes */
  {
    /* create new node */
    el=(procListEl_t*)malloc(sizeof(procListEl_t));
    /* fill-in new node */
    el->tgid=tgid;
    el->cmd=NULL;
    el->utgid=NULL;
    el->running=0;
    el->termStatus=termStatus;
    el->lastUpdate=time(NULL);
    /* set links */
    el->prev=NULL;
    el->next=NULL;
    if(list->tail)                            /* list has at least 1 element */
    {
      el->prev=list->tail;
      list->tail->next=el;
    }
    else                                             /* list has no elements */
    {
      list->head=el;
    }
    list->tail=el;
  }                                               /* if tgid not in the list */
  /* ----------------------------------------------------------------------- */
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
/* set disappeared process as terminated */
void pLautoSetTerm(procList_t *list,time_t timeOut)
{
  procListEl_t *el=NULL;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  /* ----------------------------------------------------------------------- */
  /* find tgid in the list */
  for(el=list->head;el;el=el->next)
  {
    if(time(NULL)-el->lastUpdate>timeOut)
    {
      if(el->running)
      {
        el->running=0;
        el->termStatus=-1;
      }
    }
  }
  /* ----------------------------------------------------------------------- */
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
/* remove and return the element with TGID tgid from the list */
procListEl_t pLrm(procList_t *list,pid_t tgid)
{
  procListEl_t *el=NULL;
  procListEl_t *next=NULL;
  procListEl_t *prev=NULL;
  /* procListEl_t: tgid,cmd,utgid,running,termStatus,lastUpdate,prev,next */
  procListEl_t rmEl={-1,NULL,NULL,0,0,0,0,NULL,NULL};
  /* ----------------------------------------------------------------------- */
  printInFunc;
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  /* ----------------------------------------------------------------------- */
  /* find tgid in the list */
  for(el=list->head;el;el=el->next)if(el->tgid==tgid)break;
  /* ----------------------------------------------------------------------- */
  if(el)                                                 /* tgid in the list */
  {
    rmEl=*el;
    next=el->next;
    prev=el->prev;
    if(el->next)el->next->prev=prev;
    if(el->prev)el->prev->next=next;
    if(!prev)list->head=next;
    if(!next)list->tail=prev;
    free(el);
  }
  /* ----------------------------------------------------------------------- */
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  printOutFuncOK;
  return rmEl;
}
/* ************************************************************************* */
/* remove and return one terminated process after a timeout timeOut */
procListEl_t pLrmTerm(procList_t *list,time_t timeOut)
{
  procListEl_t *el=NULL;
  procListEl_t *next=NULL;
  procListEl_t *prev=NULL;
  /* procListEl_t: tgid,cmd,utgid,running,termStatus,lastUpdate,prev,next */
  procListEl_t rmEl={-1,NULL,NULL,0,0,0,0,NULL,NULL};
  /* ----------------------------------------------------------------------- */
  printInFunc;
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  /* ----------------------------------------------------------------------- */
  /* find a timeouted process in the list */
  for(el=list->head;el;el=el->next)
  {
    if(time(NULL)-el->lastUpdate>timeOut)break;
  }
  /* ----------------------------------------------------------------------- */
  if(el)                                        /* found a timeouted process */
  {
    rmEl=*el;
    next=el->next;
    prev=el->prev;
    if(el->next)el->next->prev=prev;
    if(el->prev)el->prev->next=next;
    if(!prev)list->head=next;
    if(!next)list->tail=prev;
    free(el);
  }
  /* ----------------------------------------------------------------------- */
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  printOutFuncOK;
  return rmEl;
}
/* ************************************************************************* */
/* remove and return the element at the beginning of the list */
procListEl_t pLpop(procList_t *list)
{
  procListEl_t *el=NULL;
  procListEl_t *next=NULL;
  /* procListEl_t: tgid,cmd,utgid,running,termStatus,lastUpdate,prev,next */
  procListEl_t rmEl={-1,NULL,NULL,0,0,0,0,NULL,NULL};
  /* ----------------------------------------------------------------------- */
  printInFunc;
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  el=list->head;
  if(el)
  {
    rmEl=*el;
    next=el->next;
    list->head=next;
    if(!next)list->tail=NULL;
    free(el);
  }
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  printOutFuncOK;
  return rmEl;
}
/* ************************************************************************* */
/* print the process list */
void pLprint(procList_t *list,int severity)
{
  procListEl_t *el=NULL;
  char *msg=NULL;
  int msgl=0;
  char *p=msg;
  struct tm lastUpdateL;
  char lastUpdateS[13];                                    /* Oct12-134923\0 */
  char termStatusS[64];
  /* ----------------------------------------------------------------------- */
  printInFunc;
  msgl=snprintf(NULL,0,"List:");
  msg=(char*)malloc(msgl+1);
  sprintf(msg,"List:");
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  for(el=list->head;el;el=el->next)
  {
    char *cmd=el->cmd;
    char *utgid=el->utgid;
    if(!cmd)cmd="unknown";
    if(!utgid)utgid="unknown";
    localtime_r(&el->lastUpdate,&lastUpdateL);
    strftime(lastUpdateS,13,"%b%d-%H%M%S",&lastUpdateL);
    if(el->running==1 || WIFCONTINUED(el->termStatus))
    {
      snprintf(termStatusS,64,"running");
    }
    else if(WIFEXITED(el->termStatus))
    {
      snprintf(termStatusS,64,"terminated with exit code %d",
               WEXITSTATUS(el->termStatus));
    }
    else if(WIFSIGNALED(el->termStatus))
    {
      snprintf(termStatusS,64,"terminated on signal %d (%s)",
               WTERMSIG(el->termStatus),sig2msg(WTERMSIG(el->termStatus)));
    }
    else if(WIFSTOPPED(el->termStatus))
    {
      snprintf(termStatusS,64,"stopped on signal %d (%s)",
               WSTOPSIG(el->termStatus),sig2msg(WSTOPSIG(el->termStatus)));
    }
    else
    {
      snprintf(termStatusS,64,"terminated");
    }
    msgl+=snprintf(NULL,0," (%d,\"%s\",\"%s\",\"%s\",\"%s\",oom-score=%d)",
                   el->tgid,cmd,utgid,lastUpdateS,termStatusS,el->oomScore);
    msg=(char*)realloc(msg,msgl+1);
    p=strchr(msg,'\0');
    sprintf(p," (%d,\"%s\",\"%s\",\"%s\",\"%s\",oom-score=%d)",el->tgid,cmd,
            utgid,lastUpdateS,termStatusS,el->oomScore);
  }
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  mPrintf(errU,severity,__func__,10,"%s",msg);
  if(msg)free(msg);
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
/* get the short process list */
void pLlsSvc(procList_t *list,char **address,int *size)
{
  char *p;
  procListEl_t *el=NULL;
  static char refreshN=0;
  static char *buf=NULL;                                           /* buffer */
  static int bufSz=0;                                         /* buffer size */
  int bufBSz=0;                                       /* busy size of buffer */
  int oldBufSz=0;                                      /* old size of buffer */
  /* UTGID */
  int recordSz=UTGID_LEN+1;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  /* ----------------------------------------------------------------------- */
  refreshN++;
  /* ----------------------------------------------------------------------- */
  /* clean buffer */
  if(bufSz)memset(buf,0,bufSz);
  bufBSz=0;
  if(bufSz-bufBSz<recordSz)                 /* not enough space for 1 record */
  {
    /* (re)allocate space for buf */
    oldBufSz=bufSz;
    bufSz+=3*recordSz;
    if(deBug&0x10)
    {
      mPrintf(errU,WARN,__func__,9, "Short list buffer: %d B, busy: %d B, "
              "record size: %d B. Reallocate buffer to %d B.",
              oldBufSz,bufBSz,recordSz,bufSz);
    }
    buf=(char*)realloc(buf,bufSz);
    /* clean the new elements of buf */
    memset(buf+oldBufSz,0,bufSz-oldBufSz);
  }
  /* ----------------------------------------------------------------------- */
  if(!list->head)                                              /* empty list */
  {
    p=buf;
    sprintf(p,"(none)");
    p=1+strchr(p,'\0');
    bufBSz=p-buf;
  }
  else                                                     /* list not empty */
  {
    for(el=list->head;el;el=el->next)         /* loop over processes in list */
    {
      if(el->running==1 || WIFCONTINUED(el->termStatus))
      {
        char *utgid=el->utgid;
        /* ................................................................. */
        if(unlikely(!utgid))utgid="unknown";
        /* ................................................................. */
        if(bufSz-bufBSz<recordSz)           /* not enough space for 1 record */
        {
          /* (re)allocate space for buf */
          oldBufSz=bufSz;
          bufSz+=3*recordSz;
          if(deBug&0x10)
          {
            mPrintf(errU,WARN,__func__,9, "Short list buffer: %d B, busy: %d "
                    "B, record size: %d B. Reallocate buffer to %d B.",
                    oldBufSz,bufBSz,recordSz,bufSz);
          }
          buf=(char*)realloc(buf,bufSz);
          /* clean the new elements of buf */
          memset(buf+oldBufSz,0,bufSz-oldBufSz);
        }
        /* ................................................................. */
        /* write record */
        p=buf+bufBSz;
        sprintf(p,"%s",utgid);
        p=1+strchr(p,'\0');
        bufBSz=p-buf;
        /* ................................................................. */
      }                /* if(el->running==1 || WIFCONTINUED(el->termStatus)) */
    }                                         /* loop over processes in list */
  }                                                        /* list not empty */
  *address=buf;
  *size=bufBSz;
  /* ----------------------------------------------------------------------- */
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  /* ----------------------------------------------------------------------- */
  if(deBug&0x10 && refreshN && refreshN%10==0)
  {
    mPrintf(errU,VERB,__func__,0,"Short list buffer size: %d B, busy: %d B, "
                "free %d B = %d pss.",bufSz,bufBSz,bufSz-bufBSz,
                (bufSz-bufBSz)/recordSz);
  }
  /* ----------------------------------------------------------------------- */
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
/* get the long process list */
void pLllSvc(procList_t *list,char **address,int *size)
{
  char *p;
  procListEl_t *el=NULL;
  static char refreshN=0;
  static char *buf=NULL;                                           /* buffer */
  static int bufSz=0;                                         /* buffer size */
  int bufBSz=0;                                       /* busy size of buffer */
  int oldBufSz=0;                                      /* old size of buffer */
  struct tm lastUpdateL;
  char lastUpdateS[13];                                    /* Oct12-134923\0 */
  char termStatusS[64];
  /* TGID CMD UTGID lastUpdateS termStatusS */
  int recordSz=snprintf(NULL,0,"%d",maxPid)+1+CMD_LEN+1+UTGID_LEN+1+13+64+20;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  /* ----------------------------------------------------------------------- */
  refreshN++;
  /* ----------------------------------------------------------------------- */
  /* clean buffer */
  if(bufSz)memset(buf,0,bufSz);
  bufBSz=0;
  if(bufSz-bufBSz<recordSz)                 /* not enough space for 1 record */
  {
    /* (re)allocate space for buf */
    oldBufSz=bufSz;
    bufSz+=3*recordSz;
    if(deBug&0x10)
    {
      mPrintf(errU,WARN,__func__,9, "Long list buffer: %d B, busy: %d B, "
              "record size: %d B. Reallocate buffer to %d B.",
              oldBufSz,bufBSz,recordSz,bufSz);
    }
    buf=(char*)realloc(buf,bufSz);
    /* clean the new elements of buf */
    memset(buf+oldBufSz,0,bufSz-oldBufSz);
  }
  /* ----------------------------------------------------------------------- */
  if(!list->head)                                              /* empty list */
  {
    p=buf;
    sprintf(p,"-1");
    p=1+strchr(p,'\0');
    sprintf(p,"(none)");
    p=1+strchr(p,'\0');
    sprintf(p,"(none)");
    p=1+strchr(p,'\0');
    sprintf(p,"(none)");
    p=1+strchr(p,'\0');
    sprintf(p,"(none)");
    p=1+strchr(p,'\0');
    sprintf(p,"(none)");
    p=1+strchr(p,'\0');
    bufBSz=p-buf;
  }
  else                                                     /* list not empty */
  {
    for(el=list->head;el;el=el->next)         /* loop over processes in list */
    {
      char *cmd=el->cmd;
      char *utgid=el->utgid;
      /* ................................................................... */
      if(!cmd)cmd="unknown";
      if(!utgid)utgid="unknown";
      localtime_r(&el->lastUpdate,&lastUpdateL);
      strftime(lastUpdateS,13,"%b%d-%H%M%S",&lastUpdateL);
      if(el->running==1 || WIFCONTINUED(el->termStatus))
      {
        snprintf(termStatusS,64,"running");
      }
      else if(WIFEXITED(el->termStatus))
      {
        snprintf(termStatusS,64,"terminated with exit code %d",
                 WEXITSTATUS(el->termStatus));
      }
      else if(WIFSIGNALED(el->termStatus))
      {
        snprintf(termStatusS,64,"terminated on signal %d (%s)",
                 WTERMSIG(el->termStatus),sig2msg(WTERMSIG(el->termStatus)));
      }
      else if(WIFSTOPPED(el->termStatus))
      {
        snprintf(termStatusS,64,"stopped on signal %d (%s)",
                 WSTOPSIG(el->termStatus),sig2msg(WSTOPSIG(el->termStatus)));
      }
      else
      {
        snprintf(termStatusS,64,"terminated");
      }
      /* ................................................................... */
      if(bufSz-bufBSz<recordSz)             /* not enough space for 1 record */
      {
        /* (re)allocate space for buf */
        oldBufSz=bufSz;
        bufSz+=3*recordSz;
        if(deBug&0x10)
        {
          mPrintf(errU,WARN,__func__,9, "Long list buffer: %d B, busy: %d B, "
                  "record size: %d B. Reallocate buffer to %d B.",
                  oldBufSz,bufBSz,recordSz,bufSz);
        }
        buf=(char*)realloc(buf,bufSz);
        /* clean the new elements of buf */
        memset(buf+oldBufSz,0,bufSz-oldBufSz);
      }
      /* ................................................................... */
      /* write record */
      p=buf+bufBSz;
      sprintf(p,"%d",el->tgid);
      p=1+strchr(p,'\0');
      sprintf(p,"%s",cmd);
      p=1+strchr(p,'\0');
      sprintf(p,"%s",utgid);
      p=1+strchr(p,'\0');
      sprintf(p,"%s",lastUpdateS);
      p=1+strchr(p,'\0');
      sprintf(p,"%s",termStatusS);
      p=1+strchr(p,'\0');
      if(el->oomScore!=INT_MIN)                       /* oom_score available */
      {
        sprintf(p,"%d",el->oomScore);
      }
      else                                        /* oom_score NOT available */
      {
        sprintf(p,"%s","N/A");
      }
      p=1+strchr(p,'\0');
      bufBSz=p-buf;
      /* ................................................................... */
    }                                         /* loop over processes in list */
  }                                                        /* list not empty */
  *address=buf;
  *size=bufBSz;
  /* ----------------------------------------------------------------------- */
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  /* ----------------------------------------------------------------------- */
  if(deBug&0x10 && refreshN && refreshN%10==0)
  {
    mPrintf(errU,VERB,__func__,0,"Long list buffer size: %d B, busy: %d B, "
                "free %d B = %d pss.",bufSz,bufBSz,bufSz-bufBSz,
                (bufSz-bufBSz)/recordSz);
  }
  /* ----------------------------------------------------------------------- */
  printOutFuncOK;
  return;
}
/* ************************************************************************* */
/* return the length of the process list */
int pLlen(procList_t *list)
{
  procListEl_t *el=NULL;
  int i=0;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  for(el=list->head;el;el=el->next)i++;
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  printOutFuncOK;
  return i;
}
/* ************************************************************************* */
/* find a process in the process list */
procListEl_t *pLfind(procList_t *list,pid_t tgid)
{
  procListEl_t *el=NULL;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  if(pthread_mutex_lock(&list->mutex))eExit("pthread_mutex_lock()");
  for(el=list->head;el;el=el->next)
  {
    if(el->tgid==tgid)break;
  }
  if(pthread_mutex_unlock(&list->mutex))eExit("pthread_mutex_unlock()");
  printOutFuncOK;
  return el;
  /* ----------------------------------------------------------------------- */
}
/* ************************************************************************* */
/* set the oom killer adjustment */
int setOomScoreAdj(int oomScoreAdjVal,char *utgid,pid_t tgid)
{
  int oomAdjVal;
  char oomAdjStr[8]="";                                          /* [-17,15] */
  char oomScoreAdjStr[8]="";                                 /* [-1000,1000] */
  char *p;
  int rv=0;
  char oomScoreAdjFN[64]="";
  char oomAdjFN[64]="";
  /* ----------------------------------------------------------------------- */
  oomAdjVal=oomScoreAdjVal*16/1000;
  if(oomAdjVal<-16)oomAdjVal=-16;
  if(oomAdjVal>15)oomAdjVal=15;
  snprintf(oomScoreAdjStr,8,"%d",oomScoreAdjVal);
  snprintf(oomAdjStr,8,"%d",oomAdjVal);
  snprintf(oomScoreAdjFN,64,"/proc/%d/oom_score_adj",tgid);
  snprintf(oomAdjFN,64,"/proc/%d/oom_adj",tgid);
  /* ----------------------------------------------------------------------- */
  if(!access(oomScoreAdjFN,W_OK))                        /* kernel >= 2.6.36 */
  {
    int oomScoreAdjFD=-1;
    /* write value */
    oomScoreAdjFD=open(oomScoreAdjFN,O_WRONLY);
    if(oomScoreAdjFD==-1)
    {
      mPrintf(errU,WARN,__func__,0,"Cannot set oom_score_adj for the process "
              "utgid=\"%s\": open(2): %s!",utgid,strerror(errno));
      rv=-2;
    }
    else
    {
      int wB=-1;
      wB=write(oomScoreAdjFD,oomScoreAdjStr,strlen(oomScoreAdjStr));
      if(wB==-1)
      {
        mPrintf(errU,WARN,__func__,0,"Cannot set oom_score_adj for the process "
                "utgid=\"%s\": write(2): %s!",utgid,strerror(errno));
        rv=-3;
      }
      close(oomScoreAdjFD);
    }
    /* read written value */
    memset(oomScoreAdjStr,0,8);
    oomScoreAdjFD=open(oomScoreAdjFN,O_RDONLY);
    read(oomScoreAdjFD,oomScoreAdjStr,8);
    close(oomScoreAdjFD);
    p=strchr(oomScoreAdjStr,'\n');
    if(p)*p='\0';
    mPrintf(errU,DEBUG,__func__,0,"oom_score_adj set to %s for process "
            "utgid=\"%s\".",oomScoreAdjStr,utgid);
  }
  else if(!access(oomAdjFN,W_OK))                         /* kernel < 2.6.36 */
  {
    int oomAdjFD=-1;
    /* write value */
    oomAdjFD=open(oomAdjFN,O_WRONLY);
    if(oomAdjFD==-1)
    {
      mPrintf(errU,WARN,__func__,0,"Cannot set oom_adj for the process "
              "utgid=\"%s\": open(2): %s!",utgid,strerror(errno));
      rv=-2;
    }
    else
    {
      int wB=-1;
      wB=write(oomAdjFD,oomAdjStr,strlen(oomAdjStr));
      if(wB==-1)
      {
        mPrintf(errU,WARN,__func__,0,"Cannot set oom_adj for the process "
                "utgid=\"%s\": write(2): %s!",utgid,strerror(errno));
        rv=-3;
      }
      close(oomAdjFD);
    }
    /* read written value */
    memset(oomAdjStr,0,8);
    oomAdjFD=open(oomAdjFN,O_RDONLY);
    read(oomAdjFD,oomAdjStr,8);
    close(oomAdjFD);
    p=strchr(oomAdjStr,'\n');
    if(p)*p='\0';
    mPrintf(errU,DEBUG,__func__,0,"oom_adj set to %s for process "
            "utgid=\"%s\".",oomAdjStr,utgid);
  }
  else
  {
    mPrintf(errU,WARN,__func__,0,"OOM Killer Score Adjustment not foreseen "
            "in this OS version (%s)!",kernelVersion);
    rv=-1;
  }
  /* ----------------------------------------------------------------------- */
  return rv;
}
/* ************************************************************************* */
