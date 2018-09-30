/*****************************************************************************/
/*
 * $Log: getCoalescence.h,v $
 * Revision 1.6  2008/10/09 09:50:01  galli
 * minor changes
 *
 * Revision 1.5  2007/08/10 12:52:31  galli
 * added function getCoalSensorVersion()
 *
 * Revision 1.4  2006/10/21 06:02:05  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 1.3  2004/11/17 00:39:45  galli
 * time since started added to coalescenceFactors structure
 *
 * Revision 1.2  2004/11/08 15:09:46  galli
 * publishes sensor version
 *
 * Revision 1.1  2004/11/03 22:30:17  galli
 * Initial revision
 *
 * */
/*****************************************************************************/
struct coalescenceFactor
{
  char *ifName;                              /* names of Ethernet Interfaces */
  float coalFactor;
  float aCoalFactor;
  float mCoalFactor;
};
struct coalescenceFactors
{
  int success;
  char *version;
  char *stss;
  unsigned ethN;                                   /* number of Ethernet ifs */
  struct coalescenceFactor *data;
};
/*****************************************************************************/
struct coalescenceFactors
getCoal(struct nifsData nifs,struct interruptsStat ints,int deBug,int errU);
void resetCoal();
char *coalInfo(char *what);
char *getCoalSensorVersion(void);
/*****************************************************************************/
