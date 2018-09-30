/* ######################################################################### */
/*
 * $Log: logSrv.h,v $
 * Revision 1.3  2012/12/04 15:31:55  galli
 * Set oom_adj to -17 or oom_score_adj to -1000.
 *
 * Revision 1.2  2009/02/17 16:47:33  galli
 * added constant SYS_LOG_DATE_FMT
 *
 * Revision 1.1  2009/02/09 16:30:04  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _LOG_VIEWER_H
#define _LOG_VIEWER_H 1
/* ######################################################################### */
#define BUF_SIZE 4096                             /* size of logger messages */
/* DIM server basename */
#define SRV_NAME "logger"
/* syslog date format (for strptime(3)) */
#define SYS_LOG_DATE_FMT "%b%t%d%t%H:%M:%S%t"
/* OOM killer control */
#define LOG_OOM_ADJ_INODE "/proc/self/oom_adj"
#define LOG_OOM_SCORE_ADJ_INODE "/proc/self/oom_score_adj"
#define LOG_OOM_ADJ -17
#define LOG_OOM_SCORE_ADJ -1000
/* ######################################################################### */
#endif                                                      /* _LOG_VIEWER_H */
/* ######################################################################### */
