/* ######################################################################### */
/*
 * $Log: fmcFileUtils.h,v $
 * Revision 1.2  2008/10/23 08:04:18  galli
 * minor changes
 *
 * Revision 1.1  2008/10/18 15:31:57  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _FMC_FILE_UTILS_H
#define _FMC_FILE_UTILS_H 1
/* ######################################################################### */
#ifdef __cplusplus
extern "C" {
#endif                                                        /* __cplusplus */
/* ######################################################################### */
size_t fmcFile2str(char **buf, size_t *bufSz, size_t chunkSz, char *filePath,
                   int errU, int deBug);
char *getFmcFileUtilsVersion();
/* ######################################################################### */
#ifdef __cplusplus
}
#endif                                                        /* __cplusplus */
/* ######################################################################### */
#endif                                                   /*_FMC_FILE_UTILS_H */
/* ######################################################################### */
