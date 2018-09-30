/* ######################################################################### */
/* 
 * $Log: getPsFromProc.c,v $
 * Revision 2.19  2009/01/30 14:45:18  galli
 * Print priority as "top"
 *
 * Revision 2.18  2009/01/30 14:29:28  galli
 * bug fixed in getting shared memory used by a ps
 *
 * Revision 2.17  2009/01/22 10:47:10  galli
 * get Page Size using sysconf() instead of /usr/include/asm/page.h
 *
 * Revision 2.16  2009/01/22 10:16:10  galli
 * do not depend on kernel-devel headers
 *
 * Revision 2.15  2009/01/13 13:13:02  galli
 * getMaxPid() function become public.
 * envSz variable become public.
 * less message verbosity in file2str() and file2strvec()
 *
 * Revision 2.12  2009/01/07 09:23:12  galli
 * more comments and less diagnostic messages
 *
 * Revision 2.11  2008/12/11 14:20:22  galli
 * do not use libprocps anymore
 *
 * Revision 2.0  2008/11/20 08:25:07  galli
 * rewriting from scratch (temporary version)
 *
 * Revision 1.20  2007/12/11 13:39:24  galli
 * bug fixed
 *
 * Revision 1.19  2007/12/11 13:18:51  galli
 * If huge environment (>PROC_PID_ENV_SZ B) getUtgid() appends "..."
 *
 * Revision 1.18  2007/08/23 07:02:19  galli
 * bug fixed
 *
 * Revision 1.17  2007/08/22 07:00:35  galli
 * added to the taskdata struct: pgrp
 *
 * Revision 1.16  2007/08/21 11:24:25  galli
 * added to the taskdata struct: vm_lock, vm_rss, vm_data, vm_stack, vm_exe,
 * vm_lib.
 *
 * Revision 1.14  2007/08/20 08:59:50  galli
 * bug fixed in getEuser() and getEgroup()
 * getPrio() return a number string also for FF and RR
 *
 * Revision 1.13  2007/08/10 13:54:45  galli
 * getSensorVersion() renamed to getPSsensorVersion()
 * compatible with libFMCutils v 2
 *
 * Revision 1.12  2006/10/23 21:12:25  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.11  2005/08/29 11:39:54  galli
 * escape_command() calls modified to comply with libproc-3.2.5
 *
 * Revision 1.10  2004/11/08 01:22:51  galli
 * bug fixed
 *
 * Revision 1.9  2004/11/03 10:49:04  galli
 * cope with PID_MAX/PID_MAX_DEFAULT change in linux/threads.h
 *
 * Revision 1.8  2004/10/17 21:15:33  galli
 * getSensorVersion() function added
 *
 * Revision 1.7  2004/10/16 13:30:27  galli
 * Uses logger for diagnostics
 *
 * Revision 1.5  2004/10/16 08:30:26  galli
 * getUtgid() function added
 *
 * Revision 1.4  2004/10/16 07:19:37  galli
 * psInfo() function added
 *
 * Revision 1.3  2004/08/25 12:20:18  galli
 * use PID_MAX from linux/threads.h to evaluate maximum number of tasks
 * priority as a string (set to RT for real-time tasks)
 *
 * Revision 1.2  2004/08/24 10:41:11  galli
 * %CPU as in top (since last update), not as in ps
 *
 * Revision 1.1  2004/08/23 09:36:16  galli
 * Initial revision
 * */
/* ######################################################################### */
/* deBug mask:                                                               */
/* 0x001 in/out functions                                                    */
/* 0x002 file2strvec() diagnosttics                                          */
/* 0x004 getTasks() diagnosttics                                             */
/* 0x008 incoherence in read data                                            */
/* ######################################################################### */
/* Summary of data retrieved:                                                */
/* ------------------------------------------------------------------------- */
/* Basic:                                                                    */
/*   char[UTGID_LEN+1]       utgid               environ                     */
/*   pid_t                   tid                 status                      */
/*   pid_t                   tgid                dirent, status              */
/*   unsigned long           flags               stat                        */
/*   int                     nlwp                status, stat                */
/*   uid_t                   euid                status                      */
/*   char[USER_LEN+1]        euser               getUserFromUid()            */
/*   int                     tty                 stat                        */
/*   char[TTY_LEN+1]         ttyS                dev2tty()                   */
/*   char[CMD_LEN+1]         cmd                 status, stat, cmdline       */
/* ------------------------------------------------------------------------- */
/* Ids:                                                                      */
/*   pid_t                   ppid                status, stat                */
/*   pid_t                   pgrp                stat                        */
/*   pid_t                   sid                 stat                        */
/*   pid_t                   tpgid               stat                        */
/* ------------------------------------------------------------------------- */
/* Owner:                                                                    */
/*   uid_t                   fuid                status                      */
/*   uid_t                   ruid                status                      */
/*   uid_t                   suid                status                      */
/*   gid_t                   egid                status                      */
/*   gid_t                   fgid                status                      */
/*   gid_t                   rgid                status                      */
/*   gid_t                   sgid                status                      */
/*   char[GROUP_LEN+1]       egroup              getGroupFromGid()           */
/*   char[USER_LEN+1]        fuser               getUserFromUid()            */
/*   char[GROUP_LEN+1]       fgroup              getGroupFromGid()           */
/*   char[USER_LEN+1]        ruser               getUserFromUid()            */
/*   char[GROUP_LEN+1]       rgroup              getGroupFromGid()           */
/*   char[USER_LEN+1]        suser               getUserFromUid()            */
/*   char[GROUP_LEN+1]       sgroup              getGroupFromGid()           */
/* ------------------------------------------------------------------------- */
/* Sched:                                                                    */
/*   unsigned long           sched               stat                        */
/*   long                    prio                stat                        */
/*   unsigned long           rtprio              stat                        */
/*   long                    nice                stat                        */
/*   char[SCH_LEN+1]         schedS              processData()               */
/*   char[PRIO_LEN+1]        prioS               processData()               */
/*   char[RTPRIO_LEN+1]      rtprioS             processData()               */
/*   char[NICE_LEN+1]        niceS               processData()               */
/* ------------------------------------------------------------------------- */
/* Stat:                                                                     */
/*   int                     processor           stat                        */
/*   char                    state[0]            status, stat                */
/*   char                    state[1,STAT_LEN+1] processData()               */
/*   float                   pcpu                getPcpu()                   */
/*   float                   pmem                getPmem()                   */
/*   unsigned long           min_flt             stat                        */
/*   unsigned long           maj_flt             stat                        */
/*   unsigned long           cmin_flt            stat                        */
/*   unsigned long           cmaj_flt            stat                        */
/*   float                   minFltRate          getFltRate()                */
/*   float                   majFltRate          getFltRate()                */
/* ------------------------------------------------------------------------- */
/* Size:                                                                     */
/*   unsigned long           vm_size             status, stat                */
/*   unsigned long           vm_lock             status                      */
/*   unsigned long           vm_rss              status, stat                */
/*   unsigned long           vm_data             status                      */
/*   unsigned long           vm_stack            status                      */
/*   unsigned long           vm_exe              status                      */
/*   unsigned long           vm_lib              status                      */
/*   long                    share               statm                       */
/* ------------------------------------------------------------------------- */
/* Signals:                                                                  */
/*   char[SIG_LEN+1]         sigpending          status                      */
/*   char[SIG_LEN+1]         sigblocked          status                      */
/*   char[SIG_LEN+1]         sigignored          status                      */
/*   char[SIG_LEN+1]         sigcatched          status                      */
/* ------------------------------------------------------------------------- */
/* Time:                                                                     */
/*   unsigned long long      utime               stat                        */
/*   unsigned long long      stime               stat                        */
/*   unsigned long long      cutime              stat                        */
/*   unsigned long long      cstime              stat                        */
/*   unsigned long long      st_time             stat                        */
/*   char[STARTTIME_LEN+1]   startTime           processData()               */
/*   char[ELAPSEDTIME_LEN+1] elapsedTime         processData()               */
/*   char[CPUTIME_LEN+1]     cpuTime             processData()               */
/* ------------------------------------------------------------------------- */
/* Cmdline:                                                                  */
/*   char[CMD_LINE_LEN+1]    cmdline       cmdline                           */
/* ######################################################################### */
#include <stdio.h>
#include <sys/types.h>                  /* opendir(3), readdir_r(3), open(2) */
#include <sys/stat.h>                                             /* open(2) */
#include <fcntl.h>                                                /* open(2) */
#include <dirent.h>                              /* opendir(3), readdir_r(3) */
#include <errno.h>
#include <string.h>                                           /* strerror(3) */
#include <stdlib.h>                                            /* strtoul(3) */
#include <unistd.h>                                  /* pathconf(3), read(2) */
#include <stddef.h>                                      /* offsetof() macro */
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>                                              /* PRIu64 */
#include <asm/param.h>                                                 /* HZ */
#include <sys/utsname.h>                         /* uname(2), struct utsname */
#include <pwd.h>                                               /* getpwuid() */
#include <grp.h>                                               /* getgrgid() */
#include <time.h>                                         /* time(), ctime() */
#include <sys/time.h>                      /* gettimeofday(), struct timeval */
#include <locale.h>                               /* LC_NUMERIC, setlocale() */
/*---------------------------------------------------------------------------*/
#include "fmcMsgUtils.h"       /* mPrintf(), L_DIM, L_STD, VERB, DEBUG, etc. */
#include "fmcMacro.h"                                            /* MIN(a,b) */
#include "fmcPar.h"                                             /* UTGID_LEN */
#include "getPsFromProc.h"         /* taskData, initTasks(), getTasks(), ... */
/* ######################################################################### */
/* macro to get tty name */
#define MAJOR_OF(d) (((unsigned)(d)>>8u) & 0xfffu)
#define MINOR_OF(d) (((unsigned)(d)&0xffu)|(((unsigned)(d)&0xfff00000u)>>12u))
#define LINUX_VERSION(x,y,z) (0x10000*(x) + 0x100*(y) + z)
/* macro to get user name */
#define	HASHSIZE 64                                            /* power of 2 */
#define	HASH(x) ((x) & (HASHSIZE-1))
/*---------------------------------------------------------------------------*/
/* This macro opens path only if necessary and seeks to 0 so that            */
/* successive calls to the functions are more efficient. It also reads the   */
/* current contents of the file into the global buf.                         */
#define FILE_TO_BUF(path,fd)                             \
do{                                                      \
  static int local_n;                                    \
  if(fd==-1 && (fd=open(path,O_RDONLY))==-1)             \
  {                                                      \
    mPrintf(errU,ERROR,__func__,0,"Cannot open file "    \
            "\"%s\": %s!",path,strerror(errno));         \
    fflush(NULL);                                        \
    _exit(102);                                          \
  }                                                      \
  lseek(fd,0L,SEEK_SET);                                 \
  if((local_n=read(fd,buf,sizeof(buf)-1))<0)             \
  {                                                      \
    mPrintf(errU,ERROR,__func__,0,"Cannot read file "    \
            "\"%s\": %s!",path,strerror(errno));         \
    fflush(NULL);                                        \
    _exit(1);                                            \
  }                                                      \
  buf[local_n]='\0';                                     \
}while(0)
/* ######################################################################### */
/* private types */
/* record of /proc/<PID>/status or /proc/<PID>/task/<TID>/status files */
typedef struct statusRecord
{
  const char *name;                                     /* status field name */
  char **value;                                        /* status field value */
}statusRecord_t;
/*---------------------------------------------------------------------------*/
typedef struct tty_map_node
{
  struct tty_map_node *next;
  unsigned major_number;
  unsigned minor_first, minor_last;
  char name[16];
  char devfs_type;
}tty_map_node_t;
/*---------------------------------------------------------------------------*/
/* user name buffer */
static struct pwbuf
{
  struct pwbuf *next;
  uid_t uid;
  char name[USER_LEN+1];
}*pwhash[HASHSIZE];
/*---------------------------------------------------------------------------*/
/* group name buffer */
static struct grpbuf
{
  struct grpbuf *next;
  gid_t gid;
  char name[GROUP_LEN+1];
}*grphash[HASHSIZE];
/*---------------------------------------------------------------------------*/
/* cpu jiffies used by processes/tasks */
/* cpu jiffy element */
typedef struct cpuTcsEl
{
  unsigned long long tics;
  pid_t id;
}cpuTcsEl_t;
/* cpu jiffy array */
typedef struct cpuTcsArr
{
  int n;
  int nAvail;
  cpuTcsEl_t *data;
}cpuTcsArr_t;
/*---------------------------------------------------------------------------*/
/* minor/major page faults of processes/tasks */
/* minor/major page fault element */
typedef struct fltEl
{
  unsigned long min;
  unsigned long maj;
  pid_t id;
}fltEl_t;
/* minor/major page fault array */
typedef struct fltArr
{
  int n;
  int nAvail;
  fltEl_t *data;
}fltArr_t;
/* ######################################################################### */
/* private functions */
/*---------------------------------------------------------------------------*/
/* utilities */
static void *realloc1(void *oldp,size_t size);
static void *malloc1(size_t size);
/* read proc files */
static int file2str(const char *path,char *buf,int bufSz);
static char **file2strvec(const char* path);
/*---------------------------------------------------------------------------*/
/* read task data from /proc/<pid>/status content */
static void status2proc(char *sbuf,taskData_t *tsk, int isProc);
/* read task data from /proc/<pid>/stat content */
static void stat2proc(const char *sbuf, taskData_t *tsk);
/* read task data from /proc/<pid>/statm content */
static void statm2proc(const char *sbuf, taskData_t *tsk);
/* read task data from /proc/<pid>/cmdline content */
static void cmdline2proc(char **cmdLine, taskData_t *tsk);
/* elaborate data */
static void processData(taskData_t *tsk);
/*---------------------------------------------------------------------------*/
/* comparing function used by bsearch(3) in  status2proc() */
static int statusRecordCmp(const void *a, const void *b);
/* comparing function used by bsearch(3) in  getPcpu() */
static int tcsElCmp(const cpuTcsEl_t *x,const cpuTcsEl_t *y);
/* comparing function used by bsearch(3) in  getFltRate() */
static int fltElCmp(const fltEl_t *x,const fltEl_t *y);
/*---------------------------------------------------------------------------*/
/* get tty name */
static void initLinuxVersion(void);
static void load_drivers(void);
unsigned dev2tty(char *ttyS,unsigned ttySlen,dev_t devN,int pid);
static int getTtyFromNum(char*const buf,unsigned maj,unsigned min);
static int getTtyFromStderr(char*const buf,unsigned maj,unsigned min,int pid,
                            const char *name);
static int getTtyFromProcTtyDriver(char*const buf,unsigned maj,unsigned min);
/*---------------------------------------------------------------------------*/
/* get user/group name */
char *getUserFromUid(uid_t uid);
char *getGroupFromGid(gid_t gid);
/*---------------------------------------------------------------------------*/
time_t getUptime(void);
void saveFrameTime(void);
/*---------------------------------------------------------------------------*/
void saveCpuTics(void);
void getPcpu(cpuTcsArr_t *oldTcs,cpuTcsArr_t *newTcs,taskData_t *tsk,pid_t id,
             int n,char *type);
/*---------------------------------------------------------------------------*/
void getFltRate(fltArr_t *oldFlt,fltArr_t *newFlt,taskData_t *tsk,pid_t id,
                int n,char *type);
/*---------------------------------------------------------------------------*/
void getMemTotal(void);
void getPmem(taskData_t *tsk);
/*---------------------------------------------------------------------------*/
taskData_t *newTask(tasksData_t *tsks);
void cleanTasks(tasksData_t *tsks);
void freeTasks(tasksData_t *tsks);
/* ######################################################################### */
/* external global variables */
/* debug mask */
extern int deBug;                                             /* debug level */
extern int errU;                                               /* error unit */
/* ######################################################################### */
/* global variables */
/* limits */
static size_t procNameMax;                   /* Max Name length in /proc dir */
static size_t procPathMax;              /* Max Path length among /proc/<PID> */
static int maxPid=0;
static int maxPidChars=0;
static long pageSize=0;
/* paths */
static char *psPath=NULL;
static char *pCmdPath=NULL;
static char *pEnvPath=NULL;
static char *pStatPath=NULL;
static char *pStatmPath=NULL;
static char *pStatusPath=NULL;
/* readdir_r entry */
static int procEntrySz;
static struct dirent *procEntry=NULL;
/* %MEM */
static unsigned long memTotal;
/* %CPU */
static unsigned long long Hertz;                       /* jiffies per second */
static cpuTcsArr_t newPtcs;                 /* current process jiffies array */
static cpuTcsArr_t oldPtcs;                /* previous process jiffies array */
static float refreshTime;
/* minFltRate, majFltRate */
static fltArr_t newPflt;                 /* current process page fault array */
static fltArr_t oldPflt;                /* previous process page fault array */
/* times */
static time_t secondsSinceBoot=-1;
static time_t timeOfBoot=-1;
/* tty */
static tty_map_node_t *ttyMap=NULL;
int linuxVersionCode;
/* version */
static char rcsid[]="$Id: getPsFromProc.c,v 2.19 2009/01/30 14:45:18 galli Exp $";
/*---------------------------------------------------------------------------*/
/* major 204 is a mess -- "Low-density serial ports" */
static const char low_density_names[][4] = {
"LU0",  "LU1",  "LU2",  "LU3",
"FB0",
"SA0",  "SA1",  "SA2",
"SC0",  "SC1",  "SC2",  "SC3",
"FW0",  "FW1",  "FW2",  "FW3",
"AM0",  "AM1",  "AM2",  "AM3",  "AM4",  "AM5",  "AM6",  "AM7",
"AM8",  "AM9",  "AM10", "AM11", "AM12", "AM13", "AM14", "AM15",
"DB0",  "DB1",  "DB2",  "DB3",  "DB4",  "DB5",  "DB6",  "DB7",
"SG0",
"SMX0",  "SMX1",  "SMX2",
"MM0",  "MM1",
};
/* ######################################################################### */
/* Functions operating on tasksData_t structures                             */
/* ######################################################################### */
int initTasks(tasksData_t *prcs)
{
  printInFunc;
  /*-------------------------------------------------------------------------*/
  /* get maximum PID in running kernel */
  maxPid=getMaxPid();
  maxPidChars=snprintf(NULL,0,"%d",maxPid);
  /*-------------------------------------------------------------------------*/
  pageSize=sysconf(_SC_PAGE_SIZE);
  /*-------------------------------------------------------------------------*/
  /* get procNameMax =  maximum number of characters in a /proc filename */
  procNameMax=pathconf(PROC_PATH,_PC_NAME_MAX);
  if(procNameMax==-1)
  {
    if(!errno)procNameMax=256;                                   /* no limit */
    else
    {
      mPrintf(errU,FATAL,__func__,0,"Unable to get parameter NAME_MAX: %s!",
              strerror(errno));
      printOutFuncNOK;
      return 1;
    }
  }
  /* get procPathMax = max number of chars in string "/proc/<PID>"           */
  procPathMax=strlen("/proc/")+maxPidChars;
  /*-------------------------------------------------------------------------*/
  initLinuxVersion();
  getMemTotal();
  load_drivers();
  /*-------------------------------------------------------------------------*/
  /* get size of procEntry                                                   */
  /* "The storage pointed to by entry shall be large enough for a dirent     */
  /*  with an array of char d_name members containing at least {NAME_MAX}+1  */
  /*  elements."                                                             */
  procEntrySz=offsetof(struct dirent,d_name)+procNameMax+strlen("/proc")+1;
  /*-------------------------------------------------------------------------*/
  /* allocate space for procEntry, used by readdir_r */
  procEntry=(struct dirent*)malloc(procEntrySz);
  if(!procEntry)
  {
    mPrintf(errU,FATAL,__func__,0,"Error allocating memory for dirent: %s!",
            strerror(errno));
    printOutFuncNOK;
    return 1;
  }
  /*-------------------------------------------------------------------------*/
  /* define Hertz */
  Hertz=(unsigned long long)HZ;
  /*-------------------------------------------------------------------------*/
  /* allocate space for paths */
  psPath=(char*)malloc(procPathMax+1);
  pCmdPath=(char*)malloc(procPathMax+strlen("/cmdline")+1);
  pEnvPath=(char*)malloc(procPathMax+strlen("/environ")+1);
  pStatPath=(char*)malloc(procPathMax+strlen("/stat")+1);
  pStatmPath=(char*)malloc(procPathMax+strlen("/statm")+1);
  pStatusPath=(char*)malloc(procPathMax+strlen("/status")+1);
  /*-------------------------------------------------------------------------*/
  /* initialize prcs structure */
  prcs->n=0;
  prcs->nAvail=0;
  prcs->data=NULL;
  /*-------------------------------------------------------------------------*/
  /* initialize Ptcs/Ttcs structure */
  newPtcs.n=0;
  newPtcs.nAvail=0;
  newPtcs.data=NULL;
  oldPtcs.n=0;
  oldPtcs.nAvail=0;
  oldPtcs.data=NULL;
  /*-------------------------------------------------------------------------*/
  /* initialize Pflt/Tflt structure */
  newPflt.n=0;
  newPflt.nAvail=0;
  newPflt.data=NULL;
  oldPflt.n=0;
  oldPflt.nAvail=0;
  oldPflt.data=NULL;
  /*-------------------------------------------------------------------------*/
  printOutFuncOK;
  return 0;
}
/*****************************************************************************/
static int refreshCnt=0;
/*****************************************************************************/
int getTasks(tasksData_t *prcs)
{
  taskData_t *prc=NULL;
  DIR *procDir=NULL;
  int pdStatus;
  struct dirent *pdResult;
  pid_t tgid;
  int nP,nT;
  static char sbuf[4096];               /* buffer for status, stat and statm */
  char **cmdLine;
  int wch;
  char **procEnv;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  /* increase frame counter */
  refreshCnt++;
  /* save time and CPU tic counters */
  saveCpuTics();
  saveFrameTime();
  /*-------------------------------------------------------------------------*/
  cleanTasks(prcs);
  /*-------------------------------------------------------------------------*/
  procDir=opendir(PROC_PATH);
  if(!procDir)
  {
    mPrintf(errU,ERROR,__func__,0,"Unable to open directory \"%s\": error "
            "%d (%s)!",PROC_PATH,errno,strerror(errno));
    printOutFuncNOK;
    return 1;
  }
  /*-------------------------------------------------------------------------*/
  for(nP=0,nT=0;nT<=maxPid;)    /* loop over procs; hopefully exits at break */
  {
    char **pp;
    /*.......................................................................*/
    /* int pdStatus = 0 (OK), else pdStatus = errno */
    /* pdResult = NULL at end of dir, else pdResult points to procEntry */
    pdStatus=readdir_r(procDir,procEntry,&pdResult);  /* get directory entry */
    if(unlikely(pdStatus))                                          /* Error */
    {
      mPrintf(errU,ERROR,__func__,0,"Unable to read directory \"%s\": error "
              "%d (%s)!",PROC_PATH,pdStatus,strerror(pdStatus));
      break;
    }
    if(unlikely(!pdResult))                              /* End of directory */
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
                unlikely(*procEntry->d_name>'9')))    /* not a process entry */
    {
      continue;
    }
    /*.......................................................................*/
    /* initialize prc, creating room if needed */
    prc=newTask(prcs);
    nP++;
    /*.......................................................................*/
    /* get tgid */
    tgid=strtoul(procEntry->d_name,NULL,10);
    prc->tgid=tgid;
    /*.......................................................................*/
    sprintf(psPath,"/proc/%s",procEntry->d_name);
    /*.......................................................................*/
    /* compose task file paths */
    sprintf(pStatPath,"%s/%s",psPath,"stat");
    sprintf(pStatmPath,"%s/%s",psPath,"statm");
    sprintf(pStatusPath,"%s/%s",psPath,"status");
    sprintf(pCmdPath,"%s/%s",psPath,"cmdline");
    sprintf(pEnvPath,"%s/%s",psPath,"environ");
    /*.......................................................................*/
    /* read task data from /proc/<pid>/status content                        */
    if(likely(file2str(pStatusPath,sbuf,sizeof(sbuf))!=-1))
    {
      status2proc(sbuf,prc,1);
    }
    /* ..................................................................... */
    /* read task data from /proc/<pid>/stat content                          */
    if(likely(file2str(pStatPath,sbuf,sizeof(sbuf))!=-1))
    {
      stat2proc(sbuf,prc);
    }
    /* ..................................................................... */
    /* read task data from /proc/<pid>/statm content                         */
    if(likely(file2str(pStatmPath,sbuf,sizeof(sbuf))!=-1))
    {
      statm2proc(sbuf,prc);
    }
    /* ..................................................................... */
    /* read task data from /proc/<pid>/cmdline content                       */
    cmdLine=NULL;
    cmdLine=file2strvec(pCmdPath);
    cmdline2proc(cmdLine,prc);
    /* ..................................................................... */
    /* get UTGID and /proc/<pid>/environ size */
    procEnv=file2strvec(pEnvPath);
    strcpy(prc->utgid,"N/A");
    prc->envSz=0;
    if(likely(procEnv))
    {
      int utgidFound=0;
      for(pp=procEnv;*pp;pp++)
      {
        prc->envSz+=1+strlen(*pp);
        if(likely(!utgidFound))
        {
          if(unlikely(!strncmp(*pp,"UTGID=",6)))
          {
            wch=snprintf(prc->utgid,UTGID_LEN+1,"%s",1+strchr(*pp,'='));
            if(unlikely(wch>UTGID_LEN))
            {
              mPrintf(errU,ERROR,__func__,0,"UTGID truncated (\"%s\" -> "
                      "\"%s\"). Increase UTGID_LEN variable in fmcPar.h!",
                      1+strchr(*pp,'='),prc->utgid);
            }
          utgidFound=1;
          }
        }
      }
      prc->envSz--;
    }
    /* ..................................................................... */
    /* get %CPU, %MEM and fltRate */
    getPcpu(&oldPtcs,&newPtcs,prc,prc->tgid,nP,"process");
    getPmem(prc);
    getFltRate(&oldPflt,&newPflt,prc,prc->tgid,nP,"process");
    /* ..................................................................... */
    /* elaborate data                                                        */
    processData(prc);
    /* ..................................................................... */
    /* free memory allocated by file2strvec() */
    if(likely(cmdLine))free((void*)*cmdLine);
    if(likely(procEnv))free((void*)*procEnv);
    /* ..................................................................... */
  }                                                   /* loop over processes */
  closedir(procDir);
  /*-------------------------------------------------------------------------*/
  if(nT>=maxPid)
  {
    mPrintf(errU|L_SYS,ERROR,__func__,9,"Loop over TGIDs: Maximum number of "
            "processes (%d) reached! The current running Linux KERNEL is "
            "apparently BROKEN in the procfs readdir() wrapper: "
            "proc_pid_readdir() in source file "
            "\"linux-2.6.9/fs/proc/base.c\"! To fix the problem install "
            "a Linux SLC Kernel >= 2.6.9-55 or apply the Kernel patch "
            "\"linux-2.6.9-proc-readdir.patch\"!",maxPid);
  }
  if(deBug&0x004)
  {
    mPrintf(errU,DEBUG,__func__,9,"%d process scanned; %d task scanned.",nP,
            nT);
  }
  /*-------------------------------------------------------------------------*/
  printOutFuncOK;
  return 0;
}
/*****************************************************************************/
/* initialize tsks, creating room if needed */
taskData_t *newTask(tasksData_t *tsks)
{
  int oldNAvail;
  /*-------------------------------------------------------------------------*/
  (tsks->n)++;
  if(tsks->n>=tsks->nAvail)
  {
    /* increase allocated memory */
    oldNAvail=tsks->nAvail;
    tsks->nAvail=tsks->nAvail*5/4+10;
    tsks->data=(taskData_t*)realloc(tsks->data,
                                    (tsks->nAvail)*sizeof(taskData_t));
    /* clean new memory locations */
    memset(tsks->data+oldNAvail,0,tsks->nAvail-oldNAvail);
    if(unlikely(deBug&0x004))
    {
      mPrintf(errU,DEBUG,__func__,0,"More room allocated for proc data "
              "storage: %d -> %d.",oldNAvail,tsks->nAvail);
    }
  }
# ifdef DEBUG
  if(unlikely(deBug&0x004))
  {
    mPrintf(errU,VERB,__func__,9,"Proc %d. Room for %d procs.",tsks->n,
            tsks->nAvail);
  }
# endif
  /* clean data */
  memset(&(tsks->data[tsks->n-1]),0,sizeof(taskData_t));
  /* state initialized to "unknown" */
  tsks->data[tsks->n-1].state[0]='U';
  /* default values for older kernels */
  tsks->data[tsks->n-1].processor=0;
  tsks->data[tsks->n-1].rtprio=-1;
  tsks->data[tsks->n-1].sched=-1;
  tsks->data[tsks->n-1].nlwp=0;
  strncpy(tsks->data[tsks->n-1].schedS,"N/A",
          sizeof(tsks->data[tsks->n-1].schedS)-1);
  strncpy(tsks->data[tsks->n-1].rtprioS,"N/A",
          sizeof(tsks->data[tsks->n-1].rtprioS)-1);
  return &(tsks->data[tsks->n-1]);
}
/*****************************************************************************/
taskData_t *taskAt(tasksData_t *tsks,int i)
{
  if(i > tsks->n-1)return NULL;
  return &(tsks->data[i]);
}
/*****************************************************************************/
void cleanTasks(tasksData_t *tsks)
{
  int i;
  /*-------------------------------------------------------------------------*/
  tsks->n=0;
  for(i=0;i<tsks->nAvail;i++)
  {
    memset(&(tsks->data[i]),0,sizeof(taskData_t));
  }
}
/*****************************************************************************/
void freeTasks(tasksData_t *tsks)
{
  tsks->n=0;
  tsks->nAvail=0;
  free(tsks->data);
}
/* ######################################################################### */
/* Functions to parse the procfs                                             */
/* ######################################################################### */
/* read task data from /proc/<pid>/status content                            */
/* Data read: cmd, ruid, euid, suid, fuid, rgid, egid, sgid, fgid, nlwp,     */
/* tid, tgid, ppid, state[0], sigpending, sigblocked, sigignored,            */
/* sigcatched, vm_data, vm_exe, vm_lock, vm_lib, vm_rss, vm_size, vm_stack.  */
static void status2proc(char *sbuf, taskData_t *tsk, int isProc)
{
  char *nameHead;
  char *nameTail;
  char *valueHead;
  char *valueTail;
  char keyName[16];                     /* big enough to hold any field name */
  statusRecord_t key={keyName,NULL};
  statusRecord_t *found;
  /*pointers to found raw values */
  static char *gidVal;
  static char *nameVal;
  static char *pidVal;
  static char *ppidVal;
  static char *shdPndVal;
  static char *sigBlkVal;
  static char *sigCgtVal;
  static char *sigIgnVal;
  static char *sigPndVal;
  static char *stateVal;
  static char *tgidVal;
  static char *threadsVal;
  static char *uidVal;
  static char *vmDataVal;
  static char *vmExeVal;
  static char *vmLckVal;
  static char *vmLibVal;
  static char *vmRssVal;
  static char *vmSizeVal;
  static char *vmStkVal;
  /* strings in statusTbl must be alphabetically sorted */
  static const statusRecord_t statusTbl[]=
  {
    {"Gid",    &gidVal},
    {"Name",   &nameVal},
    {"Pid",    &pidVal},
    {"Ppid",   &ppidVal},
    {"ShdPnd", &shdPndVal},
    {"SigBlk", &sigBlkVal},
    {"SigCgt", &sigCgtVal},
    {"SigIgn", &sigIgnVal},
    {"SigPnd", &sigPndVal},
    {"State",  &stateVal},
    {"Tgid",   &tgidVal},
    {"Threads",&threadsVal},
    {"Uid",    &uidVal},
    {"VmData", &vmDataVal},
    {"VmExe",  &vmExeVal},
    {"VmLck",  &vmLckVal},
    {"VmLib",  &vmLibVal},
    {"VmRSS",  &vmRssVal},
    {"VmSize", &vmSizeVal},
    {"VmStk",  &vmStkVal}
  };
  const int statusTblRawN=sizeof(statusTbl)/sizeof(statusRecord_t);
  /*-------------------------------------------------------------------------*/
  /* initialize static pointers */
  nameVal=NULL;
  uidVal=gidVal=NULL;
  pidVal=ppidVal=tgidVal=threadsVal=stateVal=NULL;
  shdPndVal=sigBlkVal=sigCgtVal=sigIgnVal=sigPndVal=NULL;
  vmDataVal=vmExeVal=vmLckVal=vmLibVal=vmRssVal=vmSizeVal=vmStkVal=NULL;
  /*-------------------------------------------------------------------------*/
  /* read raw values from sbuf */
  nameHead=sbuf;
  for(;;)                                          /* loop over file records */
  {
    nameTail=strchr(nameHead,':');
    if(unlikely(!nameTail))break;
    *nameTail='\0';
    if(unlikely(strlen(nameHead)>=sizeof(keyName)))
    {
      /* go to the next line */
      valueHead=nameTail+1;
      valueTail=strchr(valueHead,'\n');
      if(unlikely(!valueTail))break;
      nameHead=valueTail+1;
      continue;
    }
    strcpy(keyName,nameHead);
    /* search the record "key" in the array "statusTbl" */
    found=bsearch(&key,statusTbl,statusTblRawN,sizeof(statusRecord_t),
                  statusRecordCmp);
    /* found = record of "statusTbl" matching "key" */
    valueHead=nameTail+1;
    if(!found)
    {
      /* go to the next line */
      valueTail=strchr(valueHead,'\n');
      if(unlikely(!valueTail))break;
      nameHead=valueTail+1;
      continue;
    }
    *(found->value)=valueHead;
    valueTail=strchr(valueHead,'\n');
    if(unlikely(!valueTail))break;
    *valueTail='\0';
    nameHead=valueTail+1;
  }                                                /* loop over file records */
  /*-------------------------------------------------------------------------*/
  /* strip and convert values */
  /*.........................................................................*/
  /* cmd */
  if(likely(nameVal))
  {
    int i;
    char c;
    char *p;
    nameVal+=strspn(nameVal," \t");                   /* skip heading blanks */
    for(i=0,p=nameVal;i<sizeof(tsk->cmd)-1;)      /* loop over nameVal chars */
    {
      c=*p++;
      if(unlikely(c=='\n')) break;
      if(unlikely(c=='\0')) break;                    /* should never happen */
      if(unlikely(c=='\\'))
      {
        c=*p++;
        if(c=='\n')break;                             /* should never happen */
        if(!c)break;                                  /* should never happen */
        if(c=='n')c='\n';                            /* we assume it is '\n' */
      }
      tsk->cmd[i++]=c;
    }                                             /* loop over nameVal chars */
    tsk->cmd[i]='\0';
  }                                                   /* if(likely(nameVal)) */
  /*.........................................................................*/
  /* uid, gid */
  if(likely(uidVal))
  {
    tsk->ruid=strtoul(uidVal,&uidVal,10);
    tsk->euid=strtoul(uidVal,&uidVal,10);
    tsk->suid=strtoul(uidVal,&uidVal,10);
    tsk->fuid=strtoul(uidVal,&uidVal,10);
  }
  if(likely(gidVal))
  {
    tsk->rgid=strtoul(gidVal,&gidVal,10);
    tsk->egid=strtoul(gidVal,&gidVal,10);
    tsk->sgid=strtoul(gidVal,&gidVal,10);
    tsk->fgid=strtoul(gidVal,&gidVal,10);
  }
  /*.........................................................................*/
  /* tgid, tid, ppid, nlwp, state */
  /* Linux 2.4.13-pre1 to max 2.4.xx have a useless "Tgid" that is not       */
  /* initialized for built-in kernel tasks. Only 2.6.0 and above have        */
  /* "Threads" (nlwp) info.                                                  */
  if(likely(threadsVal))tsk->nlwp=strtoul(threadsVal,(char**)NULL,10);
  if(tsk->nlwp)
  {
    pid_t tgid;
    tsk->tid=strtoul(pidVal,(char**)NULL,10);
    tgid=strtoul(tgidVal,(char**)NULL,10);
    if(unlikely(unlikely(deBug&0x008) && unlikely(tsk->tgid!=tgid)))
    {
      mPrintf(errU,ERROR,__func__,0,"Incoherence in tgid: %u from dirent, "
              "%u from /proc/pid/status!",tsk->tgid,tgid);
    }
  }
  else
  {
    pid_t tgid;
    tsk->nlwp=1;
    tsk->tid=strtoul(pidVal,(char**)NULL,10);
    tgid=tsk->tid;
    if(unlikely(unlikely(deBug&0x008) && unlikely(tsk->tgid!=tgid)))
    {
      mPrintf(errU,ERROR,__func__,0,"Incoherence in tid: %u from dirent, "
              "%u from /proc/pid/status!",tsk->tgid,tgid);
    }
  }
  if(likely(ppidVal))tsk->ppid=strtoul(ppidVal,(char**)NULL,10);
  if(likely(stateVal))
  {
    stateVal+=strspn(stateVal," \t");                 /* skip heading blanks */
    tsk->state[0]=stateVal[0];
  }
  /*.........................................................................*/
  /* signals */
  if(likely(shdPndVal))
  {
    shdPndVal+=strspn(shdPndVal," \t");               /* skip heading blanks */
    memcpy(tsk->sigpending,shdPndVal,16);
    tsk->sigpending[16]='\0';
    /* recent kernels supply per-tgid pending signals */
    if((!isProc || !tsk->sigpending[0]) && likely(sigPndVal))
    {
      sigPndVal+=strspn(sigPndVal," \t");             /* skip heading blanks */
      memcpy(tsk->sigpending,sigPndVal,16);
      tsk->sigpending[16]='\0';
    }
  }
  if(likely(sigBlkVal))
  {
    sigBlkVal+=strspn(sigBlkVal," \t");               /* skip heading blanks */
    memcpy(tsk->sigblocked,sigBlkVal,16);
    tsk->sigblocked[16]='\0';
  }
  if(likely(sigIgnVal))
  {
    sigIgnVal+=strspn(sigIgnVal," \t");               /* skip heading blanks */
    memcpy(tsk->sigignored,sigIgnVal,16);
    tsk->sigignored[16]='\0';
  }
  if(likely(sigCgtVal))
  {
    sigCgtVal+=strspn(sigCgtVal," \t");               /* skip heading blanks */
    memcpy(tsk->sigcatched,sigCgtVal,16);
    tsk->sigcatched[16]='\0';
  }
  /*.........................................................................*/
  /* memory */
  if(likely(vmDataVal))tsk->vm_data=strtoul(vmDataVal,(char**)NULL,10);
  if(likely(vmExeVal)) tsk->vm_exe=strtoul(vmExeVal,(char**)NULL,10);
  if(likely(vmLckVal)) tsk->vm_lock=strtoul(vmLckVal,(char**)NULL,10);
  if(likely(vmLibVal)) tsk->vm_lib=strtoul(vmLibVal,(char**)NULL,10);
  if(likely(vmRssVal)) tsk->vm_rss=strtoul(vmRssVal,(char**)NULL,10);
  if(likely(vmSizeVal))tsk->vm_size=strtoul(vmSizeVal,(char**)NULL,10);
  if(likely(vmStkVal)) tsk->vm_stack=strtoul(vmStkVal,(char**)NULL,10);
  /*-------------------------------------------------------------------------*/
}
/*****************************************************************************/
/* comparing function used by bsearch(3) in  status2proc() */
static int statusRecordCmp(const void *a, const void *b)
{
  return strcmp(((const statusRecord_t*)a)->name,
                ((const statusRecord_t*)b)->name);
}
/*****************************************************************************/
/* read task data from /proc/<pid>/stat content                              */
static void stat2proc(const char *sbuf, taskData_t *tsk)
{
  unsigned cmdSz;
  char* tmp;
  int rv;
  char cmd[CMD_LEN+1];
  pid_t ppid;
  char state;
  unsigned long long utime,stime,cutime,cstime,st_time;
  unsigned long start_code,end_code,start_stack,kstk_esp,kstk_eip,wchan;
  long prio,nice,vm_rss,alarm;
  unsigned long rtprio,sched,vm_size,rss_rlim,flags,min_flt,maj_flt,cmin_flt,
                cmaj_flt;
  pid_t pgrp,sid,tpgid;
  int nlwp,tty,processor,exit_signal;
  /*-------------------------------------------------------------------------*/
  /* cmd */
  sbuf=strchr(sbuf,'(')+1;
  tmp=strrchr(sbuf,')');
  /* get cmd */
  cmdSz=MIN(tmp-sbuf,sizeof(cmd)-1);
  memcpy(cmd,sbuf,cmdSz);
  cmd[cmdSz]='\0';
  if(tsk->cmd[0]=='\0' && cmd[0]!='\0')
  {
    memcpy(tsk->cmd,cmd,sizeof(cmd));
  }
  else if(unlikely(unlikely(deBug&0x008) && unlikely(strcmp(tsk->cmd,cmd))))
  {
    mPrintf(errU,ERROR,__func__,0,"Incoherencea in CMD of process TGID=%d: "
            "\"%s\" previous value, \"%s\" from /proc/pid/stat!",tsk->tgid,
            tsk->cmd,cmd);
  }
  /*-------------------------------------------------------------------------*/
  sbuf=tmp+2;                                                   /* skip ") " */
  rv=sscanf(sbuf,
    "%c "                                                           /* state */
    "%d %d %d %d %d "                             /* ppid pgrp sid tty tpgid */
    "%lu %lu %lu %lu %lu "        /* flags min_flt cmin_flt maj_flt cmaj_flt */
    "%Lu %Lu %Lu %Lu "                          /* utime stime cutime cstime */
    "%ld %ld "                                                  /* prio nice */
    "%d "                                                            /* nlwp */
    "%ld "                                                          /* alarm */
    "%Lu "                                                        /* st_time */
    "%lu "                                                        /* vm_size */
    "%ld "                                                         /* vm_rss */
    "%lu %lu %lu %lu %lu %lu "  /* rss_rlim start_code end_code ... kstk_eip */
    "%*s %*s %*s %*s "        /* discard, no RT signals & Linux 2.1 used hex */
    "%lu %*s %*s "   /* wchan. Discard nswap, cnswap, dead for 2.4.xx and up */
    "%d %d "                                        /* exit_signal processor */
    "%lu %lu",                                               /* rtprio sched */
    &state,
    &ppid, &pgrp, &sid, &tty, &tpgid,
    &flags, &min_flt, &cmin_flt, &maj_flt, &cmaj_flt,
    &utime, &stime, &cutime, &cstime,
    &prio, &nice,
    &nlwp,
    &alarm,
    &st_time,
    &vm_size,
    &vm_rss,
    &rss_rlim, &start_code, &end_code, &start_stack, &kstk_esp, &kstk_eip,
    &wchan, 
    /* -- Linux 2.0.35 ends here -- */
    &exit_signal, &processor,  /* 2.2.1 ends with "exit_signal" */
    /* -- Linux 2.2.8 to 2.5.17 end here -- */
    &rtprio, &sched  /* both added to 2.5.18 */
  );
  /*-------------------------------------------------------------------------*/
  if(!nlwp)nlwp=1;
  /*-------------------------------------------------------------------------*/
  /* state */
  if(tsk->state[0]=='U' && state!='U')
  {
    tsk->state[0]=state;
  }
  else if(unlikely(unlikely(deBug&0x008) && unlikely(tsk->state[0]!=state)))
  {
    mPrintf(errU,ERROR,__func__,0,"Incoherencea in STAT of process TGID=%d: "
            "'%c' previous value, '%c' from /proc/pid/stat!",tsk->tgid,
            tsk->state[0],state);
  }
  /*.........................................................................*/
  /* ppid pgrp sid tty tpgid */
  if(!tsk->ppid && ppid)
  {
    tsk->ppid=ppid;
  }
  else if(unlikely(unlikely(deBug&0x008) && unlikely(tsk->ppid!=ppid)))
  {
    mPrintf(errU,ERROR,__func__,0,"Incoherence in PPID of process TGID=%d: "
            "%d previous value, %d from /proc/pid/stat!",tsk->tgid,
            tsk->ppid,ppid);
  }
  tsk->pgrp=pgrp;
  tsk->sid=sid;
  tsk->tpgid=tpgid;
  tsk->tty=tty;
  /*.........................................................................*/
  /* flags min_flt cmin_flt maj_flt cmaj_flt */
  tsk->flags=flags;
  tsk->min_flt=min_flt;
  tsk->maj_flt=maj_flt;
  tsk->cmin_flt=cmin_flt;
  tsk->cmaj_flt=cmaj_flt;
  /*.........................................................................*/
  /* utime stime cutime cstime */
  tsk->utime=utime;
  tsk->stime=stime;
  tsk->cutime=cutime;
  tsk->cstime=cstime;
  /*.........................................................................*/
  /* prio nice */
  tsk->prio=prio+1;              /* prio in [-99, 40], prio<0 for RT process */
  tsk->nice=nice;
  /*.........................................................................*/
  /* nlwp */
  if(!tsk->nlwp && nlwp)
  {
    tsk->nlwp=nlwp;
  }
  else if(unlikely(unlikely(deBug&0x008) && unlikely(tsk->nlwp!=nlwp)))
  {
    mPrintf(errU,ERROR,__func__,0,"Incoherence in NLWP of process TGID=%d: "
            "%d previous value, %d from /proc/pid/stat!",tsk->tgid,
            tsk->nlwp,nlwp);
  }
  /*.........................................................................*/
  /* st_time */
  tsk->st_time=st_time;
  /*.........................................................................*/
  /* vm_size, vm_rss */
  if(!tsk->vm_size && vm_size)
  {
    tsk->vm_size=vm_size/1024;                                   /* B -> KiB */
  }
  else if(unlikely(unlikely(deBug&0x008) &&
          unlikely(tsk->vm_size!=vm_size/1024)))
  {
    mPrintf(errU,ERROR,__func__,0,"Incoherence in VSIZE of process TGID=%d: "
            "%d previous value, %d from /proc/pid/stat!",tsk->tgid,
            tsk->vm_size,vm_size/1024);
  }
  if(!tsk->vm_rss && vm_rss)
  {
    tsk->vm_rss=vm_rss*pageSize/1024;                       /* pages -> KiB */
  }
  else if(unlikely(unlikely(deBug&0x008) &&
          unlikely(tsk->vm_rss!=vm_rss*pageSize/1024)))
  {
    mPrintf(errU,ERROR,__func__,0,"Incoherence in RSS of process TGID=%d: "
            "%d previous value, %d from /proc/pid/stat!",tsk->tgid,
            tsk->vm_rss,vm_rss*pageSize/1024);
  }
  /*.........................................................................*/
  /* exit_signal processor */
  tsk->processor=processor;
  /*.........................................................................*/
  /* rtprio sched */
  tsk->rtprio=rtprio;
  tsk->sched=sched;
  /*-------------------------------------------------------------------------*/
}
/*****************************************************************************/
/* read task data from /proc/<pid>/statm content                             */
static void statm2proc(const char *sbuf, taskData_t *tsk)
{
  int rv;
  long size,resident,share,trs,lrs,drs,dt;
  /*-------------------------------------------------------------------------*/
  rv=sscanf(sbuf,"%ld %ld %ld %ld %ld %ld %ld",&size,&resident,&share,&trs,
            &lrs,&drs,&dt);
  /*-------------------------------------------------------------------------*/
  tsk->share=share*pageSize/1024;                            /* pages -> KiB */
  /*-------------------------------------------------------------------------*/
}
/*****************************************************************************/
/* read task data from /proc/<pid>/cmdline content                           */
static void cmdline2proc(char **cmdLine, taskData_t *tsk)
{
  char **s,*d;
  char *p;
  /*-------------------------------------------------------------------------*/
  if(likely(likely(cmdLine) && likely(*cmdLine)))           /* argv[0] found */
  {
    /* fill tsk->cmdline */
    for(s=cmdLine,d=tsk->cmdline;*s;s++)              /* loop over arguments */
    {
      if(s!=cmdLine)if(d<tsk->cmdline+sizeof(tsk->cmdline)-1)*d++=' ';
      for(p=*s;                             /* loop over argument characters */
          *p && d<tsk->cmdline+sizeof(tsk->cmdline)-1;
          p++,d++)
      {
        *d=*p;
      }                                     /* loop over argument characters */
      *d='\0';
    }                                                 /* loop over arguments */
  }                                                         /* argv[0] found */
  else                        /* argv[0] not found (kernel thread or zombie) */
  {
    /* fill tsk->cmdline by copying from tsk->cmd */
    for(p=tsk->cmd,d=tsk->cmdline;
        *p && d<tsk->cmdline+sizeof(tsk->cmdline)-1;
        p++,d++)
    {
      *d=*p;
    }
    /* append attribute to tsk->cmdline */
    if(tsk->state[0]=='Z')                                         /* zombie */
    {
      for(p=" [zombie]";
          *p && d<tsk->cmdline+sizeof(tsk->cmdline)-1;
          p++,d++)
      {
        *d=*p;
      }
    }                                                              /* zombie */
    else                                                    /* kernel thread */
    {
      for(p=" [kernel thread]";
          *p && d<tsk->cmdline+sizeof(tsk->cmdline)-1;
          p++,d++)
      {
        *d=*p;
      }
    }                                                       /* kernel thread */
  }                           /* argv[0] not found (kernel thread or zombie) */
  /*-------------------------------------------------------------------------*/
}
/* ######################################################################### */
/* Functions to elaborate procfs data                                        */
/* ######################################################################### */
static void processData(taskData_t *tsk)
{
  int i;
  time_t tm;
  unsigned dd,hh,mm,ss;
  /*-------------------------------------------------------------------------*/
  secondsSinceBoot=getUptime();
  timeOfBoot=time(NULL)-secondsSinceBoot;
  /*-------------------------------------------------------------------------*/
  /* multi-character process state                                           */
  /* This state display is Unix98 compliant and has lots of info like BSD.   */
  /* First character:                                                        */
  /* D    Uninterruptible sleep (usually IO)                                 */
  /* R    Running or runnable (on run queue)                                 */
  /* S    Interruptible sleep (waiting for an event to complete)             */
  /* T    Stopped, either by a job control signal or because it is being     */
  /*      traced.                                                            */
  /* W    paging (not valid since the 2.6.xx kernel)                         */
  /* X    dead (should never be seen)                                        */
  /* Z    Defunct ("zombie") process, terminated but not reaped by its       */
  /*      parent.                                                            */
  /* Following characters:                                                   */
  /* <    high-priority (not nice to other users)                            */
  /* N    low-priority (nice to other users)                                 */
  /* L    has pages locked into memory (for real-time and custom IO)         */
  /* s    is a session leader                                                */
  /* l    is multi-threaded (using CLONE_THREAD, like NPTL pthreads do)      */
  /* +    is in the foreground process group                                 */
  i=0;
  if(tsk->nice<0)tsk->state[++i]='<';
  else if(tsk->nice>0)tsk->state[++i]='N';
  if(tsk->vm_lock)tsk->state[++i]='L';
  if(tsk->sid==tsk->tgid)tsk->state[++i]='s';              /* session leader */
  if(tsk->nlwp>1)tsk->state[++i]='l';                      /* multi-threaded */
  if(tsk->pgrp==tsk->tpgid)tsk->state[++i]='+'; /* in foreground proc. group */
  tsk->state[++i]='\0';
  /*-------------------------------------------------------------------------*/
  /* tty name */
  dev2tty(tsk->ttyS,sizeof(tsk->ttyS)-1,tsk->tty,tsk->tid);
  /*-------------------------------------------------------------------------*/
  /* uid/gid */
  memcpy(tsk->euser,getUserFromUid(tsk->euid),sizeof(tsk->euser));
  memcpy(tsk->fuser,getUserFromUid(tsk->fuid),sizeof(tsk->fuser));
  memcpy(tsk->ruser,getUserFromUid(tsk->ruid),sizeof(tsk->ruser));
  memcpy(tsk->suser,getUserFromUid(tsk->suid),sizeof(tsk->suser));
  memcpy(tsk->egroup,getGroupFromGid(tsk->egid),sizeof(tsk->egroup));
  memcpy(tsk->fgroup,getGroupFromGid(tsk->fgid),sizeof(tsk->fgroup));
  memcpy(tsk->rgroup,getGroupFromGid(tsk->rgid),sizeof(tsk->rgroup));
  memcpy(tsk->sgroup,getGroupFromGid(tsk->sgid),sizeof(tsk->sgroup));
  /*-------------------------------------------------------------------------*/
  /* sched */
  switch(tsk->sched)
  {
    case -1:snprintf(tsk->schedS,6,"N/A");break; /* not reported */
    case  0: snprintf(tsk->schedS,6,"TS"); break; /* SCHED_OTHER */
    case  1: snprintf(tsk->schedS,6,"FF"); break; /* SCHED_FIFO */
    case  2: snprintf(tsk->schedS,6,"RR"); break; /* SCHED_RR */
    case  3: snprintf(tsk->schedS,6,"#3"); break; /* SCHED_BATCH? (will be B) */
    case  4: snprintf(tsk->schedS,6,"#4"); break; /* SCHED_ISO? (Con Kolivas) */
    case  5: snprintf(tsk->schedS,6,"#5"); break;
    case  8: snprintf(tsk->schedS,6,"#8"); break;
    default: snprintf(tsk->schedS,6,"?");  break; /* unknown value */
  }
  /* prio in -99...-1 for RR and FF */
  /* prio in 0...40    for TS */
  if(tsk->prio<-99 || tsk->prio>40)snprintf(tsk->prioS,5,"N/A");
  else snprintf(tsk->prioS,5,"%ld",tsk->prio);
  /* rtprio in 0...99 */
  if(tsk->rtprio<0 || tsk->rtprio>99)snprintf(tsk->rtprioS,4,"N/A");
  else snprintf(tsk->rtprioS,4,"%ld",tsk->rtprio);
  /* nice in -20...19 */
  if(tsk->sched!=0 && tsk->sched!=-1)snprintf(tsk->niceS,4,"N/A");
  else snprintf(tsk->niceS,4,"%ld",tsk->nice);
  /*-------------------------------------------------------------------------*/
  /* cpu time */
  tm=(tsk->utime+tsk->stime)/Hertz;
  ss=tm%60;
  tm/=60;
  mm=tm%60;
  tm/=60;
  hh=tm%24;
  tm/=24;
  dd=tm;
  snprintf(tsk->cpuTime,sizeof(tsk->cpuTime),"%03u-%02u:%02u:%02u",dd,hh,mm,
           ss);
  /*-------------------------------------------------------------------------*/
  /* start time */
  tm=timeOfBoot+tsk->st_time/Hertz;
  snprintf(tsk->startTime,sizeof(tsk->startTime),"%24.24s",ctime(&tm));
  /*-------------------------------------------------------------------------*/
  /* elapsed time */
  if(secondsSinceBoot>=(time_t)(tsk->st_time/Hertz))
    tm=secondsSinceBoot-(time_t)(tsk->st_time/Hertz);
  else tm=0UL;
  ss=tm%60;
  tm/=60;
  mm=tm%60;
  tm/=60;
  hh=tm%24;
  tm/=24;
  dd=tm;
  snprintf(tsk->elapsedTime,sizeof(tsk->elapsedTime),"%03u-%02u:%02u:%02u",dd,
           hh,mm,ss);
  /*-------------------------------------------------------------------------*/
}
/*****************************************************************************/
void getMemTotal(void)
{
  char sbuf[4096];
  char *p;
  /*-------------------------------------------------------------------------*/
  memTotal=0;
  if(likely(file2str(MEM_FILE,sbuf,sizeof(sbuf))!=-1))
  {
    p=strstr(sbuf,"MemTotal:");
    if(!p)
    {
      mPrintf(errU,FATAL,__func__,0,"String \"MemTotal\" not found in file "
              "\"%s\"!",MEM_FILE);
      exit(1);
    }
    memTotal=strtoul(p+strlen("MemTotal:"),NULL,10);
  }
}
/*****************************************************************************/
void getPmem(taskData_t *tsk)
{
  unsigned long long pmem=0;
  /*-------------------------------------------------------------------------*/
  pmem=tsk->vm_rss*1000ULL/memTotal;
  if(pmem>1000)pmem=1000;
  tsk->pmem=(float)pmem/10.0;
}
/*****************************************************************************/
/* write global refreshTime                                                  */
/*****************************************************************************/
void saveFrameTime(void)
{
  static struct timeval now={0,0};
  static struct timeval lastTime;
  struct timezone tz;
  int sec,usec;
  /*-------------------------------------------------------------------------*/
  /* save old time */
  memcpy(&lastTime,&now,sizeof(now));
  /* set new time */
  gettimeofday(&now,&tz);
  /* evaluate the elapsed time since the last call */
  if(now.tv_usec<lastTime.tv_usec)
  {
    now.tv_usec+=1000000;
    now.tv_sec-=1;
  }
  sec=now.tv_sec-lastTime.tv_sec;                         /* time difference */
  usec=now.tv_usec-lastTime.tv_usec;                      /* time difference */
  refreshTime=(float)sec+((float)usec/(float)1000000.0);
  return;
}
/*****************************************************************************/
/* read global: refreshTime                                                  */
/* writes tsk->pcpu                                                          */
/*****************************************************************************/
void getPcpu(cpuTcsArr_t *oldTcs,cpuTcsArr_t *newTcs,taskData_t *tsk,pid_t id,
             int n,char *type)
{
  float pcpu=0.0;
  unsigned long long tics;
  cpuTcsEl_t tmp;
  cpuTcsEl_t *p;
  int oldNAvail;
  /*-------------------------------------------------------------------------*/
  if(unlikely(newTcs->nAvail<n+1))
  {
    /* allocate more room for newTcs and oldTcs */
    oldNAvail=newTcs->nAvail;
    newTcs->nAvail=newTcs->nAvail*5/4+10;
    newTcs->data=(cpuTcsEl_t*)realloc(newTcs->data,
                                      newTcs->nAvail*sizeof(cpuTcsEl_t));
    memset(newTcs->data+oldNAvail,0,newTcs->nAvail-oldNAvail);
    oldNAvail=oldTcs->nAvail;
    oldTcs->nAvail=newTcs->nAvail;
    oldTcs->data=(cpuTcsEl_t*)realloc(oldTcs->data,
                                      oldTcs->nAvail*sizeof(cpuTcsEl_t));
    memset(oldTcs->data+oldNAvail,0,oldTcs->nAvail-oldNAvail);
    if(unlikely(deBug&0x004))
    {
      char *types=NULL;
      if(!strcmp(type,"process"))types="processes";
      else if(!strcmp(type,"task"))types="tasks";
      mPrintf(errU,DEBUG,__func__,0,"More room allocated for %s cpu tics "
              "storage: %d -> %d %s.",type,oldNAvail,
              newTcs->nAvail,types);
    }
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate and save currently jiffy counter */
  if(unlikely(newTcs->n<n+1))newTcs->n=n+1;          /* set newTcs used size */
  newTcs->data[n].id=id;                                    /* save tgid/tid */
  tics=tsk->utime+tsk->stime;       /* evaluate jiffies used by this process */
  newTcs->data[n].tics=tics;                                 /* save jiffies */
  /*-------------------------------------------------------------------------*/
  /* retrieve old jiffy counter */
  tmp.id=id;                                /* set key for bsearch(3) */
  p=bsearch(&tmp,oldTcs->data,oldTcs->n,sizeof(cpuTcsEl_t),(int(*)())tcsElCmp);
  if(p)tics-=p->tics;  /* get tics from matching element found by bsearch(3) */
  /*-------------------------------------------------------------------------*/
  if(refreshTime>0.001)pcpu=(float)tics*100.0/Hertz/refreshTime;
  else pcpu=0.0;                             /* the first call this happends */
  if(unlikely(deBug&0x004))
  {
    if(unlikely(tics))
    {
      mPrintf(errU,DEBUG,__func__,0,"Frame=%d, ps#=%d, pid=%d, cmd=\"%s\", "
              "CPU tics=%llu, %%CPU=%.2f.",refreshCnt,n,id,tsk->cmd,
              tics,pcpu);
    }
  }
  if(pcpu>99.9)pcpu=99.9;
  tsk->pcpu=pcpu;
  return;
}
/*****************************************************************************/
/* modifies globals: newPtcs, oldPtcs                                        */
/*****************************************************************************/
void saveCpuTics(void)
{
  cpuTcsArr_t tmpPtcs;
  /*-------------------------------------------------------------------------*/
# ifdef DEBUG
  {
    int i;
    printf("nP=%d\n",newPtcs.n);
    for(i=0;i<newPtcs.n;i++)
    {
      printf("====>%d %d %llu\n",i,newPtcs.data[i].id,newPtcs.data[i].tics);
    }
  }
# endif
  /*-------------------------------------------------------------------------*/
  /* swap newPtcs <-> oldPtcs to reuse allocated memory */
  memcpy(&tmpPtcs,&oldPtcs,sizeof(cpuTcsArr_t));               /* save old */
  memcpy(&oldPtcs,&newPtcs,sizeof(cpuTcsArr_t));        /* move new to old */
  memcpy(&newPtcs,&tmpPtcs,sizeof(cpuTcsArr_t));  /* move saved old to new */
  /* sort array for bsearch(3) */
  qsort(oldPtcs.data,oldPtcs.n,sizeof(cpuTcsEl_t),(int(*)())tcsElCmp);
  newPtcs.n=0;                                         /* reset id counter */
  /*-------------------------------------------------------------------------*/
  return;
}
/*****************************************************************************/
/* comparing function used by bsearch(3) in  getPcpu() */
static int tcsElCmp(const cpuTcsEl_t *x,const cpuTcsEl_t *y)
{
  return x->id - y->id;
}
/*****************************************************************************/
/* read global: refreshTime                                                  */
/* writes tsk->minFltRate, majFltRate.                                       */
/*****************************************************************************/
void getFltRate(fltArr_t *oldFlt,fltArr_t *newFlt,taskData_t *tsk,pid_t id,
             int n,char *type)
{
  float minFltRate=0.0;
  float majFltRate=0.0;
  unsigned long minFlt,majFlt;
  fltEl_t tmp;
  fltEl_t *p;
  int oldNAvail;
  /*-------------------------------------------------------------------------*/
  if(unlikely(newFlt->nAvail<n+1))
  {
    /* allocate more room for newFlt and oldFlt */
    oldNAvail=newFlt->nAvail;
    newFlt->nAvail=newFlt->nAvail*5/4+10;
    newFlt->data=(fltEl_t*)realloc(newFlt->data,newFlt->nAvail*sizeof(fltEl_t));
    memset(newFlt->data+oldNAvail,0,newFlt->nAvail-oldNAvail);
    oldNAvail=oldFlt->nAvail;
    oldFlt->nAvail=newFlt->nAvail;
    oldFlt->data=(fltEl_t*)realloc(oldFlt->data,oldFlt->nAvail*sizeof(fltEl_t));
    memset(oldFlt->data+oldNAvail,0,oldFlt->nAvail-oldNAvail);
    if(unlikely(deBug&0x004))
    {
      char *types=NULL;
      if(!strcmp(type,"process"))types="processes";
      else if(!strcmp(type,"task"))types="tasks";
      mPrintf(errU,DEBUG,__func__,0,"More room allocated for %s page fault "
              "storage: %d -> %d %s.",type,oldNAvail,
              newFlt->nAvail,types);
    }
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate and save currently jiffy counter */
  if(unlikely(newFlt->n<n+1))newFlt->n=n+1;          /* set newFlt used size */
  newFlt->data[n].id=id;                                    /* save tgid/tid */
  minFlt=tsk->min_flt;         /* get minor page faults of this process/task */
  majFlt=tsk->maj_flt;         /* get major page faults of this process/task */
  newFlt->data[n].min=minFlt;                      /* save minor page faults */
  newFlt->data[n].maj=majFlt;                      /* save major page faults */
  /*-------------------------------------------------------------------------*/
  /* retrieve old jiffy counter */
  tmp.id=id;                                /* set key for bsearch(3) */
  p=bsearch(&tmp,oldFlt->data,oldFlt->n,sizeof(fltEl_t),(int(*)())fltElCmp);
  if(p)
  {
    minFlt-=p->min;  /* get minFlt from matching element found by bsearch(3) */
    majFlt-=p->maj;  /* get minFlt from matching element found by bsearch(3) */
  }
  /*-------------------------------------------------------------------------*/
  if(refreshTime>0.001)
  {
    minFltRate=(float)minFlt/refreshTime;
    majFltRate=(float)majFlt/refreshTime;
  }
  else
  {
    minFltRate=0.0;
    majFltRate=0.0;
  }
  if(unlikely(deBug&0x004))
  {
    if(unlikely(unlikely(minFlt)||unlikely(majFlt)))
    {
      mPrintf(errU,DEBUG,__func__,0,"Frame=%d, ps#=%d, pid=%d, cmd=\"%s\", "
              "Minor page faults=%lu, Major page faults=%lu, Minor page fault "
              "rate=%.2f, Major page fault rate=%.2f.",refreshCnt,n,id,
              tsk->cmd,minFlt,majFlt,minFltRate,majFltRate);
    }
  }
  tsk->minFltRate=minFltRate;
  tsk->majFltRate=majFltRate;
  return;
}
/*****************************************************************************/
/* comparing function used by bsearch(3) in  getFltRate() */
static int fltElCmp(const fltEl_t *x,const fltEl_t *y)
{
  return x->id - y->id;
}
/*****************************************************************************/
time_t getUptime(void)
{
  double up=0, idle=0;
  char *savelocale;
  static char buf[1024];
  static int uptime_fd=-1;
  /*-------------------------------------------------------------------------*/
  FILE_TO_BUF(UPTIME_FILE,uptime_fd);
  savelocale=setlocale(LC_NUMERIC,NULL);
  setlocale(LC_NUMERIC,"C");
  if(sscanf(buf,"%lf %lf",&up,&idle)<2)
  {
    setlocale(LC_NUMERIC,savelocale);
    mPrintf(errU,ERROR,__func__,0,"Bad data in file \"%s\"!",UPTIME_FILE);
    return 0;
  }
  setlocale(LC_NUMERIC,savelocale);
  return (time_t)up;             /* assume never be zero seconds in practice */
}
/*****************************************************************************/
char *getUserFromUid(uid_t uid)
{
  struct pwbuf **p;
  struct passwd *pw;
  /*-------------------------------------------------------------------------*/
  p=&pwhash[HASH(uid)];
  while(*p)
  {
    if((*p)->uid==uid) return((*p)->name);
    p=&(*p)->next;
  }
  *p=(struct pwbuf*)malloc1(sizeof(struct pwbuf));
  (*p)->uid=uid;
  pw=getpwuid(uid);
  if(!pw||strlen(pw->pw_name)>=USER_LEN) sprintf((*p)->name,"%u",uid);
  else strcpy((*p)->name,pw->pw_name);
  (*p)->next=NULL;
  return((*p)->name);
}
/*****************************************************************************/
char *getGroupFromGid(gid_t gid)
{
  struct grpbuf **g;
  struct group *gr;
  /*-------------------------------------------------------------------------*/
  g=&grphash[HASH(gid)];
  while(*g)
  {
    if((*g)->gid==gid)return((*g)->name);
    g=&(*g)->next;
  }
  *g=(struct grpbuf*)malloc(sizeof(struct grpbuf));
  (*g)->gid=gid;
  gr=getgrgid(gid);
  if(!gr||strlen(gr->gr_name)>=GROUP_LEN) sprintf((*g)->name,"%u",gid);
  else strcpy((*g)->name,gr->gr_name);
  (*g)->next=NULL;
  return((*g)->name);
}
/* ######################################################################### */
/* functions to get tty name                                                 */
/* ######################################################################### */
/* Load /proc/tty/drivers for device name mapping use. */
static void load_drivers(void)
{
  char buf[10000];
  char *p;
  int fd;
  int readBytes;
  /*-------------------------------------------------------------------------*/
  fd=open("/proc/tty/drivers",O_RDONLY);
  if(fd==-1)
  {
    if(!ttyMap)ttyMap=(tty_map_node_t*)-1;
    return;
  }
  readBytes=read(fd,buf,sizeof(buf)-1);
  if(readBytes==-1)
  {
    close(fd);
    if(!ttyMap)ttyMap=(tty_map_node_t*)-1;
    return;
  }
  buf[readBytes]='\0';
  /*-------------------------------------------------------------------------*/
  for(p=buf;(p=strstr(p," /dev/"));)
  {
    tty_map_node_t *tmn;
    int len;
    char *end;
    p+=6;
    end=strchr(p,' ');
    if(!end)continue;
    len=end-p;
    tmn=calloc(1,sizeof(tty_map_node_t));
    tmn->next=ttyMap;
    ttyMap=tmn;
    /* if we have a devfs type name such as /dev/tts/%d then strip the %d    */
    /* but keep a flag. */
    if(len>=3 && !strncmp(end-2,"%d",2))
    {
      len-=2;
      tmn->devfs_type=1;
    }
    strncpy(tmn->name,p,len);
    p=end;             /* set p to point past the %d as well if there is one */
    while(*p == ' ')p++;
    tmn->major_number=atoi(p);
    p+=strspn(p,"0123456789");
    while(*p == ' ')p++;
    switch(sscanf(p,"%u-%u",&tmn->minor_first,&tmn->minor_last))
    {
      default:
        /* Can't finish parsing this line so we remove it from the list */
        ttyMap=ttyMap->next;
        free(tmn);
        break;
      case 1:
        tmn->minor_last=tmn->minor_first;
        break;
      case 2:
        break;
    }
  }
  /*-------------------------------------------------------------------------*/
  return;
}
/*****************************************************************************/
unsigned dev2tty(char *ttyS, unsigned ttySlen, dev_t devN, int pid)
{
  static char buf[TTY_LEN+1];
  char *p=buf;
  unsigned dev=devN;
  unsigned i=0;
  int c;
  int rv;
  /*-------------------------------------------------------------------------*/
  if(dev==0u)
  {
    strcpy(ttyS,"?");
    return 1;
  }
  rv=0;
  if(linuxVersionCode>LINUX_VERSION(2,7,0))
  {                                             /* not likely to make 2.6.xx */
    rv=getTtyFromStderr(p,MAJOR_OF(dev),MINOR_OF(dev),pid,"tty");
  }
  if(!rv)rv=getTtyFromProcTtyDriver(p,MAJOR_OF(dev),MINOR_OF(dev));
  if(!rv)rv=getTtyFromStderr(p,MAJOR_OF(dev),MINOR_OF(dev),pid,"fd/2");
  if(!rv)rv=getTtyFromNum(p,MAJOR_OF(dev),MINOR_OF(dev));
  if(!rv)rv=getTtyFromStderr(p,MAJOR_OF(dev),MINOR_OF(dev),pid,"fd/255");
  if(!rv)                                    /* unable to find a device file */
  {
    strcpy(ttyS,"?");
    return 1;
  }
  if(!strncmp(p,"/dev/",5) && *(p+5)) p+=5;                  /* strip "/dev" */
  if(ttySlen+(unsigned long)(p-buf)<=sizeof(buf)) p[ttySlen]='\0';
  /* replace non-ASCII characters with '?' and return the number of chars */
  for(;;)
  {
    c=*p;
    p++;
    if(!c)break;
    i++;
    if(c<=' ')c='?';
    if(c>126)c='?';
    *ttyS=c;
    ttyS++;
  }
  *ttyS='\0';
  return i;
}
/*****************************************************************************/
static int getTtyFromNum(char *const buf, unsigned maj, unsigned min)
{
  struct stat sbuf;
  int t0, t1;
  unsigned tmpmin = min;
  /*-------------------------------------------------------------------------*/
  switch(maj)
  {
    case   4:
      if(min<64)
      {
        sprintf(buf, "/dev/tty%d", min);
        break;
      }
      if(min<128)                                 /* to 255 on newer systems */
      {
        sprintf(buf, "/dev/ttyS%d", min-64);
        break;
      }
      tmpmin=min & 0x3f;  /* FALL THROUGH */
    case   3:                          /* /dev/[pt]ty[p-za-o][0-9a-z] is 936 */
      if(tmpmin>255)return 0; /* should never happen; array index protection */
      t0="pqrstuvwxyzabcde"[tmpmin>>4];
      t1="0123456789abcdef"[tmpmin&0x0f];
      sprintf(buf,"/dev/tty%c%c",t0,t1);
      break;
    case  11: sprintf(buf,"/dev/ttyB%d",  min); break;
    case  17: sprintf(buf,"/dev/ttyH%d",  min); break;
    case  19: sprintf(buf,"/dev/ttyC%d",  min); break;
    case  22: sprintf(buf,"/dev/ttyD%d",  min); break;        /* devices.txt */
    case  23: sprintf(buf,"/dev/ttyD%d",  min); break;        /* driver code */
    case  24: sprintf(buf,"/dev/ttyE%d",  min); break;
    case  32: sprintf(buf,"/dev/ttyX%d",  min); break;
    case  43: sprintf(buf,"/dev/ttyI%d",  min); break;
    case  46: sprintf(buf,"/dev/ttyR%d",  min); break;
    case  48: sprintf(buf,"/dev/ttyL%d",  min); break;
    case  57: sprintf(buf,"/dev/ttyP%d",  min); break;
    case  71: sprintf(buf,"/dev/ttyF%d",  min); break;
    case  75: sprintf(buf,"/dev/ttyW%d",  min); break;
    case  78: sprintf(buf,"/dev/ttyM%d",  min); break;           /* conflict */
    case 105: sprintf(buf,"/dev/ttyV%d",  min); break;
    case 112: sprintf(buf,"/dev/ttyM%d",  min); break;           /* conflict */
    /* 136 ... 143 are /dev/pts/0, /dev/pts/1, /dev/pts/2 ... */
    case 136 ... 143:  sprintf(buf,"/dev/pts/%d",min+(maj-136)*256); break;
    case 148: sprintf(buf,"/dev/ttyT%d",  min); break;
    case 154: sprintf(buf,"/dev/ttySR%d", min); break;
    case 156: sprintf(buf,"/dev/ttySR%d", min+256); break;
    case 164: sprintf(buf,"/dev/ttyCH%d",  min); break;
    case 166: sprintf(buf,"/dev/ttyACM%d", min); break;    /* bummer, 9-char */
    case 172: sprintf(buf,"/dev/ttyMX%d",  min); break;
    case 174: sprintf(buf,"/dev/ttySI%d",  min); break;
    case 188: sprintf(buf,"/dev/ttyUSB%d", min); break;    /* bummer, 9-char */
    case 204:
      if(min>=sizeof(low_density_names)/sizeof(low_density_names[0])) return 0;
      sprintf(buf,"/dev/tty%s",low_density_names[min]);
      break;
    case 208: sprintf(buf,"/dev/ttyU%d",  min); break;
    case 216: sprintf(buf,"/dev/ttyUB%d",  min); break;
    case 224: sprintf(buf,"/dev/ttyY%d",  min); break;
    case 227: sprintf(buf,"/dev/3270/tty%d", min); break;    /* bummer, HUGE */
    case 229: sprintf(buf,"/dev/iseries/vtty%d",min);break;  /* bummer, HUGE */
    default: return 0;
  }
  if(stat(buf,&sbuf)<0) return 0;
  if(min!=MINOR_OF(sbuf.st_rdev)) return 0;
  if(maj!=MAJOR_OF(sbuf.st_rdev)) return 0;
  return 1;
}
/*****************************************************************************/
/* Linux 2.2 can give us filenames that might be correct. Useful names could */
/* be in /proc/PID/fd/2 (stderr, seldom redirected) and in /proc/PID/fd/255  */
/* (used by bash to remember the tty).                                       */
static int getTtyFromStderr(char*const buf,unsigned maj,unsigned min,int pid,
                            const char *name)
{
  struct stat sbuf;
  char path[32];
  int count;
  /*-------------------------------------------------------------------------*/
  sprintf(path,"/proc/%d/%s",pid,name);           /* often permission denied */
  count=readlink(path,buf,TTY_LEN);
  if(count==-1)return 0;
  buf[count]='\0';
  if(stat(buf,&sbuf)<0)return 0;
  if(min!=MINOR_OF(sbuf.st_rdev)) return 0;
  if(maj!=MAJOR_OF(sbuf.st_rdev)) return 0;
  return 1;
}
/*****************************************************************************/
/* Try to guess the device name from /proc/tty/drivers info. */
static int getTtyFromProcTtyDriver(char*const buf,unsigned maj,unsigned min)
{
  struct stat sbuf;
  tty_map_node_t *tmn;
  /*-------------------------------------------------------------------------*/
  if(!ttyMap)load_drivers();
  if(ttyMap==(tty_map_node_t*)-1) return 0;
  tmn=ttyMap;
  for(;;)
  {
    if(!tmn)return 0;
    if(tmn->major_number==maj && tmn->minor_first<=min && tmn->minor_last>=min)
    {
      break;
    }
    tmn=tmn->next;
  }
  sprintf(buf,"/dev/%s%d",tmn->name,min);            /* like "/dev/ttyZZ255" */
  if(stat(buf,&sbuf)<0)
  {
    if(tmn->devfs_type) return 0;
    sprintf(buf,"/dev/%s",tmn->name);                /* like "/dev/ttyZZ255" */
    if(stat(buf,&sbuf)<0) return 0;
  }
  if(min!=MINOR_OF(sbuf.st_rdev)) return 0;
  if(maj!=MAJOR_OF(sbuf.st_rdev)) return 0;
  return 1;
}
/* ######################################################################### */
/* Functions to access the procfs                                            */
/* ######################################################################### */
static int file2str(const char *path,char *buf,int bufSz)
{
  int fd;
  ssize_t readChars;
  int savedErrno;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  fd=open(path,O_RDONLY,0);
  if(unlikely(fd==-1))
  {
    if(unlikely(deBug&0x002))
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s!",path,
              strerror(errno));
    }
    printOutFuncNOK;
    return -1;
  }
  readChars=read(fd,buf,bufSz-1);
  savedErrno=errno;
  close(fd);
  if(unlikely(readChars<=0))
  {
    if(unlikely(deBug&0x002))
    {
      mPrintf(errU,ERROR,__func__,0,"Error reading file \"%s\": read(2) "
              "return value: %d, error: %d (%s)!",path,readChars,savedErrno,
              strerror(savedErrno));
    }
    printOutFuncNOK;
    return -1;
  }
  buf[readChars]='\0';
  printOutFuncOK;
  return readChars;
}
/*****************************************************************************/
static char **file2strvec(const char* path)
{
  /* kernel 2.6.18 writes max 1024*3 Bytes at a time */
  /* kernel 2.6.9-78 writes max 121862 Bytes at a time */
  static char sRBuf[131072];        /* read maximum 128*1024 bytes at a time */
  char *tRBuf=NULL;
  size_t tRBufSz=0;
  char *p;
  char *endbuf;
  char **q;
  char **ret;
  int fd;
  ssize_t n;
  int c;
  int align;
  int i;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  /* read the whole file into a memory buffer, allocating memory as we go */
  fd=open(path,O_RDONLY,0);
  if(unlikely(fd==-1))
  {
    if(unlikely(deBug&0x002))
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s!",path,
              strerror(errno));
    }
    printOutFuncNOK;
    return NULL;
  }
  /*-------------------------------------------------------------------------*/
  for(i=0;;i++)                             /* loop over chunks of read data */
  {
    n=read(fd,sRBuf,sizeof sRBuf);
    if(unlikely(deBug&0x002))
    {
      if(likely(n>=0))
      {
        mPrintf(errU,DEBUG,__func__,0,"File: \"%s\", chunk: %d: read(): %zd "
                "Bytes read in a single chunk.",path,i+1,n);
      }
    }
    if(unlikely(n<0))                                          /* read error */
    {
      if(errno==EINTR)                            /* Interrupted system call */
      {
        if(unlikely(deBug&0x002))
        {
          mPrintf(errU,WARN,__func__,0,"File: \"%s\", chunk: %d: read(): %s! "
                  "Retrying...",path,i+1,strerror(errno));
        }
        continue;
      }
      else
      {
        if(unlikely(deBug&0x002))
        {
          mPrintf(errU,ERROR,__func__,0,"File: \"%s\", chunk: %d: read() "
                  "error: %s!",path,i+1,strerror(errno));
        }
        close(fd);
        if(tRBuf)free(tRBuf);
        printOutFuncNOK;
        return NULL;
      }
    }
    if(!n)                                  /* zero Bytes read the last time */
    {
      if(unlikely(!tRBuf))                          /* zero Total Bytes read */
      {
        if(unlikely(deBug&0x002))
        {
          mPrintf(errU,ERROR,__func__,0,"Zero Bytes read in file: \"%s\"!",
                  path);
        }
        close(fd);
        printOutFuncOK;
        return NULL;
      }
      else                                                            /* EOF */
      {
        if(unlikely(deBug&0x002))
        {
          if(tRBuf[tRBufSz-1])
          {
            mPrintf(errU,DEBUG,__func__,0,"File: \"%s\": last read character: "
                    "\'%c\' (ASCII %u).",path,tRBuf[tRBufSz-1],
                    tRBuf[tRBufSz-1]);
          }
          else
          {
            mPrintf(errU,DEBUG,__func__,0,"File: \"%s\": last read character: "
                    "ASCII %u.",path,tRBuf[tRBufSz-1]);
          }
        }
        if(tRBuf[tRBufSz-1]!=0)                          /* truncated string */
        {
          tRBuf=realloc1(tRBuf,tRBufSz+1);       /* 1 B memory more for NULL */
          tRBuf[tRBufSz]='\0';
          tRBufSz++;
          if(unlikely(deBug&0x002))
          {
            mPrintf(errU,DEBUG,__func__,0,"File \"%s\" truncated. NULL "
                    "terminator added.",path);
          }
        }                                                /* truncated string */
        break;
      }                                                               /* EOF */
    }                                       /* zero Bytes read the last time */
    tRBuf=realloc1(tRBuf,tRBufSz+n);                 /* allocate more memory */
    memcpy(tRBuf+tRBufSz,sRBuf,n);                    /* copy buffer into it */
    tRBufSz+=n;                                  /* increment total byte ctr */
    if(unlikely(deBug&0x002))
    {
      mPrintf(errU,DEBUG,__func__,0,"File: \"%s\", chunk: %d: total %zu "
              "Bytes read till now (in %d chunks).",path,i+1,tRBufSz,i+1);
    }
  }                                         /* loop over chunks of read data */
  /*-------------------------------------------------------------------------*/
  close(fd);
  /*-------------------------------------------------------------------------*/
  if(unlikely(deBug&0x002))
  {
    mPrintf(errU,DEBUG,__func__,0,"File: \"%s\": TOTAL %zu Bytes read in %d "
            "chunks.",path,tRBufSz,i);
  }
  /*-------------------------------------------------------------------------*/
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
/*****************************************************************************/
static void *malloc1(size_t size)
{
  void *p;
  /*-------------------------------------------------------------------------*/
  if(size==0)size++;
  p=malloc(size);
  if(!p)
  {
    mPrintf(errU,FATAL,__func__,0,"malloc1: malloc(%zu) failed: %s!",size,
            strerror(errno));
    exit(1);
  }
  return(p);
}
/*****************************************************************************/
static void *realloc1(void *oldp,size_t size)
{
  void *p;
  /*-------------------------------------------------------------------------*/
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
/*****************************************************************************/
int getMaxPid(void)
{
  char sbuf[1024];
  int pm=131072;         /* this must be larger then any possible real value */
  /*-------------------------------------------------------------------------*/
  if(file2str(PID_MAX_FILE,sbuf,sizeof(sbuf))!=-1)
  {
    pm=(int)strtol(sbuf,NULL,0);
  }
  return pm;
}
/*****************************************************************************/
static void initLinuxVersion(void)
{
  static struct utsname uts;
  int x=0,y=0,z=0;                           /* cleared in case sscanf() < 3 */
  /*-------------------------------------------------------------------------*/
  if(uname(&uts)==-1)                     /* failure implies impending death */
  {
    mPrintf(errU,FATAL,__func__,0,"uname(): %s!",strerror(errno));
    exit(1);
  }
  if(sscanf(uts.release,"%d.%d.%d",&x,&y,&z)<3)
  {
     mPrintf(errU,ERROR,__func__,0,"Non-standard uts for running kernel: "
             "release %s=%d.%d.%d gives version code %d!",uts.release,x,y,z,
             LINUX_VERSION(x,y,z));
  }
  linuxVersionCode=LINUX_VERSION(x,y,z);
}
/*****************************************************************************/
char *getPSsensorVersion()
{
  return rcsid;
}
/* ######################################################################### */
