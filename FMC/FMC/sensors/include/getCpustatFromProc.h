/*****************************************************************************/
/*
 * $Log: getCpustatFromProc.h,v $
 * Revision 1.7  2008/10/09 09:50:01  galli
 * minor changes
 *
 * Revision 1.6  2007/08/10 13:04:39  galli
 * added function getCpustatSensorVersion()
 *
 * Revision 1.5  2006/10/23 20:18:26  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.4  2004/11/03 22:32:12  galli
 * added version to data structure
 * added cpustatInfo() function
 *
 * Revision 1.3  2004/10/16 15:06:20  galli
 * Logs error to DIM logger
 *
 * Revision 1.2  2004/08/31 14:19:55  galli
 * first working version
 *
 * Revision 1.1  2004/08/28 10:59:48  galli
 * Initial revision
 * */
/*****************************************************************************/
struct cpuVal
{
  float user;                     /* normal processes executing in user mode */
  float nice;                      /* niced processes executing in user mode */
  float system;                        /* processes executing in kernel mode */
  float idle;                                            /* twiddling thumbs */
  float iowait;                               /* waiting for I/O to complete */
  float irq;                                         /* servicing interrupts */
  float softirq;                                       /* servicing softirqs */
};
struct cpusVal
{
  int success;                              /* 1 if structure in meaningfull */
  unsigned cpuN;                /* number of CPUs (including HT) in the node */
  struct cpuVal *vals;
  float ctxtRate;       /* rate of context switches across all CPUs [ctxt/s] */
  char *version;
};
/*****************************************************************************/
struct cpusVal readCpu(int deBug,int errU);
char *cpustatInfo(char *what);
char *getCpustatSensorVersion(void);
/*****************************************************************************/
