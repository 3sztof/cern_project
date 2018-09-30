/* ######################################################################### */
/*
 * $Log: tmSrv.h,v $
 * Revision 5.0  2011/11/28 16:33:03  galli
 * Set oom_adj to -17 or oom_score_adj to -1000.
 * Can set the oom_adj or oom_score_adj of started processes.
 *
 * Revision 1.7  2008/11/28 16:14:31  galli
 * added definition for PID_MAX_FILE
 *
 * Revision 1.6  2008/11/11 20:46:46  galli
 * parameters PROC_PID_ENV_SZ and PROC_MAX_READ_CHARS removed (now variables)
 * added parameters DFLT_PROC_PID_ENV_SZ and DFLT_PROC_MAX_READ_CHARS
 *
 * Revision 1.5  2008/11/10 10:06:40  galli
 * UTGID_LEN moved to fmcPar.h
 *
 * Revision 1.4  2008/04/29 15:48:30  galli
 * added definition of CMD_LEN
 * added definition of PROC_MAX_READ_CHARS
 *
 * Revision 1.3  2007/11/28 09:29:15  galli
 * added definition of PROC_PID_ENV_SZ
 *
 * Revision 1.2  2007/11/14 15:10:36  galli
 * added definition of ENV_LINE_SZ
 *
 * Revision 1.1  2007/11/13 14:04:57  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _TM_SRV_H
#define _TM_SRV_H 1
/* ######################################################################### */
/* DIM server basename */
#define SRV_NAME "task_manager"
/* ######################################################################### */
/* Server only */
#ifdef _SERVER
/* ------------------------------------------------------------------------- */
/* hard-coded filenames */
#define ACL_FILE_NAME "/etc/tmSrv.allow" /* name of access control list file */
#define PID_MAX_FILE "/proc/sys/kernel/pid_max"
/* hard-coded constant */
#define MSG_LEN 8191                                   /* max message length */
#define CMD_LEN 2047              /* max length of the executable image name */
#define WD_LEN 255                    /* max length of the working directory */
#define AUTH_STR_LEN 255          /* max length of the authentication string */
#define ENV_LINE_SZ 8192                        /* max environment line size */
/* The following parameter is the DEFAULT value of the maximum size of the   */
/* /proc/<pid>/environ files in Linux kernel 2.6.9-67 (4096 B). The kernel   */
/* patch named "Don-t-truncate-proc-PID-environ-at-4096-character" suppress  */
/* this limit (the patch is applied to kernel 2.6.9-78.0.1.EL.cern).         */
/* Do NOT change this parameter if you are not sure about what you are doing */
#define DFLT_PROC_PID_ENV_SZ 4096 /* default max size of /proc/<pid>/environ */
/* Default maximum number of characters read from procfs files in one shot.  */
/* Linux kernel 2.6.9-67 writes max 1024*3=3072 Bytes at a time */
/* Linux kernel 2.6.9-78 writes max 121862 Bytes at a time */
#define DFLT_PROC_MAX_READ_CHARS 4096
/* OOM killer control */
#define OOM_ADJ "/proc/self/oom_adj"
#define OOM_SCORE_ADJ "/proc/self/oom_score_adj"
#define TM_OOM_ADJ -17
#define TM_OOM_SCORE_ADJ -1000
/* ------------------------------------------------------------------------- */
#endif                                                            /* _SERVER */
/* ######################################################################### */
/* Client only */
#ifdef _CLIENT
/* ------------------------------------------------------------------------- */
/* number of fields which have to be set to "N/F" if server unreachable */
#define TMLS_FIELD_N_ERR 1
#define TMLL_FIELD_N_ERR 6
/* ------------------------------------------------------------------------- */
#endif                                                            /* _CLIENT */
/* ######################################################################### */
#endif                                                          /* _TM_SRV_H */
/* ######################################################################### */
