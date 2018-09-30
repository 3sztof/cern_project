/* ######################################################################### */
/*
 * $Log: fmcMsgUtils.c,v $
 * Revision 2.20  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 2.19  2011/11/29 12:01:06  galli
 * Minor changes
 *
 * Revision 2.14  2008/05/16 14:16:15  galli
 * more parameters to dfltLoggerOpen() function
 * new logger unit L_STD_C
 * bug fixed
 *
 * Revision 2.13  2008/02/22 08:52:07  galli
 * bug fixed
 *
 * Revision 2.9  2007/10/26 09:16:17  galli
 * bug fixed
 *
 * Revision 2.8  2007/10/26 09:05:37  galli
 * print also FMC version in start message.
 *
 * Revision 2.7  2007/08/14 06:45:33  galli
 * message size definition moved to header
 *
 * Revision 2.6  2007/08/10 07:18:00  galli
 * bug fixed
 *
 * Revision 2.5  2007/08/09 12:59:20  galli
 * New functions mPrintf(), rPrintf(), mfPrintf(), rfPrintf()
 * Functions printM(), printMF(), printR() and printRF() now outdated
 * and deprecated.
 *
 * Revision 2.3  2007/06/18 15:29:48  galli
 * bug fixed
 *
 * Revision 2.2  2006/10/18 11:50:59  galli
 * use the __func__ magic variable
 *
 * Revision 2.1  2006/10/12 14:42:53  galli
 * added UTGID in message header.
 * bug fixed in adding newline.
 *
 * Revision 2.0  2006/10/10 10:23:39  galli
 * Suppressed log to a specific DIM service
 * New functions loggerOpen(), loggerClose(), dfltLoggerOpen(),
 * dfltLoggerClose(), getDfltFifoFD(), printMF(), printRF().
 * Functions msgInit(), msgGetLogFD(), msgTerm(), msgSend() now outdated
 * and deprecated
 *
 * Revision 1.22  2006/08/24 12:39:00  galli
 * getPName() fuction added
 *
 * Revision 1.21  2006/08/24 09:33:35  galli
 * dimLoggerInit() takes the FIFO path as argument and returns the FIFO file
 * descriptor.
 * msgInit() looses the first (useless) argument: char **argv
 * dimLoggerInit() becomes public.
 *
 * Revision 1.19  2006/08/22 07:02:48  galli
 * bug fixed in printR()
 *
 * Revision 1.18  2006/08/22 06:47:07  galli
 * function printR() added
 * bug fixed in gMsgSend()
 *
 * Revision 1.17  2006/08/17 09:24:59  galli
 * function printM() added
 * bug fixed in gMsgSend()
 *
 * Revision 1.16  2006/02/09 11:13:13  galli
 * function msgStart() added
 *
 * Revision 1.15  2004/12/05 00:49:53  galli
 * lowPrioMsgSend() function added
 *
 * Revision 1.12  2004/12/02 22:05:14  galli
 * truncate too long messages or not NULL-terminated messages
 *
 * Revision 1.8  2004/10/20 08:55:45  galli
 * Low level write to fifo
 *
 * Revision 1.4  2004/10/19 22:40:52  galli
 * added logging as an individual DIM service
 *
 * Revision 1.3  2004/10/19 13:37:08  galli
 * Added loggerType argument in msgInit()
 *
 * Revision 1.2  2004/10/15 23:26:08  galli
 * In msgInit() check if /tmp/logSrv.fifo i-node is really a fifo
 * msgInit() retries 5 time to open /tmp/logSrv.fifo, waiting 1 second
 *
 * Revision 1.1  2004/10/12 14:45:25  galli
 * Initial revision
 *
 */
/* ######################################################################### */
#include <stdio.h>                                    /* snprintf(), fopen() */
#include <time.h>                       /* time(), localtime_r(), strftime() */
#include <errno.h>
#include <string.h>                        /* strdup(), strchr(), strerrot() */
#include <libgen.h>                                            /* basename() */
#include <unistd.h>                                           /* gethostname */
#include <sys/types.h>                                             /* open() */
#include <sys/stat.h>                                              /* open() */
#include <fcntl.h>                                                 /* open() */
#include <stdlib.h>                                                /* free() */
#include <syslog.h>                                              /* syslog() */
#include <stdarg.h>                                         /* va_list, etc. */
/*****************************************************************************/
#include "fmcMsgUtils.h"
#include "fmcVersion.h"                                       /* FMC version */
/* ######################################################################### */
/* globals */
char hostName[80]="";
char *pName=NULL;
char *utgid=NULL;
int dfltFifoFD=-1;
static int lType;                          /* deprecated: will be suppressed */
static char msgUtilsRcsid[]="$Id: fmcMsgUtils.c,v 2.20 2012/11/29 15:52:27 galli Exp galli $";
char *msgUtilsRcsidP=msgUtilsRcsid;
/*****************************************************************************/
/* private function prototype */
int dimLoggerMsgSend(char *buf,int fifoFD);
ssize_t stdErrMsgSend(char *buf);
int sysLogMsgSend(char *buf,int severity);
void getPName();
/* ######################################################################### */
/* public functions */
/*****************************************************************************/
/* int loggerOpen(char *fifoPath,int noDrop,int errU);                       */
/* Open an existing FIFO for writing.                                        */
/* Called by dfltLoggerOpen() or directly to open a secondary FIFO.          */
/* noDrop=0: congestion-proof policy;                                        */
/* noDrop=1: no-drop policy.                                                 */
/* errU=L_DIM|L_STD|L_SYS: destination of error messages.                    */
/* errL=DEBUG|INFO|WARN|ERROR|FATAL: severity of all the error messages      */
/* Return the FIFO file descriptor or -1 if an error occurred.               */
/*****************************************************************************/
int loggerOpen(char *fifoPath,int noDrop,int errU,int errL)
{
  struct stat statBuf;
  int fifoFD=-1;
  /*-------------------------------------------------------------------------*/
  /* check if fifoPath is writable */
  if(access(fifoPath,W_OK)==-1)                             /* access denied */
  {
    mPrintf(errU,errL,__func__,0,"Cannot open output FIFO: \"%s\": access(): "
            "%s!",fifoPath,strerror(errno));
    if(errno==ENOENT && errL>DEBUG)
    {
      if(!strcmp(fifoPath,dfltFifoPath))
      {
        mPrintf(errU,errL,__func__,0,"Hint: run \"logSrv\" (without -p or "
                "-s options) on the node \"%s\"!",hostName);
      }
      else
      {
        mPrintf(errU,errL,__func__,0,"Hint: run \"logSrv -p %s -s "
                "<srv_name>\" on the node \"%s\"!",fifoPath,hostName);
      }
    }
    return -1;
  }
  /* get fifoPath information */
  if(stat(fifoPath,&statBuf)==-1)
  {
    mPrintf(errU,errL,__func__,0,"stat(\"%s\"): %s!",fifoPath,strerror(errno));
    return -1;
  }
  /* check if fifoPath is a FIFO */
  if(!S_ISFIFO(statBuf.st_mode))
  {
    mPrintf(errU,errL,__func__,0,"I-node: \"%s\" is not a FIFO!",fifoPath);
    return -1;
  }
  /*-------------------------------------------------------------------------*/
  /* open error log */
  if(noDrop)fifoFD=open(fifoPath,O_WRONLY|O_NONBLOCK|O_APPEND);   /* no-drop */
  else fifoFD=open(fifoPath,O_RDWR|O_NONBLOCK|O_APPEND); /* congestion-proof */
  if(fifoFD==-1)
  {
    if(errno==ENXIO)
    {
      mPrintf(errU,errL,__func__,0,"open(\"%s\"): No process has the FIFO "
              "\"%s\" open for reading!",fifoPath,fifoPath);
      if(errL>DEBUG)
      {
        if(!strcmp(fifoPath,dfltFifoPath))
        {
          mPrintf(errU,errL,__func__,0,"Hint: run \"logSrv\" (without -p or "
                 "-s options) on the node \"%s\"!",hostName);
        }
        else
        {
          mPrintf(errU,errL,__func__,0,"Hint: run \"logSrv -p %s -s "
                  "<srv_name>\" on the node \"%s\"!",fifoPath,hostName);
        }
      }
    }
    else
    {
      mPrintf(errU,errL,__func__,0,"open(\"%s\"): %s!",fifoPath,
              strerror(errno));
    }
    return -1;
  }
  /*-------------------------------------------------------------------------*/
  if(noDrop)
  {
    /* Now we are sure that another process has the FIFO open for reading.   */
    /* We unset now the O_NONBLOCK bit to have blocking write (no-drop       */
    /* behaviour).                                                           */
    int statusFlag=0;
    statusFlag=fcntl(fifoFD,F_GETFL);
    if(statusFlag<0)
    {
      mPrintf(errU,errL,__func__,0,"fcntl(\"%s\",F_GETFL): %s!",fifoPath,
              strerror(errno));
      return -1;
    }
    statusFlag&=~O_NONBLOCK;                         /* unset O_NONBLOCK bit */
    if(fcntl(fifoFD,F_SETFL,statusFlag)==-1)
    {
      mPrintf(errU,errL,__func__,0,"fcntl(\"%s\",F_SETFL): %s!",fifoPath,
              strerror(errno));
      return -1;
    }
  }
  /*-------------------------------------------------------------------------*/
  return fifoFD;
}
/*****************************************************************************/
void loggerClose(int fifoFD)
{
  if(fifoFD!=-1)close(fifoFD);
  return;
}
/*****************************************************************************/
/* dfltLoggerOpen(int openTryN,int noDrop,int errL,int infoL,int color);     */
/* Open the default (existing) FIFO dfltFifoPath for writing to the default  */
/* logger for the current process.                                           */
/* openTryN: number of fifo open try.                                        */
/* openTryN=0: try forever.                                                  */
/* noDrop=0: congestion-proof policy;                                        */
/* noDrop=1: no-drop policy.                                                 */
/* errL=DEBUG|INFO|WARN|ERROR|FATAL: severity of the error message           */
/* infoL=DEBUG|INFO|WARN|ERROR|FATAL: severity of the info message           */
/* color=0|1 colored or b&w                                                  */
/* Set the global variable dfltFifoFD.                                       */
/* Return 0 if success or -1 if an error occurred.                           */
/*****************************************************************************/
int dfltLoggerOpen(int openTryN,int noDrop,int errL,int infoL,int color)
{
  int i;
  char *p;
  int errLm1=0;
  int errU;
  /*-------------------------------------------------------------------------*/
  /* Strip msgUtilsRcsid */
  msgUtilsRcsidP=strchr(msgUtilsRcsid,':')+2;
  for(i=0,p=msgUtilsRcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  if(color)errU=L_STD_C|L_SYS;
  else errU=L_STD|L_SYS;
  /*-------------------------------------------------------------------------*/
  /* syslog version */
  if(infoL)mPrintf(errU,infoL,__func__,0,"Using: \"%s\", \"FMC-%s\"",
                   msgUtilsRcsidP,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  errLm1=errL;
  if(errLm1<DEBUG)errLm1=DEBUG;
  /*-------------------------------------------------------------------------*/
  for(i=0;i<openTryN||openTryN==0;i++)
  {
    if(i>0)
    {
      if(infoL)mPrintf(errU,infoL,__func__,0,"Waiting 1 second. Then retry.");
      sleep(1);
    }
    dfltFifoFD=loggerOpen(dfltFifoPath,noDrop,errU,errLm1);
    if(dfltFifoFD!=-1)break;
  }
  if(dfltFifoFD==-1)
  {
    mPrintf(errU,errL,__func__,0,"Can't open default FIFO \"%s\"!",
            dfltFifoPath);
    if(errL>INFO)
      mPrintf(errU,errL,__func__,0,"Hint: run \"logSrv\" (without -p "
              "or -s options)!");
    return 1;
  }
  else
  {
    if(infoL)mPrintf(errU,infoL,__func__,0,"Default FIFO \"%s\" succesfully "
            "opened.",dfltFifoPath);
    return 0;
  }
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
int getDfltFifoFD(void)
{
  return dfltFifoFD;
}
/*****************************************************************************/
void dfltLoggerClose(void)
{
  loggerClose(dfltFifoFD);
  return;
}
/*****************************************************************************/
/* Substitute of fprintf() to print to one or more of the 3 output units:    */
/*   L_DIM: DIM Default Logger                                               */
/*   L_STD: standard error stream, stderr                                    */
/*   L_SYS: Linux syslog facility                                            */
/* Time, severity, hostname, process name, UTGID and function name prepended */
/* Arguments:                                                                */
/*   out: bitwise or of L_DIM, L_STD, L_SYS (e.g.: L_DIM|L_STD|L_SYS)        */
/*   severity: VERB, DEBUG, INFO, WARN, ERROR or FATAL                       */
/*   fName: typically __func__ magic variable                                */
/*   tryN: number of write trials                                            */
/*     if tryN==0 than tryN=1 for VERB, 2 for DEBUG, 3 for INFO,...          */
/*   format: like printf                                                     */
/*   ...: like printf                                                        */
/* Return value:                                                             */
/*    rVal & 0x0000ffff: number of write failures to L_DIM                   */
/*   (rVal & 0xffff0000) >> 16: number of write failures to L_STD            */
/*****************************************************************************/
int mPrintf(int out,int severity,const char* fName,int tryN,
            const char *format,...)
{
  char *sl[7]={"[OK]","[VERB] ","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
               "[FATAL]"};
  time_t now;
  struct tm lNow;
  char rawMsg[MSG_SZ]="";
  int rawMsgLen=0;
  char sNow[13];                                           /* Oct12-134923\0 */
  va_list ap;
  char msgHead1[MSG_HEAD_SZ]="";       /* message header for L_DIM and L_STD */
  int msgHead1Len=0;            /* message header length for L_DIM and L_STD */
  char msgHead2[MSG_HEAD_SZ]="";                 /* message header for L_SYS */
  int msgHead2Len=0;                      /* message header length for L_SYS */
  char *p;
  int rVal=0;
  /*-------------------------------------------------------------------------*/
  /* if tryN==0 than tryN=1 for VERB, 2 for DEBUG, 3 for INFO, ... */
  if(!tryN)tryN=severity;
  /*-------------------------------------------------------------------------*/
  /* compose raw message */
  va_start(ap,format);
  vsnprintf(rawMsg,MSG_SZ,format,ap);        /* truncate to MSG_SZ if longer */
  va_end(ap);
  /* NULL-terminate if truncated */
  if(!memchr(rawMsg,0,MSG_SZ))rawMsg[MSG_SZ-1]='\0';
  /* replace CR with spaces */
  for(p=rawMsg;*p;p++)if(*p=='\r')*p=' ';
  /* add last LF if missed */
  rawMsgLen=strlen(rawMsg);
  if(rawMsg[rawMsgLen-1]!='\n')
  {
    if(rawMsgLen<MSG_SZ-1)strcat(rawMsg,"\n");
    else rawMsg[rawMsgLen-1]='\n';
  }
  /*-------------------------------------------------------------------------*/
  /* time string */
  now=time(NULL);
  localtime_r(&now,&lNow);
  strftime(sNow,13,"%b%d-%H%M%S",&lNow);
  /*-------------------------------------------------------------------------*/
  /* process name */
  if(!pName)getPName();
  /*-------------------------------------------------------------------------*/
  /* utgid */
  if(!utgid)utgid=getenv("UTGID");
  if(!utgid)utgid="no UTGID";
  /*-------------------------------------------------------------------------*/
  /* host name */
  if(hostName[0]=='\0')                        /* hostName still not defined */
  {
    gethostname(hostName,80);
    p=strchr(hostName,'.');
    if(p)*p='\0';
  }
  /*-------------------------------------------------------------------------*/
  /* compose head for L_DIM and L_STD (truncate if longer than MSG_HEAD_SZ) */
  snprintf(msgHead1,MSG_HEAD_SZ,"%s%s%s: %s(%s): %s(): ",sNow,sl[severity],
           hostName,pName,utgid,fName);
  /* add string terminator if missed */
  if(!memchr(msgHead1,0,MSG_HEAD_SZ))msgHead1[MSG_HEAD_SZ-1]='\0';
  msgHead1Len=strlen(msgHead1);
  /*-------------------------------------------------------------------------*/
  /* compose head for L_SYS (truncate if longer than MSG_HEAD_SZ) */
  snprintf(msgHead2,MSG_HEAD_SZ,"(%s): %s%s(): ",utgid,sl[severity],fName);
  /* add string terminator if missed */
  if(!memchr(msgHead2,0,MSG_HEAD_SZ))msgHead2[MSG_HEAD_SZ-1]='\0';
  msgHead2Len=strlen(msgHead2);
  /*-------------------------------------------------------------------------*/
  if(out&L_DIM && dfltFifoFD!=-1)
  {
    /* Split message in message lines and message line in message segments */
    /* A message line terminates with a \n in the original message */
    /* Message segment with header must fit in FIFO_MSG_SZ for atomic write */
    int droppedN=0;
    char msgSeg[FIFO_MSG_SZ]="";                     /* message line segment */
    /* message line segment available length for text (excluding \n\0) */
    int msgSegAvailLen=FIFO_MSG_SZ-msgHead1Len-2;
    char *lp;                                        /* message line pointer */
    int ll;                                           /* message line length */
    int tryC;                                                 /* try counter */
    struct timespec delay={0,1000000};                            /* 0.001 s */
    for(lp=rawMsg;*lp;lp=1+strchr(lp,'\n'))               /* loop over lines */
    {
      char *sp;                              /* message line segment pointer */
      int sl;                                 /* message line segment length */
      ll=strcspn(lp,"\n");
      for(sp=lp;sp<lp+ll;sp+=(msgSegAvailLen))    /* loop over line segments */
      {
        sl=strcspn(sp,"\n");
        if(sl>msgSegAvailLen)sl=msgSegAvailLen;
        /* compose header + message segment + \n */
        strcpy(msgSeg,msgHead1);
        strncat(msgSeg,sp,sl);
        strcat(msgSeg,"\n");
        for(tryC=0;tryC<tryN;tryC++)
        {
          int written;
          written=write(dfltFifoFD,msgSeg,strlen(msgSeg));
          if(written!=-1||errno!=EAGAIN)break;
          nanosleep(&delay,NULL);
          delay.tv_sec*=2;
          delay.tv_nsec*=2;
          if(delay.tv_nsec>999999999)
          {
            delay.tv_sec+=1;
            delay.tv_nsec-=1000000000;
          }
        }
        if(tryC==tryN)droppedN++;
      }
    }
    if(droppedN>0xffff)droppedN=0xffff;
    rVal|=droppedN;
  }
  /*-------------------------------------------------------------------------*/
  if(out&L_STD)
  {
    char msg[MSG_HEAD_SZ+MSG_SZ];                         /* msgHead1+rawMsg */
    /* compose raw message string with header */
    snprintf(msg,MSG_SZ+MSG_HEAD_SZ,"%s%s",msgHead1,rawMsg);
    if(write(STDERR_FILENO,msg,strlen(msg))==-1)rVal|=0x00010000;
  }
  /*-------------------------------------------------------------------------*/
  if(out&L_STD_C)
  {
    char msg[MSG_HEAD_SZ+MSG_SZ];                         /* msgHead1+rawMsg */
    /* compose raw message string with header */
    switch(severity)
    {
      case 0: /* No label - white */
        snprintf(msg,MSG_SZ+MSG_HEAD_SZ,"%s%s%s%s","\033[0;37;40m",msgHead1,
                 rawMsg,"\033[0m");
        break;
      case 1: /* VERBOSE - blue */
        snprintf(msg,MSG_SZ+MSG_HEAD_SZ,"%s%s%s%s","\033[0;34;40m",msgHead1,
                 rawMsg,"\033[0m");
        break;
      case 2: /* DEBUG - green */
        snprintf(msg,MSG_SZ+MSG_HEAD_SZ,"%s%s%s%s","\033[0;32;40m",msgHead1,
                 rawMsg,"\033[0m");
        break;
      case 3: /* INFO - white */
        snprintf(msg,MSG_SZ+MSG_HEAD_SZ,"%s%s%s%s","\033[0;37;40m",msgHead1,
                 rawMsg,"\033[0m");
        break;
      case 4: /* WARN - bold yellow */
        snprintf(msg,MSG_SZ+MSG_HEAD_SZ,"%s%s%s%s","\033[0;1;33;40m",msgHead1,
                 rawMsg,"\033[0m");
        break;
      case 5: /* ERROR - bold red */
        snprintf(msg,MSG_SZ+MSG_HEAD_SZ,"%s%s%s%s","\033[0;1;31;40m",msgHead1,
                 rawMsg,"\033[0m");
        break;
      case 6: /* FATAL - bold magenta */
        snprintf(msg,MSG_SZ+MSG_HEAD_SZ,"%s%s%s%s","\033[0;1;35;40m",msgHead1,
                 rawMsg,"\033[0m");
        break;
      }
    if(write(STDERR_FILENO,msg,strlen(msg))==-1)rVal|=0x00010000;
  }
  /*-------------------------------------------------------------------------*/
  if(out&L_SYS)
  {
    /* Split message in message lines and message line in message segments */
    /* A message line terminates with a \n in the original message */
    /* A message segment with header must fit in SYSLOG_MSG_SZ */
    char msgSeg[SYSLOG_MSG_SZ]="";                   /* message line segment */
    /* message line segment available length for text (excluding \n\0) */
    int msgSegAvailLen=SYSLOG_MSG_SZ-msgHead2Len-2;
    char *lp;                                        /* message line pointer */
    int ll;                                           /* message line length */
    for(lp=rawMsg;*lp;lp=1+strchr(lp,'\n'))               /* loop over lines */
    {
      char *sp;                              /* message line segment pointer */
      int sl;                                 /* message line segment length */
      ll=strcspn(lp,"\n");
      for(sp=lp;sp<lp+ll;sp+=(msgSegAvailLen))    /* loop over line segments */
      {
        sl=strcspn(sp,"\n");
        if(sl>msgSegAvailLen)sl=msgSegAvailLen;
        /* compose header + message segment + \n */
        strcpy(msgSeg,msgHead2);
        strncat(msgSeg,sp,sl);
        strcat(msgSeg,"\n");
        if(severity<=DEBUG)syslog(LOG_DEBUG|LOG_DAEMON,msgSeg);
        else if(severity==INFO)syslog(LOG_INFO|LOG_DAEMON,msgSeg);
        else if(severity==WARN)syslog(LOG_WARNING|LOG_DAEMON,msgSeg);
        else if(severity==ERROR)syslog(LOG_ERR|LOG_DAEMON,msgSeg);
        else if(severity==FATAL)syslog(LOG_CRIT|LOG_DAEMON,msgSeg);
        else syslog(LOG_INFO|LOG_DAEMON,msgSeg);
      }
    }
  }
  /*-------------------------------------------------------------------------*/
  return rVal;
}
/*****************************************************************************/
/* Substitute of fprintf() to print to one or more of the 3 output units:    */
/*   L_DIM: DIM Default Logger                                               */
/*   L_STD: standard error stream, stderr                                    */
/*   L_SYS: Linux syslog facility                                            */
/* Nothing prepended                                                         */
/* Arguments:                                                                */
/*   out: bitwise or of L_DIM, L_STD, L_SYS (e.g.: L_DIM|L_STD|L_SYS)        */
/*   tryN: number of write trials                                            */
/*   format: like printf                                                     */
/*   ...: like printf                                                        */
/* Return value:                                                             */
/*    rVal & 0x0000ffff: number of write failures to L_DIM                   */
/*   (rVal & 0xffff0000) >> 16: number of write failures to L_STD            */
/*****************************************************************************/
int rPrintf(int out,int tryN,const char *format,...)
{
  char rawMsg[MSG_SZ]="";
  int rawMsgLen=0;
  va_list ap;
  char *p;
  int rVal=0;
  /*-------------------------------------------------------------------------*/
  /* compose raw message */
  va_start(ap,format);
  vsnprintf(rawMsg,MSG_SZ,format,ap);        /* truncate to MSG_SZ if longer */
  va_end(ap);
  /* NULL-terminate if truncated */
  if(!memchr(rawMsg,0,MSG_SZ))rawMsg[MSG_SZ-1]='\0';
  /* replace CR with spaces */
  for(p=rawMsg;*p;p++)if(*p=='\r')*p=' ';
  /* add last LF if missed */
  rawMsgLen=strlen(rawMsg);
  if(rawMsg[rawMsgLen-1]!='\n')
  {
    if(rawMsgLen<MSG_SZ-1)strcat(rawMsg,"\n");
    else rawMsg[rawMsgLen-1]='\n';
  }
  /*-------------------------------------------------------------------------*/
  if(out&L_DIM && dfltFifoFD!=-1)
  {
    /* Split message in message lines and message line in message segments */
    /* A message line terminates with a \n in the original message */
    /* Message segment must fit in FIFO_MSG_SZ for atomic write */
    int droppedN=0;
    char msgSeg[FIFO_MSG_SZ]="";                     /* message line segment */
    /* message line segment available length for text (excluding \n\0) */
    int msgSegAvailLen=FIFO_MSG_SZ-2;
    char *lp;                                        /* message line pointer */
    int ll;                                           /* message line length */
    int tryC;                                                 /* try counter */
    struct timespec delay={0,1000000};                            /* 0.001 s */
    for(lp=rawMsg;*lp;lp=1+strchr(lp,'\n'))               /* loop over lines */
    {
      char *sp;                              /* message line segment pointer */
      int sl;                                 /* message line segment length */
      ll=strcspn(lp,"\n");
      for(sp=lp;sp<lp+ll;sp+=(msgSegAvailLen))    /* loop over line segments */
      {
        sl=strcspn(sp,"\n");
        if(sl>msgSegAvailLen)sl=msgSegAvailLen;
        /* compose header + message segment + \n */
        msgSeg[0]='\0';
        strncat(msgSeg,sp,sl);
        strcat(msgSeg,"\n");
        for(tryC=0;tryC<tryN;tryC++)
        {
          int written;
          written=write(dfltFifoFD,msgSeg,strlen(msgSeg));
          if(written!=-1||errno!=EAGAIN)break;
          nanosleep(&delay,NULL);
          delay.tv_sec*=2;
          delay.tv_nsec*=2;
          if(delay.tv_nsec>999999999)
          {
            delay.tv_sec+=1;
            delay.tv_nsec-=1000000000;
          }
        }
        if(tryC==tryN)droppedN++;
      }
    }
    if(droppedN>0xffff)droppedN=0xffff;
    rVal|=droppedN;
  }
  /*-------------------------------------------------------------------------*/
  if(out&L_STD || out&L_STD_C)
  {
    if(write(STDERR_FILENO,rawMsg,strlen(rawMsg))==-1)rVal|=0x00010000;
  }
  /*-------------------------------------------------------------------------*/
  if(out&L_SYS)
  {
    /* Split message in message lines and message line in message segments */
    /* A message line terminates with a \n in the original message */
    /* A message segment must fit in SYSLOG_MSG_SZ */
    char msgSeg[SYSLOG_MSG_SZ]="";                   /* message line segment */
    /* message line segment available length for text (excluding \n\0) */
    int msgSegAvailLen=SYSLOG_MSG_SZ-2;
    char *lp;                                        /* message line pointer */
    int ll;                                           /* message line length */
    for(lp=rawMsg;*lp;lp=1+strchr(lp,'\n'))               /* loop over lines */
    {
      char *sp;                              /* message line segment pointer */
      int sl;                                 /* message line segment length */
      ll=strcspn(lp,"\n");
      for(sp=lp;sp<lp+ll;sp+=(msgSegAvailLen))    /* loop over line segments */
      {
        sl=strcspn(sp,"\n");
        if(sl>msgSegAvailLen)sl=msgSegAvailLen;
        /* compose header + message segment + \n */
        msgSeg[0]='\0';
        strncat(msgSeg,sp,sl);
        strcat(msgSeg,"\n");
        syslog(LOG_INFO|LOG_DAEMON,msgSeg);
      }
    }
  }
  /*-------------------------------------------------------------------------*/
  return rVal;
}
/*****************************************************************************/
/* Substitute of fprintf() to print to the DIM Logger running on the FIFO    */
/* with file descriptor fifoFD.                                              */
/* Time, severity, hostname, process name, UTGID and function name prepended */
/* Arguments:                                                                */
/*   fifoFD: file descriptor of output fifo                                  */
/*   severity: VERB, DEBUG, INFO, WARN, ERROR or FATAL                       */
/*   fName: typically __func__ magic variable                                */
/*   tryN: number of write trials                                            */
/*     if tryN==0 than tryN=1 for VERB, 2 for DEBUG, 3 for INFO,...          */
/*   format: like printf                                                     */
/*   ...: like printf                                                        */
/* Return value:                                                             */
/*    Number of write failures to the fifo fifoFD                            */
/*****************************************************************************/
int mfPrintf(int fifoFD,int severity,const char* fName,int tryN,
             const char *format,...)
{
  char *sl[7]={"[OK]","[VERB] ","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
               "[FATAL]"};
  time_t now;
  struct tm lNow;
  char rawMsg[MSG_SZ]="";
  int rawMsgLen=0;
  char sNow[13];                                           /* Oct12-134923\0 */
  va_list ap;
  char msgHead[MSG_HEAD_SZ]="";                            /* message header */
  int msgHeadLen=0;                                 /* message header length */
  char *p;
  int droppedN=0;
  char msgSeg[FIFO_MSG_SZ]="";                       /* message line segment */
  /* message line segment available length for text (excluding \n\0) */
  int msgSegAvailLen;
  char *lp;                                          /* message line pointer */
  int ll;                                             /* message line length */
  int tryC;                                                   /* try counter */
  struct timespec delay={0,1000000};                              /* 0.001 s */
  /*-------------------------------------------------------------------------*/
  /* if tryN==0 than tryN=1 for VERB, 2 for DEBUG, 3 for INFO, ... */
  if(!tryN)tryN=severity;
  /*-------------------------------------------------------------------------*/
  /* compose raw message */
  va_start(ap,format);
  vsnprintf(rawMsg,MSG_SZ,format,ap);        /* truncate to MSG_SZ if longer */
  va_end(ap);
  /* NULL-terminate if truncated */
  if(!memchr(rawMsg,0,MSG_SZ))rawMsg[MSG_SZ-1]='\0';
  /* replace CR with spaces */
  for(p=rawMsg;*p;p++)if(*p=='\r')*p=' ';
  /* add last LF if missed */
  rawMsgLen=strlen(rawMsg);
  if(rawMsg[rawMsgLen-1]!='\n')
  {
    if(rawMsgLen<MSG_SZ-1)strcat(rawMsg,"\n");
    else rawMsg[rawMsgLen-1]='\n';
  }
  /*-------------------------------------------------------------------------*/
  /* time string */
  now=time(NULL);
  localtime_r(&now,&lNow);
  strftime(sNow,13,"%b%d-%H%M%S",&lNow);
  /*-------------------------------------------------------------------------*/
  /* process name */
  if(!pName)getPName();
  /*-------------------------------------------------------------------------*/
  /* utgid */
  if(!utgid)utgid=getenv("UTGID");
  if(!utgid)utgid="no UTGID";
  /*-------------------------------------------------------------------------*/
  /* host name */
  if(hostName[0]=='\0')                        /* hostName still not defined */
  {
    gethostname(hostName,80);
    p=strchr(hostName,'.');
    if(p)*p='\0';
  }
  /*-------------------------------------------------------------------------*/
  /* compose header (truncate if longer than MSG_HEAD_SZ) */
  snprintf(msgHead,MSG_HEAD_SZ,"%s%s%s: %s(%s): %s(): ",sNow,sl[severity],
           hostName,pName,utgid,fName);
  /* add string terminator if missed */
  if(!memchr(msgHead,0,MSG_HEAD_SZ))msgHead[MSG_HEAD_SZ-1]='\0';
  msgHeadLen=strlen(msgHead);
  msgSegAvailLen=FIFO_MSG_SZ-msgHeadLen-2;
  /*-------------------------------------------------------------------------*/
  /* Split message in message lines and message line in message segments */
  /* A message line terminates with a \n in the original message */
  /* Message segment with header must fit in FIFO_MSG_SZ for atomic write */
  for(lp=rawMsg;*lp;lp=1+strchr(lp,'\n'))                 /* loop over lines */
  {
    char *sp;                                /* message line segment pointer */
    int sl;                                   /* message line segment length */
    ll=strcspn(lp,"\n");
    for(sp=lp;sp<lp+ll;sp+=(msgSegAvailLen))      /* loop over line segments */
    {
      sl=strcspn(sp,"\n");
      if(sl>msgSegAvailLen)sl=msgSegAvailLen;
      /* compose header + message segment + \n */
      strcpy(msgSeg,msgHead);
      strncat(msgSeg,sp,sl);
      strcat(msgSeg,"\n");
      for(tryC=0;tryC<tryN;tryC++)
      {
        int written;
        written=write(fifoFD,msgSeg,strlen(msgSeg));
        if(written!=-1||errno!=EAGAIN)break;
        nanosleep(&delay,NULL);
        delay.tv_sec*=2;
        delay.tv_nsec*=2;
        if(delay.tv_nsec>999999999)
        {
          delay.tv_sec+=1;
          delay.tv_nsec-=1000000000;
        }
      }
      if(tryC==tryN)droppedN++;
    }
  }
  /*-------------------------------------------------------------------------*/
  return droppedN;
}
/*****************************************************************************/
/* Substitute of fprintf() to print to the DIM Logger running on the FIFO    */
/* with file descriptor fifoFD.                                              */
/* Nothing prepended.                                                        */
/* Arguments:                                                                */
/*   fifoFD: file descriptor of output fifo                                  */
/*   tryN: number of write trials                                            */
/*   format: like printf                                                     */
/*   ...: like printf                                                        */
/* Return value:                                                             */
/*    Number of write failures to the fifo fifoFD                            */
/*****************************************************************************/
int rfPrintf(int fifoFD,int tryN,const char *format,...)
{
  char rawMsg[MSG_SZ]="";
  int rawMsgLen=0;
  va_list ap;
  char *p;
  int droppedN=0;
  char msgSeg[FIFO_MSG_SZ]="";                       /* message line segment */
  /* message line segment available length for text (excluding \n\0) */
  int msgSegAvailLen;
  char *lp;                                          /* message line pointer */
  int ll;                                             /* message line length */
  int tryC;                                                   /* try counter */
  struct timespec delay={0,1000000};                              /* 0.001 s */
  /*-------------------------------------------------------------------------*/
  /* compose raw message */
  va_start(ap,format);
  vsnprintf(rawMsg,MSG_SZ,format,ap);        /* truncate to MSG_SZ if longer */
  va_end(ap);
  /* NULL-terminate if truncated */
  if(!memchr(rawMsg,0,MSG_SZ))rawMsg[MSG_SZ-1]='\0';
  /* replace CR with spaces */
  for(p=rawMsg;*p;p++)if(*p=='\r')*p=' ';
  /* add last LF if missed */
  rawMsgLen=strlen(rawMsg);
  if(rawMsg[rawMsgLen-1]!='\n')
  {
    if(rawMsgLen<MSG_SZ-1)strcat(rawMsg,"\n");
    else rawMsg[rawMsgLen-1]='\n';
  }
  /*-------------------------------------------------------------------------*/
  msgSegAvailLen=FIFO_MSG_SZ-2;
  /*-------------------------------------------------------------------------*/
  /* Split message in message lines and message line in message segments */
  /* A message line terminates with a \n in the original message */
  /* Message segment with header must fit in FIFO_MSG_SZ for atomic write */
  for(lp=rawMsg;*lp;lp=1+strchr(lp,'\n'))                 /* loop over lines */
  {
    char *sp;                                /* message line segment pointer */
    int sl;                                   /* message line segment length */
    ll=strcspn(lp,"\n");
    for(sp=lp;sp<lp+ll;sp+=(msgSegAvailLen))      /* loop over line segments */
    {
      sl=strcspn(sp,"\n");
      if(sl>msgSegAvailLen)sl=msgSegAvailLen;
      /* compose header + message segment + \n */
      msgSeg[0]='\0';
      strncat(msgSeg,sp,sl);
      strcat(msgSeg,"\n");
      for(tryC=0;tryC<tryN;tryC++)
      {
        int written;
        written=write(fifoFD,msgSeg,strlen(msgSeg));
        if(written!=-1||errno!=EAGAIN)break;
        nanosleep(&delay,NULL);
        delay.tv_sec*=2;
        delay.tv_nsec*=2;
        if(delay.tv_nsec>999999999)
        {
          delay.tv_sec+=1;
          delay.tv_nsec-=1000000000;
        }
      }
      if(tryC==tryN)droppedN++;
    }
  }
  /*-------------------------------------------------------------------------*/
  return droppedN;
}
/* ######################################################################### */
/* Private functions */
/*****************************************************************************/
/* send a message to the DIM logger */
int dimLoggerMsgSend(char *buf,int fifoFD)
{
  if(fifoFD==-1)return -1;
  return write(fifoFD,buf,strlen(buf));
}
/*****************************************************************************/
/* send a message to the stderr */
ssize_t stdErrMsgSend(char *buf)
{
  return write(STDERR_FILENO,buf,strlen(buf));
}
/*****************************************************************************/
/* send a message to the syslog */
/* max syslog message length = SYSLOG_MSG_SZ */
int sysLogMsgSend(char *buf,int severity)
{
  if(strlen(buf)<SYSLOG_MSG_SZ)
  {
    if(severity==DEBUG)syslog(LOG_DEBUG|LOG_DAEMON,buf);
    else if(severity==INFO)syslog(LOG_INFO|LOG_DAEMON,buf);
    else if(severity==WARN)syslog(LOG_WARNING|LOG_DAEMON,buf);
    else if(severity==ERROR)syslog(LOG_ERR|LOG_DAEMON,buf);
    else if(severity==FATAL)syslog(LOG_CRIT|LOG_DAEMON,buf);
    else syslog(LOG_INFO|LOG_DAEMON,buf);
  }
  else                     /* message is longer than SYSLOG_MSG_SZ. split it */
  {
    char pBuf[SYSLOG_MSG_SZ]="";
    char *p=NULL;
    for(p=buf;p-buf<strlen(buf);p+=(SYSLOG_MSG_SZ-1))
    {
      memset(pBuf,0,SYSLOG_MSG_SZ);
      strncpy(pBuf,p,SYSLOG_MSG_SZ-1);
      if(!memchr(pBuf,0,SYSLOG_MSG_SZ))buf[SYSLOG_MSG_SZ-1]='\0';
      if(severity==DEBUG)syslog(LOG_DEBUG|LOG_DAEMON,pBuf);
      else if(severity==INFO)syslog(LOG_INFO|LOG_DAEMON,pBuf);
      else if(severity==WARN)syslog(LOG_WARNING|LOG_DAEMON,pBuf);
      else if(severity==ERROR)syslog(LOG_ERR|LOG_DAEMON,pBuf);
      else if(severity==FATAL)syslog(LOG_CRIT|LOG_DAEMON,pBuf);
      else syslog(LOG_INFO|LOG_DAEMON,pBuf);
    }
  }
  return 0;
}
/*****************************************************************************/
/* get the name of the current executable file */
void getPName()
{
  char pathName[PATH_LEN+1]="";
  int len=0;
  char buf[FIFO_MSG_SZ]="";
  /*-------------------------------------------------------------------------*/
  len=readlink("/proc/self/exe",pathName,sizeof(pathName));
  if(len==-1)
  {
    snprintf(buf,FIFO_MSG_SZ,"[FATAL] %s: Cannot get the name of the "
             "executable image of the current running process: "
             "readlink(/proc/self/exe): %s! Probably this is a problem of "
             "owner/permission of the executable file!",
             __func__,strerror(errno));
    syslog(LOG_ERR|LOG_DAEMON,buf);
    fprintf(stderr,"%s\n",buf);
    exit(1);
  }
  if(len<PATH_LEN)pathName[len]='\0';
  else pathName[PATH_LEN]='\0';
  pName=strdup(basename(pathName));
  return;
}
/* ######################################################################### */
/* Deprecated functions. Only for back-portability. Will be suppressed.      */
/*****************************************************************************/
/* Deprecated. Only for back-portability. Will be suppressed.                */
int msgSend(char *fName,int severity,char *rawMsg)
{
  char *sl[7]={"[OK]","[VERB] ","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
               "[FATAL]"};
  time_t now;
  struct tm lNow;
  static char msg[FIFO_MSG_SZ]="";
  char sNow[13];                                           /* Oct12-134923\0 */
  /*-------------------------------------------------------------------------*/
  /* NULL-terminate if truncated */
  if(!memchr(rawMsg,0,FIFO_MSG_SZ))rawMsg[FIFO_MSG_SZ-1]='\0';
  /*-------------------------------------------------------------------------*/
  /* time string */
  now=time(NULL);
  localtime_r(&now,&lNow);
  strftime(sNow,13,"%b%d-%H%M%S",&lNow);
  /*-------------------------------------------------------------------------*/
  /* process name */
  if(!pName)getPName();
  /*-------------------------------------------------------------------------*/
  /* utgid */
  if(!utgid)utgid=getenv("UTGID");
  if(!utgid)utgid="no UTGID";
  /*-------------------------------------------------------------------------*/
  /* host name */
  if(hostName[0]=='\0')                        /* hostName still not defined */
  {
    char *p;
    gethostname(hostName,80);
    p=strchr(hostName,'.');
    if(p)*p='\0';
  }
  /*-------------------------------------------------------------------------*/
  /* compose message string with header */
  snprintf(msg,FIFO_MSG_SZ,"%s%s%s: %s(%s): %s: %s\n",sNow,sl[severity],
           hostName,pName,utgid,fName,rawMsg);
  /*-------------------------------------------------------------------------*/
  /* add string terminator if missed */
  if(!memchr(msg,0,FIFO_MSG_SZ))msg[FIFO_MSG_SZ-1]='\0';
  /* add newline if missed */
  if(!strchr(msg,'\n'))
  {
    int msgLen=strlen(msg);
    if(msgLen<FIFO_MSG_SZ-1)strcat(msg,"\n");
    else msg[msgLen-1]='\n';
  }
  /*-------------------------------------------------------------------------*/
  if(lType&L_DIM)dimLoggerMsgSend(msg,dfltFifoFD);
  if(lType&L_STD)stdErrMsgSend(msg);
  if(lType&L_SYS)sysLogMsgSend(msg,severity);
  return 0;
}
/*****************************************************************************/
/* Deprecated. Only for back-portability. Will be suppressed.                */
/* If loggerType & L_DIM != 0 open the (existing) fifo dfltFifoPath          */
/* for writing.                                                              */
/* loggerType & L_DIM != 0: send messages to fifo dfltFifoPath.              */
/* loggerType & L_STD != 0: send messages to stderr.                         */
/* loggerType & L_SYS != 0: send messages to syslog.                         */
/* Set the global variable hostName (current host name).                     */
/* Set the global variable pName (current application name).                 */
/* Set the global variable lType = loggerType.                               */
/*****************************************************************************/
int msgInit(int loggerType,char *srvName)
{
  lType=loggerType;
  if((lType&(L_DIM|L_STD|L_SYS))==0)lType=L_STD|L_SYS;
  /*-------------------------------------------------------------------------*/
  /* process name */
  if(!pName)getPName();
  /*-------------------------------------------------------------------------*/
  /* utgid */
  if(!utgid)utgid=getenv("UTGID");
  if(!utgid)utgid="no UTGID";
  /*-------------------------------------------------------------------------*/
  /* host name */
  if(hostName[0]=='\0')                        /* hostName still not defined */
  {
    char *p;
    gethostname(hostName,80);
    p=strchr(hostName,'.');
    if(p)*p='\0';
  }
  /*-------------------------------------------------------------------------*/
  if(lType&L_DIM)return dfltLoggerOpen(10,0,ERROR,INFO,0);
  else return 0;
}
/*****************************************************************************/
/* Deprecated. Only for back-portability. Will be suppressed.                */
void msgTerm()
{
  dfltLoggerClose();
  return;
}
/*****************************************************************************/
/* Deprecated. Only for back-portability. Will be suppressed.                */
int msgGetLogFD(void)
{
  return getDfltFifoFD();
}
/*****************************************************************************/
char *getFmcMsgUtilsVersion()
{
  return msgUtilsRcsid;
}
/* ######################################################################### */
