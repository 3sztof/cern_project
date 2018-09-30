/* ######################################################################### */
/*
 * $Log: psSrv.h,v $
 * Revision 1.6  2009/07/23 09:24:53  galli
 * added SUM_CMDLINE_LEN definition (command-line length in summary SVC)
 *
 * Revision 1.5  2009/02/26 20:21:31  galli
 * added definition for type psDataSz_t
 *
 * Revision 1.4  2009/01/30 14:11:55  galli
 * Process monitoring dynamical SVCs
 *
 * Revision 1.1  2009/01/12 15:45:24  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _PS_SRV_H
#define _PS_SRV_H 1
/* ######################################################################### */
#include <values.h>                                               /* FLT_MAX */
/* ######################################################################### */
/* common */
/* DIM server basename */
#define SRV_NAME "ps"
/*****************************************************************************/
/* server */
/* ------------------------------------------------------------------------- */
/* data string lengths */
#define ID_LEN           (snprintf(NULL,0,"%d",getMaxPid()))
#define CPU_N            (sysconf(_SC_NPROCESSORS_ONLN))
#define PCPU_LEN         (snprintf(NULL,0,"%.3f",100.0*CPU_N))
#define PMEM_LEN         (snprintf(NULL,0,"%.3f",100.0))
#define FLT_LEN          (snprintf(NULL,0,"%.2e",FLT_MAX))
#define PSR_LEN          3                                      /* up to 999 */
#define MEM_LEN          (snprintf(NULL,0,"%lu",ULONG_MAX))
#define UPDATETIME_LEN   16                            /* "Oct 12, 13:49:23" */
#define SUM_CMDLINE_LEN  255
/* ..........................................................................*/
/* labels */
#ifdef SERVER
char *ssLabelV[11]={"CMD","UTGID","TGID","USER","%CPU","%MEM","STAT","VSIZE",
                    "RSS","STARTED","CMDLINE"};
char *ssUnitV[11]={"","","","","%","%","","KiB","KiB",
                   "aaa mmm dd HH:MM:SS YYYY",""};
char *dsLabelV[44]={"CMD","UTGID","TID","USER","TTY","NLWP",
                    "TGID","PPID","PGID","SID","TPGID",
                    "FUSER","RUSER","SUSER","GROUP","FGROUP","RGROUP","SGROUP",
                    "SCH","RTPRIO","NICE","PRIO",
                    "%CPU","%MEM","MIN_FLT","MAJ_FLT","STAT","PSR",
                    "VSIZE","LOCK","RSS","DATA","STACK","EXE","LIB",
                    "SHARE",
                    "IGNORED","PENDING","CATCHED","BLOCKED",
                    "STARTED","ELAPSED","CPUTIME",
                    "CMDLINE"};
char *dsUnitV[44]={"","","","","","","",
                   "","","","",
                   "","","","","","","",
                   "","","","",
                   "%","%","Hz","Hz","","",
                   "KiB","KiB","KiB","KiB","KiB","KiB","KiB","KiB",
                   "","","","",
                   "aaa mmm dd HH:MM:SS YYYY","ddd-HH:MM:SS","ddd-HH:MM:SS",
                   ""};
char *dsTypeV[44]={"basic","basic","basic","basic","basic","basic",
                   "ids","ids","ids","ids","ids",
                   "owner","owner","owner","owner","owner","owner","owner",
                   "sched","sched","sched","sched",
                   "stat","stat","stat","stat","stat","stat",
                   "size","size","size","size","size","size","size","size",
                   "signal","signal","signal","signal",
                   "time","time","time",
                   "cmdline"};
char *smLabelV[11]={"UPDATED","STATUS","TID","VSIZE","RSS","%CPU","%MEM","CMD",
                    "UTGID","USER","CMDLINE"};
char *smUnitV[11]={"","","","KiB","KiB","%","%","",
                   "","",""};
char *dmLabelV[26]={"UPDATED","STATUS","HSTATUS","TID","NLWP","PSR","VSIZE",
                    "LOCK","RSS","DATA","STACK","EXE","LIB","SHARE","RTPRIO",
                    "NICE","PRIO","%CPU","%MEM","MIN_FLT","MAJ_FLT",
                    "LAST_UPDATE","CMD","UTGID","USER","CMDLINE"};
char *dmUnitV[26]={"","","","","","","KiB",
                   "KiB","KiB","KiB","KiB","KiB","KiB","KiB","",
                   "","","%","%","Hz","Hz",
                   "bbb dd, HH:MM:SS","","","",""};
#endif                                                       /* ifdef SERVER */
/*****************************************************************************/
/* client */
/* number of fields which have to be set to "N/F" if server unreachable */
#define FIELD_N_ERR 44
/* maximum hostame length */
#define MAX_HOSTNAME_L 12
/* ------------------------------------------------------------------------- */
typedef struct psDataSz
{
  int hostName;
  int updated;                                                      /* fixed */
  int status;                                                       /* fixed */
  int hStatus;                                                      /* fixed */
  int tid;
  int nlwp;
  int processor;
  int vm_size;
  int vm_lock;
  int vm_rss;
  int vm_data;
  int vm_stack;
  int vm_exe;
  int vm_lib;
  int share;
  int rtprio;
  int nice;
  int prio;
  int pcpu;                                                         /* fixed */
  int pmem;                                                         /* fixed */
  int minFltRate;                                                   /* fixed */
  int majFltRate;                                                   /* fixed */
  int lastUpdateTimeStr;                                            /* fixed */
  int cmd;
  int utgid;
  int euser;
  int cmdline;
  int total;
}psDataSz_t;
/* ######################################################################### */
#endif                                                          /* _PS_SRV_H */
/* ######################################################################### */
