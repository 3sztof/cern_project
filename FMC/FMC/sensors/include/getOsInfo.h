/* ######################################################################### */
/*
 * $Log: getOsInfo.h,v $
 * Revision 1.3  2008/10/22 07:14:11  galli
 * minor changes
 *
 * Revision 1.1  2008/01/22 16:33:50  galli
 * Initial revision
 * */
/* ######################################################################### */
#ifndef _GET_OSINFO_FROM_PROC_H
#define _GET_OSINFO_FROM_PROC_H 1
/* ######################################################################### */
#define RELEASE_FILE_NAME "/etc/redhat-release"
#define UPTIME_FILE_NAME "/proc/uptime"
#define UPTIME_FILE_SIZE 64
#define TIME_STR_LEN 64
/* ------------------------------------------------------------------------- */
struct osInfo
{
  int success;                              /* 1 if structure in meaningfull */
  char *distribution; /* "Scientific Linux CERN SLC release 4.6 (Beryllium)" */
  char *name;                                                /* e.g. "Linux" */
  char *kernel;                           /* e.g. "2.6.9-55.0.12.EL.cernsmp" */
  char *release;                /* e.g. "#1 SMP Mon Nov 5 17:20:17 CET 2007" */
  char *machine;                                    /* e.g. "x86_64", "i686" */
};
/* ------------------------------------------------------------------------- */
struct timeInfo
{
  time_t now;
  time_t upTime;
  time_t upTimeIdle;
};
/* ######################################################################### */
#ifdef __cplusplus
extern "C" {
#endif                                                        /* __cplusplus */
/* ######################################################################### */
struct osInfo readOsInfo(int deBug,int errU);
struct timeInfo readTimeInfo(int deBug,int errU);
char *getRedhatRelease(int errU);
char *getOsInfoSensorVersion(void);
/* ######################################################################### */
#ifdef __cplusplus
}
#endif                                                        /* __cplusplus */
/* ######################################################################### */
#endif                                            /* _GET_OSINFO_FROM_PROC_H */
/* ######################################################################### */
