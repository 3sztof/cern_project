/* ######################################################################### */
/*
 * $Log: pcSrv.H,v $
 * Revision 1.8  2012/12/04 12:45:24  galli
 * Support for oomScoreAdj
 *
 * Revision 1.7  2008/10/03 15:05:13  galli
 * DIM_CONF_FILE_NAME moved to fmcPar.h
 * DIM_DNS_SRV_LIST moved to fmcPar.h
 *
 * Revision 1.6  2008/09/26 20:42:17  galli
 * added definition of DFLT_UID, DFLT_GID, DFLT_START_TIME_MAX_SIZE
 *
 * Revision 1.5  2008/09/19 21:25:53  galli
 * avoid duplicated inclusion
 * added definitions: DIM_DNS_SRV_LIST and PC_TM_WATCHER_TIMEOUT
 *
 * Revision 1.4  2008/09/16 09:07:03  galli
 * added DFLT_USER_NAME and DFLT_GROUP_NAME definition
 *
 * Revision 1.3  2008/09/15 08:06:54  galli
 * added DIM_CONF_FILE_NAME definition
 *
 * Revision 1.1  2008/09/09 15:10:51  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _CPP_PCSRV_H
#define _CPP_PCSRV_H 1
/*****************************************************************************/
#define MSG_LEN 8192                                   /* max message length */
#define SVC_SIZE 256
#define CONF_FILE_NAME "/etc/pcSrv.conf"               /* configuration file */
#define ACL_FILE_NAME  "/etc/pcSrv.allow"        /* Access Control List file */
#define INIT_FILE_NAME "/etc/pcSrv.init"              /* Initialization file */
/* base name of the process controller server (dflt: process_controller) */
#define SRV_NAME "process_controller"
#define DFLT_USER_NAME "nobody"
#define DFLT_GROUP_NAME "nobody"
#define DFLT_UID 99
#define DFLT_GID 99
/* If !=0 set the period of forced Task Manager update */
#define PC_TM_WATCHER_TIMEOUT 0
/* max number of (re)start times kept for each process for lss/pcLss service */
#define DFLT_START_TIME_MAX_SIZE 50
/* OOM killer control */
#define PC_OOM_ADJ_INODE "/proc/self/oom_adj"
#define PC_OOM_SCORE_ADJ_INODE "/proc/self/oom_score_adj"
#define PC_OOM_ADJ -17
#define PC_OOM_SCORE_ADJ -1000
/*****************************************************************************/
#endif                                                       /* _CPP_PCSRV_H */
/* ######################################################################### */
