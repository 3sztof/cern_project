/*****************************************************************************/
/*
 * $Log: getMemFromProc.h,v $
 * Revision 2.0  2011/11/11 15:03:26  galli
 * Added prototype for funcion vp()
 *
 * Revision 1.6  2007/08/10 13:30:41  galli
 * added function getMemSensorVersion()
 *
 * Revision 1.5  2006/10/23 20:16:42  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.4  2005/11/18 09:43:46  galli
 * compatible with getMemFromProc.c 1.5
 *
 * Revision 1.3  2004/11/03 22:36:03  galli
 * added version to data structure
 *
 * Revision 1.2  2004/08/10 12:57:47  galli
 * first working version
 *
 * Revision 1.1  2004/08/10 07:30:50  galli
 * Initial revision
 * */
/*****************************************************************************/
struct memVal
{
  int success;
  char *version;
  unsigned long *data;
};
/*****************************************************************************/
struct memVal readMem(int deBug,int errU);
int getItemsN(void);
int getItemsSz(void);
char *getItems(void);
char *getMemSensorVersion(void);
unsigned long *vp(char *key);
/*****************************************************************************/
