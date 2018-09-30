/*****************************************************************************/
/*
 * $Log: getNIFinfoFromEthtool.h,v $
 * Revision 1.2  2009/01/29 09:44:45  galli
 * minor changes
 *
 * Revision 1.1  2008/10/09 09:50:43  galli
 * Initial revision
 */
/*****************************************************************************/
#include <inttypes.h>                                              /* PRIu64 */
#include <sys/types.h>
#define ATTR_LEN 63
#define SENSORS_BUFFER_SIZE (10*128)
/*****************************************************************************/
typedef struct nifAttr
{
  char *version;
  int success;
  int exitStatus;
  char errorString[SENSORS_BUFFER_SIZE];
  char speed[ATTR_LEN+1];
  char duplex[ATTR_LEN+1];
  char portType[ATTR_LEN+1];
  char autoNegotiation[ATTR_LEN+1];
  char linkDetected[ATTR_LEN+1];
}nifAttr_t;
/*****************************************************************************/
nifAttr_t getNIFinfo(char *ethtoolCmd,char *ifName,int deBug,int errU);
char *getNIFinfoSensorVersion(void);
/*****************************************************************************/
