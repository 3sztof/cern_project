/* ######################################################################### */
/*
 * $Log: getPsFromProc.h,v $
 * Revision 2.6  2009/01/13 13:10:57  galli
 * envSz variable become public.
 * getMaxPid() function become public
 *
 * Revision 2.4  2008/12/11 14:22:56  galli
 * do not use libprocps anymore
 *
 * Revision 2.0  2008/11/20 08:26:02  galli
 * rewriting from scratch (temporary version)
 *
 * Revision 1.7  2007/08/22 07:00:13  galli
 * added to the taskdata struct: pgrp
 *
 * Revision 1.6  2007/08/21 11:23:29  galli
 * added to the taskdata struct: vm_lock, vm_rss, vm_data, vm_stack, vm_exe,
 * vm_lib.
 *
 * Revision 1.5  2007/08/10 13:54:40  galli
 * getSensorVersion() renamed to getPSsensorVersion()
 *
 * Revision 1.4  2006/10/23 21:11:56  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.3  2004/11/03 22:34:18  galli
 * added functions psInfo() and getSensorVersion()
 *
 * Revision 1.2  2004/10/16 08:30:48  galli
 * getUtgid() function added
 *
 * Revision 1.1  2004/08/25 12:22:28  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _GET_PS_FROM_PROC_H
#define _GET_PS_FROM_PROC_H 1
/* ######################################################################### */
#include <limits.h>                                              /* NAME_MAX */
#include <unistd.h>
#include "fmcPar.h"                                             /* UTGID_LEN */
/*---------------------------------------------------------------------------*/
#define PROC_PATH "/proc"
#define UPTIME_FILE "/proc/uptime"
#define MEM_FILE "/proc/meminfo"
#define PID_MAX_FILE "/proc/sys/kernel/pid_max"
#define CMD_LINE_LEN (PATH_MAX-1)                                /* now 4095 */
#define CMD_LEN 15                      /* max len of CMD in stat and status */
#define USER_LEN 19
#define GROUP_LEN 19
#define SIG_LEN 16                                                 /* 64 bit */
#define SCH_LEN          3                             /* N/A, TS, FF, RR, ? */
#define RTPRIO_LEN       3                                    /* 0...99, N/A */
#define NICE_LEN         3                                  /* -20...19, N/A */
#define PRIO_LEN         4                                 /* -100...39, N/A */
#define STAT_LEN         6                             /* "[DRSTXZ][N<]Lsl+" */
#define STARTTIME_LEN    24                   /*  "Mon May 28 11:54:33 2007" */
#define ELAPSEDTIME_LEN  12                                /* "086-20:44:06" */
#define CPUTIME_LEN      12                                /* "000-00:03:57" */
// This is the buffer size for a tty name. Any path is legal,
// which makes PAGE_SIZE appropriate (see kernel source), but
// that is only 99% portable and utmp only holds 32 anyway.
#define TTY_LEN 127
/* ######################################################################### */
/* public data types */
typedef struct taskData
{
  /*-------------------------------------------------------------------------*/
  /* basic */
  /* If envSz==4096 there is a strong suspect that the running Linux kernel  */
  /* truncates the data in /proc/PID/environ. The kernel patch:              */
  /* "Don-t-truncate-proc-PID-environ-at-4096-characters" appears in version */
  /* 2.6.9-78.0.1. In case of environ truncation, the FMC Task Manager uses  */
  /* the ptrace(2) workaround and the psSrv contact the running Task Manager */
  /* (if any) to get the UTGID.                                              */
  int envSz;                  /* environment size. */
  char utgid[UTGID_LEN+1];    /* UTGID */
  pid_t tid;                  /* TID (LWP, SPID) */
  pid_t tgid;                 /* TGID (PID) */
  unsigned long flags;        /* FLAGS (F, kernel flags for the process) */
  int nlwp;                   /* NLWP (THCNT) */
  uid_t euid;                 /* effective user ID */
  char euser[USER_LEN+1];     /* USER (EUSER, UNAME) */
  int tty;                    /* full device number of controlling terminal */
  char ttyS[TTY_LEN+1];       /* TTY (TT, TNAME) */
  char cmd[CMD_LEN+1];        /* CMD (COMM, COMMAND, UCMD, UCOMM) */
  /*-------------------------------------------------------------------------*/
  /* ids */
  pid_t ppid;                 /* PPID */
  pid_t pgrp;                 /* PGID (PGRP, TGID of process group leader) */
  pid_t sid;                  /* SID (SESS, SESSION, TGID of session leader) */
  pid_t tpgid;                /* TPGID (terminal process group ID) */
  /*-------------------------------------------------------------------------*/
  /* owner */
  uid_t fuid;                 /* filesystem user ID */
  uid_t ruid;                 /* real user ID */
  uid_t suid;                 /* saved user ID */
  gid_t egid;                 /* effective group ID */
  gid_t fgid;                 /* filesystem group ID */
  gid_t rgid;                 /* real group ID */
  gid_t sgid;                 /* saved group ID */
  char egroup[GROUP_LEN+1];   /* GROUP (EGROUP) */
  char fuser[USER_LEN+1];     /* FUSER */
  char fgroup[GROUP_LEN+1];   /* FGROUP */
  char ruser[USER_LEN+1];     /* RUSER */
  char rgroup[GROUP_LEN+1];   /* RGROUP */
  char suser[USER_LEN+1];     /* SUSER */
  char sgroup[GROUP_LEN+1];   /* SGROUP */
  /*-------------------------------------------------------------------------*/
  /* sched */
  unsigned long sched;
  long prio;
  unsigned long rtprio;
  long nice;
  char schedS[SCH_LEN+1];     /* SCH (CLS, POL) */
  char prioS[PRIO_LEN+1];     /* PRIO (PRI, PR) */
  char rtprioS[RTPRIO_LEN+1]; /* RTPRIO */
  char niceS[NICE_LEN+1];     /* NICE (NI) */
  /*-------------------------------------------------------------------------*/
  /* stat */
  int processor;              /* PSR (CPUID, P) */
  char state[STAT_LEN+1];     /* STAT (S) */
  float pcpu;                 /* %CPU (PCPU, CP, C) */
  float pmem;                 /* %MEM (PMEM) */
  unsigned long min_flt;      /* MIN_FLT (number of minor page faults) */
  unsigned long maj_flt;      /* MAJ_FLT (number of major page faults) */
  unsigned long cmin_flt;     /* CMIN_FLT (cumulative minor page faults) */
  unsigned long cmaj_flt;     /* CMAJ_FLT (cumulative major page faults) */
  float minFltRate;
  float majFltRate;
  /*-------------------------------------------------------------------------*/
  /* size */
  unsigned long vm_size;      /* VSIZE (VSZ, VIRT) */
  unsigned long vm_lock;      /* LOCK */
  unsigned long vm_rss;       /* RSS (RSZ, RES, RESIDENT) */
  unsigned long vm_data;      /* DATA */
  unsigned long vm_stack;     /* STACK */
  unsigned long vm_exe;       /* EXE */
  unsigned long vm_lib;       /* LIB */
  long share;                 /* SHARE (SHR, SHRD) */
  /*-------------------------------------------------------------------------*/
  /* signals */
  char sigpending[SIG_LEN+1]; /* PENDING (SIGPEND) */
  char sigblocked[SIG_LEN+1]; /* BLOCKED (SIGBLOCKED) */
  char sigignored[SIG_LEN+1]; /* IGNORED (SIGIGNORE) */
  char sigcatched[SIG_LEN+1]; /* CATCHED (SIGCATCH, CAUGHT) */
  /*-------------------------------------------------------------------------*/
  /* time */
  unsigned long long utime;   /* user-mode CPU time accumulated by process */
  unsigned long long stime;   /* kernel-mode CPU time accumulated by process */
  unsigned long long cutime;  /* cumulative utime of proc + reaped children */
  unsigned long long cstime;  /* cumulative stime of proc + reaped children */
  unsigned long long st_time; /* start time of process - secs since 1-1-70 */
  char startTime[STARTTIME_LEN+1];     /* STARTED (STARTTIME, START) */
  char elapsedTime[ELAPSEDTIME_LEN+1]; /* ELAPSED (ELAPSEDTIME) */
  char cpuTime[CPUTIME_LEN+1];         /* CPUTIME (TIME) */
  /*-------------------------------------------------------------------------*/
  /* cmdline */
  char cmdline[CMD_LINE_LEN+1]; /* CMDLINE (ARGS, CMDLINE, CMD, COMMAND) */
  /*-------------------------------------------------------------------------*/
}taskData_t;
/*****************************************************************************/
typedef struct tasksData
{
  int n;
  int nAvail;
  taskData_t *data;
}tasksData_t;
/*****************************************************************************/
/* public functions */
int initTasks(tasksData_t *prcs);
int getTasks(tasksData_t *prcs);
/* ------------------------------------------------------------------------- */
/* utilities */
int getMaxPid(void);
/*****************************************************************************/
char *getPSsensorVersion(void);
/* ######################################################################### */
#endif                                                /* _GET_PS_FROM_PROC_H */
/* ######################################################################### */
