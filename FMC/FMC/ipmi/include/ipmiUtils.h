/* ######################################################################### */
/*
 * $Log: ipmiUtils.h,v $
 * Revision 1.9  2008/12/02 15:37:41  galli
 * ipmiSrv can read passwd from file at run-time
 *
 * Revision 1.8  2008/10/08 14:48:05  galli
 * minor changes
 *
 * Revision 1.7  2007/10/15 13:49:33  galli
 * added error IPMI_NOT_AVAIL
 *
 * Revision 1.6  2007/08/02 14:54:02  galli
 * added authType, privLvl and oemType in function arguments
 *
 * Revision 1.4  2006/01/14 11:02:23  galli
 * ipmitool error codes introduced
 *
 * Revision 1.3  2005/04/09 07:57:59  galli
 * last version before global suppression
 *
 * Revision 1.2  2005/03/30 22:25:01  galli
 * first working version
 *
 * Revision 1.1  2005/03/30 22:18:31  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _IPMI_UTILS_H
#define _IPMI_UTILS_H 1
/* ######################################################################### */
/* sensor/actuator functions in ipmiUtils.c */
char *getSensorsList(char *hostName,char *userName,char *passWord,
                     char *passWordFile,int port,char *authType,char *privLvl,
                     char *oemType,char *sensorType);
int getPowerStatus(char *hostName,char *userName,char *passWord,
                   char *passWordFile,int port,char *authType,char *privLvl,
                   char *oemType);
char *getSensorsStatus(char *hostName,char *userName,char *passWord,
                       char *passWordFile,int port,char *authType,
                       char *privLvl,char *oemType);
int powerSwitchActuate(char *hostName,char *userName,char *passWord,
                       char *passWordFile,int port,char *authType,
                       char *privLvl,char *oemType,unsigned char ctl);
char *getActuatorVersion();
/* ######################################################################### */
/* error codes */
#define IPMI_OK                    0
#define IPMI_NOT_YET_AVAIL        -128
#define IPMI_SENSOR_UNREADABLE    -127
#define IPMI_INVALID_RESP         -126
#define IPMI_NOT_RESP             -125
#define IPMI_NOT_AVAIL            -124
/*---------------------------------------------------------------------------*/
/*                                          1         2         3            */
/*                                 123456789012345678901234567890            */
#define IPMI_OK_S                 "[IPMI OK]"
#define IPMI_NOT_YET_AVAIL_S      "[IPMI NOT YET AVAIL]"
#define IPMI_SENSOR_UNREADABLE_S  "[IPMI SENSOR UNREADABLE]"
#define IPMI_INVALID_RESP_S       "[IPMI INVALID RESP]"
#define IPMI_NOT_RESP_S           "[IPMI NOT RESP]"
#define IPMI_NOT_AVAIL_S          "[IPMI NOT AVAIL]"
/*---------------------------------------------------------------------------*/
/*                                          1         2         3            */
/*                                 123456789012345678901234567890            */
#define IPMI_OK_SL                "[ok]"
#define IPMI_NOT_YET_AVAIL_SL     "[IPMI values not yet available]"
#define IPMI_SENSOR_UNREADABLE_SL "[IPMI sensor unreadable]"
#define IPMI_INVALID_RESP_SL      "[invalid IPMI response]"
#define IPMI_NOT_RESP_SL          "[IPMI not responding]"
#define IPMI_NOT_AVAIL_SL         "[IPMI values not available]"
/* ######################################################################### */
#endif                                                      /* _IPMI_UTILS_H */
/* ######################################################################### */
