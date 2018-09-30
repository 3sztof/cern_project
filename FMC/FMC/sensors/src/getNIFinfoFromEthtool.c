/*****************************************************************************/
// to compile as standalone for test:
//gcc -Wall -std=gnu99 -I../include -I../../utils/include -D TEST -L /opt/FMC/lib64 -lFMCutils-5 -lm getNIFinfoFromEthtool.c -o getNIFinfoFromEthtool
/*****************************************************************************/
/*
 * $Log: getNIFinfoFromEthtool.c,v $
 * Revision 1.3  2009/01/29 09:44:45  galli
 * minor changes
 *
 * Revision 1.1  2008/02/22 08:57:25  galli
 * Initial revision
 * */
/*****************************************************************************/
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <sys/wait.h>                                       /* WEXITSTATUS() */
#include <errno.h>                                                  /* errno */
#include <string.h>                                  /* strerror(), strcmp() */
#include <unistd.h>                                  /* access(2), getuid(2) */
/*****************************************************************************/
#include "getNIFinfoFromEthtool.h"                         /* struct nifAttr */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
#include "fmcProcNameUtils.h"                               /* getProcName() */
/*****************************************************************************/
/* globals */
static char rcsid[]="$Id: getNIFinfoFromEthtool.c,v 1.3 2009/01/29 09:44:45 galli Exp $";
/*****************************************************************************/
nifAttr_t getNIFinfo(char *ethtoolCmd,char *ifName,int deBug,int errU)
{
  nifAttr_t nifInfo;
  char ethtoolCmdLine[1024]="";
  FILE *ethtoolOutErrFP=NULL;
  char ethtoolOutErrStr[SENSORS_BUFFER_SIZE]="";
  char *p;
  char *lp;
  char *line;
  char *lfp,*dfp;                 /* label field pointer, data field pointer */
  uid_t uid=-1;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  /* reset values */
  nifInfo.version=rcsid;
  nifInfo.success=0;
  nifInfo.exitStatus=-1;
  strcpy(nifInfo.errorString,"");
  strcpy(nifInfo.speed,"N/A");
  strcpy(nifInfo.duplex,"N/A");
  strcpy(nifInfo.portType,"N/A");
  strcpy(nifInfo.autoNegotiation,"N/A");
  strcpy(nifInfo.linkDetected,"N/A");
  /*-------------------------------------------------------------------------*/
  /* check accessibility of ethtool */
  if(access(ethtoolCmd,X_OK)==-1)                           /* access denied */
  {
    snprintf(nifInfo.errorString,SENSORS_BUFFER_SIZE,"Cannot execute command: "
             "\"%s\": %s!",ethtoolCmd,strerror(errno));
    return nifInfo;
  }
  /*-------------------------------------------------------------------------*/
  /* check if the current process is executed as user root */
  uid=getuid();
  if(uid!=0)
  {
    snprintf(nifInfo.errorString,SENSORS_BUFFER_SIZE,"In order to retrieve "
             "NIF information (speed, duplex, etc.), the process \"%s\" must "
             "be executed by the user \"root\"!",getProcName(errU));
    return nifInfo;
  }
  /*-------------------------------------------------------------------------*/
  /* execute ethtool command */
  sprintf(ethtoolCmdLine,"%s %s 2>&1",ethtoolCmd,ifName);
  ethtoolOutErrFP=popen(ethtoolCmdLine,"r");
  fread(ethtoolOutErrStr,sizeof(char),SENSORS_BUFFER_SIZE,ethtoolOutErrFP);
  nifInfo.exitStatus=WEXITSTATUS(pclose(ethtoolOutErrFP));
  /*-------------------------------------------------------------------------*/
  if(!nifInfo.exitStatus)nifInfo.success=1;
  else
  {
    snprintf(nifInfo.errorString,SENSORS_BUFFER_SIZE,"%s",ethtoolOutErrStr);
    /* substitutes newlines with semicolumns */
    for(p=nifInfo.errorString;*p;p++)if(*p=='\n'||*p=='\r')*p='.';
    return nifInfo;
  }
  /*-------------------------------------------------------------------------*/
  /* skip header line */
  p=ethtoolOutErrStr;
  p=strchr(p,'\n');
  if(!p)                                                    /* EOL not found */
  {
    snprintf(nifInfo.errorString,SENSORS_BUFFER_SIZE,"%s",ethtoolOutErrStr);
    /* substitutes newlines with dots */
    for(p=nifInfo.errorString;*p;p++)if(*p=='\n'||*p=='\r')*p='.';
    nifInfo.success=0;
    return nifInfo;
  }
  p++;
  /*-------------------------------------------------------------------------*/
  /* parse output */
  for(;;)
  {
    line=p;
    p=strchr(p,'\n');                                                 /* EOL */
    if(!p)break;                                            /* EOL not found */
    if(!(p+1))break;                                  /* NULL char after EOL */
    *p='\0';
    if(!strlen(line))break;                              /* zero-length line */
    /* parse line */
    lp=line;
    lp+=strspn(lp," \t");                                     /* skip blanks */
    if((lfp=strstr(lp,"Speed:")))
    {
       dfp=lfp+strlen("Speed:");
       dfp+=strspn(dfp," \t");                                /* skip blanks */
       if(strstr(dfp,"Unknown"))snprintf(nifInfo.speed,ATTR_LEN+1,"%s","N/A");
       else snprintf(nifInfo.speed,ATTR_LEN+1,"%s",dfp);
    }
    else if((lfp=strstr(lp,"Duplex:")))
    {
       dfp=lfp+strlen("Duplex:");
       dfp+=strspn(dfp," \t");                                /* skip blanks */
       if(strstr(dfp,"Unknown"))snprintf(nifInfo.duplex,ATTR_LEN+1,"%s","N/A");
       else snprintf(nifInfo.duplex,ATTR_LEN+1,"%s",dfp);
    }
    else if((lfp=strstr(lp,"Port:")))
    {
       dfp=lfp+strlen("Port:");
       dfp+=strspn(dfp," \t");                                /* skip blanks */
       if(strstr(dfp,"Unknown"))
         snprintf(nifInfo.portType,ATTR_LEN+1,"%s","N/A");
       else snprintf(nifInfo.portType,ATTR_LEN+1,"%s",dfp);
    }
    else if((lfp=strstr(lp,"Auto-negotiation:")))
    {
       dfp=lfp+strlen("Auto-negotiation:");
       dfp+=strspn(dfp," \t");                                /* skip blanks */
       if(strstr(dfp,"Unknown"))
         snprintf(nifInfo.autoNegotiation,ATTR_LEN+1,"%s","N/A");
       else snprintf(nifInfo.autoNegotiation,ATTR_LEN+1,"%s",dfp);
    }
    else if((lfp=strstr(lp,"Link detected:")))
    {
       dfp=lfp+strlen("Link detected:");
       dfp+=strspn(dfp," \t");                                /* skip blanks */
       if(strstr(dfp,"Unknown"))
         snprintf(nifInfo.linkDetected,ATTR_LEN+1,"%s","N/A");
       else snprintf(nifInfo.linkDetected,ATTR_LEN+1,"%s",dfp);
    }
    p++;
  }
  /*-------------------------------------------------------------------------*/
  return nifInfo;
}
/*****************************************************************************/
/* getFSsensorVersion() - Returns the RCS identifier of this file.           */
/*****************************************************************************/
char *getNIFinfoSensorVersion(void)
{
  return rcsid;
}
/*****************************************************************************/
#ifdef TEST
int main(int argc,char **argv)
{
  char *ethtoolCmd="/usr/sbin/ethtool";
  nifAttr_t na;
  if(argc!=2)
  {
    fprintf(stderr,"usage: %s ifname\n",argv[0]);
    return 1;
  }
  na=getNIFinfo(ethtoolCmd,argv[1],0,2);
  printf("\n");
  printf("success:     %d\n",na.success);
  printf("exitStatus:  %d\n",na.exitStatus);
  printf("errorString: \"%s\"\n",na.errorString);
  printf("\n");
  printf("speed           = \"%s\"\n",na.speed);
  printf("duplex          = \"%s\"\n",na.duplex);
  printf("portType        = \"%s\"\n",na.portType);
  printf("autoNegotiation = \"%s\"\n",na.autoNegotiation);
  printf("linkDetected    = \"%s\"\n",na.linkDetected);
  return 0;
}
#endif
/*****************************************************************************/
