/* ######################################################################### */
/* ipmiSrv.c - IPMI Server                                                   */
/* ######################################################################### */
/*
 * $Log: ipmiSrv.c,v $
 * Revision 3.32  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 3.31  2009/10/03 15:33:04  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 3.28  2008/12/02 15:38:55  galli
 * ipmiSrv can read passwd from file at run-time
 *
 * Revision 3.27  2008/10/31 10:05:57  galli
 * bug fixed
 *
 * Revision 3.20  2008/10/10 15:51:17  galli
 * improved manual
 *
 * Revision 3.19  2008/10/10 12:56:45  galli
 * groff manual
 *
 * Revision 3.16  2008/10/09 12:02:07  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 * signal_number_to_name() replaced by sig2msg() in libFMCutils
 *
 * Revision 3.14  2008/10/08 16:42:00  galli
 * Does not try to control itself
 *
 * Revision 3.13  2008/10/08 16:29:22  galli
 * prefix "/FMC" in published SVCs and CMDs
 *
 * Revision 3.12  2008/10/08 16:20:07  galli
 * Discards duplicated records in configuration file
 * If no node configured remains in standby
 *
 * Revision 3.10  2008/03/05 13:29:56  galli
 * exit(1) if no node configured
 *
 * Revision 3.9  2008/03/04 23:34:37  galli
 * Empty configuration file exception handled
 *
 * Revision 3.5  2007/10/26 12:17:21  galli
 * publishes fmc_version
 *
 * Revision 3.4  2007/10/23 16:08:32  galli
 * usage() prints FMC version
 *
 * Revision 3.3  2007/10/15 12:26:40  galli
 * improved check of IPMI sensor records
 * if no sensors of some kind publishes [IPMI values not available]
 *
 * Revision 3.2  2007/10/12 15:19:38  galli
 * cmd-line option --get-try-n and --cmd-try-n to set the number of tries
 *
 * Revision 3.1  2007/10/10 12:29:48  galli
 * THREAD_STACK_SIZE increased
 *
 * Revision 3.0  2007/10/05 13:22:36  galli
 * thread number limit settable
 * -t,--max-threads-number CMD-line option to set a thread number limit
 * --min-period, --max-period and --sensor-period CMD-line option to set
 * update periods.
 * powerSwitchHandler() enqueue commands in a global queue
 * powerSwitchStartCmds() dequeue commands form the main thread
 *
 * Revision 2.25  2007/09/18 09:27:06  galli
 * bug fixed
 *
 * Revision 2.23  2007/08/10 12:20:16  galli
 * bug fixed
 *
 * Revision 2.21  2007/08/09 21:30:01  galli
 * compatible with libFMCutils v 2
 *
 * Revision 2.19  2007/08/03 12:22:59  galli
 * added Current sensors
 *
 * Revision 2.18  2007/08/03 08:58:39  galli
 * authType, privLvl and oemType checked
 *
 * Revision 2.17  2007/08/03 08:11:34  galli
 * usage() updated
 *
 * Revision 2.15  2007/08/02 14:55:47  galli
 * added userName, passWord, port, authType, privLvl, oemType in the
 * per-node configuration
 *
 * Revision 2.9  2006/10/20 15:28:39  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 2.8  2006/03/02 07:02:49  galli
 * sensors dictionary written to file and read from file
 * sensors dictionary discovery using -d option
 *
 * Revision 2.7  2006/02/24 15:28:21  galli
 * sensor selection based on ipmi query in init phase
 *
 * Revision 2.6  2006/02/22 12:42:12  galli
 * temperature is read and published as float
 *
 * Revision 2.5  2006/02/16 10:44:09  galli
 * more flexible sensor selection
 *
 * Revision 2.4  2006/02/10 14:20:01  galli
 * added timestamps for sensor readings
 *
 * Revision 2.3  2006/02/10 12:22:32  galli
 * added timestamp of power status
 *
 * Revision 2.2  2006/02/09 12:11:36  galli
 * x86_64 compatibility problems fixed
 * stacks of new threads decreased
 * receives signals waiting for cm configuration
 *
 * Revision 2.1  2006/01/26 14:37:23  galli
 * clean-up
 *
 * Revision 2.0  2006/01/26 07:32:09  galli
 * new single access BMC arbitration
 *
 * Revision 1.28  2006/01/20 07:53:41  galli
 * added ipmi fan and voltage sensors
 *
 * Revision 1.26  2006/01/17 16:04:02  galli
 * added ipmi temperature sensors
 *
 * Revision 1.23  2006/01/10 10:44:55  galli
 * bug fixed
 *
 * Revision 1.22  2006/01/08 22:15:44  galli
 * uses SIGHUP instead of condition variable to signal the main thread the
 * need of refresh.
 * processes signals synchronously
 *
 * Revision 1.21  2006/01/08 21:39:22  galli
 * signalHandler() removes commands
 * debug level can be increased both by cmd line option and env variable
 *
 * Revision 1.20  2006/01/08 00:10:21  galli
 * bug fixed
 *
 * Revision 1.19  2006/01/05 15:55:31  galli
 * can read configuration both from file and from cmSrv
 *
 * Revision 1.17  2005/05/09 22:41:21  galli
 * recognize comments in config file
 * skip illegal lines in config file
 *
 * Revision 1.16  2005/05/04 22:08:50  galli
 * reads user & passwd from environment
 *
 * Revision 1.15  2005/05/04 20:50:56  galli
 * cleanup
 * CMD: on, off, cycle, soft_off, hard_reset, pulse_diag
 *
 * Revision 1.14  2005/05/04 20:40:08  galli
 * function powerCtlHandlerCmd() added
 * powerSwitchHandler() calls powerCtlHandlerCmd()
 * instead of powerOnHandlerCmd() and powerOffHandlerCmd()
 *
 * Revision 1.11  2005/04/19 11:24:20  galli
 * read configuration from file /etc/ipmi.list
 *
 * Revision 1.10  2005/04/18 14:04:52  galli
 * added arbitration among accesses to the same farm node
 *
 * Revision 1.8  2005/04/12 12:22:47  galli
 * added a condition variable for instant update after switch operations
 *
 * Revision 1.7  2005/04/11 22:36:45  galli
 * working version
 * CMD: on, off
 * SVC: status
 *
 * Revision 1.1  2005/04/01 08:31:54  galli
 * Initial revision
 */
/* ######################################################################### */
/*
 * SRV: /FMC/<CTRLPC_HOSTNAME>/<SRV_NAME>
 * SVC: /FMC/<CTRLPC_HOSTNAME>/<SRV_NAME>/success
 * SVC: /FMC/<CTRLPC_HOSTNAME>/<SRV_NAME>/server_version
 * SVC: /FMC/<CTRLPC_HOSTNAME>/<SRV_NAME>/actuator_version
 * SVC: /FMC/<CTRLPC_HOSTNAME>/<SRV_NAME>/fmc_version
 * CMD: /FMC/<HOSTNAME>/power_switch on|off|cycle|soft_off|hard_reset|pulse_diag
 * SVC: /FMC/<HOSTNAME>/power_status
 * SVC: /FMC/<HOSTNAME>/power_status_timestamp
 * SVC: /FMC/<HOSTNAME>/sensors_timestamp
 * SVC: /FMC/<HOSTNAME>/sensors/temp/names
 * SVC: /FMC/<HOSTNAME>/sensors/temp/input
 * SVC: /FMC/<HOSTNAME>/sensors/temp/units
 * SVC: /FMC/<HOSTNAME>/sensors/temp/status
 * SVC: /FMC/<HOSTNAME>/sensors/fan/names
 * SVC: /FMC/<HOSTNAME>/sensors/fan/input
 * SVC: /FMC/<HOSTNAME>/sensors/fan/units
 * SVC: /FMC/<HOSTNAME>/sensors/fan/status
 * SVC: /FMC/<HOSTNAME>/sensors/voltage/names
 * SVC: /FMC/<HOSTNAME>/sensors/voltage/input
 * SVC: /FMC/<HOSTNAME>/sensors/voltage/units
 * SVC: /FMC/<HOSTNAME>/sensors/voltage/status
 * SVC: /FMC/<HOSTNAME>/sensors/current/names
 * SVC: /FMC/<HOSTNAME>/sensors/current/input
 * SVC: /FMC/<HOSTNAME>/sensors/current/units
 * SVC: /FMC/<HOSTNAME>/sensors/current/status
 *
 * sensor status:
 *   cr = critical
 *   nc = non-critical
 *   nr = non-recoverable
 *   ns = not specified (e.g. node switched off)
 *   ok = ok
 *   us = unspecified
*/
/* ######################################################################### */
/*
 * CMD processing:
 * 1 - powerSwitchHandler()
 *     * Started by DIM
 *     * Running in the DIM I/O thread
 *     * Enqueues CMDs in global CMD queue globalQ_t gCmdQ
 * 2 - powerSwitchStartCmds()
 *     * Started by main()
 *     * Running in the DIM Main thread
 *     * Dequeues CMDs found (if any) from the global CMD queue globalQ_t gCmdQ
 *     * If the maximum allowed number of threads is reached, it waits until
 *       the number of threads reduces.
 *     * Create the new short-living threads to execute powerSwitchExecuteCmd()
 * 3 - powerSwitchExecuteCmd()
 *     * Started by powerSwitchStartCmds()
 *     * Running in a short-living thread
 *     * Enqueues CMDs in the per-node queue bmc.cmdQ[node]
 *     * Waits until BMC free and CMD is the first of the queue
 *     * call powerSwitchActuate()
 * 4 - powerSwitchActuate()
 *     * Started by powerSwitchExecuteCmd()
 *     * Running in the same short-living thread
 *     * Starts ipmitool and process its output
 */
/* ######################################################################### */
/* headers */
#include <stdio.h>
#include <unistd.h>                                     /* getopt(), pause() */
#include <string.h>                                 /* strdup(3), strcspn(3) */
#include <errno.h>                                                  /* errno */
#include <argz.h>           /* argz_create_sep(), argz_len(), argz_extract() */
#include <syslog.h>                                              /* syslog() */
#include <stdlib.h>              /* strtol(), strtoul(), exit(3), realloc(3) */
#include <signal.h>                                              /* signal() */
#include <pwd.h>                                               /* getpwnam() */
#include <sys/types.h>          /* getpwnam(3), waitpid(2), stat(2), open(2) */
#include <sys/wait.h>         /* waitpid(), WIFEXITED(), WEXITSTATUS(), etc. */
#include <envz.h>                                              /* envz_add() */
#include <ctype.h>         /* toupper(3), isupper(3), islower(3), isdigit(3) */
#include <pthread.h>          /* pthread_create(), pthread_attr_init(), etc. */
#include <fnmatch.h>                                            /* fnmatch() */
#include <ipmi_chassis.h>                 /* IPMI_CHASSIS_CTL_POWER_UP, etc. */
#include <limits.h>                                    /* LONG_MIN, LONG_MAX */
#include <sys/stat.h>                                    /* stat(2), open(2) */
#include <fcntl.h>                                      /* open(2), O_RDONLY */
#include <getopt.h>                                        /* getopt_long(3) */
#include <sys/wait.h>         /* waitpid(), WIFEXITED(), WEXITSTATUS(), etc. */
/*****************************************************************************/
/* DIM */
#include <dis.h>                  /* dis_add_cmnd(), dis_add_service(), etc. */
/*****************************************************************************/
/* fmc */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcMacro.h"                                             /* eExit() */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
#include "fmcDate.h"                                     /* compilation date */
/*****************************************************************************/
/* fmc utils */
#include "fmcSrvNameUtils.h"                               /* getSrvPrefix() */
#include "fmcMsgUtils.h"           /* mPrintf(), dfltLoggerOpen(), rPrintf() */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "fmcThreadUtils.h"                                  /* getThreadN() */
#include "fmcSigUtils.h"                                        /* sig2msg() */
/*****************************************************************************/
/* power manager */
#include "ipmiSrv.h"                             /* SRV_NAME, CONF_FILE_NAME */
#include "ipmiUtils.h"       /* getPowerStatus(), powerSwitchActuate(), etc. */
/* ######################################################################### */
/* macro to free-up m-allocated thread strctures */
#define FREE_PCD \
do{ \
  if(pcd) \
  { \
    free(pcd); \
    pcd=NULL; \
  } \
}while(0)
/*---------------------------------------------------------------------------*/
#define thReturn(text) \
do{ \
  mPrintf(errU,ERROR,__func__,0,"%s: %s!",text,strerror(errno)); \
  if(pcd) \
  { \
    free(pcd); \
    pcd=NULL; \
  } \
  return; \
}while(0)
/* ######################################################################### */
/* structure definition */
typedef struct ipmiConf
{
  int nodeN;
  char **hostNameList;
  char **userNameList;
  char **passWordList;
  long *portList;
  char **authTypeList;
  char **privLvlList;
  char **oemTypeList;
}ipmiConf_t;
/*---------------------------------------------------------------------------*/
typedef struct powerCtrlData                                  /* 780 bytes */
{
  int nodeN;
  char hostName[128];
  char userName[128];
  char passWord[128];
  char passWordFile[128];
  int port;
  char authType[128];
  char privLvl[128];
  char oemType[128];
  unsigned char ctl;
}powerCtrlData_t;
/*---------------------------------------------------------------------------*/
/* BMC command fifo element                                                  */
typedef struct bmcQe
{
  pthread_t tid;
  struct bmcQe *next;
}bmcQe_t;
/* BMC command fifo                                                          */
typedef struct bmcQ
{
  bmcQe_t *head;
  bmcQe_t *tail;
}bmcQ_t;
/*---------------------------------------------------------------------------*/
/* global command fifo element                                               */
typedef struct globalQe
{
  int nodeN;
  unsigned char cmdN;
  struct globalQe *next;
}globalQe_t;
/* global command fifo                                                       */
typedef struct globalQ
{
  pthread_mutex_t mutex;
  globalQe_t *head;
  globalQe_t *tail;
}globalQ_t;
/*---------------------------------------------------------------------------*/
/* structure that associates a service to a service type */
typedef struct sc
{
  char *name;
  char *type;
}sc_t;
/*---------------------------------------------------------------------------*/
typedef struct ic
{
  pthread_mutex_t mutex;
  int c;
}ic_t;
/*---------------------------------------------------------------------------*/
/* mutex & condition variable to arbitrate among threads which are accessing */
/* the BMC on the same host                                                  */
/* busy[ipmiCfg.nodeN]:     array containing for each node:                  */
/*                          0: no access to the BMC in progress              */
/*                          1: BMC busy                                      */
/* cmdQ[ipmiCfg.nodeN]:     queue containing the thread identifiers of the   */
/*                          power commands waiting for the access to the BMC */
/*                          (to preserve the execution order)                */
/* pwSvcQ[ipmiCfg.nodeN]:   queue containing the thread identifiers of the   */
/*                          power services handler waiting for the access to */
/*                          the BMC (to preserve the execution order and     */
/*                          avoid thread pile-up)                            */
/* sensSvcQ[ipmiCfg.nodeN]: queue containing the thread identifiers of the   */
/*                          sensor services handler waiting for the access   */
/*                          to the BMC (to preserve the execution order and  */
/*                          avoid thread pile-up)                            */
typedef struct
{
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int *busy;
  bmcQ_t *cmdQ;
  bmcQ_t *pwSvcQ;
  bmcQ_t *sensSvcQ;
}bmcSignal_t;
/*---------------------------------------------------------------------------*/
/* mutex to arbitrate access to status array */
typedef struct
{
  pthread_mutex_t mutex;
  /* array containing for each node: 0=off, 1=on */
  int *status;
  /* time stamp of status read. 32 bit. Problem foreseen in year 2038 */
  unsigned *timeStamp;
}powerStatus_t;
/*---------------------------------------------------------------------------*/
/* sensor dictionary and mutex to arbitrate its access */
typedef struct
{
  pthread_mutex_t mutex;
  int sensDicN;
  sc_t *sensDic;
  char **status;
  /* time stamp of sensors read. 32 bit. Problem foreseen in year 2038 */
  unsigned *timeStamp;
}sensors_t;
/* ######################################################################### */
/* global variables */
int deBug=0;                                     /* debug level, see usage() */
/* maximum number of command attempts before to give-up */
int maxCmdAttemptN=10;
/* maximum number of attempts in getting state before to give-up */
int maxGetAttemptN=5;
/* error unit */
int errU=L_DIM;
sigset_t signalMask;
/* string containing the ipmi configuration to parse */
char *ipmiCfgString=NULL;
/* ipmi configuration (number and list of nodes controlled by this Ctrl PC) */
ipmiConf_t ipmiCfg;
/* DIM SVC/CMD identifiers, passed from main() to other functions */
int *pwStatSvcID=NULL;
int *pwStatTSSvcID=NULL;
int *pwSwitchCmdID=NULL;
int *sensorsTSSvcID=NULL;
int *tempNamesSvcID=NULL;
int *tempInputSvcID=NULL;
int *tempUnitsSvcID=NULL;
int *tempStatusSvcID=NULL;
int *fanNamesSvcID=NULL;
int *fanInputSvcID=NULL;
int *fanUnitsSvcID=NULL;
int *fanStatusSvcID=NULL;
int *voltageNamesSvcID=NULL;
int *voltageInputSvcID=NULL;
int *voltageUnitsSvcID=NULL;
int *voltageStatusSvcID=NULL;
int *currentNamesSvcID=NULL;
int *currentInputSvcID=NULL;
int *currentUnitsSvcID=NULL;
int *currentStatusSvcID=NULL;
int versionSvcID=0;
int aVersionSvcID=0;
int fmcVersionSvcID=0;
int successSvcID=0;
/* thread identifier of the main thread */
pthread_t main_tid;
/* sleep time to cope with IPMI firmware problem */
int ipmiCtlSleepTime=1;
/* dummy */
int success=1;
/* variables related with threads created                                    */
pthread_t tid;
pthread_attr_t t_attr;
/* global CDM queue */
globalQ_t gCmdQ={PTHREAD_MUTEX_INITIALIZER,NULL,NULL};
/* mutex & condition variable to arbitrate among threads which are accessing */
/* the BMC on the same host                                                  */
bmcSignal_t bmc={PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER,NULL,NULL,
                 NULL,NULL};
/* mutex to arbitrate access to status array */
powerStatus_t pwStatus={PTHREAD_MUTEX_INITIALIZER,NULL,NULL};
sensors_t sensors={PTHREAD_MUTEX_INITIALIZER,0,NULL,NULL,NULL};
ic_t sensInitCount={PTHREAD_MUTEX_INITIALIZER,0};
/* derfault IPMI related variables */
int dfltIpmiPort=623;
char *ipmiCmd=NULL;
char *dfltIpmiUserName=NULL;
char *dfltIpmiPassWord=NULL;
char *passWordFile=NULL;
/* versions */
static char rcsid[]="$Id: ipmiSrv.c,v 3.32 2012/11/29 15:52:27 galli Exp galli $";
char *aRcsid;
char *rcsidP=NULL;
char *aRcsidP=NULL;
/* thread number and maximum thread number */
int threadN=0;
int maxThreadN=0;
/* update periods */
float updateMaxPeriod=30.0;                       /* maximum update period */
float updateMinPeriod=4.0;                        /* minimum update period */
const float updateIncPeriod=20;       /* percent increment in updatePeriod */
float sensorsUpdatePeriod=60.0;
float updatePeriod=4.0;                           /* current update period */
/* ######################################################################### */
/* function prototype */
/*---------------------------------------------------------------------------*/
/* DIM server CMD handler (executed in the DIM command thread) */
/* Enqueue the received command in the global command queue gCmdQ. */
void powerSwitchHandler(long *tag,int *cmnd,int *size);
/*---------------------------------------------------------------------------*/
/* DIM server SVC handler (executed in the DIM command thread) */
/* Send the status stored in the variable pwStatus.status[*tag] */
void powerStatusHandler(long *tag,int **address,int *size);
/* send the timestamp stored in the variable pwStatus.timeStamp[*tag] */
void powerStatusTSHandler(long *tag,int **address,int *size);
void tempNamesHandler(long *tag,int **address,int *size);
void tempInputHandler(long *tag,int **address,int *size);
void tempUnitsHandler(long *tag,int **address,int *size);
void tempStatusHandler(long *tag,int **address,int *size);
void fanNamesHandler(long *tag,int **address,int *size);
void fanInputHandler(long *tag,int **address,int *size);
void fanUnitsHandler(long *tag,int **address,int *size);
void fanStatusHandler(long *tag,int **address,int *size);
void voltageNamesHandler(long *tag,int **address,int *size);
void voltageInputHandler(long *tag,int **address,int *size);
void voltageUnitsHandler(long *tag,int **address,int *size);
void voltageStatusHandler(long *tag,int **address,int *size);
void currentNamesHandler(long *tag,int **address,int *size);
void currentInputHandler(long *tag,int **address,int *size);
void currentUnitsHandler(long *tag,int **address,int *size);
void currentStatusHandler(long *tag,int **address,int *size);
void sensorNamesHandler(long *tag,int **address,int *size,char *sType);
void sensorInputIntHandler(long *tag,int **address,int *size,char *sType);
void sensorInputFloatHandler(long *tag,int **address,int *size,char *sType);
void sensorUnitsHandler(long *tag,int **address,int *size,char *sType);
void sensorStatusHandler(long *tag,int **address,int *size,char *sType);
void sensorsTSHandler(long *tag,int **address,int *size);
/*---------------------------------------------------------------------------*/
/* called periodically to retrieve data from IPMI */
void sensorsDicUpdate(int nodeN);
void powerStatusUpdate(int nodeN);
void sensorsStatusUpdate(int nodeN);
/*---------------------------------------------------------------------------*/
/* called periodically to execute enqueued pwSwitch commands */
void powerSwitchStartCmds(void);
/*---------------------------------------------------------------------------*/
/* running in spawned short-living threads */
void *sensorsDicUpdateCmd(void *data);
void *powerStatusUpdateCmd(void *data);
void *sensorsStatusUpdateCmd(void *data);
void *powerSwitchExecuteCmd(void *data);
/*---------------------------------------------------------------------------*/
/* global CMD fifo management utilities */
void gQpush(globalQ_t *queue,int nodeN,int cmdN);
globalQe_t gQpop(globalQ_t *queue);
void gQprint(globalQ_t *queue,int severity);
int gQlen(globalQ_t *queue);
/*---------------------------------------------------------------------------*/
/* BMC fifo management utilities */
void qAdd(bmcQ_t *queue,pthread_t tid);
void qRm(bmcQ_t *queue);
int qIsFirst(bmcQ_t *queue,pthread_t tid);
void qMsgSnd(char *hostName,bmcQ_t *queue);
int qLen(bmcQ_t *queue);
/*---------------------------------------------------------------------------*/
/* other handler */
void dimErrorHandler(int severity,int errorCode,char *message);
static void signalHandler(int signo);
/*---------------------------------------------------------------------------*/
/* other utilities */
char *strDupUpper(char *s);
int getHostN(char *hostName);
char *getHostName(int hostN);
int sensorSelect(char *string,char *sType);
static int sensCmp(const void *s1, const void *s2);
void sensAdd(int *sensDicN,sc_t **sensDic,char *name,char *type);
void sensPrint(int sensDicN,sc_t *sensDic);
char *getSensType(char *name,int sensDicN,sc_t *sensDic);
char *getCmdStr(unsigned char code);
unsigned char getCmdCode(char *cmnd);
/*---------------------------------------------------------------------------*/
/* IPMI per-node configuration */
ipmiConf_t parseIpmiConf(char *s);
ipmiConf_t readIpmiConf(char *confFileName);
int findIpmiConf(ipmiConf_t cfg,char *hostname);
void printIpmiConf(ipmiConf_t cfg);
void printIpmiConfLong(ipmiConf_t cfg);
/*---------------------------------------------------------------------------*/
/* other */
void usage(int mode);
void shortUsage(void);
/* ######################################################################### */
int main(int argc,char **argv)
{
  int i=0;
  char *p;
  char *srvName=SRV_NAME;
  char srvPath[DIM_SRV_LEN+1]="";
  char svcPath[DIM_SVC_LEN+1]="";
  char cmdPath[DIM_SVC_LEN+1]="";
  int flag=0;
  struct timespec updatePeriodTS={0,0};
  FILE *sfp=NULL;
  char readLine[64]="";
  int readLineN=0;
  char *deBugS=NULL;
  char *confFileName=CONF_FILE_NAME;     /* power manager configuration file */
  char *sensDicFile=DICT_FILE_NAME;                /* sensor dictionary file */
  char *dimConfFile=DIM_CONF_FILE_NAME;            /* dim configuration file */
  char *dimDnsNode=NULL;
  int doUpdateSensorList=0;
  time_t lastSensorsUpdate=0;
  int signo=0;
  const char *shortOptions="+h::l:c:s:dt:N:f:";
  static struct option longOptions[]=
  {
    {"conf-file",required_argument,NULL,'c'},
    {"sensor-dictionary-file",required_argument,NULL,'s'},
    {"logger",required_argument,NULL,'l'},
    {"update-dictionary-file",no_argument,NULL,'d'},
    {"max-threads-number",required_argument,NULL,'t'},
    {"sensor-period",required_argument,NULL,0},
    {"min-period",required_argument,NULL,1},
    {"max-period",required_argument,NULL,2},
    {"get-try-n",required_argument,NULL,3},
    {"cmd-try-n",required_argument,NULL,4},
    {"dim_dns_node",required_argument,NULL,'N'},
    {"password_file",required_argument,NULL,'f'},
    {"help",no_argument,NULL,'h'},
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  aRcsid=getActuatorVersion();
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid and aRcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  aRcsidP=strchr(aRcsid,':')+2;
  for(i=0,p=aRcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* syslog version */
  mPrintf(L_SYS,INFO,__func__,0,"Starting FMC Power Manager Server... "
          "Using: \"%s\", \"%s\", \"FMC-%s\".",rcsidP,aRcsidP,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* process command-line arguments */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt_long(argc,argv,shortOptions,longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 'c': /* -c, --conf-file */
        confFileName=optarg;
        break;
      case 's': /* -s, --sensor-dictionary-file */
        /* strncpy(sensDicFile,optarg,sizeof(sensDicFile)); */
        sensDicFile=optarg;
        break;
      case 'l': /* -l, --logger */
        errU=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'd': /* -d, --update-dictionary-file */
        doUpdateSensorList=1;
        break;
      case 't': /* -t, --max-threads-number */
        maxThreadN=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 0:  /* --sensor-period */
        sensorsUpdatePeriod=(float)strtod(optarg,(char**)NULL);
        break;
      case 1:  /* --min-period */
        updateMinPeriod=(float)strtod(optarg,(char**)NULL);
        break;
      case 2:  /* --max-period */
        updateMaxPeriod=(float)strtod(optarg,(char**)NULL);
        break;
      case 3: /* --get-try-n */
        maxGetAttemptN=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 4: /* --cmd-try-n */
        maxCmdAttemptN=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'N': /* -N, --dim_dns_node */
        dimDnsNode=optarg;
        break;
      case 'f': /* -f, --password_file */
        passWordFile=optarg;
        break;
      case 'h': /* -h, -hh, --help */
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        mPrintf(L_STD|L_SYS,FATAL,__func__,0,"getopt_long(): invalid option "
                "\"%s\"!",argv[optind-1]);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* check non-option arguments */
  if(optind<argc)
  {
    char msg[4096];
    char *msgP;
    msgP=msg;
    sprintf(msgP,"Invalid non-option command-line arguments: ");
    msgP=strchr(msgP,'\0');
    while(optind<argc)
    {
      sprintf(msgP,"\"%s\" ",argv[optind++]);
      msgP=strchr(msgP,'\0');
    }
    sprintf(msgP,".");
    mPrintf(L_STD|L_SYS,FATAL,__func__,0,msg);
    shortUsage();
  }
  if(argc-optind!=0)shortUsage();
  /*-------------------------------------------------------------------------*/
  /* check command line options */
  if(errU<0||errU>7)
  {
    mPrintf(L_STD|L_SYS,FATAL,__func__,0,"Logger type %d not allowed. Logger "
            "type must be in the range 0..7! Exiting...",errU);
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* open error logger */
  if(errU&L_DIM)
  {
    if(dfltLoggerOpen(10,0,ERROR,DEBUG,0)==-1)
    {
      mPrintf(L_STD|L_SYS,FATAL,__func__,0,"Can't initialize error logger! "
              "Exiting...");
      exit(1);
    }
  }
  mPrintf(errU,DEBUG,__func__,0,"Starting FMC Power Manager Server... Using: "
          "\"%s\", \"%s\", \"FMC-%s\".",rcsidP,aRcsidP,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* check command line options (contd) */
  if(maxThreadN && maxThreadN<5)
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"The maximum thread number defined "
            "with the \"-t\" command-line option (now %d) must be at least 6! "
            "Exiting...",maxThreadN);
    exit(1);
  }
  if(updateMinPeriod<4.0)
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"The minimum power status refresh "
            "period, defined with the \"--min-period\" command-line option "
            "(now set to %f) must be at least 4.0 seconds! Exiting...",
            updateMinPeriod);
    exit(1);
  }
  if(updateMaxPeriod<4.0)
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"The maximum power status refresh "
            "period, defined with the \"--max-period\" command-line option "
            "(now set to %f) must be at least 4.0 seconds! Exiting...",
            updateMaxPeriod);
    exit(1);
  }
  if(updateMaxPeriod<updateMinPeriod)
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"The maximum power status refresh "
            "period, defined with the \"--max-period\" command-line option "
            "(now set to %f) must be greater than or equal to tne minimum "
            "power status refresh period, defined with the \"--min-period\" "
            "command-line option (now set to %f)! Exiting...",updateMaxPeriod,
            updateMinPeriod);
    exit(1);
  }
  if(sensorsUpdatePeriod<4.0)
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"The sensor refresh period, defined "
            "with the \"--sensor-period\" command-line option (now set to %f) "
            "must be at least 4.0 seconds! Exiting...",sensorsUpdatePeriod);
    exit(1);
  }
  if(maxGetAttemptN<1)
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"The maximum number of attempts in "
            "getting IPMI information before giving-up, defined with the "
            "\"--get-try-n\" command-line option (now set to %d) must be at "
            "least 1! Exiting...",maxGetAttemptN);
    exit(1);
  }
  if(maxCmdAttemptN<1)
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"The maximum number of IPMI command "
            "attempts before giving-up, defined with the \"--cmd-try-n\" "
            "command-line option (now set to %d) must be at least 1! "
            "Exiting...",maxCmdAttemptN);
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* set the DIM DNS node */
  if(dimDnsNode)
  {
    mPrintf(errU,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from -N command-line "
            "option).",dimDnsNode);
  }
  else
  {
    dimDnsNode=getenv("DIM_DNS_NODE");
    if(dimDnsNode)
    {
      mPrintf(errU,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from DIM_DNS_NODE "
              "environment variable).",dimDnsNode);
    }
    else
    {
      dimDnsNode=getDimDnsNode(dimConfFile,0,1);
      if(dimDnsNode)
      {
        mPrintf(errU,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from "
                "\""DIM_CONF_FILE_NAME"\" file).",dimDnsNode);
      }
      else
      {
        mPrintf(errU,FATAL,__func__,0,"DIM Name Server (DIM_DNS_NODE) not "
                "defined!\nDIM Name Server can be defined (in decreasing "
                "order of priority):\n"
                "  1. Specifying the -N DIM_DNS_NODE command-line option;\n"
                "  2. Specifying the DIM_DNS_NODE environment variable;\n"
                "  3. Defining the DIM_DNS_NODE in the file "
                "\""DIM_CONF_FILE_NAME"\".");
        exit(1);
      }
    }
  }
  if(setenv("DIM_DNS_NODE",dimDnsNode,1))
  {
    mPrintf(errU,FATAL,__func__,0,"setenv(): %s!",strerror(errno));
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* Block SIGINT, SIGTERM and SIGHUP, to be handled synchronously by        */
  /* sigtimedwait().                                                         */
  /* Signals must be blocked before the dim_init() call, which creates 2 new */
  /* threads, to keep the signals blocked in all the 3 threads.              */
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigaddset(&signalMask,SIGHUP);
  if(pthread_sigmask(SIG_BLOCK,&signalMask,NULL))eExit("pthread_sigmask()");
  /*-------------------------------------------------------------------------*/
  /* Start DIM. Here threads becomes 3 */
  dim_init();
  /*-------------------------------------------------------------------------*/
  /* read debug level from environment */
  deBugS=getenv("deBug");
  if(deBugS)deBug=(int)strtol(deBugS,(char**)NULL,0);
  mPrintf(errU,DEBUG,__func__,0,"Debug level = %#02x.",deBug);
  /*-------------------------------------------------------------------------*/
  /* read configuration (list of nodes to be controlled) from confFileName   */
  ipmiCfg=readIpmiConf(confFileName);
  printIpmiConf(ipmiCfg);
  if(deBug&0x80)printIpmiConfLong(ipmiCfg);
  if(!ipmiCfg.nodeN)
  {
    mPrintf(errU,ERROR,__func__,0,"No node configured for Power Manager "
            "control!");
  }
  /*-------------------------------------------------------------------------*/
  /* read ipmitool path from environment */
  ipmiCmd=getenv("IPMI_CMD");
  if(!ipmiCmd)
  {
    ipmiCmd="/usr/bin/ipmitool";
    mPrintf(errU,DEBUG,__func__,0,"Parameter: \"IPMI_CMD\" defined neither in "
            "configuration file \"%s\" nor in environment. Using the default "
            "path \"%s\" for ipmitool.",confFileName,ipmiCmd);
  }
  else
  {
    mPrintf(errU,DEBUG,__func__,0,"Parameter: \"IPMI_CMD\" read. Using path: "
            "\"%s\" for ipmitool.",ipmiCmd);
  }
  /*-------------------------------------------------------------------------*/
  /* read IPMI default username from environment */
  dfltIpmiUserName=getenv("IPMI_USER");
  if(!dfltIpmiUserName)
  {
    dfltIpmiUserName="noUser";
    mPrintf(errU,WARN,__func__,0,"Parameter: \"IPMI_USER\" defined neither in "
            "configuration file \"%s\" nor in environment. Using \"%s\" as "
            "the default user for ipmitool!",confFileName,dfltIpmiUserName);
  }
  else
  {
    mPrintf(errU,DEBUG,__func__,0,"Parameter: \"IPMI_USER\" read. Using "
            "\"%s\" as the default user for ipmitool.",dfltIpmiUserName);
  }
  /*-------------------------------------------------------------------------*/
  /* read either IPMI default passWord or IPMI passWordFile */
  if(passWordFile)                    /* default IPMI passwd in passWordFile */
  {
    /* check read accessibility of passWordFile */
    if(access(passWordFile,R_OK)==-1)                       /* access denied */
    {
      mPrintf(errU|L_SYS|L_STD,FATAL,__func__,0,"Can't access password file "
              "\"%s\" for reading: %s!",passWordFile,strerror(errno));
      exit(1);
    }
    dfltIpmiPassWord=NULL;
    mPrintf(errU,INFO,__func__,0,"IPMI default password will be read from "
            "the password file \"%s\".",passWordFile);
  }                                   /* default IPMI passwd in passWordFile */
  else                                                     /* no passwd file */
  {
    passWordFile=NULL;
    dfltIpmiPassWord=getenv("IPMI_PASSWD");
    if(!dfltIpmiPassWord)
    {
      dfltIpmiPassWord="no_default_passwd";
      mPrintf(errU|L_STD|L_SYS,WARN,__func__,0,"Parameter: \"IPMI_PASSWD\" "
            "defined neither in configuration file \"%s\" nor in environment. "
            "Using \"%s\" as the default password for ipmitool!",confFileName,
            dfltIpmiPassWord);
    }
    else
    {
      mPrintf(errU,DEBUG,__func__,0,"Environment variable: \"IPMI_PASSWD\" "
              "read.");
    }
  }                                                        /* no passwd file */
  /*-------------------------------------------------------------------------*/
  /* check accessibility of ipmitool */
  if(access(ipmiCmd,X_OK)==-1)                              /* access denied */
  {
    mPrintf(errU,FATAL,__func__,0,"Can't execute ipmitool command: %s: %s!",
            ipmiCmd,strerror(errno));
    exit(1);
  }
  else
  {
    mPrintf(errU,DEBUG,__func__,0,"Ipmitool command \"%s\" found and "
            "executable.",ipmiCmd);
  }
  /*-------------------------------------------------------------------------*/
  /* define server name */
  snprintf(srvPath,DIM_SRV_LEN+1,"%s/%s",getSrvPrefix(),srvName);
  /*-------------------------------------------------------------------------*/
  /* allocate memory for arrays */
  pwStatSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  pwStatTSSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  pwSwitchCmdID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  sensorsTSSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  tempNamesSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  tempInputSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  tempUnitsSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  tempStatusSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  fanNamesSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  fanInputSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  fanUnitsSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  fanStatusSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  voltageNamesSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  voltageInputSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  voltageUnitsSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  voltageStatusSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  currentNamesSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  currentInputSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  currentUnitsSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  currentStatusSvcID=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  pwStatus.status=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  for(i=0;i<ipmiCfg.nodeN;i++)pwStatus.status[i]=IPMI_NOT_YET_AVAIL;
  pwStatus.timeStamp=(unsigned*)malloc(ipmiCfg.nodeN*sizeof(unsigned));
  memset(pwStatus.timeStamp,0,ipmiCfg.nodeN*sizeof(unsigned));
  sensors.status=(char**)malloc(ipmiCfg.nodeN*sizeof(char*));
  for(i=0;i<ipmiCfg.nodeN;i++)sensors.status[i]=strdup(IPMI_NOT_YET_AVAIL_S);
  sensors.timeStamp=(unsigned*)malloc(ipmiCfg.nodeN*sizeof(unsigned));
  memset(sensors.timeStamp,0,ipmiCfg.nodeN*sizeof(unsigned));
  bmc.busy=(int*)malloc(ipmiCfg.nodeN*sizeof(int));
  memset(bmc.busy,0,ipmiCfg.nodeN*sizeof(int));
  bmc.cmdQ=(bmcQ_t*)malloc(ipmiCfg.nodeN*sizeof(bmcQ_t));
  memset(bmc.cmdQ,0,ipmiCfg.nodeN*sizeof(bmcQ_t));
  bmc.pwSvcQ=(bmcQ_t*)malloc(ipmiCfg.nodeN*sizeof(bmcQ_t));
  memset(bmc.pwSvcQ,0,ipmiCfg.nodeN*sizeof(bmcQ_t));
  bmc.sensSvcQ=(bmcQ_t*)malloc(ipmiCfg.nodeN*sizeof(bmcQ_t));
  memset(bmc.sensSvcQ,0,ipmiCfg.nodeN*sizeof(bmcQ_t));
  /*-------------------------------------------------------------------------*/
  /* define DIM commands and services */
  for(i=0;i<ipmiCfg.nodeN;i++)
  {
    /* power_switch */
    snprintf(cmdPath,DIM_SVC_LEN+1,"%s/%s/power_switch",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    pwSwitchCmdID[i]=dis_add_cmnd(cmdPath,"C",powerSwitchHandler,
                                  (long*)ipmiCfg.hostNameList[i]);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added CMD: %s.",cmdPath);
    }
    /* power_status */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/power_status",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    pwStatSvcID[i]=dis_add_service(svcPath,"I",0,0,powerStatusHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* power_status_timestamp */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/power_status_timestamp",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    pwStatTSSvcID[i]=dis_add_service(svcPath,"I",0,0,powerStatusTSHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/temp/names */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/temp/names",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    tempNamesSvcID[i]=dis_add_service(svcPath,"C",0,0,tempNamesHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/temp/input */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/temp/input",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    tempInputSvcID[i]=dis_add_service(svcPath,"F",0,0,tempInputHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/temp/units */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/temp/units",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    tempUnitsSvcID[i]=dis_add_service(svcPath,"C",0,0,tempUnitsHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/temp/status */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/temp/status",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    tempStatusSvcID[i]=dis_add_service(svcPath,"C",0,0,tempStatusHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/fan/names */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/fan/names",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    fanNamesSvcID[i]=dis_add_service(svcPath,"C",0,0,fanNamesHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/fan/input */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/fan/input",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    fanInputSvcID[i]=dis_add_service(svcPath,"I",0,0,fanInputHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/fan/units */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/fan/units",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    fanUnitsSvcID[i]=dis_add_service(svcPath,"C",0,0,fanUnitsHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/fan/status */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/fan/status",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    fanStatusSvcID[i]=dis_add_service(svcPath,"C",0,0,fanStatusHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/voltage/names */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/voltage/names",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    voltageNamesSvcID[i]=dis_add_service(svcPath,"C",0,0,voltageNamesHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/voltage/input */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/voltage/input",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    voltageInputSvcID[i]=dis_add_service(svcPath,"F",0,0,voltageInputHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/voltage/units */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/voltage/units",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    voltageUnitsSvcID[i]=dis_add_service(svcPath,"C",0,0,voltageUnitsHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/voltage/status */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/voltage/status",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    voltageStatusSvcID[i]=dis_add_service(svcPath,"C",0,0,voltageStatusHandler,
                                          i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/current/names */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/current/names",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    currentNamesSvcID[i]=dis_add_service(svcPath,"C",0,0,currentNamesHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/current/input */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/current/input",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    currentInputSvcID[i]=dis_add_service(svcPath,"F",0,0,currentInputHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/current/units */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/current/units",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    currentUnitsSvcID[i]=dis_add_service(svcPath,"C",0,0,currentUnitsHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors/current/status */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors/current/status",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    currentStatusSvcID[i]=dis_add_service(svcPath,"C",0,0,currentStatusHandler,
                                          i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
    /* sensors_timestamp */
    snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/sensors_timestamp",SVC_HEAD,
             strDupUpper(ipmiCfg.hostNameList[i]));
    sensorsTSSvcID[i]=dis_add_service(svcPath,"I",0,0,sensorsTSHandler,i);
    if(deBug&0x20)
    {
      mPrintf(errU,DEBUG,__func__,0,"Added SVC: %s.",svcPath);
    }
  }
  /* server_version */
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/server_version",getSrvPrefix(),
           srvName);
  versionSvcID=dis_add_service(svcPath,"C",rcsidP,1+strlen(rcsidP),0,0);
  /* actuator_version */
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/actuator_version",getSrvPrefix(),
           srvName);
  aVersionSvcID=dis_add_service(svcPath,"C",aRcsidP,1+strlen(aRcsidP),0,0);
  /* actuator_version */
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/fmc_version",getSrvPrefix(),srvName);
  fmcVersionSvcID=dis_add_service(svcPath,"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  /* success */
  snprintf(svcPath,DIM_SVC_LEN+1,"%s/%s/success",getSrvPrefix(),srvName);
  successSvcID=dis_add_service(svcPath,"I",&success,sizeof(int),0,0);
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  dis_add_error_handler(dimErrorHandler);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  dis_start_serving(srvPath);
  /*-------------------------------------------------------------------------*/
  /* save the thread identifier of the main thread, to send it a SIGHUP      */
  main_tid=pthread_self();
  /*-------------------------------------------------------------------------*/
  dis_update_service(versionSvcID);
  dis_update_service(aVersionSvcID);
  dis_update_service(fmcVersionSvcID);
  dis_update_service(successSvcID);
  /*-------------------------------------------------------------------------*/
  /* read ipmi sensor list from file */
  mPrintf(errU,DEBUG,__func__,0,"Trying to read ipmi sensor dictionary from "
          "file: \"%s\"...",sensDicFile);
  sfp=fopen(sensDicFile,"r");
  if(!sfp)
  {
    mPrintf(errU,WARN,__func__,0,"Can't open file: \"%s\" for reading: %s! "
            "IPMI sensor dictionary discovery forced (this can take up to "
            "400 s)! Please wait!",sensDicFile,strerror(errno));
    doUpdateSensorList=1;
  }
  else
  {
    for(readLineN=0;fgets(readLine,64,sfp);)
    {
      char *delim=0;
      *strchr(readLine,'\n')='\0';
      readLineN++;
      delim=strchr(readLine,';');
      if(delim)
      {
        *delim='\0';
        sensAdd(&sensors.sensDicN,&sensors.sensDic,readLine,strdup(1+delim));
      }
    }
    fclose(sfp);
    mPrintf(errU,DEBUG,__func__,0,"Sensor dictionary read from file: \"%s\".",
            sensDicFile);
    mPrintf(errU,WARN,__func__,0,"If you add new brands or models of PC to "
            "the Power Manager Server configuration, please delete the sensor "
            "dictionary file: \"%s\" and restart the Power Manager Server "
            "ipmiSrv, to force the discovery of the new sensors.",sensDicFile);
  }
  /*-------------------------------------------------------------------------*/
  if(doUpdateSensorList)
  {
    mPrintf(errU,DEBUG,__func__,0,"Trying to read sensor dictionary through "
            "IPMI...");
    /* initialize sensor dictionary */
    for(i=0;i<ipmiCfg.nodeN;i++)sensorsDicUpdate(i);
    /* wait for initialization */
    for(;;)
    {
      int sic;
      if(pthread_mutex_lock(&sensInitCount.mutex))
        eExit("pthread_mutex_lock()");
      sic=sensInitCount.c;
      if(pthread_mutex_unlock(&sensInitCount.mutex))
        eExit("pthread_mutex_lock()");
      if(sic>=ipmiCfg.nodeN)break;
      dtq_sleep(1);
    }
    sensPrint(sensors.sensDicN,sensors.sensDic);
    mPrintf(errU,DEBUG,__func__,0,"Sensor dictionary discovered using IPMI "
            "from %d controlled nodes.",ipmiCfg.nodeN);
    /* rename old file, if exists */
    if(access(sensDicFile,F_OK)!=-1)                      /* old file exists */
    {
      char oldSensDicFile[261];
      sprintf(oldSensDicFile,"%s.old",sensDicFile);
      if(rename(sensDicFile,oldSensDicFile)==-1)
      {
        mPrintf(errU,ERROR,__func__,0,"Can't rename old sensor list file: "
                "\"%s\" to \"%s\": %s!",sensDicFile,oldSensDicFile,
                strerror(errno));
      }
    }
    if(access(sensDicFile,F_OK)==-1)              /* old file does not exist */
    {
      sfp=fopen(sensDicFile,"w");
      if(!sfp)
      {
        mPrintf(errU,ERROR,__func__,0,"Can't open file: \"%s\" for writing: "
                "%s! ",sensDicFile,strerror(errno));
      }
      else                                               /* new file created */
      {
        for(i=0;i<sensors.sensDicN;i++)
        {
          fprintf(sfp,"%s;%s\n",sensors.sensDic[i].name,
                                sensors.sensDic[i].type);
        }
        fclose(sfp);
        mPrintf(errU,DEBUG,__func__,0,"Sensor dictionary written to file: "
                "\"%s\".",sensDicFile);
      }
    }
  }
  /*-------------------------------------------------------------------------*/
  mPrintf(L_SYS|errU,INFO,__func__,0,"FMC Power Manager Server started. "
          "Using \"%s\", \"%s\", \"FMC-%s\". TGID=%d, srvPath=\"%s\".",rcsidP,
          aRcsidP,FMC_VERSION,getpid(),srvPath);
  /*-------------------------------------------------------------------------*/
  /* main loop */
  for(updatePeriod=updateMinPeriod,lastSensorsUpdate=0,i=0;;i++)
  {
    int j;
    if(i%250==0 && i!=0)mPrintf(errU,DEBUG,__func__,0,"%d updates.",i);
    if(deBug&0x1)
    {
      mPrintf(errU,DEBUG,__func__,0,"Update period: %d s.",(int)updatePeriod);
    }
    /*.......................................................................*/
    /* update the power status */
    for(j=0;j<ipmiCfg.nodeN;j++)
    {
      /*.....................................................................*/
      if(maxThreadN)for(;;)               /* wait until threadN < maxThreadN */
      {
        struct timespec delay;
        powerSwitchStartCmds();
        threadN=getThreadN(errU);
        if(threadN<maxThreadN)break;
        if(deBug&0x100)
        {
          mPrintf(errU,VERB,__func__,0,"powerStatusUpdate() is waiting for "
                  "some thread to finish. Current/maximum thread number = "
                  "%d/%d.",threadN,maxThreadN);
        }
        delay.tv_sec=1;
        delay.tv_nsec=0;
        signo=sigtimedwait(&signalMask,NULL,&delay);
        if(signo==-1){}                                /* no signal received */
        else if(signo==SIGHUP)    /* pwSwitch command issued in the meantime */
        {
          updatePeriod=updateMinPeriod;
        }
        else                                   /* SIGINT or SIGTERM received */
        {
          signalHandler(signo);              /* process signal synchronously */
        }
      }
      /*.....................................................................*/
      if(deBug&0x100)
      {
        mPrintf(errU,DEBUG,__func__,0,"Current/maximum thread number = "
                "%d/%d.",threadN,maxThreadN);
      }
      powerSwitchStartCmds();
      /* update one power status */
      powerStatusUpdate(j);
      /*.....................................................................*/
    }
    /*.......................................................................*/
    /* update the sensor measurements */
    if(time(NULL)-lastSensorsUpdate>=(int)sensorsUpdatePeriod)
    {
      if(!lastSensorsUpdate)lastSensorsUpdate=time(NULL);
      else lastSensorsUpdate+=(int)sensorsUpdatePeriod;
      for(j=0;j<ipmiCfg.nodeN;j++)
      {
        /*...................................................................*/
        if(maxThreadN)for(;;)             /* wait until threadN < maxThreadN */
        {
          struct timespec delay;
          powerSwitchStartCmds();
          threadN=getThreadN(errU);
          if(threadN<maxThreadN)break;
          if(deBug&0x100)
          {
            mPrintf(errU,VERB,__func__,0,"sensorsStatusUpdate() is waiting "
                    "for some thread to finish. Current/maximum thread number "
                    "= %d/%d.",threadN,maxThreadN);
          }
          delay.tv_sec=1;
          delay.tv_nsec=0;
          signo=sigtimedwait(&signalMask,NULL,&delay);
          if(signo==-1){}                              /* no signal received */
          else if(signo==SIGHUP)  /* pwSwitch command issued in the meantime */
          {
            updatePeriod=updateMinPeriod;
          }
          else                 /* SIGINT or SIGTERM received in the meantime */
          {
            signalHandler(signo);            /* process signal synchronously */
          }
        }
        /*...................................................................*/
        if(deBug&0x100)
        {
          mPrintf(errU,DEBUG,__func__,0,"Current/maximum thread number = "
                  "%d/%d.",threadN,maxThreadN);
        }
        powerSwitchStartCmds();
        /* update one sensor measurement */
        sensorsStatusUpdate(j);
        /*...................................................................*/
      }
    }
    powerSwitchStartCmds();
    /*.......................................................................*/
    /* sleep for updatePeriod seconds, but wake-up immediately if a blocked  */
    /* signal is received, to process it (synchronously)                     */
    /* blocked signals are SIGINT, SIGTERM and SIGHUP                        */
    /* SIGHUP is sent by powerSwitchExecuteCmd() to signal a power cmd       */
    /* issued                                                                */
    updatePeriodTS.tv_sec=(int)updatePeriod;
    updatePeriodTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&updatePeriodTS);
    if(signo==-1)                                      /* no signal received */
    {
      updatePeriod*=1.+updateIncPeriod/100.;
      if(updatePeriod>updateMaxPeriod)updatePeriod=updateMaxPeriod;
    }
    else if(signo==SIGHUP)                           /* power command issued */
    {
      updatePeriod=updateMinPeriod;
    }
    else                                       /* SIGINT or SIGTERM received */
    {
      signalHandler(signo);                  /* process signal synchronously */
    }
  }                                                         /* end main loop */
}                                                              /* end main() */
/*****************************************************************************/
/* run periodically in the DIM main thread */
void powerStatusUpdate(int nodeN)
{
  powerCtrlData_t *pcd=NULL;
  int doSkip=0;
  /*-------------------------------------------------------------------------*/
  /* copy ipmi data in the pcd structure passed to the new thread */
  pcd=(powerCtrlData_t*)malloc(sizeof(powerCtrlData_t));
  /* node number */
  pcd->nodeN=nodeN;
  /* node hostname */
  strncpy(pcd->hostName,ipmiCfg.hostNameList[nodeN],128);
  /* username */
  if(ipmiCfg.userNameList[nodeN])
    strncpy(pcd->userName,ipmiCfg.userNameList[nodeN],128);
  else
    strncpy(pcd->userName,dfltIpmiUserName,128);
  /* password */
  if(ipmiCfg.passWordList[nodeN])
    strncpy(pcd->passWord,ipmiCfg.passWordList[nodeN],128);
  else if(!passWordFile)
    strncpy(pcd->passWord,dfltIpmiPassWord,128);
  else
    *(pcd->passWord)='\0';
  /* password file */
  if(passWordFile)
    strncpy(pcd->passWordFile,passWordFile,128);
  else
    *(pcd->passWordFile)='\0';
  /* port */
  if(ipmiCfg.portList[nodeN]!=-1)
    pcd->port=ipmiCfg.portList[nodeN];
  else
    pcd->port=dfltIpmiPort;
  /* authentication type */
  if(ipmiCfg.authTypeList[nodeN])
    strncpy(pcd->authType,ipmiCfg.authTypeList[nodeN],128);
  else
    pcd->authType[0]='\0';
  /* privilege level */
  if(ipmiCfg.privLvlList[nodeN])
    strncpy(pcd->privLvl,ipmiCfg.privLvlList[nodeN],128);
  else
    pcd->privLvl[0]='\0';
  /* OEM type */
  if(ipmiCfg.oemTypeList[nodeN])
    strncpy(pcd->oemType,ipmiCfg.oemTypeList[nodeN],128);
  else
    pcd->oemType[0]='\0';
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s.",pcd->hostName);
  }
  /* to avoid unlimited pile-up of threads in case of ipmi not responding    */
  /* if another powerStatusUpdateCmd() is pending then skip this update      */
  /* single BMC access arbitration */
  if(pthread_mutex_lock(&bmc.mutex))eExit("pthread_mutex_lock()");
  if(qLen(&bmc.pwSvcQ[pcd->nodeN]))doSkip=1;
  if(pthread_mutex_unlock(&bmc.mutex))eExit("pthread_mutex_unlock()");
  /* end single BMC access arbitration */
  if(doSkip)
  {
    FREE_PCD;
    if(deBug&0x100)
    {
      mPrintf(errU,VERB,__func__,0,"Power status update cancelled for node "
              "\"%s\" due to single BMC access arbitration (BMC busy on node "
              "\"%s\"). Total thread number: %d.",ipmiCfg.hostNameList[nodeN],
              ipmiCfg.hostNameList[nodeN],getThreadN(errU));
    }
    return;
  }
  /* thread attributes */
  if(pthread_attr_init(&t_attr)==-1)
    thReturn("pthread_attr_init()");
  if(pthread_attr_setinheritsched(&t_attr,PTHREAD_INHERIT_SCHED)==-1)
    thReturn("pthread_attr_setinheritsched()");
  if(pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_DETACHED)==-1)
    thReturn("pthread_attr_setdetachstate()");
  if(pthread_attr_setstacksize(&t_attr,THREAD_STACK_SIZE)==-1)
    thReturn("pthread_attr_setstacksize()");
  /* create the new thread for powerStatusUpdateCmd() */
  if(pthread_create(&tid,&t_attr,powerStatusUpdateCmd,pcd)==-1)
    thReturn("pthread_create()");
  if(pthread_detach(tid)==-1)
    thReturn("pthread_detach()");
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"returned. Hostname = %s.",pcd->hostName);
  }
}
/*****************************************************************************/
/* run in a short-living thread created by powerStatusUpdate() */
void *powerStatusUpdateCmd(void *data)
{
  powerCtrlData_t *pcd=NULL;
  int status=IPMI_NOT_YET_AVAIL;
  unsigned timeStamp=0;
  /*-------------------------------------------------------------------------*/
  pthread_detach(pthread_self());
  pcd=(powerCtrlData_t*)data;
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s.",pcd->hostName);
  }
  /* single BMC access arbitration */
  if(pthread_mutex_lock(&bmc.mutex))eExit("pthread_mutex_lock()");
  /* add this thread to the power service queue */
  qAdd(&bmc.pwSvcQ[pcd->nodeN],pthread_self());
  while(bmc.busy[pcd->nodeN]||
        !qIsFirst(&bmc.pwSvcQ[pcd->nodeN],pthread_self())||
        qLen(&bmc.cmdQ[pcd->nodeN]))
  {
    if(pthread_cond_wait(&bmc.cond,&bmc.mutex))eExit("pthread_cond_wait()");
  }
  bmc.busy[pcd->nodeN]=1;
  if(pthread_cond_broadcast(&bmc.cond))eExit("pthread_cond_broadcast()");
  if(pthread_mutex_unlock(&bmc.mutex))eExit("pthread_mutex_unlock()");
  /* end single BMC access arbitration */
  status=getPowerStatus(pcd->hostName,pcd->userName,pcd->passWord,
                        pcd->passWordFile,pcd->port,pcd->authType,pcd->privLvl,
                        pcd->oemType);
  timeStamp=(unsigned)time(NULL);
  /* single BMC access arbitration */
  if(pthread_mutex_lock(&bmc.mutex))eExit("pthread_mutex_lock()");
  bmc.busy[pcd->nodeN]=0;
  /* remove this thread from the power service queue */
  qRm(&bmc.pwSvcQ[pcd->nodeN]);
  if(pthread_cond_broadcast(&bmc.cond))eExit("pthread_cond_broadcast()");
  if(pthread_mutex_unlock(&bmc.mutex))eExit("pthread_mutex_unlock()");
  /* end single BMC access arbitration */
  if(pthread_mutex_lock(&pwStatus.mutex))eExit("pthread_mutex_lock()");
  pwStatus.status[pcd->nodeN]=status;
  pwStatus.timeStamp[pcd->nodeN]=timeStamp;
  if(pthread_mutex_unlock(&pwStatus.mutex))eExit("pthread_mutex_unlock()");
  dis_update_service(pwStatSvcID[pcd->nodeN]);
  dis_update_service(pwStatTSSvcID[pcd->nodeN]);
  if(status<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Failed to get the power status of the "
            "node: %s: %s!",pcd->hostName,IPMI_NOT_RESP_SL);
  }
  else
  {
    if(deBug&0x2)
    {
      mPrintf(errU,DEBUG,__func__,0,"Node: %s. Power status: %d",pcd->hostName,
              status);
    }
  }
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done. Hostname = %s.",pcd->hostName);
  }
  FREE_PCD;
  pthread_exit(NULL);
}
/* ######################################################################### */
/* DIM SVC handlers                                                          */
/* ------------------------------------------------------------------------- */
/* powerSwitchHandler(), powerSwitchStartCmds(), powerSwitchExecuteCmd()     */
/* ######################################################################### */
/* powerSwitchHandler()                                                      */
/* Run in the DIM I/O thread when a powerSwitch command is received from a   */
/* client. This function simply enqueues the received command in the global  */
/* command queue globalQ_t gCmdQ.                                            */
/* *tag=hostName (string);                                                   */
/* cmnd="up","down","cycle","soft_off","hard_reset","pulse_diag" (string).   */
/*****************************************************************************/
void powerSwitchHandler(long *tag,int *cmnd,int *size)
{
  int hostN;
  unsigned char cmdCode;
  /*-------------------------------------------------------------------------*/
  hostN=getHostN((char*)*tag);
  if(hostN==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"Command \"%s\" received for unknown node "
            "\"%s\"! Command skipped!",(char*)cmnd,(char*)*tag);
    return;
  }
  cmdCode=getCmdCode((char*)cmnd);
  if(cmdCode==0xff)
  {
    mPrintf(errU,ERROR,__func__,0,"Unknown command \"%s\" received for node "
            "\"%s\"! Command skipped!",(char*)cmnd,(char*)*tag);
    return;
  }
  mPrintf(errU,INFO,__func__,0,"Received command \"pwSwitch %s\" for node "
          "\"%s\".",(char*)cmnd,(char*)*tag);
  gQpush(&gCmdQ,hostN,cmdCode);
  if(deBug&0x100)
  {
    mPrintf(errU,VERB,__func__,0,"gQlen=%d",gQlen(&gCmdQ));
    gQprint(&gCmdQ,VERB);
  }
  /* signal status change */
  pthread_kill(main_tid,SIGHUP);
  return;
}
/*****************************************************************************/
/* powerSwitchStartCmds()                                                    */
/* Run in the DIM Main thread periodically.                                  */
/* Called by the main loop of the main() function.                           */
/* This function dequeues the commands found (if any) in the global command  */
/* queue globalQ_t gCmdQ and starts new short-living threads to execute      */
/* them. If the maximum allowed number of threads is reached, it waits until */
/* the number of threads reduces.                                            */
/* Uses global variables: dfltIpmiPort, dfltIpmiUserName, dfltIpmiPassWord   */
/* Uses global variable: ipmiCfg                                             */
/*****************************************************************************/
void powerSwitchStartCmds(void)
{
  globalQe_t command;
  int signo;
  powerCtrlData_t *pcd=NULL;
  void *(*startRoutine)(void*)=NULL;
  int i;
  int iLen;
  /*-------------------------------------------------------------------------*/
  iLen=gQlen(&gCmdQ);
  if(deBug&0x100)
  {
    mPrintf(errU,VERB,__func__,0,"pwSwitch command queue length: %d",iLen);
  }
  /*-------------------------------------------------------------------------*/
  for(;;)         /* loop over global commands in the  globalQ_t gCmdQ queue */
  {
    if(!gQlen(&gCmdQ))break;
    command=gQpop(&gCmdQ);
    mPrintf(errU,DEBUG,__func__,0,"Processing command \"%s\" to node \"%s\""
            "...",getCmdStr(command.cmdN),getHostName(command.nodeN));
    /*.......................................................................*/
    if(maxThreadN)for(i=0;;i++)           /* wait until threadN < maxThreadN */
    {
      struct timespec delay;
      threadN=getThreadN(errU);
      if(threadN<maxThreadN)break;
      if(deBug&0x100)
      {
        mPrintf(errU,VERB,__func__,0," Command \"%s\" to node \"%s\" is "
                "waiting for some thread to finish. Current/maximum thread "
                "number = %d/%d.",getCmdStr(command.cmdN),
                getHostName(command.nodeN),threadN,maxThreadN);
      }
      if(i && !(i%30))
      {
        mPrintf(errU,INFO,__func__,0,"%d pwSwitch command(s) waiting for "
                "some thread to finish.",gQlen(&gCmdQ));
        gQprint(&gCmdQ,DEBUG);
      }
      delay.tv_sec=1;
      delay.tv_nsec=0;
      signo=sigtimedwait(&signalMask,NULL,&delay);
      if(signo==-1){}                                  /* no signal received */
      else if(signo==SIGHUP)      /* pwSwitch command issued in the meantime */
      {
        updatePeriod=updateMinPeriod;
      }
      else                 /* SIGINT or SIGTERM received in the meantime */
      {
        signalHandler(signo);            /* process signal synchronously */
      }
    }
    /*.......................................................................*/
    if(deBug&0x100)
    {
      mPrintf(errU,DEBUG,__func__,0,"Current/maximum thread number = "
              "%d/%d.",threadN,maxThreadN);
    }
    /*.......................................................................*/
    /* Issue a command */
    mPrintf(errU,DEBUG,__func__,0,"Dispatching command \"%s\" to node \"%s\". "
            "Current/maximum thread number = %d/%d.",getCmdStr(command.cmdN),
            getHostName(command.nodeN),threadN,maxThreadN);
    startRoutine=powerSwitchExecuteCmd;
    /* copy ipmi data in the pcd structure passed to the new thread          */
    /* we know hostName from tag but not nodeN, so we get first hostName and */
    /* then we find nodeN from hostName by using getHostN()                  */
    pcd=(powerCtrlData_t*)malloc(sizeof(powerCtrlData_t));
    /* node hostname */
    strncpy(pcd->hostName,getHostName(command.nodeN),128);
    /* node number */
    pcd->nodeN=command.nodeN;
    /* username */
    if(ipmiCfg.userNameList[pcd->nodeN])
      strncpy(pcd->userName,ipmiCfg.userNameList[pcd->nodeN],128);
    else
      strncpy(pcd->userName,dfltIpmiUserName,128);
    /* password */
    if(ipmiCfg.passWordList[pcd->nodeN])
      strncpy(pcd->passWord,ipmiCfg.passWordList[pcd->nodeN],128);
    else if(!passWordFile)
      strncpy(pcd->passWord,dfltIpmiPassWord,128);
    else
      *(pcd->passWord)='\0';
    /* password file */
    if(passWordFile)
      strncpy(pcd->passWordFile,passWordFile,128);
    else
      *(pcd->passWordFile)='\0';
    /* port */
    if(ipmiCfg.portList[pcd->nodeN]!=-1)
      pcd->port=ipmiCfg.portList[pcd->nodeN];
    else
      pcd->port=dfltIpmiPort;
    /* authentication type */
    if(ipmiCfg.authTypeList[pcd->nodeN])
      strncpy(pcd->authType,ipmiCfg.authTypeList[pcd->nodeN],128);
    else
      pcd->authType[0]='\0';
    /* privilege level */
    if(ipmiCfg.privLvlList[pcd->nodeN])
      strncpy(pcd->privLvl,ipmiCfg.privLvlList[pcd->nodeN],128);
    else
      pcd->privLvl[0]='\0';
    /* OEM type */
    if(ipmiCfg.oemTypeList[pcd->nodeN])
      strncpy(pcd->oemType,ipmiCfg.oemTypeList[pcd->nodeN],128);
    else
      pcd->oemType[0]='\0';
    pcd->ctl=command.cmdN;
    if(pcd->ctl==0xff)
    {
      mPrintf(errU,ERROR,__func__,0,"Unknown command \"%s\" received by DIM "
              "command: power_switch!",getCmdStr(command.cmdN));
      FREE_PCD;
      return;
    }
    /* thread attributes */
    if(pthread_attr_init(&t_attr)==-1)thReturn("pthread_attr_init()");
    if(pthread_attr_setinheritsched(&t_attr,PTHREAD_INHERIT_SCHED)==-1)
      thReturn("pthread_attr_setinheritsched()");
    if(pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_DETACHED)==-1)
      thReturn("pthread_attr_setdetachstate()");
    if(pthread_attr_setstacksize(&t_attr,THREAD_STACK_SIZE)==-1)
      thReturn("pthread_attr_setstacksize()");
    /* create the new thread for startRoutine() */
    if(pthread_create(&tid,&t_attr,startRoutine,pcd)==-1)
      thReturn("pthread_create()");
    if(pthread_detach(tid)==-1)
      thReturn("pthread_detach()");
    /*.......................................................................*/
  }               /* loop over global commands in the  globalQ_t gCmdQ queue */
  /*-------------------------------------------------------------------------*/
  if(iLen)
  {
    mPrintf(errU,INFO,__func__,0,"No (more) pwSwitch command to process.");
  }
  else if(deBug&0x100)
  {
    mPrintf(errU,VERB,__func__,0,"No (more) pwSwitch command to process.");
  }
  /*-------------------------------------------------------------------------*/
  return;
}
/*****************************************************************************/
/* run in a short-living thread created by powerSwitchStartCmds() */
void *powerSwitchExecuteCmd(void *data)
{
  powerCtrlData_t *pcd=NULL;
  int status=IPMI_NOT_YET_AVAIL;
  /*-------------------------------------------------------------------------*/
  pthread_detach(pthread_self());
  pcd=(powerCtrlData_t*)data;
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s, cmnd n.: %d.",
            pcd->hostName,pcd->ctl);
  }
  if(pcd->ctl==IPMI_CHASSIS_CTL_POWER_UP)
    mPrintf(errU,DEBUG,__func__,0,"Switching on node: %s...",pcd->hostName);
  else if(pcd->ctl==IPMI_CHASSIS_CTL_POWER_DOWN)
    mPrintf(errU,DEBUG,__func__,0,"Switching off node: %s...",pcd->hostName);
  else if(pcd->ctl==IPMI_CHASSIS_CTL_POWER_CYCLE)
    mPrintf(errU,DEBUG,__func__,0,"Power cycling node: %s...",pcd->hostName);
  else if(pcd->ctl==IPMI_CHASSIS_CTL_ACPI_SOFT)
    mPrintf(errU,DEBUG,__func__,0,"Shutting down node: %s...",pcd->hostName);
  else if(pcd->ctl==IPMI_CHASSIS_CTL_HARD_RESET)
    mPrintf(errU,DEBUG,__func__,0,"Hard resetting node: %s...",pcd->hostName);
  else if(pcd->ctl==IPMI_CHASSIS_CTL_PULSE_DIAG)
    mPrintf(errU,DEBUG,__func__,0,"Pulsing diagnostic interrupt to node: %s...",
            pcd->hostName);
  /* single BMC access arbitration */
  if(pthread_mutex_lock(&bmc.mutex))eExit("pthread_mutex_lock()");
  /* add this thread to the command queue */
  qAdd(&bmc.cmdQ[pcd->nodeN],pthread_self());
  if(deBug&0x4)qMsgSnd(pcd->hostName,&bmc.cmdQ[pcd->nodeN]);
  while(bmc.busy[pcd->nodeN]||!qIsFirst(&bmc.cmdQ[pcd->nodeN],pthread_self()))
  {
    if(pthread_cond_wait(&bmc.cond,&bmc.mutex))eExit("pthread_cond_wait()");
  }
  bmc.busy[pcd->nodeN]=1;
  if(pthread_cond_broadcast(&bmc.cond))eExit("pthread_cond_broadcast()");
  if(pthread_mutex_unlock(&bmc.mutex))eExit("pthread_mutex_unlock()");
  /* end single BMC access arbitration */
  /* wait a second to cope with IPMI firmware problem */
  dtq_sleep(ipmiCtlSleepTime);
  status=powerSwitchActuate(pcd->hostName,pcd->userName,pcd->passWord,
                            pcd->passWordFile,pcd->port,pcd->authType,
                            pcd->privLvl,pcd->oemType,pcd->ctl);
  /* wait a second to cope with IPMI firmware problem */
  dtq_sleep(ipmiCtlSleepTime);
  /* single BMC access arbitration */
  if(pthread_mutex_lock(&bmc.mutex))eExit("pthread_mutex_lock()");
  bmc.busy[pcd->nodeN]=0;
  /* remove this thread from the command queue */
  qRm(&bmc.cmdQ[pcd->nodeN]);
  if(deBug&0x4)qMsgSnd(pcd->hostName,&bmc.cmdQ[pcd->nodeN]);
  if(pthread_cond_broadcast(&bmc.cond))eExit("pthread_cond_broadcast()");
  if(pthread_mutex_unlock(&bmc.mutex))eExit("pthread_mutex_unlock()");
  /* end single BMC access arbitration */
  if(status<0)
  {
    if(pcd->ctl==IPMI_CHASSIS_CTL_POWER_UP)
      mPrintf(errU,ERROR,__func__,0,"Failed to switch on node: %s!",
              pcd->hostName);
    else if(pcd->ctl==IPMI_CHASSIS_CTL_POWER_DOWN)
      mPrintf(errU,ERROR,__func__,0,"Failed to switch off node: %s!",
              pcd->hostName);
    else if(pcd->ctl==IPMI_CHASSIS_CTL_POWER_CYCLE)
      mPrintf(errU,ERROR,__func__,0,"Failed to power cycle node: %s!",
              pcd->hostName);
    else if(pcd->ctl==IPMI_CHASSIS_CTL_ACPI_SOFT)
      mPrintf(errU,ERROR,__func__,0,"Failed to shut down node: %s!",
              pcd->hostName);
    else if(pcd->ctl==IPMI_CHASSIS_CTL_HARD_RESET)
      mPrintf(errU,ERROR,__func__,0,"Failed to hard reset node: %s!",
              pcd->hostName);
    else if(pcd->ctl==IPMI_CHASSIS_CTL_PULSE_DIAG)
      mPrintf(errU,ERROR,__func__,0,"Failed to pulsing diagnostic interrupt to "
              "node: %s!",pcd->hostName);
  }
  else
  {
    if(pcd->ctl==IPMI_CHASSIS_CTL_POWER_UP)
      mPrintf(errU,INFO,__func__,0,"Node: %s switched on!",pcd->hostName);
    else if(pcd->ctl==IPMI_CHASSIS_CTL_POWER_DOWN)
      mPrintf(errU,INFO,__func__,0,"Node: %s switched off!",pcd->hostName);
    else if(pcd->ctl==IPMI_CHASSIS_CTL_POWER_CYCLE)
      mPrintf(errU,INFO,__func__,0,"Node: %s power cycled!",pcd->hostName);
    else if(pcd->ctl==IPMI_CHASSIS_CTL_ACPI_SOFT)
      mPrintf(errU,INFO,__func__,0,"Node: %s shut down!",pcd->hostName);
    else if(pcd->ctl==IPMI_CHASSIS_CTL_HARD_RESET)
      mPrintf(errU,INFO,__func__,0,"Node: %s hard reset!",pcd->hostName);
    else if(pcd->ctl==IPMI_CHASSIS_CTL_PULSE_DIAG)
      mPrintf(errU,INFO,__func__,0,"Node: %s diagnostic interrupt pulsed!",
              pcd->hostName);
  }
  /* signal status change */
  pthread_kill(main_tid,SIGHUP);
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done. Hostname = %s, cmnd n.: %d.",
            pcd->hostName,pcd->ctl);
  }
  FREE_PCD;
  pthread_exit(NULL);
}
/* ######################################################################### */
/* DIM SVC handlers                                                          */
/* ------------------------------------------------------------------------- */
/* powerStatusHandler(), powerStatusTSHandler(),                             */
/* tempNamesHandler(), tempInputHandler(), tempUnitsHandler(),               */
/* tempStatusHandler(), fanNamesHandler(), fanInputHandler(),                */
/* fanUnitsHandler(), fanStatusHandler(), voltageNamesHandler(),             */
/* voltageInputHandler(), voltageUnitsHandler(), voltageStatusHandler(),     */
/* currentNamesHandler(), currentInputHandler(), currentUnitsHandler(),      */
/* currentStatusHandler(),                                                   */
/* sensorsTSHandler()                                                        */
/* ------------------------------------------------------------------------- */
/* sensorNamesHandler(), sensorInputFloatHandler(), sensorInputIntHandler(), */
/* sensorUnitsHandler(), sensorStatusHandler().                              */
/* ######################################################################### */
/* run in the DIM I/O thread when an update of service powerStatus is        */
/* required from a client                                                    */
void powerStatusHandler(long *tag,int **address,int *size)
{
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered.");
  }
  if(pthread_mutex_lock(&pwStatus.mutex))eExit("pthread_mutex_lock()");
  *address=&pwStatus.status[*tag];
  if(pthread_mutex_unlock(&pwStatus.mutex))eExit("pthread_mutex_unlock()");
  *size=sizeof(int);
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done.");
  }
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service powerStatusTS is      */
/* required from a client                                                    */
void powerStatusTSHandler(long *tag,int **address,int *size)
{
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered.");
  }
  if(pthread_mutex_lock(&pwStatus.mutex))eExit("pthread_mutex_lock()");
  *address=(int*)&pwStatus.timeStamp[*tag];
  if(pthread_mutex_unlock(&pwStatus.mutex))eExit("pthread_mutex_unlock()");
  *size=sizeof(unsigned);
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done.");
  }
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service temp/names is         */
/* required from a client                                                    */
void tempNamesHandler(long *tag,int **address,int *size)
{
  sensorNamesHandler(tag,address,size,"Temperature");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service temp/input is         */
/* required from a client                                                    */
void tempInputHandler(long *tag,int **address,int *size)
{
  sensorInputFloatHandler(tag,address,size,"Temperature");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service temp/units is         */
/* required from a client                                                    */
void tempUnitsHandler(long *tag,int **address,int *size)
{
  sensorUnitsHandler(tag,address,size,"Temperature");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service temp/status is        */
/* required from a client                                                    */
void tempStatusHandler(long *tag,int **address,int *size)
{
  sensorStatusHandler(tag,address,size,"Temperature");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service fan/names is          */
/* required from a client                                                    */
void fanNamesHandler(long *tag,int **address,int *size)
{
  sensorNamesHandler(tag,address,size,"Fan");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service fan/input is          */
/* required from a client                                                    */
void fanInputHandler(long *tag,int **address,int *size)
{
  sensorInputIntHandler(tag,address,size,"Fan");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service fan/units is          */
/* required from a client                                                    */
void fanUnitsHandler(long *tag,int **address,int *size)
{
  sensorUnitsHandler(tag,address,size,"Fan");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service fan/status is         */
/* required from a client                                                    */
void fanStatusHandler(long *tag,int **address,int *size)
{
  sensorStatusHandler(tag,address,size,"Fan");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service voltage/names is      */
/* required from a client                                                    */
void voltageNamesHandler(long *tag,int **address,int *size)
{
  sensorNamesHandler(tag,address,size,"Voltage");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service voltage/input is      */
/* required from a client                                                    */
void voltageInputHandler(long *tag,int **address,int *size)
{
  sensorInputFloatHandler(tag,address,size,"Voltage");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service voltage/units is      */
/* required from a client                                                    */
void voltageUnitsHandler(long *tag,int **address,int *size)
{
  sensorUnitsHandler(tag,address,size,"Voltage");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service voltage/status is     */
/* required from a client                                                    */
void voltageStatusHandler(long *tag,int **address,int *size)
{
  sensorStatusHandler(tag,address,size,"Voltage");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service current/names is      */
/* required from a client                                                    */
void currentNamesHandler(long *tag,int **address,int *size)
{
  sensorNamesHandler(tag,address,size,"Current");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service current/input is      */
/* required from a client                                                    */
void currentInputHandler(long *tag,int **address,int *size)
{
  sensorInputFloatHandler(tag,address,size,"Current");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service current/units is      */
/* required from a client                                                    */
void currentUnitsHandler(long *tag,int **address,int *size)
{
  sensorUnitsHandler(tag,address,size,"Current");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service current/status is     */
/* required from a client                                                    */
void currentStatusHandler(long *tag,int **address,int *size)
{
  sensorStatusHandler(tag,address,size,"Current");
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service required from client  */
void sensorNamesHandler(long *tag,int **address,int *size,char *sType)
{
  static char *sensorNames=NULL;
  static char *buff=NULL;
  int buffLen=0;
  char *lp=NULL;                                             /* line pointer */
  char *elp=NULL;                                        /* end line pointer */
  char *fp=NULL;                                            /* field pointer */
  char *efp=NULL;                                       /* end field pointer */
  int sensorN=0;
  int sensorSz=0;
  int len=0;
  int selected=0;
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered.");
  }
  /* copy the node sensor data to a local buffer */
  if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
  buffLen=1+strlen(sensors.status[*tag]);
  buff=(char*)realloc(buff,buffLen*sizeof(char));
  strcpy(buff,sensors.status[*tag]);
  if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
  /* parse the local buffer */
  if(!strcmp(buff,IPMI_NOT_YET_AVAIL_S))         /* data not (yet) available */
  {
    sensorN=1;
    sensorSz=1+strlen(IPMI_NOT_YET_AVAIL_SL);
    sensorNames=(char*)realloc(sensorNames,sensorSz*sizeof(char));
    strcpy(sensorNames,IPMI_NOT_YET_AVAIL_SL);
  }
  else if(!strcmp(buff,IPMI_NOT_RESP_S))              /* ipmi not responding */
  {
    sensorN=1;
    sensorSz=1+strlen(IPMI_NOT_RESP_SL);
    sensorNames=(char*)realloc(sensorNames,sensorSz*sizeof(char));
    strcpy(sensorNames,IPMI_NOT_RESP_SL);
  }
  else                                  /* data gathered AND ipmi responding */
  {
    for(lp=buff,sensorN=0,sensorSz=0;*lp;)              /* loop over sensors */
    {
      elp=strchr(lp,';');
      if(!elp)break;
      *elp='\0';
      /* lp=record, i.e. null-terminated string containing data for 1 sensor */
      selected=sensorSelect(lp,sType);
      if(selected==-1)
      {
        if(deBug&0x400)
        {
          mPrintf(errU,VERB,__func__,0,"Badly formatted sensor record \"%s\" "
                  "received by host \"%s\": the number of comma-separated "
                  "fields in record is different from 4!",lp,
                  getHostName(*tag));
        }
      }
      else if(selected==-2)
      {
        if(deBug&0x400)
        {
          mPrintf(errU,VERB,__func__,0,"Badly formatted sensor record \"%s\" "
                  "received by host \"%s\": first comma-separated field in "
                  "record is empty!",lp,getHostName(*tag));
        }
      }
      else if(selected)
      {
        /* Here we are sure that the record has 4 comma-separated fields and */
        /* that the 1-st comma-separated field is not empty                  */
        /* (e.g.: lp="Planar Temp,28.0,degrees C,ok")                        */
        sensorN++;
        fp=lp;                                           /* name (1st field) */
        efp=strchr(fp,',');
        *efp='\0';
        len=1+strlen(fp);
        sensorSz+=len;
        sensorNames=(char*)realloc(sensorNames,sensorSz);
        strcpy(sensorNames+sensorSz-len,fp);
      }
      lp=elp+1;
    }                                                   /* loop over sensors */
    if(!sensorN)
    {
      sensorN=1;
      sensorSz=1+strlen(IPMI_NOT_AVAIL_SL);
      sensorNames=(char*)realloc(sensorNames,sensorSz*sizeof(char));
      strcpy(sensorNames,IPMI_NOT_AVAIL_SL);
    }
  }                                     /* data gathered AND ipmi responding */
  *size=sensorSz;
  *address=(int*)sensorNames;
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done.");
  }
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service required from client  */
void sensorInputIntHandler(long *tag,int **address,int *size,char *sType)
{
  static int *sensorInput=NULL;
  static char *buff=NULL;
  int buffLen=0;
  char *lp=NULL;                                             /* line pointer */
  char *elp=NULL;                                        /* end line pointer */
  char *fp=NULL;                                            /* field pointer */
  char *efp=NULL;                                       /* end field pointer */
  int sensorN=0;
  int selected=0;
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered.");
  }
  /* copy the node sensor data to a local buffer */
  if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
  buffLen=1+strlen(sensors.status[*tag]);
  buff=(char*)realloc(buff,buffLen*sizeof(char));
  strcpy(buff,sensors.status[*tag]);
  if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
  /* parse the local buffer */
  if(!strcmp(buff,IPMI_NOT_YET_AVAIL_S))         /* data not (yet) available */
  {
    sensorN=1;
    sensorInput=(int*)realloc(sensorInput,sensorN*sizeof(int));
    sensorInput[0]=IPMI_NOT_YET_AVAIL;
  }
  else if(!strcmp(buff,IPMI_NOT_RESP_S))              /* ipmi not responding */
  {
    sensorN=1;
    sensorInput=(int*)realloc(sensorInput,sensorN*sizeof(int));
    sensorInput[0]=IPMI_NOT_RESP;
  }
  else                                  /* data gathered AND ipmi responding */
  {
    for(lp=buff,sensorN=0;*lp;)                         /* loop over sensors */
    {
      elp=strchr(lp,';');
      if(!elp)break;
      *elp='\0';
      /* lp=record, i.e. null-terminated string containing data for 1 sensor */
      selected=sensorSelect(lp,sType);
      if(selected==-1)
      {
        if(deBug&0x400)
        {
          mPrintf(errU,VERB,__func__,0,"Badly formatted sensor record \"%s\" "
                  "received by host \"%s\": the number of comma-separated "
                  "fields in record is different from 4!",lp,
                  getHostName(*tag));
        }
      }
      else if(selected==-2)
      {
        if(deBug&0x400)
        {
          mPrintf(errU,VERB,__func__,0,"Badly formatted sensor record \"%s\" "
                  "received by host \"%s\": first comma-separated field in "
                  "record is empty!",lp,getHostName(*tag));
        }
      }
      else if(selected)
      {
        /* Here we are sure that the record has 4 comma-separated fields and */
        /* that the 1-st comma-separated field is not empty                  */
        /* (e.g.: lp="Planar Temp,28.0,degrees C,ok")                        */
        sensorN++;
        sensorInput=(int*)realloc(sensorInput,sensorN*sizeof(int));
        fp=lp;                                           /* name (1st field) */
        efp=strchr(fp,',');
        fp=efp+1;                                      /* input (2nd field ) */
        efp=strchr(fp,',');
        *efp='\0';
        if(!strcmp(fp,""))sensorInput[sensorN-1]=IPMI_SENSOR_UNREADABLE;
        else sensorInput[sensorN-1]=(int)strtol(fp,(char**)NULL,0);
      }
      lp=elp+1;
    }                                                   /* loop over sensors */
    if(!sensorN)
    {
      sensorN=1;
      sensorInput=(int*)realloc(sensorInput,sensorN*sizeof(int));
      sensorInput[0]=IPMI_NOT_AVAIL;
    }
  }                                     /* data gathered AND ipmi responding */
  *size=sensorN*sizeof(int);
  *address=sensorInput;
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done.");
  }
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service required from client  */
void sensorInputFloatHandler(long *tag,int **address,int *size,char *sType)
{
  static float *sensorInput=NULL;
  static char *buff=NULL;
  int buffLen=0;
  char *lp=NULL;                                             /* line pointer */
  char *elp=NULL;                                        /* end line pointer */
  char *fp=NULL;                                            /* field pointer */
  char *efp=NULL;                                       /* end field pointer */
  int sensorN=0;
  int selected=0;
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered.");
  }
  /* copy the node sensor data to a local buffer */
  if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
  buffLen=1+strlen(sensors.status[*tag]);
  buff=(char*)realloc(buff,buffLen*sizeof(char));
  strcpy(buff,sensors.status[*tag]);
  if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
  /* parse the local buffer */
  if(!strcmp(buff,IPMI_NOT_YET_AVAIL_S))         /* data not (yet) available */
  {
    sensorN=1;
    sensorInput=(float*)realloc(sensorInput,sensorN*sizeof(float));
    sensorInput[0]=(float)IPMI_NOT_YET_AVAIL;
  }
  else if(!strcmp(buff,IPMI_NOT_RESP_S))              /* ipmi not responding */
  {
    sensorN=1;
    sensorInput=(float*)realloc(sensorInput,sensorN*sizeof(float));
    sensorInput[0]=(float)IPMI_NOT_RESP;
  }
  else                                  /* data gathered AND ipmi responding */
  {
    for(lp=buff,sensorN=0;*lp;)                         /* loop over sensors */
    {
      elp=strchr(lp,';');
      if(!elp)break;
      *elp='\0';
      /* lp=record, i.e. null-terminated string containing data for 1 sensor */
      selected=sensorSelect(lp,sType);
      if(selected==-1)
      {
        if(deBug&0x400)
        {
          mPrintf(errU,VERB,__func__,0,"Badly formatted sensor record \"%s\" "
                  "received by host \"%s\": the number of comma-separated "
                  "fields in record is different from 4!",lp,
                  getHostName(*tag));
        }
      }
      else if(selected==-2)
      {
        if(deBug&0x400)
        {
          mPrintf(errU,VERB,__func__,0,"Badly formatted sensor record \"%s\" "
                  "received by host \"%s\": first comma-separated field in "
                  "record is empty!",lp,getHostName(*tag));
        }
      }
      else if(selected)
      {
        /* Here we are sure that the record has 4 comma-separated fields and */
        /* that the 1-st comma-separated field is not empty                  */
        /* (e.g.: lp="Planar Temp,28.0,degrees C,ok")                        */
        sensorN++;
        sensorInput=(float*)realloc(sensorInput,sensorN*sizeof(float));
        fp=lp;                                           /* name (1st field) */
        efp=strchr(fp,',');
        fp=efp+1;                                      /* input (2nd field ) */
        efp=strchr(fp,',');
        *efp='\0';
        if(!strcmp(fp,""))sensorInput[sensorN-1]=(float)IPMI_SENSOR_UNREADABLE;
        else sensorInput[sensorN-1]=(float)strtof(fp,(char**)NULL);
      }
      lp=elp+1;
    }                                                   /* loop over sensors */
    if(!sensorN)
    {
      sensorN=1;
      sensorInput=(float*)realloc(sensorInput,sensorN*sizeof(float));
      sensorInput[0]=IPMI_NOT_AVAIL;
    }
  }                                     /* data gathered AND ipmi responding */
  *size=sensorN*sizeof(float);
  *address=(int*)sensorInput;
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done.");
  }
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service required from client  */
void sensorUnitsHandler(long *tag,int **address,int *size,char *sType)
{
  static char *sensorUnits=NULL;
  static char *buff=NULL;
  int buffLen=0;
  char *lp=NULL;                                             /* line pointer */
  char *elp=NULL;                                        /* end line pointer */
  char *fp=NULL;                                            /* field pointer */
  char *efp=NULL;                                       /* end field pointer */
  int sensorN=0;
  int sensorSz=0;
  int len=0;
  int selected=0;
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered.");
  }
  /* copy the node sensor data to a local buffer */
  if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
  buffLen=1+strlen(sensors.status[*tag]);
  buff=(char*)realloc(buff,buffLen*sizeof(char));
  strcpy(buff,sensors.status[*tag]);
  if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
  /* parse the local buffer */
  if(!strcmp(buff,IPMI_NOT_YET_AVAIL_S))         /* data not (yet) available */
  {
    sensorN=1;
    sensorSz=1+strlen(IPMI_NOT_YET_AVAIL_SL);
    sensorUnits=(char*)realloc(sensorUnits,sensorSz*sizeof(char));
    strcpy(sensorUnits,IPMI_NOT_YET_AVAIL_SL);
  }
  else if(!strcmp(buff,IPMI_NOT_RESP_S))              /* ipmi not responding */
  {
    sensorN=1;
    sensorSz=1+strlen(IPMI_NOT_RESP_SL);
    sensorUnits=(char*)realloc(sensorUnits,sensorSz*sizeof(char));
    strcpy(sensorUnits,IPMI_NOT_RESP_SL);
  }
  else                                  /* data gathered AND ipmi responding */
  {
    for(lp=buff,sensorN=0,sensorSz=0;*lp;)              /* loop over sensors */
    {
      elp=strchr(lp,';');
      if(!elp)break;
      *elp='\0';
      /* lp=record, i.e. string null-terminated containing data for 1 sensor */
      selected=sensorSelect(lp,sType);
      if(selected==-1)
      {
        if(deBug&0x400)
        {
          mPrintf(errU,VERB,__func__,0,"Badly formatted sensor record \"%s\" "
                  "received by host \"%s\": the number of comma-separated "
                  "fields in record is different from 4!",lp,
                  getHostName(*tag));
        }
      }
      else if(selected==-2)
      {
        if(deBug&0x400)
        {
          mPrintf(errU,VERB,__func__,0,"Badly formatted sensor record \"%s\" "
                  "received by host \"%s\": first comma-separated field in "
                  "record is empty!",lp,getHostName(*tag));
        }
      }
      else if(selected)
      {
        /* Here we are sure that the record has 4 comma-separated fields and */
        /* that the 1-st comma-separated field is not empty                  */
        /* (e.g.: lp="Planar Temp,28.0,degrees C,ok")                        */
        sensorN++;
        fp=lp;                                           /* name (1st field) */
        efp=strchr(fp,',');
        fp=efp+1;                                      /* input (2nd field ) */
        efp=strchr(fp,',');
        fp=efp+1;                                      /* units (3rd field ) */
        efp=strchr(fp,',');
        *efp='\0';
        if(!strcmp(fp,""))
        {
          len=1+strlen(IPMI_SENSOR_UNREADABLE_SL);
          sensorSz+=len;
          sensorUnits=(char*)realloc(sensorUnits,sensorSz);
          strcpy(sensorUnits+sensorSz-len,IPMI_SENSOR_UNREADABLE_SL);
        }
        else
        {
          len=1+strlen(fp);
          sensorSz+=len;
          sensorUnits=(char*)realloc(sensorUnits,sensorSz);
          strcpy(sensorUnits+sensorSz-len,fp);
        }
      }
      lp=elp+1;
    }                                                   /* loop over sensors */
    if(!sensorN)
    {
      sensorN=1;
      sensorSz=1+strlen(IPMI_NOT_AVAIL_SL);
      sensorUnits=(char*)realloc(sensorUnits,sensorSz*sizeof(char));
      strcpy(sensorUnits,IPMI_NOT_AVAIL_SL);
    }
  }                                     /* data gathered AND ipmi responding */
  *size=sensorSz;
  *address=(int*)sensorUnits;
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done.");
  }
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service required from client  */
void sensorStatusHandler(long *tag,int **address,int *size,char *sType)
{
  static char *sensorStatus=NULL;
  static char *buff=NULL;
  int buffLen=0;
  char *lp=NULL;                                             /* line pointer */
  char *elp=NULL;                                        /* end line pointer */
  char *fp=NULL;                                            /* field pointer */
  char *efp=NULL;                                       /* end field pointer */
  int sensorN=0;
  int sensorSz=0;
  int len=0;
  int selected=0;
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered.");
  }
  /* copy the node sensor data to a local buffer */
  if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
  buffLen=1+strlen(sensors.status[*tag]);
  buff=(char*)realloc(buff,buffLen*sizeof(char));
  strcpy(buff,sensors.status[*tag]);
  if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
  /* parse the local buffer */
  if(!strcmp(buff,IPMI_NOT_YET_AVAIL_S))         /* data not (yet) available */
  {
    sensorN=1;
    sensorSz=1+strlen(IPMI_NOT_YET_AVAIL_SL);
    sensorStatus=(char*)realloc(sensorStatus,sensorSz*sizeof(char));
    strcpy(sensorStatus,IPMI_NOT_YET_AVAIL_SL);
  }
  else if(!strcmp(buff,IPMI_NOT_RESP_S))              /* ipmi not responding */
  {
    sensorN=1;
    sensorSz=1+strlen(IPMI_NOT_RESP_SL);
    sensorStatus=(char*)realloc(sensorStatus,sensorSz*sizeof(char));
    strcpy(sensorStatus,IPMI_NOT_RESP_SL);
  }
  else                                  /* data gathered AND ipmi responding */
  {
    for(lp=buff,sensorN=0,sensorSz=0;*lp;)              /* loop over sensors */
    {
      elp=strchr(lp,';');
      if(!elp)break;
      *elp='\0';
      /* lp=record, i.e. null-terminated string containing data for 1 sensor */
      selected=sensorSelect(lp,sType);
      if(selected==-1)
      {
        if(deBug&0x400)
        {
          mPrintf(errU,VERB,__func__,0,"Badly formatted sensor record \"%s\" "
                  "received by host \"%s\": the number of comma-separated "
                  "fields in record is different from 4!",lp,
                  getHostName(*tag));
        }
      }
      else if(selected==-2)
      {
        if(deBug&0x400)
        {
          mPrintf(errU,VERB,__func__,0,"Badly formatted sensor record \"%s\" "
                  "received by host \"%s\": first comma-separated field in "
                  "record is empty!",lp,getHostName(*tag));
        }
      }
      else if(selected)
      {
        /* Here we are sure that the record has 4 comma-separated fields and */
        /* that the 1-st comma-separated field is not empty                  */
        /* (e.g.: lp="Planar Temp,28.0,degrees C,ok")                        */
        sensorN++;
        fp=lp;                                           /* name (1st field) */
        efp=strchr(fp,',');
        fp=efp+1;                                      /* input (2nd field ) */
        efp=strchr(fp,',');
        fp=efp+1;                                      /* units (3rd field ) */
        efp=strchr(fp,',');
        fp=efp+1;                                     /* status (4th field ) */
        if(!strcmp(fp,""))
        {
          len=1+strlen(IPMI_SENSOR_UNREADABLE_SL);
          sensorSz+=len;
          sensorStatus=(char*)realloc(sensorStatus,sensorSz);
          strcpy(sensorStatus+sensorSz-len,IPMI_SENSOR_UNREADABLE_SL);
        }
        else
        {
          len=1+strlen(fp);
          sensorSz+=len;
          sensorStatus=(char*)realloc(sensorStatus,sensorSz);
          strcpy(sensorStatus+sensorSz-len,fp);
        }
      }
      lp=elp+1;
    }                                                   /* loop over sensors */
    if(!sensorN)
    {
      sensorN=1;
      sensorSz=1+strlen(IPMI_NOT_AVAIL_SL);
      sensorStatus=(char*)realloc(sensorStatus,sensorSz*sizeof(char));
      strcpy(sensorStatus,IPMI_NOT_AVAIL_SL);
    }
  }                                     /* data gathered AND ipmi responding */
  *size=sensorSz;
  *address=(int*)sensorStatus;
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done.");
  }
  return;
}
/*****************************************************************************/
/* run in the DIM I/O thread when an update of service powerStatusTS is      */
/* required from a client                                                    */
void sensorsTSHandler(long *tag,int **address,int *size)
{
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered.");
  }
  if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
  *address=(int*)&sensors.timeStamp[*tag];
  if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
  *size=sizeof(unsigned);
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done.");
  }
}
/* ######################################################################### */
/* end DIM SVC handlers                                                      */
/* ######################################################################### */
/* Updating functions                                                        */
/* ------------------------------------------------------------------------- */
/* sensorsStatusUpdate(), sensorsStatusUpdateCmd(), sensorsDicUpdate(),      */
/* sensorsDicUpdateCmd()                                                     */
/* ######################################################################### */
/* run periodically in the DIM main thread */
void sensorsStatusUpdate(int nodeN)
{
  powerCtrlData_t *pcd=NULL;
  int doSkip=0;
  /*-------------------------------------------------------------------------*/
  /* copy ipmi data in the pcd structure passed to the new thread */
  pcd=(powerCtrlData_t*)malloc(sizeof(powerCtrlData_t));
  /* node number */
  pcd->nodeN=nodeN;
  /* node hostname */
  strncpy(pcd->hostName,ipmiCfg.hostNameList[nodeN],128);
  /* username */
  if(ipmiCfg.userNameList[nodeN])
    strncpy(pcd->userName,ipmiCfg.userNameList[nodeN],128);
  else
    strncpy(pcd->userName,dfltIpmiUserName,128);
  /* password */
  if(ipmiCfg.passWordList[nodeN])
    strncpy(pcd->passWord,ipmiCfg.passWordList[nodeN],128);
  else if(!passWordFile)
    strncpy(pcd->passWord,dfltIpmiPassWord,128);
  else
    *(pcd->passWord)='\0';
  /* password file */
  if(passWordFile)
    strncpy(pcd->passWordFile,passWordFile,128);
  else
    *(pcd->passWordFile)='\0';
  /* port */
  if(ipmiCfg.portList[nodeN]!=-1)
    pcd->port=ipmiCfg.portList[nodeN];
  else
    pcd->port=dfltIpmiPort;
  /* authentication type */
  if(ipmiCfg.authTypeList[nodeN])
    strncpy(pcd->authType,ipmiCfg.authTypeList[nodeN],128);
  else
    pcd->authType[0]='\0';
  /* privilege level */
  if(ipmiCfg.privLvlList[nodeN])
    strncpy(pcd->privLvl,ipmiCfg.privLvlList[nodeN],128);
  else
    pcd->privLvl[0]='\0';
  /* OEM type */
  if(ipmiCfg.oemTypeList[nodeN])
    strncpy(pcd->oemType,ipmiCfg.oemTypeList[nodeN],128);
  else
    pcd->oemType[0]='\0';
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s.",pcd->hostName);
  }
  /* to avoid unlimited pile-up of threads in case of ipmi not responding    */
  /* if another sensorsStatusUpdateCmd() is pending then skip this update    */
  /* single BMC access arbitration */
  if(pthread_mutex_lock(&bmc.mutex))eExit("pthread_mutex_lock()");
  if(qLen(&bmc.sensSvcQ[pcd->nodeN]))doSkip=1;
  if(pthread_mutex_unlock(&bmc.mutex))eExit("pthread_mutex_unlock()");
  /* end single BMC access arbitration */
  if(doSkip)
  {
    FREE_PCD;
    if(deBug&0x100)
    {
      mPrintf(errU,VERB,__func__,0,"Sensor status update cancelled for node "
              "\"%s\" due to single BMC access arbitration (BMC busy on node "
              "\"%s\"). Total thread number: %d.",ipmiCfg.hostNameList[nodeN],
              ipmiCfg.hostNameList[nodeN],getThreadN(errU));
    }
    return;
  }
  /* thread attributes */
  if(pthread_attr_init(&t_attr)==-1)
    thReturn("pthread_attr_init()");
  if(pthread_attr_setinheritsched(&t_attr,PTHREAD_INHERIT_SCHED)==-1)
    thReturn("pthread_attr_setinheritsched()");
  if(pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_DETACHED)==-1)
    thReturn("pthread_attr_setdetachstate()");
  if(pthread_attr_setstacksize(&t_attr,THREAD_STACK_SIZE)==-1)
    thReturn("pthread_attr_setstacksize()");
  /* create the new thread for sensorsStatusUpdateCmd() */
  if(pthread_create(&tid,&t_attr,sensorsStatusUpdateCmd,pcd)==-1)
    thReturn("pthread_create()");
  if(pthread_detach(tid)==-1)
    thReturn("pthread_detach()");
}
/*****************************************************************************/
/* run in a short-living thread created by sensorsStatusUpdate() */
void *sensorsStatusUpdateCmd(void *data)
{
  powerCtrlData_t *pcd=NULL;
  char *status=NULL;
  unsigned timeStamp=0;
  /*-------------------------------------------------------------------------*/
  pthread_detach(pthread_self());
  pcd=(powerCtrlData_t*)data;
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s.",pcd->hostName);
  }
  /* single BMC access arbitration */
  if(pthread_mutex_lock(&bmc.mutex))eExit("pthread_mutex_lock()");
  /* add this thread to the sensor service queue */
  qAdd(&bmc.sensSvcQ[pcd->nodeN],pthread_self());
  while(bmc.busy[pcd->nodeN]||
       !qIsFirst(&bmc.sensSvcQ[pcd->nodeN],pthread_self())||
       qLen(&bmc.cmdQ[pcd->nodeN]))
  {
    if(pthread_cond_wait(&bmc.cond,&bmc.mutex))eExit("pthread_cond_wait()");
  }
  bmc.busy[pcd->nodeN]=1;
  if(pthread_cond_broadcast(&bmc.cond))eExit("pthread_cond_broadcast()");
  if(pthread_mutex_unlock(&bmc.mutex))eExit("pthread_mutex_unlock()");
  /* end single BMC access arbitration */
  status=getSensorsStatus(pcd->hostName,pcd->userName,pcd->passWord,
                          pcd->passWordFile,pcd->port,pcd->authType,
                          pcd->privLvl,pcd->oemType);
  timeStamp=(unsigned)time(NULL);
  /* single BMC access arbitration */
  if(pthread_mutex_lock(&bmc.mutex))eExit("pthread_mutex_lock()");
  bmc.busy[pcd->nodeN]=0;
  /* remove this thread from the sensor service queue */
  qRm(&bmc.sensSvcQ[pcd->nodeN]);
  if(pthread_cond_broadcast(&bmc.cond))eExit("pthread_cond_broadcast()");
  if(pthread_mutex_unlock(&bmc.mutex))eExit("pthread_mutex_unlock()");
  /* end single BMC access arbitration */
  if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
  if(sensors.status[pcd->nodeN])free(sensors.status[pcd->nodeN]);
  sensors.status[pcd->nodeN]=status;
  sensors.timeStamp[pcd->nodeN]=timeStamp;
  if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
  dis_update_service(tempNamesSvcID[pcd->nodeN]);
  dis_update_service(tempInputSvcID[pcd->nodeN]);
  dis_update_service(tempStatusSvcID[pcd->nodeN]);
  dis_update_service(tempUnitsSvcID[pcd->nodeN]);
  dis_update_service(fanNamesSvcID[pcd->nodeN]);
  dis_update_service(fanInputSvcID[pcd->nodeN]);
  dis_update_service(fanStatusSvcID[pcd->nodeN]);
  dis_update_service(fanUnitsSvcID[pcd->nodeN]);
  dis_update_service(voltageNamesSvcID[pcd->nodeN]);
  dis_update_service(voltageInputSvcID[pcd->nodeN]);
  dis_update_service(voltageStatusSvcID[pcd->nodeN]);
  dis_update_service(voltageUnitsSvcID[pcd->nodeN]);
  dis_update_service(currentNamesSvcID[pcd->nodeN]);
  dis_update_service(currentInputSvcID[pcd->nodeN]);
  dis_update_service(currentStatusSvcID[pcd->nodeN]);
  dis_update_service(currentUnitsSvcID[pcd->nodeN]);
  dis_update_service(sensorsTSSvcID[pcd->nodeN]);
  //dis_update_service(pwStatTSSvcID[pcd->nodeN]);
  if(!strcmp(status,IPMI_NOT_RESP_S))
  {
    mPrintf(errU,ERROR,__func__,0,"Failed to get the sensor data of the node "
            "\"%s\": %s!",pcd->hostName,IPMI_NOT_RESP_SL);
  }
  else
  {
    if(deBug&0x2)
    {
      mPrintf(errU,DEBUG,__func__,0,"Node: %s. Sensor data: %s",pcd->hostName,
              status);
    }
  }
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done. Hostname = %s.",pcd->hostName);
  }
  FREE_PCD;
  pthread_exit(NULL);
}
/*****************************************************************************/
/* run in the initialization phase in the DIM main thread */
void sensorsDicUpdate(int nodeN)
{
  powerCtrlData_t *pcd=NULL;
  /*-------------------------------------------------------------------------*/
  /* copy ipmi data in the pcd structure passed to the new thread */
  pcd=(powerCtrlData_t*)malloc(sizeof(powerCtrlData_t));
  /* node number */
  pcd->nodeN=nodeN;
  /* node hostname */
  strncpy(pcd->hostName,ipmiCfg.hostNameList[nodeN],128);
  /* username */
  if(ipmiCfg.userNameList[nodeN])
    strncpy(pcd->userName,ipmiCfg.userNameList[nodeN],128);
  else
    strncpy(pcd->userName,dfltIpmiUserName,128);
  /* password */
  if(ipmiCfg.passWordList[nodeN])
    strncpy(pcd->passWord,ipmiCfg.passWordList[nodeN],128);
  else if(!passWordFile)
    strncpy(pcd->passWord,dfltIpmiPassWord,128);
  else
    *(pcd->passWord)='\0';
  /* password file */
  if(passWordFile)
    strncpy(pcd->passWordFile,passWordFile,128);
  else
    *(pcd->passWordFile)='\0';
  /* port */
  if(ipmiCfg.portList[nodeN]!=-1)
    pcd->port=ipmiCfg.portList[nodeN];
  else
    pcd->port=dfltIpmiPort;
  /* authentication type */
  if(ipmiCfg.authTypeList[nodeN])
    strncpy(pcd->authType,ipmiCfg.authTypeList[nodeN],128);
  else
    pcd->authType[0]='\0';
  /* privilege level */
  if(ipmiCfg.privLvlList[nodeN])
    strncpy(pcd->privLvl,ipmiCfg.privLvlList[nodeN],128);
  else
    pcd->privLvl[0]='\0';
  /* OEM type */
  if(ipmiCfg.oemTypeList[nodeN])
    strncpy(pcd->oemType,ipmiCfg.oemTypeList[nodeN],128);
  else
    pcd->oemType[0]='\0';
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s.",pcd->hostName);
  }
  /* thread attributes */
  if(pthread_attr_init(&t_attr)==-1)
    thReturn("pthread_attr_init()");
  if(pthread_attr_setinheritsched(&t_attr,PTHREAD_INHERIT_SCHED)==-1)
    thReturn("pthread_attr_setinheritsched()");
  if(pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_DETACHED)==-1)
    thReturn("pthread_attr_setdetachstate()");
  if(pthread_attr_setstacksize(&t_attr,THREAD_STACK_SIZE)==-1)
    thReturn("pthread_attr_setstacksize()");
  /* create the new thread for sensorsDicUpdateCmd() */
  if(pthread_create(&tid,&t_attr,sensorsDicUpdateCmd,pcd)==-1)
    thReturn("pthread_create()");
  if(pthread_detach(tid)==-1)
    thReturn("pthread_detach()");
}
/*****************************************************************************/
/* run in a short-living thread created by sensorsDicUpdate() */
void *sensorsDicUpdateCmd(void *data)
{
  powerCtrlData_t *pcd=NULL;
  char *tempList=NULL;
  char *fanList=NULL;
  char *voltList=NULL;
  char *currList=NULL;
  char *tempList2=NULL;
  char *fanList2=NULL;
  char *voltList2=NULL;
  char *currList2=NULL;
  /*-------------------------------------------------------------------------*/
  pthread_detach(pthread_self());
  pcd=(powerCtrlData_t*)data;
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s.",pcd->hostName);
  }
  /* single BMC access arbitration */
  if(pthread_mutex_lock(&bmc.mutex))eExit("pthread_mutex_lock()");
  while(bmc.busy[pcd->nodeN])
  {
    if(pthread_cond_wait(&bmc.cond,&bmc.mutex))eExit("pthread_cond_wait()");
  }
  bmc.busy[pcd->nodeN]=1;
  if(pthread_cond_broadcast(&bmc.cond))eExit("pthread_cond_broadcast()");
  if(pthread_mutex_unlock(&bmc.mutex))eExit("pthread_mutex_unlock()");
  /* end single BMC access arbitration */
  tempList=getSensorsList(pcd->hostName,pcd->userName,pcd->passWord,
                          pcd->passWordFile,pcd->port,pcd->authType,
                          pcd->privLvl,pcd->oemType,"Temperature");
  fanList=getSensorsList(pcd->hostName,pcd->userName,pcd->passWord,
                         pcd->passWordFile,pcd->port,pcd->authType,
                         pcd->privLvl,pcd->oemType,"Fan");
  voltList=getSensorsList(pcd->hostName,pcd->userName,pcd->passWord,
                          pcd->passWordFile,pcd->port,pcd->authType,
                          pcd->privLvl,pcd->oemType,"Voltage");
  currList=getSensorsList(pcd->hostName,pcd->userName,pcd->passWord,
                          pcd->passWordFile,pcd->port,pcd->authType,
                          pcd->privLvl,pcd->oemType,"Current");
  /* single BMC access arbitration */
  if(pthread_mutex_lock(&bmc.mutex))eExit("pthread_mutex_lock()");
  bmc.busy[pcd->nodeN]=0;
  if(pthread_cond_broadcast(&bmc.cond))eExit("pthread_cond_broadcast()");
  if(pthread_mutex_unlock(&bmc.mutex))eExit("pthread_mutex_unlock()");
  /* end single BMC access arbitration */
  tempList2=strdup(tempList);
  fanList2=strdup(fanList);
  voltList2=strdup(voltList);
  currList2=strdup(currList);
  /* update sensor list */
  if(strcmp(tempList2,IPMI_NOT_RESP_S))
  {
    char *blp,*elp,*efp;
    for(blp=tempList2;blp<tempList2+strlen(tempList);)
    {
      elp=strchr(blp,';');
      if(elp)*elp='\0';
      efp=strchr(blp,',');
      if(efp)*efp='\0';
      if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
      sensAdd(&sensors.sensDicN,&sensors.sensDic,blp,"Temperature");
      if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
      blp=1+elp;
    }
  }
  if(strcmp(fanList2,IPMI_NOT_RESP_S))
  {
    char *blp,*elp,*efp;
    for(blp=fanList2;blp<fanList2+strlen(fanList);)
    {
      elp=strchr(blp,';');
      if(elp)*elp='\0';
      efp=strchr(blp,',');
      if(efp)*efp='\0';
      if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
      sensAdd(&sensors.sensDicN,&sensors.sensDic,blp,"Fan");
      if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
      blp=1+elp;
    }
  }
  if(strcmp(voltList2,IPMI_NOT_RESP_S))
  {
    char *blp,*elp,*efp;
    for(blp=voltList2;blp<voltList2+strlen(voltList);)
    {
      elp=strchr(blp,';');
      if(elp)*elp='\0';
      efp=strchr(blp,',');
      if(efp)*efp='\0';
      if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
      sensAdd(&sensors.sensDicN,&sensors.sensDic,blp,"Voltage");
      if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
      blp=1+elp;
    }
  }
  if(strcmp(currList2,IPMI_NOT_RESP_S))
  {
    char *blp,*elp,*efp;
    for(blp=currList2;blp<currList2+strlen(currList);)
    {
      elp=strchr(blp,';');
      if(elp)*elp='\0';
      efp=strchr(blp,',');
      if(efp)*efp='\0';
      if(pthread_mutex_lock(&sensors.mutex))eExit("pthread_mutex_lock()");
      sensAdd(&sensors.sensDicN,&sensors.sensDic,blp,"Current");
      if(pthread_mutex_unlock(&sensors.mutex))eExit("pthread_mutex_unlock()");
      blp=1+elp;
    }
  }
  if(!strcmp(tempList,IPMI_NOT_RESP_S)||
     !strcmp(fanList,IPMI_NOT_RESP_S)||
     !strcmp(voltList,IPMI_NOT_RESP_S)||
     !strcmp(currList,IPMI_NOT_RESP_S))
  {
    mPrintf(errU,ERROR,__func__,0,"Failed to get the sensor list of the node "
            "\"%s\": %s!",pcd->hostName,IPMI_NOT_RESP_SL);
  }
  else
  {
    if(deBug&0x2)
    {
      mPrintf(errU,DEBUG,__func__,0,"Node: %s. Temperature sensor list: %s",
              pcd->hostName,tempList);
      mPrintf(errU,DEBUG,__func__,0,"Node: %s. Fan sensor list: %s",
              pcd->hostName,fanList);
      mPrintf(errU,DEBUG,__func__,0,"Node: %s. Voltage sensor list: %s",
              pcd->hostName,voltList);
      mPrintf(errU,DEBUG,__func__,0,"Node: %s. Current sensor list: %s",
              pcd->hostName,currList);
    }
  }
  if(tempList){free(tempList);tempList=NULL;}
  if(fanList){free(fanList);fanList=NULL;}
  if(voltList){free(voltList);voltList=NULL;}
  if(currList){free(currList);currList=NULL;}
  if(tempList2){free(tempList2);tempList2=NULL;}
  if(fanList2){free(fanList2);fanList2=NULL;}
  if(voltList2){free(voltList2);voltList2=NULL;}
  if(currList2){free(currList2);currList2=NULL;}
  if(pthread_mutex_lock(&sensInitCount.mutex))eExit("pthread_mutex_lock()");
  sensInitCount.c++;
  if(pthread_mutex_unlock(&sensInitCount.mutex))eExit("pthread_mutex_lock()");
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done. Hostname = %s.",pcd->hostName);
  }
  FREE_PCD;
  pthread_exit(NULL);
}
/* ######################################################################### */
/* end Updating functions                                                    */
/* ######################################################################### */
/* Exception Handlers                                                        */
/* ------------------------------------------------------------------------- */
/* dimErrorHandler(), signalHandler()                                        */
/* ######################################################################### */
void dimErrorHandler(int severity,int errorCode,char *message)
{
  mPrintf(errU|L_SYS,severity+3,__func__,0,"DIM error message: "
          "\"%s\". (DIM error code = %d)!",message,errorCode);
  if(severity==DIM_FATAL)exit(1);
  return;
}
/*****************************************************************************/
/* signalHandler() executed synchronously in the main thread. So don't warry */
/* about async-signal-safe.                                                  */
static void signalHandler(int signo)
{
  int i=0;
  /*-------------------------------------------------------------------------*/
  for(i=0;i<ipmiCfg.nodeN;i++)
  {
    dis_remove_service(pwSwitchCmdID[i]);
    dis_remove_service(pwStatSvcID[i]);
    dis_remove_service(pwStatTSSvcID[i]);
    dis_remove_service(tempNamesSvcID[i]);
    dis_remove_service(tempInputSvcID[i]);
    dis_remove_service(tempUnitsSvcID[i]);
    dis_remove_service(tempStatusSvcID[i]);
    dis_remove_service(fanNamesSvcID[i]);
    dis_remove_service(fanInputSvcID[i]);
    dis_remove_service(fanUnitsSvcID[i]);
    dis_remove_service(fanStatusSvcID[i]);
    dis_remove_service(voltageNamesSvcID[i]);
    dis_remove_service(voltageInputSvcID[i]);
    dis_remove_service(voltageUnitsSvcID[i]);
    dis_remove_service(voltageStatusSvcID[i]);
    dis_remove_service(currentNamesSvcID[i]);
    dis_remove_service(currentInputSvcID[i]);
    dis_remove_service(currentUnitsSvcID[i]);
    dis_remove_service(currentStatusSvcID[i]);
    dis_remove_service(sensorsTSSvcID[i]);
  }
  dis_remove_service(versionSvcID);
  dis_remove_service(aVersionSvcID);
  dis_remove_service(fmcVersionSvcID);
  dis_remove_service(successSvcID);
  dis_stop_serving();
  if(signo==SIGTERM || signo==SIGINT)
  {
    mPrintf(errU|L_SYS,WARN,__func__,0,"Signal %d (%s) caught. "
          "Exiting...",signo,sig2msg(signo));
    _exit(0);
  }
  else
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"Signal %d (%s) caught. "
          "Exiting...",signo,sig2msg(signo));
    _exit(signo);
  }
  /*-------------------------------------------------------------------------*/
}
/* ######################################################################### */
/* end Exception Handlers                                                    */
/* ######################################################################### */
/* Utilities                                                                 */
/* ------------------------------------------------------------------------- */
/* strDupUpper(), sensorSelect(), getHostN(), getHostName(), getCmdStr(),    */
/* getCmdCode()                                                              */
/* ######################################################################### */
char *strDupUpper(char *s)
{
  char *us=NULL;
  char *p=NULL;
  us=(char*)malloc(1+strlen(s));
  strncpy(us,s,1+strlen(s));
  for(p=us;*p;p++)*p=(char)toupper(*p);
  return us;
}
/*****************************************************************************/
/* return 1 if string has, as first field, a sensor name of type sType       */
int sensorSelect(char *string,char *sType)
{
  char *sName=NULL;
  char *fp=NULL;
  char *efp=NULL;
  int rv=0;
  int commaCnt;
  /*-------------------------------------------------------------------------*/
  sName=strdup(string);
  /* count the number of commas in record sName (should be 3, i.e. 4 fields) */
  for(fp=sName,commaCnt=0;;)
  {
    efp=strchr(fp,',');
    if(!efp)break;
    commaCnt++;
    fp=efp+1;
  }
  if(commaCnt!=3)             /* number of fields in record different from 4 */
  {
    if(sName)free(sName);
    return -1;
  }
  efp=strchr(sName,',');
  if(efp)*efp='\0';                                    /* select first field */
  if(sName[0]=='\0')                       /* first field in record is empty */
  {
    if(sName)free(sName);
    return -2;
  }
  rv=!strcmp(getSensType(sName,sensors.sensDicN,sensors.sensDic),sType);
  if(sName)free(sName);
  return rv;
}
/*****************************************************************************/
int getHostN(char *hostName)
{
  int i;
  /*-------------------------------------------------------------------------*/
  for(i=0;i<ipmiCfg.nodeN;i++)
    if(!strcmp(hostName,ipmiCfg.hostNameList[i]))
      return i;
  return -1;
}
/*****************************************************************************/
char *getHostName(int hostN)
{
  if(hostN>=0 && hostN<ipmiCfg.nodeN)return ipmiCfg.hostNameList[hostN];
  else return ("Unknown host");
}
/*****************************************************************************/
char *getCmdStr(unsigned char code)
{
  if(code==IPMI_CHASSIS_CTL_POWER_UP)return("on");
  if(code==IPMI_CHASSIS_CTL_POWER_DOWN)return("off");
  if(code==IPMI_CHASSIS_CTL_POWER_CYCLE)return("cycle");
  if(code==IPMI_CHASSIS_CTL_ACPI_SOFT)return("soft_off");
  if(code==IPMI_CHASSIS_CTL_HARD_RESET)return("hard_reset");
  if(code==IPMI_CHASSIS_CTL_PULSE_DIAG)return("pulse_diag");
  return("Unknown command");
}
/*****************************************************************************/
unsigned char getCmdCode(char *cmnd)
{
  if(!strncmp(cmnd,"on",2))return IPMI_CHASSIS_CTL_POWER_UP;
  if(!strncmp(cmnd,"off",3))return IPMI_CHASSIS_CTL_POWER_DOWN;
  if(!strncmp(cmnd,"cycle",5))return IPMI_CHASSIS_CTL_POWER_CYCLE;
  if(!strncmp(cmnd,"soft_off",8))return IPMI_CHASSIS_CTL_ACPI_SOFT;
  if(!strncmp(cmnd,"hard_reset",10))return IPMI_CHASSIS_CTL_HARD_RESET;
  if(!strncmp(cmnd,"pulse_diag",10))return IPMI_CHASSIS_CTL_PULSE_DIAG;
  return 0xff;
}
/* ######################################################################### */
/* end Utilities                                                             */
/* ######################################################################### */
/* Sensor Dictionary Management                                              */
/* ------------------------------------------------------------------------- */
/* sensAdd(), sensCmp(), sensPrint(), getSensType()                          */
/* ######################################################################### */
void sensAdd(int *sensDicN,sc_t **sensDic,char *name,char *type)
{
  sc_t key,*res;
  key.name=name;
  res=bsearch(&key,*sensDic,*sensDicN,sizeof(sc_t),sensCmp);
  if(res)return;
  (*sensDicN)++;
  *sensDic=(sc_t*)realloc(*sensDic,(*sensDicN)*sizeof(sc_t));
  (*sensDic)[(*sensDicN)-1].name=strdup(name);
  (*sensDic)[(*sensDicN)-1].type=type;
  qsort(*sensDic,*sensDicN,sizeof(sc_t),sensCmp);
}
/*****************************************************************************/
/* used by sensAdd() and getSensType() for qsort and bsearch */
static int sensCmp(const void *s1, const void *s2)
{
  sc_t *sc1=(sc_t*)s1;
  sc_t *sc2=(sc_t*)s2;
  return strcmp(sc1->name,sc2->name);
}
/*****************************************************************************/
void sensPrint(int sensDicN,sc_t *sensDic)
{
  int i;
  for(i=0;i<sensDicN;i++)
  {
    mPrintf(errU,DEBUG,__func__,0,"sensor %d: name=\"%s\", type=\"%s\".",i,
            sensDic[i].name,sensDic[i].type);
  }
}
/*****************************************************************************/
char *getSensType(char *name,int sensDicN,sc_t *sensDic)
{
  sc_t key,*res;
  key.name=name;
  res=bsearch(&key,sensDic,sensDicN,sizeof(sc_t),sensCmp);
  if(res)return res->type;
  if(!fnmatch("*TEMP*",name,0))return "Temperature";
  if(!fnmatch("*[tT]emp*",name,0))return "Temperature";
  if(!fnmatch("*FAN*",name,0))return "Fan";
  if(!fnmatch("*[fF]an*",name,0))return "Fan";
  if(!fnmatch("*BATTERY*",name,0))return "Voltage";
  if(!fnmatch("*[bB]attery*",name,0))return "Voltage";
  if(!fnmatch("*VCORE*",name,0))return "Voltage";
  if(!fnmatch("*[vV]core*",name,0))return "Voltage";
  if(!fnmatch("*CURRENT*",name,0))return "Current";
  if(!fnmatch("*[cC]urrent*",name,0))return "Current";
  return "Unknown";
}
/* ######################################################################### */
/* end Sensor Dictionary Management                                          */
/* ######################################################################### */
/* Global CMD Fifo Management Utilities                                      */
/* ------------------------------------------------------------------------- */
/* gQpush(), gQpop(), gQprint(), gQlen()                                     */
/* ######################################################################### */
/* add one element at the end of the queue */
void gQpush(globalQ_t *queue,int nodeN,int cmdN)
{
  globalQe_t *el=NULL;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&queue->mutex))eExit("pthread_mutex_lock()");
  el=(globalQe_t*)malloc(sizeof(globalQe_t));
  el->nodeN=nodeN;
  el->cmdN=cmdN;
  el->next=NULL;
  if(queue->tail)queue->tail->next=el;
  else queue->head=el;
  queue->tail=el;
  if(pthread_mutex_unlock(&queue->mutex))eExit("pthread_mutex_unlock()");
  return;
}
/*****************************************************************************/
/* remove and return the element at the beginning of the queue */
globalQe_t gQpop(globalQ_t *queue)
{
  globalQe_t *el=NULL;
  globalQe_t *next=NULL;
  globalQe_t rv={-1,0xff,NULL};
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&queue->mutex))eExit("pthread_mutex_lock()");
  el=queue->head;
  if(el)
  {
    rv=*el;
    next=el->next;
    queue->head=next;
    if(!next)queue->tail=NULL;
    free(el);
  }
  if(pthread_mutex_unlock(&queue->mutex))eExit("pthread_mutex_unlock()");
  return rv;
}
/*****************************************************************************/
void gQprint(globalQ_t *queue,int severity)
{
  globalQe_t *el=NULL;
  char *msg=NULL;
  int msgl=0;
  char *p=msg;
  /*-------------------------------------------------------------------------*/
  msgl=snprintf(NULL,0,"pwSwitch command queue:");
  msg=(char*)malloc(msgl+1);
  sprintf(msg,"pwSwitch command queue:");
  if(pthread_mutex_lock(&queue->mutex))eExit("pthread_mutex_lock()");
  for(el=queue->head;el;el=el->next)
  {
    msgl+=snprintf(NULL,0," (%s,%s)",getHostName(el->nodeN),
                   getCmdStr(el->cmdN));
    msg=(char*)realloc(msg,msgl+1);
    p=strchr(msg,'\0');
    sprintf(p," (%s,%s)",getHostName(el->nodeN),getCmdStr(el->cmdN));
  }
  if(pthread_mutex_unlock(&queue->mutex))eExit("pthread_mutex_unlock()");
  mPrintf(errU,severity,__func__,0,"%s",msg);
  if(msg)free(msg);
}
/*****************************************************************************/
int gQlen(globalQ_t *queue)
{
  globalQe_t *el=NULL;
  int i=0;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&queue->mutex))eExit("pthread_mutex_lock()");
  for(el=queue->head;el;el=el->next)i++;
  if(pthread_mutex_unlock(&queue->mutex))eExit("pthread_mutex_unlock()");
  return i;
}
/* ######################################################################### */
/* end Global CMD Fifo Management Utilities                                  */
/* ######################################################################### */
/* BMC Fifo Management Utilities                                             */
/* ------------------------------------------------------------------------- */
/* qAdd(), qRm(), qIsFirst(), qMsgSnd(), qLen()                              */
/* ######################################################################### */
/* add element at the end of the queue */
void qAdd(bmcQ_t *queue,pthread_t tid)
{
  bmcQe_t *el=NULL;
  /*-------------------------------------------------------------------------*/
  el=(bmcQe_t*)malloc(sizeof(bmcQe_t));
  el->tid=tid;
  el->next=NULL;
  if(queue->tail)queue->tail->next=el;
  else queue->head=el;
  queue->tail=el;
}
/*****************************************************************************/
/* remove element from the beginning of the queue */
void qRm(bmcQ_t *queue)
{
  bmcQe_t *el=NULL;
  bmcQe_t *next=NULL;
  /*-------------------------------------------------------------------------*/
  el=queue->head;
  if(el)
  {
    next=el->next;
    queue->head=next;
    if(!next)queue->tail=NULL;
    free(el);
  }
}
/*****************************************************************************/
/* check if it is the first element of the queue */
int qIsFirst(bmcQ_t *queue,pthread_t tid)
{
  if(!queue->head)return 0;
  return pthread_equal(queue->head->tid,tid);
}
/*****************************************************************************/
void qMsgSnd(char *hostName,bmcQ_t *queue)
{
  bmcQe_t *el=NULL;
  char msg[MSG_SIZE]="";
  char *p=msg;
  /*-------------------------------------------------------------------------*/
  sprintf(p,"%s: cmd queue:",hostName);
  p=strchr(p,'\0');
  for(el=queue->head;el;el=el->next)
  {
    sprintf(p," %ld",(long)el->tid);
    p=strchr(p,'\0');
  }
  mPrintf(errU,DEBUG,__func__,0,"%s",msg);
}
/*****************************************************************************/
int qLen(bmcQ_t *queue)
{
  bmcQe_t *el=NULL;
  int i=0;
  /*-------------------------------------------------------------------------*/
  for(el=queue->head;el;el=el->next)i++;
  return i;
}
/* ######################################################################### */
/* end BMC Fifo Management Utilities                                         */
/* ######################################################################### */
/* Configuration Management                                                  */
/* ------------------------------------------------------------------------- */
/* findIpmiConf(), printIpmiConf(), printIpmiConfLong(), readIpmiConf(),     */
/* parseIpmiConf().                                                          */
/* ######################################################################### */
int findIpmiConf(ipmiConf_t cfg,char *hostname)
{
  int i;
  int found=0;
  /*-------------------------------------------------------------------------*/
  for(i=0;i<cfg.nodeN;i++)
  {
    if(!strncmp(cfg.hostNameList[i],hostname,256))
    {
      found=1;
      break;
    }
  }
  return found;
}
/*****************************************************************************/
void printIpmiConf(ipmiConf_t cfg)
{
  int i;
  char *p;
  char *s=NULL;
  int sl=0;
  /*-------------------------------------------------------------------------*/
  sl=snprintf(NULL,0,"Configured Nodes (%d): ",cfg.nodeN);
  s=(char*)realloc(s,sl+1);
  sprintf(s,"Configured Nodes (%d): ",cfg.nodeN);
  for(i=0;i<cfg.nodeN;i++)
  {
    if(!i)
    {
      sl+=snprintf(NULL,0,"%s",cfg.hostNameList[i]);
      s=(char*)realloc(s,sl+1);
      p=strchr(s,'\0');
      sprintf(p,"%s",cfg.hostNameList[i]);
    }
    else
    {
      sl+=snprintf(NULL,0,", %s",cfg.hostNameList[i]);
      s=(char*)realloc(s,sl+1);
      p=strchr(s,'\0');
      sprintf(p,", %s",cfg.hostNameList[i]);
    }
  }
  sl+=1;
  s=(char*)realloc(s,sl+1);
  p=strchr(s,'\0');
  sprintf(p,".");
  mPrintf(errU,INFO,__func__,0,s);
  if(s)free(s);
}
/*****************************************************************************/
void printIpmiConfLong(ipmiConf_t cfg)
{
  /* print configuration */
  /*-------------------------------------------------------------------------*/
  char cpBuf[4096]="";
  int i;
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,INFO,__func__,0,"Configured node number: %d.",cfg.nodeN);
  for(i=0;i<cfg.nodeN;i++)
  {
    char cpsBuf[256]="";
    cpBuf[0]='\0';
    snprintf(cpsBuf,256,"Configured node # %3d: \"%s\"",i+1,
             cfg.hostNameList[i]);
    strcat(cpBuf,cpsBuf);
    if(cfg.userNameList[i])
    {
      snprintf(cpsBuf,256,", userName=\"%s\"",cfg.userNameList[i]);
      strcat(cpBuf,cpsBuf);
    }
    if(cfg.passWordList[i])
    {
      snprintf(cpsBuf,256,", passWord=\"XXXXXXXX\"");
      strcat(cpBuf,cpsBuf);
    }
    if(cfg.portList[i])
    {
      snprintf(cpsBuf,256,", port=%ld",cfg.portList[i]);
      strcat(cpBuf,cpsBuf);
    }
    if(cfg.authTypeList[i])
    {
      snprintf(cpsBuf,256,", authType=\"%s\"",cfg.authTypeList[i]);
      strcat(cpBuf,cpsBuf);
    }
    if(cfg.privLvlList[i])
    {
      snprintf(cpsBuf,256,", privLvl=\"%s\"",cfg.privLvlList[i]);
      strcat(cpBuf,cpsBuf);
    }
    if(cfg.oemTypeList[i])
    {
      snprintf(cpsBuf,256,", oemType=\"%s\"",cfg.oemTypeList[i]);
      strcat(cpBuf,cpsBuf);
    }
    strcat(cpBuf,".");
    mPrintf(errU,INFO,__func__,0,cpBuf);
  }
}
/*****************************************************************************/
ipmiConf_t readIpmiConf(char *confFileName)
{
  /* get IPMI configuration from the file confFileName */
  /*-------------------------------------------------------------------------*/
  ipmiConf_t cfg;
  struct stat confFileStat;
  char *cfgString=NULL;
  size_t cfgStringBufLen=0;
  ssize_t cfgStringLen=0;
  int confFileFD=-1;
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,DEBUG,__func__,0,"Reading Power Manager configuration file: "
          "\"%s\"...",confFileName);
  /* check file accessibility */
  if(access(confFileName,R_OK)==-1)
  {
    mPrintf(errU,FATAL,__func__,0,"Can't access Power Manager configuration "
            "file: \"%s\". access(2) error: %s!",confFileName,strerror(errno));
    exit(1);
  }
  /* get configuration file size */
  if(lstat(confFileName,&confFileStat)<0)
  {
    mPrintf(errU,FATAL,__func__,0,"Can't get info about Power Manager "
            "configuration file: \"%s\". stat(2) error: %s!",confFileName,
            strerror(errno));
    exit(1);
  }
  if(deBug&0x80)
  {
    mPrintf(errU,VERB,__func__,0,"Size of Power Manager configuration file "
            "\"%s\": %d B.",confFileName,(int)confFileStat.st_size);
  }
  /* allocate buffer to store confFileName */
  cfgStringBufLen=confFileStat.st_size*1.1;
  if(cfgStringBufLen==0)cfgStringBufLen=1;                 /* empty confFile */
  cfgString=(char*)malloc(cfgStringBufLen*sizeof(char));
  memset(cfgString,0,cfgStringBufLen);
  /* read confFileName */
  confFileFD=open(confFileName,O_RDONLY);
  cfgStringLen=read(confFileFD,cfgString,cfgStringBufLen);
  close(confFileFD);
  if(cfgStringLen<confFileStat.st_size)
  {
    mPrintf(errU,ERROR,__func__,0,"Problem reading Power Manager "
            "configuration file \"%s\": %d B read out of %d B!",confFileName,
            cfgStringLen,confFileStat.st_size);
  }
  if(deBug&0x80)
  {
    mPrintf(errU,VERB,__func__,0,"Configuration string retieved from the "
            "Power Manager configuration file \"%s\": \"%s\".",confFileName,
            cfgString);
  }
  mPrintf(errU,DEBUG,__func__,9,"Power Manager configuration file \"%s\" "
          "read.",confFileName);
  mPrintf(errU,DEBUG,__func__,9,"Parsing Power Manager configuration file: "
          "\"%s\"...",confFileName);
  /* parse confFileName */
  cfg=parseIpmiConf(cfgString);
  if(cfgString)free(cfgString);
  mPrintf(errU,DEBUG,__func__,9,"Power Manager configuration file: \"%s\" " 
          "parsed.",confFileName);
  return cfg;
}
/*****************************************************************************/
ipmiConf_t parseIpmiConf(char *confStr)
{
  ipmiConf_t cfg={0,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  int i,j,k;
  char *s=0;
  int confLen;
  char *sp;
  size_t recordLen;
  char *record=0;
  char *recordP;
  int ll;
  char *lp;
  size_t itemSL=0;
  char *itemS=0;
  char *itemSP;
  int allowed;
  int tbp;
  char curHostName[128];
  char *startComm;
  char *eqCh;
  /*-------------------------------------------------------------------------*/
  gethostname(curHostName,128);
  s=strchr(curHostName,'.');
  if(s)*s='\0';
  /*-------------------------------------------------------------------------*/
  /* confStr stores the whole file /etc/ipmiSrv.conf */
  if(confStr==0||confStr[0]=='\0')return cfg;
  confLen=strlen(confStr);
  s=strdup(confStr);
  for(i=0;i<confLen;i++)if(s[i]=='\r')s[i]=' ';    /* replace \r with spaces */
  if(s[confLen-1]=='\n')s[confLen-1]=' ';        /* suppress last empty line */
  /*-------------------------------------------------------------------------*/
  /* split configuration string in lines */
  for(sp=s,i=0;;i++)                      /* loop over configuration records */
  {
    recordLen=strcspn(sp,"\n");
    record=(char*)realloc(record,recordLen+1);
    memset(record,'\0',recordLen+1);
    strncpy(record,sp,recordLen);           /* record stores 1 record (line) */
    recordP=record+strspn(record," \t");              /* skip heading blanks */
    /*-----------------------------------------------------------------------*/
    if(recordP[0]=='\0')                                /* skip blank records */
    {
      if(deBug&0x80)
      {
        mPrintf(errU,VERB,__func__,0,"Power Manager configuration, record %d "
                "skipped (blank record).",i+1);
      }
      goto nextRecord;
    }                                                  /* skip blank records */
    /*-----------------------------------------------------------------------*/
    else if(recordP[0]=='#')                         /* skip comment records */
    {
      if(deBug&0x80)
      {
        mPrintf(errU,VERB,__func__,0,"Power Manager configuration, comment "
                "record %d (\"%s\") skipped (comment record).",i+1,recordP);
      }
      goto nextRecord;
    }                                                /* skip comment records */
    /*-----------------------------------------------------------------------*/
    else if(deBug&0x80)
    {
      mPrintf(errU,DEBUG,__func__,0,"Power Manager configuration, record %3d: "
              "\"%s\".",i+1,recordP);
    }
    /*-----------------------------------------------------------------------*/
    if((eqCh=strchr(recordP,'=')) && (strstr(recordP,"IPMI_CMD") ||
                                      strstr(recordP,"IPMI_USER") ||
                                      strstr(recordP,"IPMI_PASSWD")))
    {                                            /* global definition record */
      char *vName=NULL;
      char *vValue=NULL;
      int l;
      char *p;
      /*.....................................................................*/
      startComm=strchr(recordP,'#');
      if(startComm)*startComm='\0';                       /* cut line at '#' */
      /*.....................................................................*/
      if(strstr(recordP,"IPMI_CMD"))vName="IPMI_CMD";
      else if(strstr(recordP,"IPMI_USER"))vName="IPMI_USER";
      else if(strstr(recordP,"IPMI_PASSWD"))vName="IPMI_PASSWD";
      p=strstr(recordP,vName)+strlen(vName);        /* fist char after vName */
      if(!*p)                                   /* no characters after vName */
      {
        mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, global "
                "definition record %d (\"%s\") skipped (malformed, no "
                "characters after \"%s\")!",i+1,recordP,vName);
        goto nextRecord;
      }                                         /* no characters after vName */
      p+=strspn(p," \t");                         /* skip blanks after vName */
      if(!*p || *p!='=')
      {
        mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, global "
                "definition record %d (\"%s\") skipped (malformed, \"%s\" is "
                "not followed by '=')!",i+1,recordP,vName);
        goto nextRecord;
      }
      /*.....................................................................*/
      vValue=eqCh+1;
      if(vValue<strstr(recordP,vName)+strlen(vName)+1)       /* name after = */
      {
        mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, global "
                "definition record %d (\"%s\") skipped (malformed, \"%s\" "
                "after '=')!",i+1,recordP,vName);
        goto nextRecord;
      }                                                      /* name after = */
      /*.....................................................................*/
      vValue+=strspn(vValue," \t");       /* skip heading blanks in variable */
      /*.....................................................................*/
      if(*vValue=='"')                             /* if value starts with " */
      {
        char *b,*e;
        b=vValue+1;
        if(!*b)                                          /* no chars after " */
        {
          mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, global "
                  "definition record %d (\"%s\") skipped (malformed, no "
                  "characters after \")!",i+1,recordP);
          goto nextRecord;
        }                                                /* no chars after " */
        e=strchr(b,'"');                       /* value terminates at next " */
        if(!e)                                       /* matching " not found */
        {
          mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, global "
                  "definition record %d (\"%s\") skipped (malformed, "
                  "matching \" not found)!",i+1,recordP);
          goto nextRecord;
        }                                            /* matching " not found */
        vValue=b;
        *e='\0';
        if(deBug&0x80)
        {
          mPrintf(errU,DEBUG,__func__,0,"NAME=\"%s\", VALUE=\"%s\".",vName,
                  vValue);
        }
        if(!getenv(vName))           /* vName not yet defined in environment */
        {
          if(setenv(vName,vValue,1))                     /* setenv() failure */
          {
            mPrintf(errU,ERROR,__func__,0,"setenv(3) error: %s",
                    strerror(errno));
            goto nextRecord;
          }                                              /* setenv() failure */
        }                            /* vName not yet defined in environment */
        goto nextRecord;
      }                                            /* if value starts with " */
      /*.....................................................................*/
      /* here value does not start with " */
      l=strcspn(vValue," \t");            /* first space after value, if any */
      if(l)*(vValue+l)='\0';       /* value terminates at next blank, if any */
      if(deBug&0x80)
      {
        mPrintf(errU,DEBUG,__func__,0,"NAME=\"%s\", VALUE=\"%s\".",vName,
                vValue);
      }
      if(!getenv(vName))             /* vName not yet defined in environment */
      {
        if(setenv(vName,vValue,1))                       /* setenv() failure */
        {
          mPrintf(errU,ERROR,__func__,0,"setenv(3) error: %s",
                  strerror(errno));
          goto nextRecord;
        }                                                /* setenv() failure */
      }                              /* vName not yet defined in environment */
      goto nextRecord;
      /*.....................................................................*/
    }                                            /* global definition record */
    /*-----------------------------------------------------------------------*/
    /* here we have standard record */
    startComm=strchr(recordP,'#');
    if(startComm)*startComm='\0';                         /* cut line at '#' */
    /*-----------------------------------------------------------------------*/
    ll=strlen(recordP);
    /*-----------------------------------------------------------------------*/
    /* split configuration line in items */
    for(lp=recordP,j=0,allowed=1;;j++)         /* loop over items in a record */
    {
      itemSL=strcspn(lp,",");
      itemS=(char*)realloc(itemS,itemSL+1);
      memset(itemS,'\0',itemSL+1);
      strncpy(itemS,lp,itemSL);
      itemSP=itemS+strspn(itemS," \t");                       /* skip blanks */
      /*.....................................................................*/
      if(j==0)                                                   /* hostname */
      {
        /* remove trailing blanks */
        if((tbp=strcspn(itemSP," \t"))<strlen(itemSP))*(itemSP+tbp)='\0';
        /* identify host names not allowed */
        for(allowed=1,k=0;k<strlen(itemSP);k++)
        {
          if(!isupper(itemSP[k]) && !islower(itemSP[k]) && 
             !isdigit(itemSP[k]) && itemSP[k]!='-' && itemSP[k]!='.')
          {
            allowed=0;
            break;
          }
        }
        if(!allowed)                                 /* hostname not allowed */
        {
          mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, record "
                  "%d (\"%s\") discarded (invalid host name \"%s\")!",i+1,
                  recordP,itemSP);
          break;
        }                                            /* hostname not allowed */
        else if(findIpmiConf(cfg,itemSP))             /* duplicated hostname */
        {                      
          mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, record "
                  "%d (\"%s\") discarded (duplicated host name \"%s\")!",i+1,
                  recordP,itemSP);
          break;
        }                                             /* duplicated hostname */
        else if(!strncmp(curHostName,itemSP,
                         MAX(strlen(curHostName),strlen(itemSP))))
        {                                                     /* node itself */
          mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, record "
                  "%d (\"%s\") discarded (host name \"%s\" cannot control "
                  "itself)!",i+1,recordP,itemSP);
          break;
        }                                                     /* node itself */
        else                                                  /* hostname OK */
        {
          cfg.nodeN++;
          cfg.hostNameList=(char**)realloc(cfg.hostNameList,
                                           cfg.nodeN*sizeof(char*));
          cfg.userNameList=(char**)realloc(cfg.userNameList,
                                           cfg.nodeN*sizeof(char*));
          cfg.passWordList=(char**)realloc(cfg.passWordList,
                                           cfg.nodeN*sizeof(char*));
          cfg.portList=(long*)realloc(cfg.portList,cfg.nodeN*sizeof(long));
          cfg.authTypeList=(char**)realloc(cfg.authTypeList,
                                           cfg.nodeN*sizeof(char*));
          cfg.privLvlList=(char**)realloc(cfg.privLvlList,
                                          cfg.nodeN*sizeof(char*));
          cfg.oemTypeList=(char**)realloc(cfg.oemTypeList,
                                          cfg.nodeN*sizeof(char*));
          cfg.hostNameList[cfg.nodeN-1]=strdup(itemSP);
          cfg.userNameList[cfg.nodeN-1]=0;
          cfg.passWordList[cfg.nodeN-1]=0;
          cfg.portList[cfg.nodeN-1]=0;
          cfg.authTypeList[cfg.nodeN-1]=0;
          cfg.privLvlList[cfg.nodeN-1]=0;
          cfg.oemTypeList[cfg.nodeN-1]=0;
        }                                                     /* hostname OK */
      }                                                          /* hostname */
      /*.....................................................................*/
      else if(j==1)                                              /* userName */
      {
        /* remove trailing blanks */
        if((tbp=strcspn(itemSP," \t"))<strlen(itemSP))*(itemSP+tbp)='\0';
        if(!strcmp(itemSP,"NULL"))cfg.userNameList[cfg.nodeN-1]=0;
        else cfg.userNameList[cfg.nodeN-1]=strdup(itemSP);
      }                                                          /* userName */
      /*.....................................................................*/
      else if(j==2)                                              /* passWord */
      {
        /* remove trailing blanks */
        if((tbp=strcspn(itemSP," \t"))<strlen(itemSP))*(itemSP+tbp)='\0';
        if(!strcmp(itemSP,"NULL"))cfg.passWordList[cfg.nodeN-1]=0;
        else cfg.passWordList[cfg.nodeN-1]=strdup(itemSP);
      }                                                          /* passWord */
      /*.....................................................................*/
      else if(j==3)                                                  /* port */
      {
        long portN=-1;
        /* remove trailing blanks */
        if((tbp=strcspn(itemSP," \t"))<strlen(itemSP))*(itemSP+tbp)='\0';
        if(!strcmp(itemSP,"NULL"))cfg.portList[cfg.nodeN-1]=-1;
        else if(!strcmp(itemSP,""))cfg.portList[cfg.nodeN-1]=-1;
        else
        {
          char *endptr=NULL;
          portN=(int)strtol(itemSP,&endptr,0);
          if((portN==0 || portN==-1 || portN==LONG_MIN || portN==LONG_MAX) && 
             (errno==EINVAL || errno==ERANGE))
          {
            mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, "
                    "record %d (\"%s\"): invalid port \"%s\": %s! Using "
                    "default port for node \"%s\"!",i+1,recordP,itemSP,
                    strerror(errno), cfg.hostNameList[cfg.nodeN-1]);
            cfg.portList[cfg.nodeN-1]=-1;
          }
          else if(!endptr||*endptr)
          {
            mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, "
                    "record %d (\"%s\"): invalid port \"%s\". Using default "
                    "port for node \"%s\"!",i+1,recordP,itemSP,
                    cfg.hostNameList[cfg.nodeN-1]);
            cfg.portList[cfg.nodeN-1]=-1;
          }
          else if(portN<0||portN>0xffff)
          {
            mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, "
                    "record %d (\"%s\"): invalid port \"%s\". Port number "
                    "must be in the range [0,65535]. Using default port for "
                    "node \"%s\"!",i+1,recordP,itemSP,
                    cfg.hostNameList[cfg.nodeN-1]);
            cfg.portList[cfg.nodeN-1]=-1;
          }
          else cfg.portList[cfg.nodeN-1]=portN;
        }
      }                                                              /* port */
      /*.....................................................................*/
      else if(j==4)                                              /* authType */
      {
        /* remove trailing blanks */
        if((tbp=strcspn(itemSP," \t"))<strlen(itemSP))*(itemSP+tbp)='\0';
        if(!strcmp(itemSP,"NULL"))cfg.authTypeList[cfg.nodeN-1]=0;
        else if(strcmp(itemSP,"NONE") && strcmp(itemSP,"PASSWORD") &&
                strcmp(itemSP,"MD2") && strcmp(itemSP,"MD5") &&
                strcmp(itemSP,"OEM"))
        {
            mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, "
                    "record %d (\"%s\"): invalid authentication type \"%s\". "
                    "Allowed authentication types are: \"NONE\", "
                    "\"PASSWORD\", \"MD2\", \"MD5\", and \"OEM\". Using "
                    "default authentication type for node \"%s\"!",i+1,
                    recordP,itemSP,cfg.hostNameList[cfg.nodeN-1]);
          cfg.authTypeList[cfg.nodeN-1]=0;
        }
        else cfg.authTypeList[cfg.nodeN-1]=strdup(itemSP);
      }                                                          /* authType */
      /*.....................................................................*/
      else if(j==5)                                               /* privLvl */
      {
        /* remove trailing blanks */
        if((tbp=strcspn(itemSP," \t"))<strlen(itemSP))*(itemSP+tbp)='\0';
        if(!strcmp(itemSP,"NULL"))cfg.privLvlList[cfg.nodeN-1]=0;
        else if(strcmp(itemSP,"CALLBACK") && strcmp(itemSP,"USER") &&
                strcmp(itemSP,"OPERATOR") && strcmp(itemSP,"ADMINISTRATOR"))
        {
            mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, "
                    "record %d (\"%s\"): invalid privilege level \"%s\". "
                    "Allowed privilege levels are: \"CALLBACK\", \"USER\", "
                    "\"OPERATOR\" and \"ADMINISTRATOR\". Using default "
                    "privilege level \"ADMINISTRATOR\" for node \"%s\"!",i+1,
                    recordP,itemSP,cfg.hostNameList[cfg.nodeN-1]);
          cfg.privLvlList[cfg.nodeN-1]=0;
        }
        else cfg.privLvlList[cfg.nodeN-1]=strdup(itemSP);
      }                                                           /* privLvl */
      /*.....................................................................*/
      else if(j==6)                                               /* oemType */
      {
        /* remove trailing blanks */
        if((tbp=strcspn(itemSP," \t"))<strlen(itemSP))*(itemSP+tbp)='\0';
        if(!strcmp(itemSP,"NULL"))cfg.oemTypeList[cfg.nodeN-1]=0;
        else if(strcmp(itemSP,"supermicro") && strcmp(itemSP,"intelwv2") &&
                strcmp(itemSP,"intelplus") && strcmp(itemSP,"icts") &&
                strcmp(itemSP,"ibm"))
        {
            mPrintf(errU,ERROR,__func__,0,"Power Manager configuration, "
                    "record %d (\"%s\"): invalid OEM type \"%s\". Allowed "
                    "OEM types are: \"supermicro\", \"intelwv2\", "
                    "\"intelplus\", \"icts\" and \"ibm\". OEM type not set "
                    "for node \"%s\"!",i+1,recordP,itemSP,
                    cfg.hostNameList[cfg.nodeN-1]);
          cfg.oemTypeList[cfg.nodeN-1]=0;
        }
        else cfg.oemTypeList[cfg.nodeN-1]=strdup(itemSP);
      }                                                           /* oemType */
      /*.....................................................................*/
      lp+=(itemSL+1);
      if(lp>recordP+ll)break;
    }                                         /* loop over items in a record */
    /*-----------------------------------------------------------------------*/
    nextRecord:
    sp+=(recordLen+1);
    if(sp>s+confLen)break;
  }                                       /* loop over configuration records */
  /*-------------------------------------------------------------------------*/
  if(s)free(s);
  if(record)free(record);
  if(itemS)free(itemS);
  return cfg;
}
/* ######################################################################### */
/* end Configuration Management                                              */
/* ######################################################################### */
void shortUsage(void)
{
  char *shortUsageStr=
"SYNOPSIS\n"
"ipmiSrv [-N | --dim_dns_node DIM_DNS_NODE] [-l | --logger LOGGER]\n"
"        [-c | --conf-file CONF_FILE]\n"
"        [-s | --sensor-dictionary-file SENS_FILE]\n"
"        [-d | --update-dictionary-file]\n"
"        [-t | --max-threads-number MAX_THREAD_NUMBER]\n"
"        [--min-period MIN_PERIOD] [--max-period MAX_PERIOD]\n"
"        [--sensor-period SENSOR_PERIOD]\n"
"        [--get-try-n GET_TRY_NUMBER] [--cmd-try-n CMD_TRY_NUMBER]\n"
"ipmiSrv {-h | --help}\n"
"Try \"ipmiSrv -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  mPrintf(L_DIM|L_STD|L_SYS,INFO,__func__,0,"\n%s",shortUsageStr);
  exit(1);
}
/*****************************************************************************/
void usage(int mode)
{
  FILE *fpOut;
  int status;
  const char *formatter;
  /*-------------------------------------------------------------------------*/
  if(!mode)formatter="/usr/bin/groff -man -Tascii | /usr/bin/less -iRs";
  else formatter="/bin/cat";
  fpOut=popen(formatter,"w");
  fprintf(fpOut,
".ig\n"
"ipmiSrv.man\n"
"\n"
"..\n"
"%s"
".hw sensor\\[hy]dictionary\\[hy]file  max\\[hy]threads\\[hy]number \n"
".hw password\\[hy]file\n"
".TH ipmiSrv 8  %s \"FMC-%s\" \"FMC Servers\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis ipmiSrv\\ \\-\n"
"FMC Power Manager Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis ipmiSrv\n"
".DoubleOpt[] N dim\\[ru]dns\\[ru]node DIM_DNS_NODE\n"
".DoubleOpt[] l logger LOGGER\n"
".DoubleOpt[] c conf\\[hy]file CONF_FILE\n"
".DoubleOpt[] s sensor\\[hy]dictionary\\[hy]file SENS_FILE\n"
".DoubleOpt[] d update\\[hy]dictionary\\[hy]file\n"
".DoubleOpt[] f password\\[hy]file PASSWD_FILE\n"
".DoubleOpt[] t max\\[hy]threads\\[hy]number MAX_THREAD_NUMBER\n"
".LongOpt[] min\\[hy]period MIN_PERIOD\n"
".LongOpt[] max\\[hy]period MAX_PERIOD\n"
".LongOpt[] sensor\\[hy]period SENSOR_PERIOD\n"
".LongOpt[] get\\[hy]try\\[hy]n GET_TRY_NUMBER\n"
".LongOpt[] cmd\\[hy]try\\[hy]n CMD_TRY_NUMBER\n"
".EndSynopsis\n"
".sp\n"
".Synopsis ipmiSrv\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH INTRODUCTION\n"
"The FMC Power Manager is a tool which allows, in an OS-independent manner, "
"to \\fBswitch-on\\fP, \\fBswitch-off\\fP, and \\fBpower-cycle\\fP the farm "
"nodes, and to \\fBmonitor\\fP their physical condition: \\fBpower status\\fP "
"(on/off), and sensor information retrieved through the I2C bus "
"(\\fBtemperatures\\fP, \\fBfan speeds\\fP, \\fBvoltages\\fP and "
"\\fBcurrents\\fP).\n"
".PP\n"
"The FMC Power Manager can operate on the farm nodes whose motherboards and "
"network interface cards implement the \\fBIPMI\\fP (Intelligent Platform "
"Management Interface) specifications, version 1.5 or subsequent, and copes "
"with several IPMI limitations.\n"
".PP\n"
"The FMC Power Manager Server, \\fBipmiSrv\\fP(8) typically runs on a few "
"control PCs (each one watching up to 200-1000 farm nodes) and uses the IPMI "
"protocol to communicate with the BMC (Baseboard Management Controller) of "
"the farm nodes, and the DIM network communication layer to communicate with "
"the Power Manager Clients.\n"
".PP\n"
"The Power Manager Clients \\fBpwSwitch\\fP(1), \\fBpwStatus\\fP(1) and "
"\\fBipmiViewer\\fP(1) can contact one or more Power Manager Servers, "
"running on remote Control PCs, to switch on/off the farm worker nodes "
"controlled by these Control PCs and to retrieve their physical condition.\n"
".PP\n"
"The FMC Power Manager Server \\fBipmiSrv\\fP(8) accesses the farm node BMCs "
"both periodically and on user demand: it inquires \\fBperiodically\\fP the "
"farm node BMCs to get their power status and sensor information (and keeps "
"the information and its time stamp to be able to answer immediately to a "
"client request); it sends commands (power on, power off, power cycle, etc.) "
"to the farm node BMCs \\fBon user request\\fP.\n"
".PP\n"
"The Power Manager Server copes with the long IPMI response time by "
"\\fBparallelly accessing each node\\fP from a different short-living thread. "
"Every IPMI access request by \\fBipmiSrv\\fP(8) starts, by default, as many "
"short-living threads as the number of the controlled nodes and each "
"short-living thread accesses one node only. In case of a huge number of "
"configured nodes (>~\\ 500), the number of short-living threads can be "
"limited: in this running condition the IPMI accesses which exceed the "
"maximum thread number are queued up.\n"
".PP\n"
"The Power Manager Server copes also with the IPMI limitation to be able to "
"process only one access at a time, by \\fBarbitrating\\fP among the \\fBIPMI "
"accesses to the same node\\fP. In case of overlapping commands or "
"and periodical inquiries, the Power Manager Server \\fBenqueues "
"commands\\fP, in order for all the received commands to be executed, one at "
"a time, exactly in the same order they were issued, and \\fBcancel "
"periodical sensor inquiries\\fP, in order to avoid indefinite thread "
"pile-up for not-responding IPMI interfaces.\n"
".PP\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The FMC Power Manager Server, \\fBipmiSrv\\fP(8) controls a cluster of PCs "
"through the IPMI protocol. It reads the list of the controlled farm nodes "
"and their properties from the Power Manager configuration file "
"\\fICONF_FILE\\fP (by default: \\fB/etc/ipmiSrv.conf\\fP). See section FILE "
"below for a description of the configuration file format.\n"
".PP\n"
"The FMC Power Manager Server parallelly accesses the BMCs of the farm nodes "
"using short-living threads. Every IPMI access request by \\fBipmiSrv\\fP(8) "
"starts, by \\fBdefault\\fP, as many short-living threads as the number of "
"the controlled nodes and each short-living thread accesses one node only.\n"
".PP\n"
"The \\fBmaximum number of threads\\fP which could run simultaneously can be "
"\\fBlimited\\fP by the command line option \\fB-t\\fP "
"\\fIMAX_THREAD_NUMBER\\fP. This option can be useful when the number of "
"configured nodes is very large (>~\\ 500) to avoid exhausting the computer "
"resources, with the drawback of a longer global access time. If a limit is "
"set then IPMI accesses are queued up and a maximum number of "
"\\fIMAX_THREAD_NUMBER\\fP can be running at a time. "
"\\fIMAX_THREAD_NUMBER\\fP must be at least 6, otherwise a FATAL "
"error is raised. If \\fIMAX_THREAD_NUMBER\\fP = 0 no limit is set to the "
"number of threads running simultaneously.\n"
".PP\n"
"The refresh of sensor information is performed periodically by "
"\\fBipmiSrv\\fP(8). The period (by default 60 s) can be changed by means of "
"the \\fB--sensor-period\\fP \\fISENSOR_PERIOD\\fP command line option.\n"
".PP\n"
"The refresh of power status information is also performed periodically by "
"\\fBipmiSrv\\fP(8), but with a period which varies from \\fIMIN_PERIOD\\fP "
"to \\fIMAX_PERIOD\\fP, being set to \\fIMIN_PERIOD\\fP soon after commands "
"(to report soon the change produced by the command) and increasing at each "
"iteration by the 20%% until \\fIMAX_PERIOD\\fP is reached. "
"\\fIMIN_PERIOD\\fP (by default 4 s) can be changed by the "
"\\fB--min-period\\fP \\fIMIN_PERIOD\\fP command line option and "
"\\fIMAX_PERIOD\\fP (by default 30 s) can be changed by the "
"\\fB--max-period\\fP \\fIMAX_PERIOD\\fP command line option.\n"
".PP\n"
"The IPMI \\fBuser name\\fP can be defined either \\fBglobally\\fP, by means "
"of the \\fIIPMI_USER\\fP global configuration record in file "
"\\fI/etc/ipmiSrv.conf\\fP, or on a \\fBper-node basis\\fP, by means of the "
"\\fIuserName\\fP field in the node configuration record in the same file "
"\\fI/etc/ipmiSrv.conf\\fP (see FILES section below). Per-node user name, if "
"defined, overrides global user name.\n"
".PP\n"
"The special user name \\fBnoUser\\fP can be defined, either as a global "
"user name or as a per-node user name, to send \\fIno user name\\fP to the "
"IPMI hardware.\n"
".PP\n"
"The IPMI \\fBpass-word\\fP can be defined in three different ways (in "
"decreasing order of priority):\n"
".RS\n"
".TP 3\n1.\n"
"\\fBglobally\\fP, by writing the pass-word in the ASCII file "
"\\fIPASSWD_FILE\\fP and starting \\fBipmiSrv\\fP(8) with the command-line "
"option \\fB--password-file\\fP \\fIPASSWD_FILE\\fP or \\fB-f\\fP "
"\\fIPASSWD_FILE\\fP. This option passes to the \\fBipmitool\\fP(1) command "
"the name of the file instead of the password, and is therefore preferable "
"for the security.\n"
".TP 3\n2.\n"
"on a \\fBper-node basis\\fP, by means of the \\fBpassWord\\fP field in the "
"node configuration record in the file \\fI/etc/ipmiSrv.conf\\fP (see FILES "
"section below).\n"
".TP 3\n3.\n"
"\\fBglobally\\fP, by means of the \\fIIPMI_PASSWD\\fP global configuration "
"record in file \\fI/etc/ipmiSrv.conf\\fP (see FILES section below);\n"
".RE\n"
".PP\n"
"The DIM Name Server, looked up by \\fBipmiSrv\\fP to register the provided\n"
"DIM services and commands, can be chosen (in decreasing order of priority) "
"by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\\fB%s\\fP.\n"
".RE\n"
".\n"
".\n"
".SH OPTIONS\n"
".\n"
".OptDef h help\n"
"Get basic usage help from the command line then exit.\n"
".\n"
".OptDef N dim_dns_node DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef l logger LOGGER (integer)\n"
"Use the logger units defined in the \\fILOGGER\\fP mask to send diagnostic\n"
"and information messages. The \\fILOGGER\\fP mask can be the bitwise\n"
"\\fIOR\\fP of the following logger units:\n"
".RS\n"
".TP\n"
"0x1\nthe default FMC Message Logger (fifo: /tmp/logSrv.fifo);\n"
".TP\n"
"0x2\nthe standard error stream;\n"
".TP\n"
"0x4\nthe operating system syslog facility.\n"
".PP\n"
"\\fBDefault\\fP: \\fILOGGER\\fP = 0x1.\n"
".RE\n"
".\n"
".OptDef c conf-file CONF_FILE (string)\n"
"Read the IPMI configuration from the file \\fICONF_FILE\\fP (see FILE "
"section below for the format of \\fICONF_FILE\\fP). \\fBDefault\\fP: "
"\\fICONF_FILE\\fP = \\fB/etc/ipmiSrv.conf\\fP.\n"
".\n"
".OptDef c sensor-dictionary-file SENS_FILE (string)\n"
"Read/write sensor dictionary from/to the file \\fISENS_FILE\\fP. "
"\\fBDefault\\fP: \\fISENS_FILE\\fP = "
"\\fB/etc/ipmi_sensor_dictionary.txt\\fP.\n"
".\n"
".OptDef d update-dictionary-file\n"
"Discover the sensor list of the nodes in the cluster, at \\fBipmiSrv\\fP(8) "
"start-up, using IPMI, and update the sensor dictionary file "
"\\fISENS_FILE\\fP (this can take up to 400 s). This option allows to enable "
"the access to the sensors of new brands or models of PC added to the "
"cluster. \\fBDefault\\fP: do not discover the sensor list through IPMI, but "
"read the sensor list from the previously stored sensor dictionary file "
"\\fISENS_FILE\\fP.\n"
".\n"
".OptDef f password-file PASSWD_FILE (string)\n"
"Read the IPMI pass-word from the the file \\fIPASSWD_FILE\\fP at run-time. "
"\\fBDefault\\fP: read the IPMI pass-word from the file \\fICONF_FILE\\fP at "
"start-up.\n"
".\n"
".OptDef t max-threads-number MAX_THREAD_NUMBER (integer)\n"
"Set the maximum number of threads which could run simultaneously to "
"\\fIMAX_THREAD_NUMBER\\fP. This option can be useful when the number of "
"configured nodes is very large (> 500) to avoid exhausting the computer "
"resources, with the drawback of a longer global access time. Consider that, "
"if no limit is set, IPMI access needs one thread "
"for each controlled node. If a limit is set then IPMI accesses are queued "
"and a maximum number of \\fIMAX_THREAD_NUMBER\\fP can be running at a time. "
"\\fIMAX_THREAD_NUMBER\\fP must be at least 6, otherwise a FATAL "
"error is raised. If \\fIMAX_THREAD_NUMBER\\fP = 0 no limit is set to the "
"number of threads running simultaneously. \\fBDefault\\fP: "
"\\fIMAX_THREAD_NUMBER\\fP = 0.\n"
".\n"
".OptDef \"\" min-period MIN_PERIOD (float)\n"
"Set the minimum power status refresh period, expressed in seconds to "
"\\fIMIN_PERIOD\\fP. The power status refresh period is set to "
"\\fIMIN_PERIOD\\fP soon after a command is issued, to report soon the change "
"produced by the command, then in increased by the 20%% each time until "
"\\fIMAX_PERIOD\\fP is reached. \\fBDefault\\fP: 4 s. "
"\\fBMinimum\\fP: 4 s.\n"
".\n"
".OptDef \"\" max-period MAX_PERIOD (float)\n"
"Set the maximum power status refresh period, expressed in seconds to "
"\\fIMAX_PERIOD\\fP. \\fBDefault\\fP: 30 s. \\fBMinimum\\fP: 4 s.\n"
".\n"
".OptDef \"\" sensor-period SENSOR_PERIOD (float)\n"
"Set the sensor refresh period, expressed in seconds to "
"\\fISENSOR_PERIOD\\fP. \\fBDefault\\fP: 60 s. \\fBMinimum\\fP: 4 s.\n"
".\n"
".OptDef \"\" get-try-n GET_TRY_NUMBER (integer)\n"
"Set the maximum number of attempts in getting IPMI information before "
"giving-up to \\fIGET_TRY_NUMBER\\fP. \\fBDefault\\fP: 5. \\fBMinimum\\fP: "
"1.\n"
".\n"
".OptDef \"\" get-try-n GET_TRY_NUMBER (integer)\n"
"Set the maximum number of IPMI command attempts before giving-up to "
"\\fICMD_TRY_NUMBER\\fP. \\fBDefault\\fP: 10. \\fBMinimum\\fP: 1.\n"
".\n"
".\n"
".SH ENVIRONMENT\n"
".TP\n"
".EnvVar DIM_DNS_NODE \\ (string,\\ mandatory\\ if\\ not\\ defined\\ "
"otherwise,\\ see\\ above)\n"
"Host name of the node which is running the DIM DNS.\n"
".\n"
".TP\n"
".EnvVar LD_LIBRARY_PATH \\ (string,\\ mandatory\\ if\\ not\\ set\\ using\\ "
"ldconfig)\n"
"Must include the path to the libraries \"libdim\" and \"libFMCutils\".\n"
".\n"
".TP\n"
".EnvVar IPMI_USER \\ (string,\\ mandatory\\ if\\ not\\ set\\ in\\ "
"\\fICONF_FILE\\fP)\n"
"Default IPMI username. Can be overridden by the per-host specifications "
"provided by \\fICONF_FILE\\fP.\n"
".\n"
".TP\n"
".EnvVar IPMI_PASSWD \\ (string,\\ mandatory\\ if\\ not\\ set\\ in\\ "
"\\fICONF_FILE\\fP)\n"
"Default IPMI password. Can be overridden by the per-host specifications "
"provided by \\fICONF_FILE\\fP.\n"
".\n"
".TP\n"
".EnvVar IPMI_CMD \\ (string,\\ optional)\n"
"Path to the ipmitool executable image. \\fBDefault\\fP: \\fIIPMI_CMD\\fP = "
"\\fB/usr/bin/ipmitool\\fP.\n"
".\n"
".TP\n"
".EnvVar UTGID \\ (string,\\ optional)\n"
"UTGID of the ipmiSrv process.\n"
".\n"
".TP\n"
".EnvVar deBug \\ (integer,\\ decimal\\ or\\ hexadecimal,\\ optional)\n"
"Debug mask. Can be set to 0x00...0x7ff. The meaning of the mask bit are:\n"
".PP\n"
".RS\n"
"0x001: print update period;\n"
".br\n"
"0x002: print retrieved data;\n"
".br\n"
"0x004: print command queue;\n"
".br\n"
"0x008: print output of ipmitool commands;\n"
".br\n"
"0x010: print start/end functions;\n"
".br\n"
"0x020: print published services;\n"
".br\n"
"0x040: print sent IPMI commands;\n"
".br\n"
"0x080: print IPMI per-node configuration;\n"
".br\n"
"0x100: print thread management;\n"
".br\n"
"0x200: print Single IPMI access failures;\n"
".br\n"
"0x400: print Sensor errors.\n"
".PP\n"
"\\fBDefault\\fP: \\fIdeBug\\fP = 0.\n"
".RE\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Start the FMC Power Manager, getting the configuration from the file "
"\"/etc/ipmiSrv.conf\", getting the DIM_DNS_NODE from the file "
"\"/etc/sysconfig/dim\", sending messages to the default FMC Message Logger, "
"with no limit to the number of threads, a power status refresh period which "
"varies from 4 s to 30 s, a sensor refresh period of 60 s, a maximum number "
"of 10 IPMI access attempt for commands and 5 IPMI access attempt for "
"monitor:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/ipmiSrv\n"
".PP\n"
"Update the sensor dictionary at start-up (this can take up to 400 s) to "
"enable the access to the sensors of new brands or models of PC added to the "
"cluster:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/ipmiSrv -d\n"
".ShellCommand /opt/FMC/sbin/ipmiSrv --update-dictionary-file\n"
".PP\n"
"Set the maximum number of \\fBipmiSrv\\fP(8) threads which could run "
"simultaneously to 800, in order to save computer resources if the number "
"of configured nodes is greater then 800:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/ipmiSrv -t 800\n"
".ShellCommand /opt/FMC/sbin/ipmiSrv --max-threads-number 800\n"
".PP\n"
"Read the global IPMI pass-word at run-time from the file \\fI/etc/ipmiSrv.passwd\\fP:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/ipmiSrv -f /etc/ipmiSrv.passwd\n"
".ShellCommand /opt/FMC/sbin/ipmiSrv --password-file /etc/ipmiSrv.passwd\n"
".PP\n"
".\n"
".\n"
".SH FILES\n"
".\n"
".I /etc/ipmiSrv.conf\n"
".\n"
".\n"
".SS Configuration File\n"
".I /etc/ipmiSrv.conf\n"
".PP\n"
".B Description\n"
".PP\n"
"The file \\fI/etc/ipmiSrv.conf\\fP stores the IPMI configuration of the "
"cluster nodes which have to be controlled by the current instance of the FMC "
"Power Manager. The record of the file can be either a \\fBglobal "
"configuration record\\fP - which set the default user, the default password "
"and the path to the ipmitool executable image - or a \\fBnode configuration "
"record\\fP which add the node to the list of the controlled nodes and "
"optionally set, for the specific node, a user and a password which override "
"the default user and passwors, and further details of the specific node "
"configuration (UDP port number, authentication type, etc.).\n"
".PP\n"
".B File format\n"
".PP\n"
"File is made of an arbitrary number of records, which can be either a "
"\\fBglobal configuration record\\fP or a \\fBnode configuration record\\fP. "
"Records are separated by the character '\\[rs]n' (new-line, line-feed, LF, "
"ASCII 0x0a). Blank records as well as comment records (i.e. records starting "
"with the '#' character) are skipped. If a record contains the '#' character, "
"all charaters following the '#' character (including the '#' itself) are "
"ignored.\n"
".PP\n"
".B Global configuration record format\n"
".PP\n"
"Can have one of the following three formats:\n"
".br\n"
".RS\n"
".TP\n"
".B IPMI_USER=string\n"
"Set the default username (mandatory).\n"
".br\n"
".TP\n"
".B IPMI_PASSWD=string\n"
"Set the default password (mandatory).\n"
".br\n"
".TP\n"
".B IPMI_CMD=string\n"
"Set the path to the \\fBipmitool\\fP executable image (optional).\n"
".RE\n"
".PP\n"
"where the strings need to be quoted (by double quotation marks) only if they "
"include spaces.\n"
".PP\n"
".B Node configuration record format\n"
".PP\n"
"The record is made of 7 fields, separated by commas, in the sequence:\n"
".IP\n"
".B hostName,userName,passWord,port,authType,privLvl,oemType\n"
".PP\n"
"The first field (hostName) is mandatory; the following fields can be "
"omitted. The last fields can be omitted simply avoid writing them. "
"Intermediate fields can be omitted by writing the string \"\\fBNULL\\fP\" "
"(see example below).\n"
".PP\n"
".B Node configuration field description\n"
".br\n"
".RS\n"
".TP\n"
"\\fBhostName\\fP (string, mandatory)\n"
"hostname of the controlled node.\n"
".TP\n"
"\\fBuserName\\fP (string, optional)\n"
"username used on the controlled node (overrides the default user set in a "
"global configuration record).\n"
".TP\n"
"\\fBpassWord\\fP (string, optional)\n"
"passWord used on the controlled node (overrides the default password set in "
"a global configuration record).\n"
".TP\n"
"\\fBport\\fP (integer, optional)\n"
"UDP port contacted on the controlled node.\n"
".TP\n"
"\\fBauthType\\fP (string, optional)\n"
"Authentication type to use during the IPMIv1.5 lan session activation on the "
"controlled node. Supported types are: \\fINONE\\fP, \\fIPASSWORD\\fP, "
"\\fIMD2\\fP, \\fIMD5\\fP, or \\fIOEM\\fP.\n"
".TP\n"
"\\fBprivLvl\\fP (string, optional)\n"
"Force session privilege level on the controlled node. Allowed level: "
"\\fICALLBACK\\fP, \\fIUSER\\fP, \\fIOPERATOR\\fP, \\fIADMINISTRATOR\\fP. "
"\\fBDefault level\\fP: \\fIADMINISTRATOR\\fP.\n"
".TP\n"
"\\fBoemType\\fP (string, optional)\n"
"Select OEM type to support on the controlled node. This usually involves "
"minor hacks in place in the code to work around quirks in various BMCs from "
"various manufacturers. Supported types are: \\fIsupermicro\\fP, "
"\\fIintelwv2\\fP, \\fIintelplus\\fP, \\fIicts\\fP, \\fIibm\\fP.\n"
".RE\n"
".PP\n"
".B Record Examples:\n"
".PP\n"
"# Sample configuration file\n"
".br\n"
"# Global configuration records\n"
".br\n"
"IPMI_USER=root         # default user\n"
".br\n"
"IPMI_PASSWD=my_passwd  # default password\n"
".br\n"
"# Node configuration records\n"
".br\n"
"# hostName,userName,passWord,port,authType,privLvl,oemType\n"
".br\n"
"farm0101\n"
".br\n"
"farm0102,myuser\n"
".br\n"
"farm0102,noUser\n"
".br\n"
"farm0103,noUser,mypassword\n"
".br\n"
"farm0104,myuser,mypassword,1623\n"
".br\n"
"farm0105,myuser,mypassword,1623,MD2,OPERATOR,intelwv2\n"
".br\n"
"farm0106,NULL,NULL,NULL,MD2,OPERATOR,intelwv2\n"
".PP\n"
".\n"
".\n"
".SH DIM INFORMATION\n"
".PP\n"
".SS DIM Server Name\n"
".\n"
".TP\n"
"%s/\\fICTRLPC_HOST_NAME\\fP/%s\n"
"where \\fICTRLPC_HOST_NAME\\fP is the host name of the current PC (running "
"\\fBipmiSrv\\fP(8)), as returned by the command \"hostname -s\", but written "
"in upper case characters.\n"
".\n"
".\n"
".SS Published DIM Services\n"
".\n"
".TP\n"
"%s/\\fICTRLPC_HOST_NAME\\fP/%s/server_version (string)\n"
"Publishes a NULL-terminated string containing the RCS revision label of "
"the \\fBipmiSrv.c\\fP source code of the current \\fBipmiSrv\\fP(8) "
"instance, as returned by the command \"ident /opt/FMC/sbin/ipmiSrv\"; in "
"the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
"%s/\\fICTRLPC_HOST_NAME\\fP/%s/actuator_version (string)\n"
"Publishes a NULL-terminated string containing the RCS revision label of "
"the \\fBipmiUtils.c\\fP source code of the current \\fBipmiSrv\\fP(8) "
"instance, as returned by the command \"ident /opt/FMC/sbin/ipmiSrv\"; in "
"the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
"%s/\\fICTRLPC_HOST_NAME\\fP/%s/fmc_version (string)\n"
"Publishes a NULL-terminated string containing the revision label of the FMC "
"package which includes the current \\fBipmiSrv\\fP(8) executable, in the "
"present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
"%s/\\fICTRLPC_HOST_NAME\\fP/%s/success (int)\n"
"Always publishes the integer 1. This dummy services can be used to check "
"whether the current instance of \\fBipmiSrv\\fP(8) is up and running.\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/power_status (integer)\n"
"Publishes an array of integers which are the last power status probe of the "
"nodes (0 = off, 1 = on). \\fIHOST_NAME\\fP is the host name of the "
"controlled node, not the host name of the PC running \\fBipmiSrv\\fP(8).\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/power_status_timestamp (integer)\n"
"Publishes an array of integers which are the time of the last power status "
"probe of the nodes, measured in seconds since the Epoch (00:00:00 UTC, "
"January 1, 1970). \\fIHOST_NAME\\fP is the host name of the "
"controlled node, not the host name of the PC running \\fBipmiSrv\\fP(8).\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/sensors_timestamp (integer)\n"
"Publishes an array of integers which are the time of the last sensor "
"measure of the nodes, measured in seconds since the Epoch (00:00:00 UTC, "
"January 1, 1970). \\fIHOST_NAME\\fP is the host name of the "
"controlled node, not the host name of the PC running \\fBipmiSrv\\fP(8).\n"
".\n"
".PP\n"
"%s/\\fIHOST_NAME\\fP/sensors/temp/names (string)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/fan/names (string)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/voltage/names (string)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/current/names (string)\n"
".RS\n"
"Publishes a sequence of NULL-terminated strings which are labels to tag the "
"sensors (e.g.: \"Planar temp\", \"VRD 0 Temp\", \"ambienttemp\", "
"\"cpu0.dietemp\", \"cpu0.memtemp\", etc.). \\fIHOST_NAME\\fP is the host "
"name of the controlled node, not the host name of the PC running "
"\\fBipmiSrv\\fP(8).\n"
".RE\n"
".PP\n"
"%s/\\fIHOST_NAME\\fP/sensors/temp/units (string)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/fan/units (string)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/voltage/units (string)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/current/units (string)\n"
".RS\n"
"Publishes a sequence of NULL-terminated strings which are the units of the "
"sensors measures (e.g.: \"degrees C\", \"degrees F\", \"RPM\", \"Volts\", "
"etc.). \\fIHOST_NAME\\fP is the host name of the controlled node, not the "
"host name of the PC running \\fBipmiSrv\\fP(8).\n"
".RE\n"
".PP\n"
"%s/\\fIHOST_NAME\\fP/sensors/temp/input (float)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/fan/input (integer)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/voltage/input (float)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/current/input (float)\n"
".RS\n"
"Publishes an array of numbers (integer or float) which are the last "
"values of the sensors measures. \\fIHOST_NAME\\fP is the host name of the "
"controlled node, not the host name of the PC running \\fBipmiSrv\\fP(8).\n"
".RE\n"
".PP\n"
"%s/\\fIHOST_NAME\\fP/sensors/temp/status (string)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/fan/status (string)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/voltage/status (string)\n"
".br\n"
"%s/\\fIHOST_NAME\\fP/sensors/current/status (string)\n"
".RS\n"
"Publishes a sequence of NULL-terminated strings which are the sensor status "
"evaluated by the IPMI hardware. The meanings of the strings are:\n"
".PP\n"
".RS\n"
"\\fBcr\\fP: critical\n"
".br\n"
"\\fBnc\\fP: non-critical\n"
".br\n"
"\\fBnr\\fP: non-recoverable\n"
".br\n"
"\\fBns\\fP: not specified (e.g. node switched off)\n"
".br\n"
"\\fBok\\fP: ok\n"
".br\n"
"\\fBus\\fP: unspecified\n"
".RE\n"
".PP\n"
"\\fIHOST_NAME\\fP is the host name of the controlled node, not the host name "
"of the PC running \\fBipmiSrv\\fP(8).\n"
".RE\n"
".\n"
".\n"
".SS Published DIM Commands:\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/power_switch\n"
"Accepts, as argument, a NULL-terminated string which can be one of the "
"following:\n"
".RS\n"
".TP\n"
"\\fBon\\fP:\nPower up the node\n"
".TP\n"
"\\fBoff\\fP:\nPower down the node into soft off (S4/S5 state). This command "
"does not initiate a clean shutdown of the operating system prior to "
"powering down the system.\n"
".TP\n"
"\\fBcycle\\fP:\nPower down and then power up the node, providing a power off "
"interval of at least 1 second. No action should occur if the node power is "
"in S4/S5 state.\n"
".TP\n"
"\\fBsoft_off\\fP:\nInitiate a soft-shutdown of the OS via ACPI. This can be "
"done in a number of ways, commonly by simulating an overtemperture or by "
"simulating a power button press. It is necessary to have OS support for ACPI "
"and some sort of daemon watching for soft power events.\n"
".TP\n"
"\\fBhard_reset\\fP:\nPulse the system reset signal.\n"
".TP\n"
"\\fBpulse_diag\\fP:\nPulse a diagnostic interrupt (NMI) directly to the "
"processor(s). This is typically used to cause the operating system to do a "
"diagnostic dump (OS dependent).\n"
".PP\n"
"\\fIHOST_NAME\\fP is the host name of the controlled node, not the host name "
"of the PC running \\fBipmiSrv\\fP(8).\n"
".RE\n"

".\n"
".\n"
".SH AVAILABILITY\n"
".\n"
"Information on how to get the FMC (Farm Monitoring and Control System)\n"
"package and related information is available at the web sites:\n"
".PP\n"
".ad l\n"
".URL https://\\:lhcbweb.bo.infn.it/\\:twiki\\:/\\:bin/\\:view.cgi/\\:"
"LHCbBologna/\\:FmcLinux \"FMC Linux Development Homepage\".\n"
".PP\n"
".URL http://\\:itcobe\\:.\\:web\\:.\\:cern\\:.\\:ch\\:/\\:itcobe/\\:"
"Projects/\\:Framework/\\:"
"Download/\\:Components/\\:SystemOverview\\:/\\:fwFMC/\\:welcome.html "
"\"CERN ITCOBE: FW Farm Monitor and Control\".\n"
".PP\n"
".URL http://\\:lhcb-daq.web.cern.ch/\\:lhcb-daq/\\:online-rpm-repo\\:/\\:"
"index.html \"LHCb Online RPMs\".\n"
".ad b\n"
".PP\n"
".\n"
".\n"
".SH AUTHORS\n"
".\n"
".MTO domenico.galli@bo.infn.it \"Domenico Galli\"\n"
".\n"
".\n"
".SH VERSION\n"
".\n"
"Source:  %s\n"
".br\n"
"Source:  %s\n"
".br\n"
"Package: FMC-%s\n"
".\n"
".\n"
".SH SEE ALSO:\n"
".BR \\%%pwStatus (1),\n"
".BR \\%%ipmiViewer (1),\n"
".BR \\%%pwSwitch (1).\n"
".br\n"
".BR \\%%ipmitool (1).\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro
,FMC_DATE,FMC_VERSION
,DIM_CONF_FILE_NAME
,SVC_HEAD,SRV_NAME
,SVC_HEAD,SRV_NAME
,rcsidP
,SVC_HEAD,SRV_NAME
,aRcsidP
,SVC_HEAD,SRV_NAME
,FMC_VERSION
,SVC_HEAD,SRV_NAME
,SVC_HEAD,SVC_HEAD,SVC_HEAD
,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD
,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD
,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD
,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD
,SVC_HEAD
,rcsidP,aRcsidP,FMC_VERSION
       );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/* ######################################################################### */
