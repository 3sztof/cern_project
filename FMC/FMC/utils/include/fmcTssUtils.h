/* ######################################################################### */
/*
 * $Log: fmcTssUtils.h,v $
 * Revision 1.3  2009/01/29 08:59:05  galli
 * minor changes
 *
 * Revision 1.1  2008/10/09 07:49:58  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _FMC_TSS_UTILS_H
#define _FMC_TSS_UTILS_H 1
/* ######################################################################### */
#define TSS_LEN 23
/* ######################################################################### */
#ifdef __cplusplus
extern "C" {
#endif
/* ######################################################################### */
char *getTssString(float tss);
char *getFmcTssUtilsVersion();
/* ######################################################################### */
#ifdef __cplusplus
}
#endif
/* ######################################################################### */
#endif                                                   /* _FMC_TSS_UTILS_H */
/* ######################################################################### */
