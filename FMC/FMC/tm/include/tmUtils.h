/* ######################################################################### */
/*
 * $Log: tmUtils.h,v $
 * Revision 5.0  2011/11/28 16:34:01  galli
 * Can set the oom_adj or oom_score_adj of started processes.
 *
 * Revision 1.15  2008/11/28 16:15:23  galli
 * added prototype for function getMaxPid()
 *
 * Revision 1.13  2008/05/02 15:29:59  galli
 * added function sig2msg()
 *
 * Revision 1.12  2008/04/29 15:51:11  galli
 * defined macros likely() and unlikely()
 * defined macros printInFunc(), printOutFuncOK() and printOutFuncNOK()
 * defined functions pLlsSvc() and pLllSvc()
 *
 * Revision 1.11  2007/12/05 16:31:41  galli
 * added prototype for functions pLadd(), pLsetTerm(), pLrm(), pLrmTerm(),
 * pLautoSetTerm(), pLpop(), pLfind(), pLlen(), pLprint()
 *
 * Revision 1.10  2007/11/28 10:51:45  galli
 * back to one version of findTgid()
 *
 * Revision 1.9  2007/11/27 15:26:58  galli
 * added prototypes for the 2 versions of findTgid()
 *
 * Revision 1.8  2007/11/23 15:18:04  galli
 * Added prototype for findUtgidLargeEnv()
 * Modified prototype for getUtgidFromTgid()
 *
 * Revision 1.7  2007/11/14 15:09:35  galli
 * added prototype for getUtgidFromTgid()
 *
 * Revision 1.6  2007/11/13 14:38:45  galli
 * added prototypes for the 2 versions of findUtgid
 *
 * Revision 1.4  2006/08/25 13:44:48  galli
 * Parameters: outFIFO, errFIFO and noDrop added in startPs() call.
 *
 * Revision 1.3  2004/11/21 10:35:06  galli
 * char *envz and int envz_len arguments added to startPs to set additional
 * environment variables
 *
 * Revision 1.2  2004/10/28 00:33:47  galli
 * startPs can set uid, scheduler, nice level and real-time priority
 *
 * Revision 1.1  2004/09/20 00:04:00  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _TM_UTILS_H
#define _TM_UTILS_H 1
/* ######################################################################### */
/* headers */
#include <sched.h>                                              /* cpu_set_t */
/* ------------------------------------------------------------------------- */
/* process array element */
typedef struct
{
  char *cmd;
  char *utgid;
  unsigned tgid;
  int oomScore;
}psinfo;
/* process array */
typedef struct
{
  int psN;
  psinfo *ps;
}pssinfo;
/*---------------------------------------------------------------------------*/
/* process list element */
typedef struct procListEl
{
  pid_t tgid;
  char *cmd;
  char *utgid;
  int running;
  int oomScore;
  int termStatus;
  time_t lastUpdate;
  struct procListEl *prev;
  struct procListEl *next;
}procListEl_t;
/* process list */
typedef struct procList
{
  pthread_mutex_t mutex;
  procListEl_t *head;
  procListEl_t *tail;
}procList_t;
/*****************************************************************************/
int startPs(int asDaemon,int doClearenv,int doRedirectStdout,
            int doRedirectStderr,char *outFIFO,char *errFIFO,int noDrop,
            uid_t uid,gid_t gid,int scheduler,int niceLevel,int rtPriority,
            cpu_set_t* cpuSet,int oomScoreAdj,char *utgid,char *wd,
            char *path,char **argv,char *envz,int envz_len);
int killPs(char *utgidPattern,int signal);
pssinfo findUtgid(char *utgid);
pssinfo findTgid(pid_t tgid);
void freePssinfo(pssinfo *pss);
char *genUtgid(char *path);
char *getActuatorVersion();
char *getUtgidFromTgid(pid_t tgid,int tryN);
const char *sig2msg(int signo);
int getMaxPid(void);
int setOomScoreAdj(int oomScoreAdjVal,char *utgid,pid_t tgid);
/*---------------------------------------------------------------------------*/
procListEl_t pLadd(procList_t *list,pid_t tgid,char *cmd,char *utgid,
                   int oomScore);
void pLsetTerm(procList_t *list,pid_t tgid,int status);
procListEl_t pLrm(procList_t *list,pid_t tgid);
procListEl_t pLrmTerm(procList_t *list,time_t timeOut);
void pLautoSetTerm(procList_t *list,time_t timeOut);
procListEl_t pLpop(procList_t *list);
procListEl_t *pLfind(procList_t *list,pid_t tgid);
int pLlen(procList_t *list);
void pLprint(procList_t *list,int severity);
void pLlsSvc(procList_t *list,char **address,int *size);
void pLllSvc(procList_t *list,char **address,int *size);
/* ######################################################################### */
#endif                                                        /* _TM_UTILS_H */
/* ######################################################################### */
