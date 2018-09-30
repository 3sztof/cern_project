/* ######################################################################### */
/*
 * $Log: ipmiSrv.h,v $
 * Revision 1.2  2008/10/15 14:47:32  galli
 * added definition for DICT_FILE_NAME
 *
 * Revision 1.1  2008/10/08 14:42:20  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _IPMI_SRV_H
#define _IPMI_SRV_H 1
/* ######################################################################### */
/* hardcoded parameters for ipmiSrv, ipmiViewer, pwSwitch and pwStatus       */
#define SRV_NAME "power_manager"
#define CONF_FILE_NAME "/etc/ipmiSrv.conf"             /* configuration file */
#define DICT_FILE_NAME "/etc/ipmi_sensor_dictionary.txt"
#define MSG_SIZE 4000
/* Minimum stack size (in B) allocated for the created threads stack.*/
/* Default 10*1024*1024. Minimum: PTHREAD_STACK_MIN = 16*1024 */
/* If "segfault at ... rip ... rsp ... error 6" must be increased */
#define THREAD_STACK_SIZE (128*1024)
/* ######################################################################### */
#endif                                                        /* _IPMI_SRV_H */
/* ######################################################################### */
