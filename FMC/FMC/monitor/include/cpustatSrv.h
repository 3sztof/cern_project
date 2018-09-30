/* ######################################################################### */
/*
 * $Log: cpustatSrv.h,v $
 * Revision 1.4  2009/01/29 08:08:03  galli
 * minor changes
 *
 * Revision 1.1  2009/01/07 11:55:30  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _CPU_STAT_SRV_H
#define _CPU_STAT_SRV_H 1
/* ######################################################################### */
/* common */
/* DIM server basename */
#define SRV_NAME "cpu/stat"
/*---------------------------------------------------------------------------*/
/* server */
/* maximim length of the name of a CPU core (e.g.: core_01) */
#define DEVICE_LEN 15
/* labels to be published */
char *ctxtLabelV[1]={"ctxt_rate"};
char *ctxtUnitsV[1]={"Hz"};
char *loadLabelV[7]={"user","system","nice","idle","iowait","irq","softirq"};
char *loadUnitsV[7]={"%","%","%","%","%","%","%"};
/*---------------------------------------------------------------------------*/
/* client */
/* number of fields which have to be set to "N/F" if server unreachable */
#define DEV_FIELD_N_ERR 1
#define CTXT_FIELD_N_ERR 1
#define LOAD_FIELD_N_ERR 7
/* ######################################################################### */
#endif                                                    /* _CPU_STAT_SRV_H */
/* ######################################################################### */
