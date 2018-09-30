/* ######################################################################### */
/*
 * $Log: osSrv.h,v $
 * Revision 1.5  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.4  2009/02/26 20:22:13  galli
 * definitions DIM_SRV_LEN DIM_SVC_LEN moved to fmcPar.h
 *
 * Revision 1.3  2008/10/22 07:02:37  galli
 * minor changes
 *
 * Revision 1.1  2008/10/21 23:28:09  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _OS_SRV_H
#define _OS_SRV_H 1
/* ######################################################################### */
/* common */
/* DIM server basename */
#define SRV_NAME "os"
/*---------------------------------------------------------------------------*/
/* server */
/* labels to be published */
const char *labelV[7]={"name","distribution","kernel","release","machine",
                       "lastBootUpTime","localDateTime"};
/*---------------------------------------------------------------------------*/
/* client */
/* number of fields which have to be set to "N/F" if server unreachable */
#define FIELD_N_ERR 7
/* ######################################################################### */
#endif                                                          /* _OS_SRV_H */
/* ######################################################################### */
