/* ######################################################################### */
/*
 * $Log: getCpuinfoFromProc.h,v $
 * Revision 1.9  2008/10/22 07:10:33  galli
 * minor changes
 *
 * Revision 1.8  2008/10/21 11:25:12  galli
 * get also parameters core_id & cpu_cores
 *
 * Revision 1.5  2008/01/31 23:35:53  galli
 * all cpuInfo struct elements are char* (to accomodate "N/A")
 *
 * Revision 1.4  2007/08/10 12:35:32  galli
 * added function getCpuinfoSensorVersion()
 *
 * Revision 1.3  2006/10/23 20:19:07  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.2  2004/11/03 22:31:13  galli
 * added version to data structure
 *
 * Revision 1.1  2004/10/17 09:26:30  galli
 * Initial revision
 *
 * */
/* ######################################################################### */
#ifndef _GET_CPUINFO_FROM_PROC_H
#define _GET_CPUINFO_FROM_PROC_H 1
/* ######################################################################### */
#define CPU_INFO_FILE_NAME "/proc/cpuinfo"
/*---------------------------------------------------------------------------*/
struct cpuInfo
{
  char *vendor_id;
  char *cpu_family;
  char *model;
  char *model_name;
  char *stepping;
  char *cpu_MHz;
  char *kb_cache_size;
  char *physical_id;
  char *siblings;
  char *core_id;
  char *cpu_cores;
  char *bogomips;
};
struct cpusInfo
{
  int success;                              /* 1 if structure in meaningfull */
  unsigned cpuN;                /* number of CPUs (including HT) in the node */
  struct cpuInfo *info;
  char *version;
};
/* ######################################################################### */
#ifdef __cplusplus
extern "C" {
#endif                                                        /* __cplusplus */
/* ######################################################################### */
struct cpusInfo readCpuinfo(int deBug,int errU);
char *getCpuinfoSensorVersion(void);
/* ######################################################################### */
#ifdef __cplusplus
}
#endif                                                        /* __cplusplus */
/* ######################################################################### */
#endif                                           /* _GET_CPUINFO_FROM_PROC_H */
/* ######################################################################### */
