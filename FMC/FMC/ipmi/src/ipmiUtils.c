/*****************************************************************************/
/*
 * $Log: ipmiUtils.c,v $
 * Revision 2.4  2008/12/02 15:38:55  galli
 * ipmiSrv can read passwd from file at run-time
 *
 * Revision 2.2  2007/10/12 15:18:51  galli
 * get number of tries from global variables
 *
 * Revision 2.1  2007/10/03 12:07:41  galli
 * bug fixed
 *
 * Revision 2.0  2007/10/03 10:07:59  galli
 * 5 retries for power switch
 *
 * Revision 1.32  2007/08/09 21:42:03  galli
 * compatible with libFMCutils v 2
 *
 * Revision 1.31  2007/08/03 11:51:49  galli
 * uses authType, privLvl and oemType
 *
 * Revision 1.29  2007/06/18 15:38:28  galli
 * bug fixed in print error messages from IPMI
 *
 * Revision 1.28  2007/06/07 09:12:56  galli
 * if IPMI_USER=="noUser" do not send "-U user" ipmitool option
 *
 * Revision 1.27  2006/10/20 15:44:05  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 1.26  2006/03/07 12:53:48  galli
 * improved error check in sensorsList()
 *
 * Revision 1.25  2006/02/24 15:30:03  galli
 * sensorsList() function added
 *
 * Revision 1.24  2006/02/09 12:10:25  galli
 * changed deBug level meaning
 *
 * Revision 1.22  2006/01/14 11:01:45  galli
 * ipmitool error codes introduced
 *
 * Revision 1.20  2006/01/11 09:47:34  galli
 * unmask SIGALRM starting ipmitool
 *
 * Revision 1.19  2006/01/10 10:46:41  galli
 * run ipmitool in a different process, communicating with a pipe
 *
 * Revision 1.16  2005/05/04 20:14:53  galli
 * cleanup
 *
 * Revision 1.15  2005/05/04 20:13:12  galli
 * function powerSwitchActuate() added
 * functions powerOn() and powerOff() rewritten (call powerSwitchActuate())
 *
 * Revision 1.14  2005/05/04 12:14:12  galli
 * stack allocation of intf variable
 *
 * Revision 1.12  2005/04/12 12:28:38  galli
 * cleanup
 *
 * Revision 1.11  2005/04/11 22:35:48  galli
 * working version
 * functions: getPowerStatus(), powerOff(), powerOn()
 *
 * Revision 1.2  2005/03/30 22:23:24  galli
 * first working version
 *
 * Revision 1.1  2005/03/30 21:47:34  galli
 * Initial revision
 *
 * */
/*****************************************************************************/
/* N.B.: In case of lan error, calls can take up to 8x2 seconds              */
/*****************************************************************************/
/* headers */
#include <stdio.h>                                             /* snprintf() */
#include <stdlib.h>                                             /* snprintf() */
#include <string.h>                                              /* memset() */
#include <pthread.h>                           /* pthread_mutex_lock(), etc. */
#include <errno.h>
#include <sys/wait.h>                                       /* WEXITSTATUS() */
#include <fnmatch.h>                                            /* fnmatch() */
#include <ipmi_chassis.h>                 /* IPMI_CHASSIS_CTL_POWER_UP, etc. */
/*****************************************************************************/
#include "ipmiUtils.h"                                   /* getPowerStatus() */
#include "fmcMsgUtils.h"                                   /* mPrintf(), ... */
#define OUT_ERR_SIZE 8192
#define SENSORS_BUFFER_SIZE 8192
/*****************************************************************************/
/* external global */
extern int deBug;                                             /* debug level */
extern char *ipmiCmd;                                    /* ipmitool command */
extern int errU;                                               /* error unit */
extern int dfltIpmiPort;
/* maximum number of command attempts before to give-up */
extern int maxCmdAttemptN;
/* maximum number of attempts in getting state before to give-up */
extern int maxGetAttemptN;
/*****************************************************************************/
/* globals */
static char rcsid[]="$Id: ipmiUtils.c,v 2.4 2008/12/02 15:38:55 galli Exp $";
/*****************************************************************************/
/* Returns the power status got with the "ipmitool -c chassis power status"  */
/* command                                                                   */
/*****************************************************************************/
int getPowerStatus(char *hostName,char *userName,char *passWord,
                   char *passWordFile,int port,char *authType,char *privLvl,
                   char *oemType)
{
  FILE *ipmiOutErrFP=NULL;
  int exitStatus=0;
  char ipmiCmdLine[1024]="";
  char ipmiOutErrStr[OUT_ERR_SIZE]="";
  int powerStatus=IPMI_NOT_YET_AVAIL;
  int attemptsN=0;
  char *p=NULL;
  char userNameStr[64]="";
  char portStr[16]="";
  char authTypeStr[16]="";
  char privLvlStr[24]="";
  char oemTypeStr[32]="";
  struct timespec delay;
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s.",hostName);
  }
  /*-------------------------------------------------------------------------*/
  /* optional userName option string */
  if(strcmp(userName,"noUser"))
    snprintf(userNameStr,64,"-U %s ",userName);
  else
    userNameStr[0]='\0';
  /* optional port option string */
  if(port!=dfltIpmiPort)
    snprintf(portStr,16,"-p %d ",port);
  else
    portStr[0]='\0';
  /* optional authType option string */
  if(authType[0]!='\0')
    snprintf(authTypeStr,16,"-A %s ",authType);
  else
    authTypeStr[0]='\0';
  /* optional privLvl option string */
  if(privLvl[0]!='\0')
    snprintf(privLvlStr,24,"-L %s ",privLvl);
  else
    privLvlStr[0]='\0';
  /* optional oemType option string */
  if(oemType[0]!='\0')
    snprintf(oemTypeStr,32,"-o %s ",oemType);
  else
    oemTypeStr[0]='\0';
  /* compose ipmitool command line */
  if(!*passWordFile)                     /* passWord as command-line argument */
  {
    sprintf(ipmiCmdLine,"%s -I lan -H %s %s%s%s%s-P %s %schassis power "
            "status 2>&1",ipmiCmd,hostName,portStr,userNameStr,authTypeStr,
            privLvlStr,passWord,oemTypeStr);
  }
  else                              /* passWord written in file passWordFile */
  {
    sprintf(ipmiCmdLine,"%s -I lan -H %s %s%s%s%s-f %s %schassis power "
            "status 2>&1",ipmiCmd,hostName,portStr,userNameStr,authTypeStr,
            privLvlStr,passWordFile,oemTypeStr);
  }
  if(deBug&0x40)
  {
    mPrintf(errU,VERB,__func__,0,"Sending IPMI command: \"%s\".",ipmiCmdLine);
  }
  /*-------------------------------------------------------------------------*/
  for(attemptsN=0;attemptsN<maxGetAttemptN;attemptsN++)
  {
    /* execute ipmitool command */
    ipmiOutErrFP=popen(ipmiCmdLine, "r");
    fread(ipmiOutErrStr,sizeof(char),OUT_ERR_SIZE,ipmiOutErrFP);
    /* substitutes newlines with dots */
    for(p=ipmiOutErrStr;*p;p++)if(*p=='\n'||*p=='\r')*p='.';
    if(deBug&0x8)
    {
      mPrintf(errU,DEBUG,__func__,0,"ipmitool \"chassis power status\" "
              "command on host \"%s\" returned string: \"%s\"", hostName,
              ipmiOutErrStr);
    }
    exitStatus=WEXITSTATUS(pclose(ipmiOutErrFP));
    if(!exitStatus)break;
    if(deBug&0x200)
    {
      mPrintf(errU,VERB,__func__,0,"ipmitool \"chassis power status\" command "
              "on host \"%s\" un-succeeded. Attempt %d/%d. String returned by "
              "ipmitool: \"%s\"!",hostName,attemptsN+1,maxGetAttemptN,
              ipmiOutErrStr);
    }
    if(attemptsN<maxGetAttemptN-1)
    {
      delay.tv_sec=attemptsN;
      delay.tv_nsec=0;
      nanosleep(&delay,NULL);
    }
  }
  /*-------------------------------------------------------------------------*/
  /* set the return value */
  if(attemptsN==maxGetAttemptN)powerStatus=IPMI_NOT_RESP;        /* no reply */
  else if(strstr(ipmiOutErrStr,"Chassis Power is on"))powerStatus=1;
  else if(strstr(ipmiOutErrStr,"Chassis Power is off"))powerStatus=0;
  else powerStatus=IPMI_INVALID_RESP;                       /* other replies */
  /*-------------------------------------------------------------------------*/
  if(powerStatus==IPMI_NOT_RESP)
  {
    mPrintf(errU,ERROR,__func__,0,"ipmitool \"chassis power status\" command "
            "on host \"%s\" failed (%d/%d attempts failed). Status is set to "
            "\"%s\". Last string returned by ipmitool: \"%s\"!",hostName,
            attemptsN,maxGetAttemptN,
            powerStatus==0?"off":powerStatus==1?"on":IPMI_NOT_RESP_SL,
            ipmiOutErrStr);
  }
  else if(powerStatus!=IPMI_INVALID_RESP && attemptsN>0 && 
          attemptsN<maxGetAttemptN)
  {
    mPrintf(errU,WARN,__func__,0,"ipmitool \"chassis power status\" command "
            "on host \"%s\" succeeded after %d/%d attempts. Status is \"%s\"!",
            hostName,attemptsN+1,maxGetAttemptN,
            powerStatus==0?"off":powerStatus==1?"on":IPMI_NOT_RESP_SL);
  }
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done. Hostname = %s.",hostName);
  }
  /*-------------------------------------------------------------------------*/
  return powerStatus;
}
/*****************************************************************************/
/* Issue "ipmitool chassis power on/off/cycle/etc." command                  */
/*****************************************************************************/
int powerSwitchActuate(char *hostName,char *userName,char *passWord,
                       char *passWordFile,int port,char *authType,
                       char *privLvl,char *oemType,unsigned char ctl)
{
  FILE *ipmiOutErrFP=NULL;
  int rv=IPMI_NOT_YET_AVAIL;
  int exitStatus=0;
  char ipmiCmdStr[16]="";
  char ipmiCmdLine[1024]="";
  char ipmiOutErrStr[OUT_ERR_SIZE]="";
  int attemptsN=0;
  char *p=NULL;
  sigset_t signalMask,savedSignalMask;
  char userNameStr[64]="";
  char portStr[16]="";
  char authTypeStr[16]="";
  char privLvlStr[24]="";
  char oemTypeStr[32]="";
  struct timespec delay;
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s, ctl = %d.",hostName,
            ctl);
  }
  /*-------------------------------------------------------------------------*/
  /* compose ipmitool command line */
  switch(ctl)
  {
    case IPMI_CHASSIS_CTL_POWER_DOWN:
      strcpy(ipmiCmdStr,"off");
      break;
    case IPMI_CHASSIS_CTL_POWER_UP:
      strcpy(ipmiCmdStr,"on");
      break;
    case IPMI_CHASSIS_CTL_POWER_CYCLE:
      strcpy(ipmiCmdStr,"cycle");
      break;
    case IPMI_CHASSIS_CTL_HARD_RESET:
      strcpy(ipmiCmdStr,"reset");
      break;
    case IPMI_CHASSIS_CTL_PULSE_DIAG:
      strcpy(ipmiCmdStr,"diag");
      break;
    case IPMI_CHASSIS_CTL_ACPI_SOFT:
      strcpy(ipmiCmdStr,"soft");
      break;
  }
  /* optional userName option string */
  if(strcmp(userName,"noUser"))
    snprintf(userNameStr,64,"-U %s ",userName);
  else
    userNameStr[0]='\0';
  /* optional port option string */
  if(port!=dfltIpmiPort)
    snprintf(portStr,16,"-p %d ",port);
  else
    portStr[0]='\0';
  /* optional authType option string */
  if(authType[0]!='\0')
    snprintf(authTypeStr,16,"-A %s ",authType);
  else
    authTypeStr[0]='\0';
  /* optional privLvl option string */
  if(privLvl[0]!='\0')
    snprintf(privLvlStr,24,"-L %s ",privLvl);
  else
    privLvlStr[0]='\0';
  /* optional oemType option string */
  if(oemType[0]!='\0')
    snprintf(oemTypeStr,32,"-o %s ",oemType);
  else
    oemTypeStr[0]='\0';
  /* compose ipmitool command line */
  if(!*passWordFile)                     /* passWord as command-line argument */
  {
    sprintf(ipmiCmdLine,"%s -I lan -H %s %s%s%s%s-P %s %schassis power %s "
            "2>&1",ipmiCmd,hostName,portStr,userNameStr,authTypeStr,
            privLvlStr,passWord,oemTypeStr,ipmiCmdStr);
  }
  else                              /* passWord written in file passWordFile */
  {
    sprintf(ipmiCmdLine,"%s -I lan -H %s %s%s%s%s-f %s %schassis power %s "
            "2>&1",ipmiCmd,hostName,portStr,userNameStr,authTypeStr,
            privLvlStr,passWordFile,oemTypeStr,ipmiCmdStr);
  }
  if(deBug&0x40)
  {
    mPrintf(errU,VERB,__func__,0,"Sending IPMI command: \"%s\".",ipmiCmdLine);
  }
  /*-------------------------------------------------------------------------*/
  for(attemptsN=0;attemptsN<maxCmdAttemptN;attemptsN++)
  {
    /* unmask SIGALRM */ 
    sigemptyset(&signalMask);
    sigaddset(&signalMask,SIGALRM);
    pthread_sigmask(SIG_UNBLOCK,&signalMask,&savedSignalMask);
    /* execute ipmitool command */
    ipmiOutErrFP=popen(ipmiCmdLine,"r");
    /* re-mask SIGALRM */ 
    pthread_sigmask(SIG_SETMASK,&savedSignalMask,NULL);
    /* read ipmitool stdout/stderr */
    fread(ipmiOutErrStr,sizeof(char),OUT_ERR_SIZE,ipmiOutErrFP);
    /* substitutes newlines with dots */
    for(p=ipmiOutErrStr;*p;p++)if(*p=='\n'||*p=='\r')*p='.';
    exitStatus=WEXITSTATUS(pclose(ipmiOutErrFP));
    if(!exitStatus && fnmatch("*[Ee][Rr][Rr][Oo][Rr]*",ipmiOutErrStr,0))break;
    if(deBug&0x200)
    {
      mPrintf(errU,VERB,__func__,0,"ipmitool \"chassis power %s\" command on "
              "host \"%s\" un-succeeded. Attempt %d/%d. String returned by "
              "ipmitool: \"%s\"",ipmiCmdStr,hostName,attemptsN+1,
              maxCmdAttemptN,ipmiOutErrStr);
    }
    if(attemptsN<maxCmdAttemptN-1)
    {
      delay.tv_sec=attemptsN;
      delay.tv_nsec=0;
      nanosleep(&delay,NULL);
    }
  }
  /*-------------------------------------------------------------------------*/
  if(attemptsN==maxCmdAttemptN)                                  /* no reply */
  {
    rv=IPMI_NOT_RESP;
    mPrintf(errU,ERROR,__func__,0,"ipmitool \"chassis power %s\" command on "
            "host \"%s\" failed (%d/%d attempts failed). Last String returned "
            "by ipmitool: \"%s\"!",ipmiCmdStr,hostName,attemptsN,
            maxCmdAttemptN,ipmiOutErrStr);
  }  
  else if(attemptsN && attemptsN<maxCmdAttemptN)
  {
    rv=0;
    mPrintf(errU,WARN,__func__,0,"ipmitool \"chassis power %s\" command on "
            "host \"%s\" succeeded after %d/%d attempts. Last string returned "
            "by ipmitool: \"%s\".",ipmiCmdStr,hostName,attemptsN+1,
            maxCmdAttemptN,ipmiOutErrStr);
  }
  else
  {
    rv=0;
    mPrintf(errU,DEBUG,__func__,0,"ipmitool \"chassis power %s\" command on "
            "host \"%s\" returned string: \"%s\".",ipmiCmdStr,hostName,
            ipmiOutErrStr);
  }
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done. Hostname = %s.",hostName);
  }
  /*-------------------------------------------------------------------------*/
  return rv;
}
/*****************************************************************************/
/* Returns the output of "ipmitool -c sdr list full" command                 */
/* (to be parsed elsewhere)                                                  */
/*****************************************************************************/
char *getSensorsStatus(char *hostName,char *userName,char *passWord,
                       char *passWordFile,int port,char *authType,
                       char *privLvl,char *oemType)
{
  FILE *ipmiOutErrFP=NULL;
  int exitStatus=0;
  char ipmiCmdLine[1024]="";
  char ipmiOutErrStr[SENSORS_BUFFER_SIZE]="";
  char rv[32]=IPMI_NOT_YET_AVAIL_S;
  char rvv[32]=IPMI_NOT_YET_AVAIL_SL;
  int attemptsN=0;
  char *p=NULL;
  char userNameStr[64]="";
  char portStr[16]="";
  char authTypeStr[16]="";
  char privLvlStr[24]="";
  char oemTypeStr[32]="";
  struct timespec delay;
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s.",hostName);
  }
  /*-------------------------------------------------------------------------*/
  /* optional userName option string */
  if(strcmp(userName,"noUser"))
    snprintf(userNameStr,64,"-U %s ",userName);
  else
    userNameStr[0]='\0';
  /* optional port option string */
  if(port!=dfltIpmiPort)
    snprintf(portStr,16,"-p %d ",port);
  else
    portStr[0]='\0';
  /* optional authType option string */
  if(authType[0]!='\0')
    snprintf(authTypeStr,16,"-A %s ",authType);
  else
    authTypeStr[0]='\0';
  /* optional privLvl option string */
  if(privLvl[0]!='\0')
    snprintf(privLvlStr,24,"-L %s ",privLvl);
  else
    privLvlStr[0]='\0';
  /* optional oemType option string */
  if(oemType[0]!='\0')
    snprintf(oemTypeStr,32,"-o %s ",oemType);
  else
    oemTypeStr[0]='\0';
  /* compose ipmitool command line */
  if(!*passWordFile)                     /* passWord as command-line argument */
  {
    sprintf(ipmiCmdLine,"%s -c -I lan -H %s %s%s%s%s-P %s %ssdr list full "
            "2>&1",ipmiCmd,hostName,portStr,userNameStr,authTypeStr,
            privLvlStr,passWord,oemTypeStr);
  }
  else                              /* passWord written in file passWordFile */
  {
    sprintf(ipmiCmdLine,"%s -c -I lan -H %s %s%s%s%s-f %s %ssdr list full "
            "2>&1",ipmiCmd,hostName,portStr,userNameStr,authTypeStr,
            privLvlStr,passWordFile,oemTypeStr);
  }
  if(deBug&0x40)
  {
    mPrintf(errU,VERB,__func__,0,"Sending IPMI command: \"%s\".",ipmiCmdLine);
  }
  /*-------------------------------------------------------------------------*/
  for(attemptsN=0;attemptsN<maxGetAttemptN;attemptsN++)
  {
    /* execute ipmitool command */
    ipmiOutErrFP=popen(ipmiCmdLine, "r");
    fread(ipmiOutErrStr,sizeof(char),SENSORS_BUFFER_SIZE,ipmiOutErrFP);
    /* substitutes newlines with semicolumns */
    for(p=ipmiOutErrStr;*p;p++)if(*p=='\n'||*p=='\r')*p=';';
    if(deBug&0x8)
    {
      mPrintf(errU,DEBUG,__func__,0,"ipmitool \"sdr list full\" command on "
              "host \"%s\" returned string: \"%s\"", hostName,ipmiOutErrStr);
    }
    exitStatus=WEXITSTATUS(pclose(ipmiOutErrFP));
    if(!exitStatus && !strstr(ipmiOutErrStr,"Error obtaining SDR info"))break;
    if(deBug&0x200)
    {
      mPrintf(errU,VERB,__func__,0,"ipmitool \"sdr list full\" command on "
              "host \"%s\" un-succeeded. Attempt %d/%d. String returned by "
              "ipmitool: \"%s\"!",hostName,attemptsN+1,maxGetAttemptN,
              ipmiOutErrStr);
    }
    if(attemptsN<maxGetAttemptN-1)
    {
      delay.tv_sec=attemptsN;
      delay.tv_nsec=0;
      nanosleep(&delay,NULL);
    }
  }
  /*-------------------------------------------------------------------------*/
  /* set the return value */
  if(attemptsN==maxGetAttemptN)                                  /* no reply */
  {
    strcpy(rv,IPMI_NOT_RESP_S);
    strcpy(rvv,IPMI_NOT_RESP_SL);
  }
  else
  {
    strcpy(rv,IPMI_OK_S);
    strcpy(rvv,IPMI_OK_SL);
  }
  /*-------------------------------------------------------------------------*/
  if(!strcmp(rv,IPMI_NOT_RESP_S))
  {
    mPrintf(errU,ERROR,__func__,0,"ipmitool \"sdr list full\" command on "
            "host \"%s\" failed (%d/%d attempts failed). Status is set to "
            "\"%s\". Last string returned by ipmitool: \"%s\"!",hostName,
            attemptsN,maxGetAttemptN,rvv,ipmiOutErrStr);
  }
  else if(attemptsN>0 && attemptsN<maxGetAttemptN)
  {
    mPrintf(errU,WARN,__func__,0,"ipmitool \"sdr list full\" command on "
            "host \"%s\" succeeded after %d/%d attempts. Status is \"%s\"!",
            hostName,attemptsN+1,maxGetAttemptN,rvv);
  }
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done. Hostname = %s.",hostName);
  }
  /*-------------------------------------------------------------------------*/
  if(!strcmp(rv,IPMI_NOT_RESP_S))return strdup(rv);
  else return strdup(ipmiOutErrStr);
}
/*****************************************************************************/
char *getSensorsList(char *hostName,char *userName,char *passWord,
                     char *passWordFile,int port,char *authType,char *privLvl,
                     char *oemType,char *sensorType)
{
  FILE *ipmiOutErrFP=NULL;
  int exitStatus=0;
  char ipmiCmdLine[1024]="";
  char ipmiOutErrStr[SENSORS_BUFFER_SIZE]="";
  char rv[32]=IPMI_NOT_YET_AVAIL_S;
  char rvv[32]=IPMI_NOT_YET_AVAIL_SL;
  int attemptsN=0;
  char *p=NULL;
  char userNameStr[64]="";
  char portStr[16]="";
  char authTypeStr[16]="";
  char privLvlStr[24]="";
  char oemTypeStr[32]="";
  struct timespec delay;
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"entered. Hostname = %s.",hostName);
  }
  /*-------------------------------------------------------------------------*/
  /* optional userName option string */
  if(strcmp(userName,"noUser"))
    snprintf(userNameStr,64,"-U %s ",userName);
  else
    userNameStr[0]='\0';
  /* optional port option string */
  if(port!=dfltIpmiPort)
    snprintf(portStr,16,"-p %d ",port);
  else
    portStr[0]='\0';
  /* optional authType option string */
  if(authType[0]!='\0')
    snprintf(authTypeStr,16,"-A %s ",authType);
  else
    authTypeStr[0]='\0';
  /* optional privLvl option string */
  if(privLvl[0]!='\0')
    snprintf(privLvlStr,24,"-L %s ",privLvl);
  else
    privLvlStr[0]='\0';
  /* optional oemType option string */
  if(oemType[0]!='\0')
    snprintf(oemTypeStr,32,"-o %s ",oemType);
  else
    oemTypeStr[0]='\0';
  /* compose ipmitool command line */
  if(!*passWordFile)                     /* passWord as command-line argument */
  {
    sprintf(ipmiCmdLine,"%s -c -I lan -H %s %s%s%s%s-P %s %ssdr type %s 2>&1",
            ipmiCmd,hostName,portStr,userNameStr,authTypeStr,privLvlStr,
            passWord,oemTypeStr,sensorType);
  }
  else                              /* passWord written in file passWordFile */
  {
    sprintf(ipmiCmdLine,"%s -c -I lan -H %s %s%s%s%s-f %s %ssdr type %s 2>&1",
            ipmiCmd,hostName,portStr,userNameStr,authTypeStr,privLvlStr,
            passWordFile,oemTypeStr,sensorType);
  }
  if(deBug&0x40)
  {
    mPrintf(errU,VERB,__func__,0,"Sending IPMI command: \"%s\".",ipmiCmdLine);
  }
  /*-------------------------------------------------------------------------*/
  for(attemptsN=0;attemptsN<maxGetAttemptN;attemptsN++)
  {
    /* execute ipmitool command */
    ipmiOutErrFP=popen(ipmiCmdLine, "r");
    fread(ipmiOutErrStr,sizeof(char),SENSORS_BUFFER_SIZE,ipmiOutErrFP);
    /* substitutes newlines with semicolumns */
    for(p=ipmiOutErrStr;*p;p++)if(*p=='\n'||*p=='\r')*p=';';
    if(deBug&0x8)
    {
      mPrintf(errU,DEBUG,__func__,0,"ipmitool \"sdr type %s\" command on "
              "host \"%s\" returned string: \"%s\"",sensorType,hostName,
              ipmiOutErrStr);
    }
    exitStatus=WEXITSTATUS(pclose(ipmiOutErrFP));
    if(!exitStatus && 
       !strstr(ipmiOutErrStr,"Error") &&
       !strstr(ipmiOutErrStr,"failed") &&
       !strstr(ipmiOutErrStr,"Unable"))break;
    if(deBug&0x200)
    {
      mPrintf(errU,VERB,__func__,0,"ipmitool \"sdr type %s\" command on "
              "host \"%s\" un-succeeded. Attempt %d/%d. String returned by "
              "ipmitool: \"%s\"!",sensorType,hostName,attemptsN+1,
              maxGetAttemptN,ipmiOutErrStr);
    }
    if(attemptsN<maxGetAttemptN-1)
    {
      delay.tv_sec=attemptsN;
      delay.tv_nsec=0;
      nanosleep(&delay,NULL);
    }
  }
  /*-------------------------------------------------------------------------*/
  /* set the return value */
  if(attemptsN==maxGetAttemptN)                                  /* no reply */
  {
    strcpy(rv,IPMI_NOT_RESP_S);
    strcpy(rvv,IPMI_NOT_RESP_SL);
  }
  else
  {
    strcpy(rv,IPMI_OK_S);
    strcpy(rvv,IPMI_OK_SL);
  }
  /*-------------------------------------------------------------------------*/
  if(!strcmp(rv,IPMI_NOT_RESP_S))
  {
    mPrintf(errU,ERROR,__func__,0,"ipmitool \"sdr type %s\" command on "
            "host \"%s\" failed (%d/%d attempts failed). Status is set to "
            "\"%s\". Last string returned by ipmitool: \"%s\"!",sensorType,
            hostName,attemptsN,maxGetAttemptN,rvv,ipmiOutErrStr);
  }
  else if(attemptsN>0 && attemptsN<maxGetAttemptN)
  {
    mPrintf(errU,WARN,__func__,0,"ipmitool \"sdr type %s\" command on "
            "host \"%s\" succeeded after %d/%d attempts. Status is \"%s\"!",
            sensorType,hostName,attemptsN+1,maxGetAttemptN,rvv);
  }
  /*-------------------------------------------------------------------------*/
  if(deBug&0x10)
  {
    mPrintf(errU,DEBUG,__func__,0,"done. Hostname = %s.",hostName);
  }
  /*-------------------------------------------------------------------------*/
  if(!strcmp(rv,IPMI_NOT_RESP_S))return strdup(rv);
  else return strdup(ipmiOutErrStr);
}
/*****************************************************************************/
/* getActuatorVersion() - Returns the RCS identifier of this file.           */
/*****************************************************************************/
char *getActuatorVersion()
{
  return rcsid;
}
/*****************************************************************************/
