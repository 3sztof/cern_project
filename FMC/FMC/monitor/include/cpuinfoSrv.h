/* ######################################################################### */
/*
 * $Log: cpuinfoSrv.h,v $
 * Revision 1.3  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.2  2009/01/29 08:05:21  galli
 * minor changes
 *
 * Revision 1.1  2008/10/22 07:39:58  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _CPU_INFO_SRV_H
#define _CPU_INFO_SRV_H 1
/* ######################################################################### */
/* common */
/* DIM server basename */
#define SRV_NAME "cpu/info"
/*---------------------------------------------------------------------------*/
/* server */
/* maximim length of the name of a CPU core (e.g.: core_01) */
#define DEVICE_LEN 15
/* labels and units to be published */
const char *labelV[14]={"vendor id","family","model","model name","stepping",
                        "clock","cache size","hyper-thread","multi-core",
                        "physical id","siblings","core id","cpu cores",
                        "performance"};
const char *unitV[14]={"","","","","","MHz","KiB","","","","","","","bogomips"};
/*---------------------------------------------------------------------------*/
/* client */
/* nr of device fields which have to be set to "N/F" if server unreachable */
#define DEV_FIELD_N_ERR 1
/* nr of data fields which have to be set to "N/F" if server unreachable */
#define DATA_FIELD_N_ERR 11
/* ######################################################################### */
#endif                                                    /* _CPU_INFO_SRV_H */
/* ######################################################################### */
