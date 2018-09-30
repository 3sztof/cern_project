/* ######################################################################### */
/*
 * $Log: fmcPar.h,v $
 * Revision 1.5  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.4  2009/01/29 11:27:55  galli
 * DIM_SVC_LEN increased from 127 to 511
 *
 * Revision 1.3  2009/01/29 07:54:51  galli
 * added DIM_SRV_LEN and DIM_SVC_LEN
 *
 * Revision 1.2  2008/11/13 10:44:54  galli
 * added UTGID_LEN
 *
 * Revision 1.1  2008/10/14 11:57:50  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _FMC_PAR_H
#define _FMC_PAR_H 1
/* ######################################################################### */
/* All constant *_LEN, if referred to strings should be the length of the    */
/* string EXCLUDING the terminating \0                                       */
/* ######################################################################### */
#define DIM_CONF_FILE_NAME "/etc/sysconfig/dim"    /* DIM configuration file */
/* DIM DNS service providing the DIM server list */
#define DIM_DNS_SRV_LIST "DIS_DNS/SERVER_LIST"
/* max size of file /proc/cpuinfo */
#define PROC_CPUINFO_SIZE 16384
/* max length of the UTGID string */
#define UTGID_LEN 255
/* max length of a host name */
#define HOSTNAME_LEN (HOST_NAME_MAX-1)
/* maximum length of the name of the DIM server */
#define DIM_SRV_LEN 63
/* maximum length of the name of a DIM service (75 on 2009.01.29) */
#define DIM_SVC_LEN 511
/* ######################################################################### */
#endif                                                         /* _FMC_PAR_H */
/* ######################################################################### */
