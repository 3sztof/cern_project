/*****************************************************************************/
/*
 * $Log: mPrint.C,v $
 * Revision 1.32  2012/12/13 16:17:56  galli
 * minor changes in man page references
 *
 * Revision 1.30  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.29  2009/01/29 11:15:45  galli
 * minor changes
 *
 * Revision 1.28  2009/01/22 10:17:26  galli
 * do not depend on kernel-devel headers
 *
 * Revision 1.13  2008/08/21 13:32:03  galli
 * groff manual
 *
 * Revision 1.9  2008/08/16 08:06:27  galli
 * first working version
 *
 * Revision 1.1  2008/08/14 12:40:47  galli
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>                                             /* strtol(3) */
#include <unistd.h>                                            /* getppid(2) */
#include <fcntl.h>                                      /* open(2), O_RDONLY */
#include <time.h>                       /* time(), localtime_r(), strftime() */
#include <errno.h>
#include <syslog.h>                                              /* syslog() */
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcMsgUtils.h"           /* mPrintf(), dfltLoggerOpen(), rPrintf() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcRef.h"                                  /* FMC_URLS FMC_AUTHORS */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcMacro.h"     /* eExit(), likely(), unlikely(), printOutFuncOK() */
#include "fmcVersion.h"                                       /* FMC version */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*---------------------------------------------------------------------------*/
#define PIPE_NAME_SIZE 128
#define DEF_FIFO_PATH "/tmp/logSrv.fifo"
#define PATH_BUF_LEN 1023
/* Maximum number of characters read from procfs files in one shot. */
/* Linux kernel 2.6.18 writes max 1024*3=3072 Bytes at a time */
#define PROC_MAX_READ_CHARS 4096
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
char *getCPnameFromPid(pid_t pid);
static char **file2strvec(const char* path);
void *realloc1(void *oldp,size_t size);
/*****************************************************************************/
/* scripting interpreters */
const char *siList[]={"bash","python","java","sh","zsh","ksh","perl","csh",
                      "tcsh","tcl"};
/* severity levels */
const char *sl[7]={"[OK]","[VERB] ","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
                   "[FATAL]"};
static char rcsid[]="$Id: mPrint.C,v 1.32 2012/12/13 16:17:56 galli Exp galli $";
char *rcsidP;
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i;
  /* pointers */
  char *p;
  /* getopt */
  int flag=0;
  /* cmd-line options */
  int outU=0;
  int noDrop=0;
  int retryN=0;
  severity sev=VERB;
  char pipePath[PIPE_NAME_SIZE]=DEF_FIFO_PATH;
  char *cpName=NULL;
  char *fName=NULL;
  char *utgid=NULL;
  /* time */
  time_t now;
  struct tm lNow;
  char sNow[13];
  /* message building */
  pid_t ppid;
  char hostName[80]="";
  char rawMsg[MSG_SZ]="";
  int rawMsgLen=0;
  char msgHeadDim[MSG_HEAD_SZ]="";     /* message header for L_DIM and L_STD */
  int msgHeadDimLen=0;          /* message header length for L_DIM and L_STD */
  char msgHeadSys[MSG_HEAD_SZ]="";               /* message header for L_SYS */
  int msgHeadSysLen=0;                    /* message header length for L_SYS */
  /* fifo */
  int fifoFD=-1;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt(argc,argv,"h::l:s:p:An:P:f:u:"))!=EOF)
  {
    char *p=NULL;
    switch(flag)
    {
      case 'l':
        if(!strncmp(optarg,"STD",3))outU|=L_STD;
        else if(!strncmp(optarg,"SYS",3))outU|=L_SYS;
        else if(!strncmp(optarg,"FMC",3))outU|=L_DIM;
        else
        {
          fprintf(stderr,"\ngetopt(): invalid OUTPUT_UNIT: \"%s\"!\n",optarg);
          shortUsage();
        }
        break;
      case 's':
        sev=(severity)strtol(optarg,&p,0);
        if(!p||!*p)break;
        if(p==optarg)
        {
          if(!strncasecmp(optarg,"VERB",4))sev=VERB;
          else if(!strncasecmp(optarg,"DEBUG",5))sev=DEBUG;
          else if(!strncasecmp(optarg,"INFO",4))sev=INFO;
          else if(!strncasecmp(optarg,"WARN",4))sev=WARN;
          else if(!strncasecmp(optarg,"ERROR",5))sev=ERROR;
          else if(!strncasecmp(optarg,"FATAL",5))sev=FATAL;
          else
          {
            fprintf(stderr,"\ngetopt(): invalid SEVERITY: \"%s\"!\n",optarg);
            shortUsage();
          }
        }
        else
        {
          fprintf(stderr,"\ngetopt(): invalid SEVERITY: \"%s\"!\n",optarg);
          shortUsage();
        }
        break;
      case 'p':
        snprintf(pipePath,PIPE_NAME_SIZE,"%s",optarg);
        break;
      case 'A':
        noDrop=1;
        break;
      case 'n':
        retryN=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'P':
        cpName=optarg;
        break;
      case 'f':
        fName=optarg;
        break;
      case 'u':
        utgid=optarg;
        break;
      case 'h':
        if(!optarg)usage(0);                  /* -h: opne manual using less */
        else usage(1);                        /* -hh: print groff to stdout */
      default:
        fprintf(stderr,"\ngetopt(): invalid option: \"-%c\"!\n",optopt);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* check non-option arguments */
  if(optind!=argc-1)
  {
    printf("\nOne and only one MESSAGE required as non-option argument!\n");
    shortUsage();
  }
  /*-------------------------------------------------------------------------*/
  /* check cmd-line opts */
  if(sev<VERB)sev=VERB;
  if(sev>FATAL)sev=FATAL;
  /*-------------------------------------------------------------------------*/
  /* defaults */
  if(!outU)outU=L_DIM;
  if(!retryN)retryN=sev;
  /*-------------------------------------------------------------------------*/
  /* open output unit */
  if(outU&L_DIM)
  {
    fifoFD=loggerOpen(pipePath,noDrop,L_STD,ERROR);
    if(fifoFD==-1)
    {
      fprintf(stderr,"Can't open FIFO \"%s\"!",pipePath);
      return 1;
    }
  }                                                        /* if(outU&L_DIM) */
  /*-------------------------------------------------------------------------*/
  /* compose raw message */
  snprintf(rawMsg,MSG_SZ,"%s",argv[argc-1]);
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
  /* calling process name */
  ppid=getppid();
  if(!cpName)cpName=getCPnameFromPid(ppid);
  if(!cpName)cpName=(char*)"unknown";
  /* utgid */
  if(!utgid)utgid=getenv("UTGID");
  if(!utgid)utgid=(char*)"no UTGID";
  /* host name */
  if(hostName[0]=='\0')                        /* hostName still not defined */
  {
    gethostname(hostName,80);
    p=strchr(hostName,'.');
    if(p)*p='\0';
  }
  /*-------------------------------------------------------------------------*/
  /* compose header for L_DIM and L_STD */
  /* (truncate if longer than MSG_HEAD_SZ) */
  if(!fName)
  {
    snprintf(msgHeadDim,MSG_HEAD_SZ,"%s%s%s: %s(%s): ",sNow,sl[sev],
             hostName,cpName,utgid);
  }
  else
  {
    snprintf(msgHeadDim,MSG_HEAD_SZ,"%s%s%s: %s(%s): %s(): ",sNow,sl[sev],
             hostName,cpName,utgid,fName);
  }
  /* add string terminator if missed */
  if(!memchr(msgHeadDim,0,MSG_HEAD_SZ))msgHeadDim[MSG_HEAD_SZ-1]='\0';
  msgHeadDimLen=strlen(msgHeadDim);
  /*-------------------------------------------------------------------------*/
  /* compose head for L_SYS */
  /* (truncate if longer than MSG_HEAD_SZ) */
  if(!fName)
  {
    snprintf(msgHeadSys,MSG_HEAD_SZ,"%s %s(%s): ",sl[sev],cpName,utgid);
  }
  else
  {
    snprintf(msgHeadSys,MSG_HEAD_SZ,"%s %s(%s): %s(): ",sl[sev],cpName,utgid,
             fName);
  }
  /* add string terminator if missed */
  if(!memchr(msgHeadSys,0,MSG_HEAD_SZ))msgHeadSys[MSG_HEAD_SZ-1]='\0';
  msgHeadSysLen=strlen(msgHeadSys);
  /*-------------------------------------------------------------------------*/
  /* print message to output unit */
  if(outU&L_DIM)
  {
    rfPrintf(fifoFD,sev,"%s%s",msgHeadDim,rawMsg);
  }
  if(outU&L_STD)
  {
    char msg[MSG_HEAD_SZ+MSG_SZ];                         /* msgHead1+rawMsg */
    /* compose raw message string with header */
    snprintf(msg,MSG_SZ+MSG_HEAD_SZ,"%s%s",msgHeadDim,rawMsg);
    write(STDERR_FILENO,msg,strlen(msg));
  }
  if(outU&L_SYS)
  {
    /* Split message in message lines and message line in message segments */
    /* A message line terminates with a \n in the original message */
    /* A message segment with header must fit in SYSLOG_MSG_SZ */
    char msgSeg[SYSLOG_MSG_SZ]="";                   /* message line segment */
    /* message line segment available length for text (excluding \n\0) */
    int msgSegAvailLen=SYSLOG_MSG_SZ-msgHeadSysLen-2;
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
        strcpy(msgSeg,msgHeadSys);
        strncat(msgSeg,sp,sl);
        strcat(msgSeg,"\n");
        if(sev<=DEBUG)syslog(LOG_DEBUG|LOG_DAEMON,msgSeg);
        else if(sev==INFO)syslog(LOG_INFO|LOG_DAEMON,msgSeg);
        else if(sev==WARN)syslog(LOG_WARNING|LOG_DAEMON,msgSeg);
        else if(sev==ERROR)syslog(LOG_ERR|LOG_DAEMON,msgSeg);
        else if(sev==FATAL)syslog(LOG_CRIT|LOG_DAEMON,msgSeg);
        else syslog(LOG_INFO|LOG_DAEMON,msgSeg);
      }
    }
  }
  /*-------------------------------------------------------------------------*/
  /* close output unit */
  if(outU&L_DIM)
  {
    loggerClose(fifoFD);
  }
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
/* get the name of the calling program */
char *getCPnameFromPid(pid_t pid)
{
  char exePath[PATH_BUF_LEN+1]="";
  char cmdlinePath[PATH_BUF_LEN+1]="";
  char pathName[PATH_BUF_LEN+1]="";
  int len=0;
  char *cpName=NULL;
  char **cmdline=NULL;
  int isSi=0;
  unsigned i;
  /*-------------------------------------------------------------------------*/
  snprintf(exePath,PATH_BUF_LEN+1,"/proc/%u/exe",pid);
  snprintf(cmdlinePath,PATH_BUF_LEN+1,"/proc/%u/cmdline",pid);
  len=readlink(exePath,pathName,sizeof(pathName));
  if(len==-1)
  {
    fprintf(stderr,"[ERROR] %s: readlink(): %s!\n",__func__,strerror(errno));
    return NULL;
  }
  if(len<PATH_BUF_LEN)pathName[len]='\0';
  else pathName[PATH_BUF_LEN]='\0';
  cpName=strdup(basename(pathName));
  /* if cpName is a scrip interpreter */
  for(i=0;i<sizeof(siList)/sizeof(char*);i++)
  {
    if(!strcmp(cpName,siList[i]))isSi=1;
  }
  if(isSi)
  {
    /* get the script name */
    int i;
    char *p;
    char **pp;
    /* get the complete cmdline */
    cmdline=file2strvec(cmdlinePath);
    if(cmdline)
    {
      /* find cmdline argument number */
      for(pp=cmdline,i=0;*pp;pp++,i++){}
      /* get the last argument */
      cpName=cmdline[i-1];
      /* get the basename */
      p=strrchr(cpName,'/');
      if(p && *(p+1))cpName=p+1;
    }
  }
  return cpName;
}
/*****************************************************************************/
static char **file2strvec(const char* path)
{
  /* Read maximum PROC_MAX_READ_CHARS bytes at a time */
  /* kernel 2.6.18 writes max 1024*3=3072 Bytes at a time */
  static char sRBuf[PROC_MAX_READ_CHARS];           /* buffer for one read() */
  char *tRBuf=NULL;                                     /* buffer for a file */
  int tRBufSz=0;                                            /* size of tRBuf */
  char *p;
  char *endbuf;
  char **q;
  char **ret;
  int fd;
  ssize_t n;
  int c;
  int align;
  /*-------------------------------------------------------------------------*/
  /* read the whole file into a memory buffer, allocating memory as we go */
  fd=open(path,O_RDONLY,0);
  if(unlikely(fd==-1))
  {
    fprintf(stderr,"[ERROR] %s: Cannot open file \"%s\": %s!",__func__,path,
            strerror(errno));
    return NULL;
  }
  /*-------------------------------------------------------------------------*/
  for(;;)
  {
    /*.......................................................................*/
    n=read(fd,sRBuf,sizeof sRBuf);                       /* I expect n<=3072 */
    /*.......................................................................*/
    if(unlikely(n<0))                                          /* read error */
    {
      if(errno==EINTR)                            /* Interrupted system call */
      {
        continue;
      }
      else                                              /* severe read error */
      {
        fprintf(stderr,"[ERROR] %s: Error reading file \"%s\": %s!",__func__,
                path,strerror(errno));
        close(fd);
        if(tRBuf)free(tRBuf);
        return NULL;
      }                                              /* if severe read error */
    }                                                       /* if read error */
    /*.......................................................................*/
    if(!n)                                  /* zero Bytes read the last time */
    {
      if(unlikely(!tRBuf))                          /* zero Total Bytes read */
      {
         fprintf(stderr,"[ERROR] %s: Zero Bytes read in file: \"%s\"!",
                 __func__,path);
        close(fd);
        if(tRBuf)free(tRBuf);
        return NULL;
      }
      else                                                            /* EOF */
      {
        if(tRBuf[tRBufSz-1]!=0)                          /* truncated string */
        {
          tRBuf=(char*)realloc1(tRBuf,tRBufSz+1);   /* alloc 1 B more for \0 */
          tRBuf[tRBufSz]='\0';
          tRBufSz++;
        }
        break;
      }
    }                                    /* if zero Bytes read the last time */
    /*.......................................................................*/
    tRBuf=(char*)realloc1(tRBuf,tRBufSz+n);          /* allocate more memory */
    memcpy(tRBuf+tRBufSz,sRBuf,n);                    /* copy buffer into it */
    tRBufSz+=n;                                  /* increment total byte ctr */
  }                                                               /* for(;;) */
  /*-------------------------------------------------------------------------*/
  close(fd);
  /*-------------------------------------------------------------------------*/
  endbuf=tRBuf+tRBufSz;
  align=(sizeof(char*)-1)-((tRBufSz+sizeof(char*)-1)&(sizeof(char*)-1));
  /* one string pointer for each string */
  for(c=0,p=tRBuf;p<endbuf;p++)if(!*p)c+=sizeof(char*);
  c+=sizeof(char*);    /* one extra string pointer for the last NULL pointer */
  /* make room for pointers at the end of tRBuf */
  tRBuf=(char*)realloc1(tRBuf,tRBufSz+c+align);
  endbuf=tRBuf+tRBufSz;
  q=ret=(char**)(endbuf+align);              /* use free(*ret) to deallocate */
  *q++=p=tRBuf;                             /* point pointers to the strings */
  endbuf--;                                     /* do not traverse final NUL */
  /* '\0' char implies that next string starts at the next character */
  while(++p<endbuf)if(!*p)*q++=p+1;
  *q=0;                                      /* NULL pointer list terminator */
  return ret;
}
/*****************************************************************************/
void *realloc1(void *oldp,size_t size)
{
  void *p;
  /*-------------------------------------------------------------------------*/
  if(size==0)size++;
  p=realloc(oldp,size);
  if(!p)
  {
    fprintf(stderr,"[ERROR] %s: realloc(%zu) failed: %s!",__func__,size,
            strerror(errno));
    exit(1);
  }
  return(p);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"mPrint [-l OUTPUT_UNIT ...] [-s SEVERITY] [-p FIFO_PATH] [-A]\n"
"       [-n RETRY_NUMBER] [-P PROG_NAME] [-f FUNC_NAME] [-u UTGID]\n"
"       MESSAGE\n"
"mPrint -h\n"
"\n"
"OUTPUT_UNIT = FMC | STD | DIM\n"
"SEVERITY    = VERB | DEBUG | INFO | WARN | ERROR | FATAL\n"
"\n"
"Try \"mPrint -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  fprintf(stderr,"\n%s\n",shortUsageStr);
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
"mPrint.man\n"
"\n"
"..\n"
"%s"
".hw MESSAGE welcome\n"
".TH mPrint 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis mPrint\\ \\-\n"
"Send a message to the FMC Message Logger Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis mPrint\n"
".ShortOpt[] l OUTPUT_UNIT...\n"
".ShortOpt[] s SEVERITY\n"
".ShortOpt[] p FIFO_PATH\n"
".ShortOpt[] A\n"
".ShortOpt[] n RETRY_NUMBER\n"
".ShortOpt[] P PROG_NAME\n"
".ShortOpt[] f FUNC_NAME\n"
".ShortOpt[] u UTGID\n"
"\\fIMESSAGE\\fP\n"
".EndSynopsis\n"
".sp\n"
".Synopsis mPrint\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH INTRODUCTION\n"
".\n"
"The \\fBFMC Message Logger\\fP is a tool to \\fIcollect\\fP, \\fImanage\\fP "
"(i.e. merge, duplicate, filter, suppress duplicates) and \\fIdisplay\\fP on "
"a central console the messages sent by the applications running on the farm "
"nodes. It uses \\fIDIM\\fP to move messages throughout the network and a "
"POSIX.1 \\fIFIFO\\fP (alias \\fInamed pipe\\fP) on each node as local buffer "
"to collect message sent by the application running on the same node.\n"
".PP\n"
"The FMC Message Logger is a \\fIgeneral purpose\\fP utility, not tailored "
"on a specific application; it can even collect messages sent by the "
"applications to their \\fIstandard output\\fP and \\fIerror\\fP streams by "
"\\fBredirecting the streams to the FIFO\\fP at the application start-up "
"(the FMC Task Manager provides a flag in the \\fBtmStart\\fP(1) command to "
"activate such a redirection).\n"
".PP\n"
"The \\fBFMC Message Logger\\fP consists in the server \\fBlogSrv\\fP(8), "
"which gets messages from a local FIFO and publish them using DIM, in the "
"client \\fBlogViewer\\fP(1), which can display the messages got using DIM or "
"forward them to another \\fBlogSrv\\fP(8), and in the clients "
"\\fBlogSetProperties\\fP(1) and \\fBlogGetProperties\\fP(1) which can change "
"and retrieve the \\fBlogSrv\\fP(8) settings at run-time.\n"
".PP\n"
"Besides, the \\fBFMC Message Logger\\fP provides several utilities to help "
"sending formatted messages with header to a the FMC Message Logger: the "
"command \\fBmPrint\\fP(1) and the library calls \\fBdfltLoggerOpen\\fP(3), "
"\\fBmPrintf\\fP(3), \\fBrPrintf\\fP(3), \\fBdfltLoggerClose\\fP(3), "
"\\fBgetDfltFifoFD\\fP(3), \\fBloggerOpen\\fP(3), \\fBmfPrintf\\fP(3), "
"\\fBrfPrintf\\fP(3) and \\fBloggerClose\\fP(3).\n"
".PP\n"
"Look at section \\fIWRITING TO THE FMC LOGGER\\fP in the \\fBlogSrv\\fP(8) "
"man page for a comprehensive review of the ways of sending messages to the "
"FMC Message Logger.\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"Send the message \\fIMESSAGE\\fP to the\n"
".B FMC Message Logger facility\n"
"(-l FMC) and/or to the\n"
".B standard error stream\n"
"(-l STD) and/or to the operating system\n"
".B syslog facility\n"
"(-l SYS).\n"
".P\n"
"The sent message is prefixed by a \\fBheader\\fP\n"
"including month (bbb), day (dd), time (HHMMSS), severity (SEVER), hostname\n"
"(host), command name (cmd), the FMC User-defined Thread Group\n"
"IDentifier (utgid), and the function name (fName), according to the\n"
"following format:\n"
".IP \"\" 3\n"
".B bbbdd-HHMMSS[SEVER]host: cmd(UTGID): fName(): message\n"
".PP\n"
"By default, \\fBcmd\\fP is the name of the \\fBcalling program\\fP or\n"
"\\fBscript\\fP and \\fBUTGID\\fP is got from the \\fBenvironment\\fP.\n"
".PP\n"
"See \\fBlogSrv\\fP(8) for other methods to send messages to the FMC Message "
"Logger.\n"
".\n"
".\n"
".SH OPTIONS\n"
".\n"
".OptDef h\n"
"Get basic usage help from the command line.\n"
".\n"
".OptDef l \"\" OUTPUT_UNIT (string, repeatable)\n"
"Add \\fIOUTPUT_UNIT\\fP to the list of the output units.\n"
"\\fIOUTPUT_UNIT\\fP can be:\n"
".RS\n"
".TP\n"
"\\fBFMC\\fP\nThe FMC Message Logger Server;\n"
".TP\n"
"\\fBSTD\\fP\nThe standard error stream;\n"
".TP\n"
"\\fBSYS\\fP\nThe OS syslog facility.\n"
".PP\n"
"\\fBDefault\\fP: \\fIOUTPUT_UNIT\\fP = \\fBFMC\\fP.\n"
".RE\n"
".\n"
".OptDef s \"\" SEVERITY (integer or string)\n"
"Set the message \\fBseverity\\fP to \\fISEVERITY\\fP.\n"
"\\fISEVERITY\\fP can be either\n"
"an integer in the range 1..6 or a string (\\fBVERB\\fP\\~=\\~1,\n"
"\\fBDEBUG\\fP\\~=\\~2,\n"
"\\fBINFO\\fP\\~=\\~3, \\fBWARN\\fP\\~=\\~4, \\fBERROR\\fP\\~=\\~5,\n"
"\\fBFATAL\\fP\\~=\\~6).\n"
"\\fBDefault\\fP: \\fISEVERITY\\fP\\~=\\~\\fBVERB\\fP\\~=\\~1.\n"
".\n"
".OptDef p \"\" FIFO_PATH (string)\n"
"If \\fIOUTPUT_UNIT\\fP list includes \\fBFMC\\fP then send messages to the\n"
"fifo (named pipe) \\fIFIFO_PATH\\fP. \\fBDefault\\fP: \"/tmp/logSrv.fifo\".\n"
".\n"
".OptDef A\n"
"Use the no-drop policy to write to the FIFO. \\fBDefault\\fP: use\n"
"the congestion-proof policy.\n"
".\n"
".OptDef n \"\" RETRY_NUMBER (integer)\n"
"Using the congestion-proof policy, if write fails retry for a\n"
"total of \\fIRETRY_NUMBER\\fP times. \\fBDefault\\fP: "
"\\fIRETRY_NUMBER\\fP\\~=\\~\\fISEVERITY\\fP.\n"
".\n"
".OptDef P \"\" PROG_NAME (string)\n"
"Force the calling program name to \\fIPROG_NAME\\fP. \\fBDefault\\fP: "
"Get the\n"
"calling program name from procfs. If the calling program is\n"
"a script, \\fIPROG_NAME\\fP is got from the last argument of the\n"
"script command line.\n"
".\n"
".OptDef f \"\" FUNC_NAME (string)\n"
"Specify \\fIFUNC_NAME\\fP as function name. \\fBDefault\\fP: "
"\\fIFUNC_NAME\\fP omitted.\n"
".\n"
".OptDef u \"\" UTGID (string)\n"
"Force the utgid to \\fIUTGID\\fP. \\fBDefault\\fP: utgid is got from the\n"
"environment.\n"
".\n"
".\n"
".SH ENVIRONMENT\n"
".TP\n"
".EnvVar LD_LIBRARY_PATH \\ (string,\\ mandatory\\ if\\ not\\ set\\ using\\ "
"ldconfig)\n"
"Must include the path to the library \"libFMCutils\".\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Print message with \\fBVERB\\fP severity to the default FMC logger:\n"
".\n"
".P\n"
".ShellCommand mPrint 'blah blah blah'\n"
".\n"
".P\n"
"Print message with \\fBWARN\\fP severity to the default FMC logger:\n"
".\n"
".P\n"
".ShellCommand mPrint -s WARN 'blah blah blah'\n"
".\n"
".P\n"
"Print message with \\fBWARN\\fP severity to the default FMC logger, to the\n"
"standard error stream and to the syslog facility:\n"
".\n"
".P\n"
".ShellCommand mPrint -s WARN -l FMC -l STD -l SYS 'blah blah blah'\n"
".\n"
".P\n"
"Print message with \\fBWARN\\fP severity to the gaudi FMC logger:\n"
".\n"
".P\n"
".ShellCommand mPrint -s WARN -p /tmp/logGaudi.fifo 'blah blah blah'\n"
".\n"
".P\n"
"Specify the function name:\n"
".\n"
".P\n"
".ShellCommand mPrint -s WARN -f main 'blah blah blah'\n"
".\n"
".P\n"
"Force the name of the calling program:\n"
".\n"
".P\n"
".ShellCommand mPrint -s WARN -P my_program 'blah blah blah'\n"
".\n"
".P\n"
"Force the UTGID:\n"
".\n"
".P\n"
".ShellCommand mPrint -s WARN -u my_utgid 'blah blah blah'\n"
".\n"
".P\n"
"From a bash script:\n"
".\n"
".P\n"
".RS\n"
".B #!/bin/bash\n"
".br\n"
".B /opt/FMC/bin/mPrint -s WARN 'blah blah blah'\n"
".RE\n"
".\n"
".P\n"
"From a python script:\n"
".\n"
".P\n"
".RS\n"
".B #!/usr/bin/env python\n"
".br\n"
".B import os\n"
".br\n"
".B os.system(\"/opt/FMC/bin/mPrint -s WARN 'blah blah blah'\")\n"
".RE\n"
"%s"                                                             /* FMC_URLS */
"%s"                                                          /* FMC_AUTHORS */
".\n"
".\n"
".SH VERSION\n"
".\n"
"Source:  %s\n"
".br\n"
"Package: FMC-%s\n"
".\n"
".\n"
".SH SEE ALSO\n"
".ad l\n"
".SS API for formatted writing to the default FMC Message Logger\n"
".BR \\%%dfltLoggerOpen (3),\n"
".BR \\%%getDfltFifoFD (3),\n"
".BR \\%%mPrintf (3),\n"
".BR \\%%rPrintf (3),\n"
".BR \\%%dfltLoggerClose (3).\n"
".SS API for formatted writing to an arbitrary FMC Message Logger\n"
".BR \\%%loggerOpen (3),\n"
".BR \\%%mfPrintf (3),\n"
".BR \\%%rfPrintf (3),\n"
".BR \\%%loggerClose (3).\n"
".SS FMC Message Logger Server and User Commands\n"
".BR \\%%logSrv (8),\n"
".BR \\%%logViewer (1),\n"
".BR \\%%logGetProperties (1),\n"
".BR \\%%logSetProperties (1).\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro,FMC_DATE,FMC_VERSION
,FMC_URLS,FMC_AUTHORS
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/*****************************************************************************/
