/* ######################################################################### */
/*
 * $Log: memSrv.h,v $
 * Revision 3.0  2011/11/21 12:29:27  galli
 * Compatible with memSrv.c version 3.0
 *
 * Revision 1.2  2009/02/26 20:23:15  galli
 * definitions DIM_SRV_LEN DIM_SVC_LEN moved to fmcPar.h
 *
 * Revision 1.1  2009/01/08 09:44:21  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _MEM_SRV_H
#define _MEM_SRV_H 1
/* ######################################################################### */
/* Common to SERVER and CLIENT */
/* DIM server basename */
#define SRV_NAME "memory"
/* ######################################################################### */
/* Server only */
#ifdef _SERVER
/* maximim length of the name of the DIM server */
char *sumLabelV[4]={"MemTotal","MemFree","SwapTotal","SwapFree"};
char *sumUnitsV[4]={"KiB","KiB","KiB","KiB"};
/* ------------------------------------------------------------------------- */
/* Function Prototype */
static void logData(int detFieldN,char *detLabelBuf,struct memVal mems);
static void signalHandler(int signo);
void dimErrorHandler(int severity,int errorCode,char *message);
void usage(int mode);
void shortUsage(void);
/* ------------------------------------------------------------------------- */
#endif                                                            /* _SERVER */
/* ######################################################################### */
/* Client only */
#ifdef _CLIENT
/* number of fields which have to be set to "N/F" if server unreachable */
#define DET_FIELD_N_ERR 44
#endif                                                            /* _CLIENT */
/* ######################################################################### */
#endif                                                         /* _MEM_SRV_H */
/* ######################################################################### */
