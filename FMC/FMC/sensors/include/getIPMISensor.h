/*****************************************************************************/
/*
 * $Log: getIPMISensor.h,v $
 * Revision 1.1  2004/11/03 21:15:24  galli
 * Initial revision
 * */
/*****************************************************************************/
struct tempVal
{	
  char name[128];
  int input;
  char status[128];
};
struct fanVal
{
  char name[128];
  int input;
  char status[128];
};
struct batteryVal
{
  char name[128];
  float input;
  char status[128];
};
struct sensorsVal
{
  int success;
  char *version;
  int nTemps;
  int nFans;
  int nBatteries;
  struct tempVal temp[128];
  struct fanVal fan[128];
  struct batteryVal batt[128];
};
struct sensorsVal readIPMISensor(void);
/*****************************************************************************/
