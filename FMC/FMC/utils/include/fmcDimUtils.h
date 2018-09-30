/* ######################################################################### */
/*
 * $Log: fmcDimUtils.h,v $
 * Revision 1.4  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.3  2008/10/23 08:04:18  galli
 * minor changes
 *
 * Revision 1.1  2007/09/20 07:25:22  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _FMC_DIM_UTILS_H
#define _FMC_DIM_UTILS_H 1
/* ######################################################################### */
#ifdef __cplusplus
extern "C" {
#endif                                                        /* __cplusplus */
/* ######################################################################### */
char *getDimDnsNode(const char *dimCfgFileName,int errU,int deBug);
char *getFmcDimUtilsVersion();
/* ######################################################################### */
#ifdef __cplusplus
}
#endif                                                        /* __cplusplus */
/* ######################################################################### */
#endif                                                   /* _FMC_DIM_UTILS_H */
/* ######################################################################### */
