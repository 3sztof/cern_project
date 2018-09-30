/*****************************************************************************/
/*
 * $Log: getIrqFromProc.h,v $
 * Revision 1.9  2008/10/09 09:50:01  galli
 * minor changes
 *
 * Revision 1.8  2007/08/10 13:13:20  galli
 * added function getIRQsensorVersion()
 *
 * Revision 1.7  2006/10/23 20:17:38  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.6  2004/11/17 00:04:23  galli
 * time since started added to interruptStat structure
 *
 * Revision 1.5  2004/11/03 22:36:35  galli
 * added version to data structure
 *
 * Revision 1.4  2004/09/18 21:59:42  galli
 * added interruptInfo() function prototype
 *
 * Revision 1.3  2004/09/14 12:05:02  galli
 * added average and maximum values
 *
 * Revision 1.2  2004/09/02 12:39:50  galli
 * first working version
 *
 * Revision 1.1  2004/09/01 17:56:49  galli
 * Initial revision
 * */
/*****************************************************************************/
struct interruptsStat
{
  int success;                              /* 1 if structure in meaningfull */
  char *version;
  char *stss;                              /* time since start or last reset */
  unsigned cpuN;                /* number of CPUs (including HT) in the node */
  unsigned srcN;                              /* number of interrupt sources */
  char **srcName;                              /* names of interrupt sources */
  int *irqN;                                                  /* irq numbers */
  float **irqRate;
  float **aIrqRate;
  float **mIrqRate;
};
/*****************************************************************************/
struct interruptsStat readInterrupts(int deBug,int errU);
void resetInterrupts();
char *interruptInfo(char *what,int errU);
char *getIRQsensorVersion();
/*****************************************************************************/
