/* ######################################################################### */
/*
 * $Log: cmSrv.H,v $
 * Revision 1.4  2008/10/05 22:46:04  galli
 * removed definition of MAX_FILENAME_LEN
 *
 * Revision 1.3  2008/10/03 22:04:12  galli
 * added definition for SRV_NAME
 * removed definitions for MSG_LEN, SVC_LEN and ERR_MSG_LEN
 *
 * Revision 1.2  2008/10/02 07:33:15  galli
 * added def for CONF_FILE_NAME and ACL_FILE_NAME
 *
 * Revision 1.1  2008/10/01 11:00:35  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _CM_SRV_H
#define _CM_SRV_H 1
/* ######################################################################### */
/* hardcoded parameters for cmSrv, cmSet and cmGet */
#define SRV_NAME "config_manager"
#define RPC_TIME_OUT 5
#define BUF_CHUNK_SZ 1024                         /* for stdin read in cmSet */
#define CONF_FILE_NAME "/etc/cmSrv.conf"               /* configuration file */
#define ACL_FILE_NAME  "/etc/cmSrv.allow"        /* Access Control List file */
/* ######################################################################### */
#endif                                                          /* _CM_SRV_H */
/* ######################################################################### */
