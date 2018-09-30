/* ######################################################################### */
/*
 * $Log: fmcProcNameUtils.h,v $
 * Revision 1.4  2009/01/29 11:11:33  galli
 * minor changes
 *
 * Revision 1.1  2008/02/22 08:29:00  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _FMC_PROC_NAME_UTILS_H
#define _FMC_PROC_NAME_UTILS_H 1
/* ######################################################################### */
#define PATH_BUF_LEN 1023
/* ######################################################################### */
#ifdef __cplusplus
extern "C" {
#endif
/* ######################################################################### */
char *getProcName(int errU);
char *getFmcProcNameUtilsVersion();
/* ######################################################################### */
#ifdef __cplusplus
}
#endif                                                        /* __cplusplus */
/* ######################################################################### */
#endif                                             /* _FMC_PROC_NAME_UTILS_H */
/* ######################################################################### */
