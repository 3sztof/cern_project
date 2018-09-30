/*****************************************************************************/
/*
 * $Log: logSrv.c,v $
 * Revision 3.34  2012/12/07 15:48:07  galli
 * Minor change in man page reference
 *
 * Revision 3.33  2012/12/04 15:54:34  galli
 * bug fixed
 *
 * Revision 3.32  2012/12/04 15:31:10  galli
 * Set oom_adj to -17 or oom_score_adj to -1000.
 *
 * Revision 3.31  2009/10/03 15:20:19  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 3.26  2009/02/17 16:09:11  galli
 * improved syslog header processing in sysLogProcHdr()
 *
 * Revision 3.25  2009/02/16 13:53:36  galli
 * added long command-line options
 * new command-line option -y | --syslog to correctly format syslog messages
 * added function sysLogProcHdr()
 *
 * Revision 3.13  2008/10/09 09:41:35  galli
 * sig2msg() moved to fmcSigUtils.c
 *
 * Revision 3.8  2008/08/30 10:16:47  galli
 * groff manual
 *
 * Revision 3.3  2008/06/24 15:34:29  galli
 * race condition fixed
 *
 * Revision 3.2  2008/06/24 10:52:16  galli
 * -E options takes 0|1 as argument
 *
 * Revision 3.0  2008/06/23 13:54:33  galli
 * publishes DIM SVC get_properties and DIM CMD set_properties instead of
 * log_filter, log_duplicate_suppression, log_stored, log_set_filter,
 * log_set_duplicate_suppression and log_set_stored
 *
 * Revision 2.27  2008/05/15 08:12:45  galli
 * lowered diagnostic message severity
 *
 * Revision 2.26  2008/05/13 14:34:56  galli
 * changed SRV and SVC names
 *
 * Revision 2.25  2008/05/06 10:20:45  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 * Added cmd-line option -x and -X to filter-out messages based on
 * extended regualr expression and wildcard pattern
 *
 * Revision 2.24  2007/12/12 15:49:02  galli
 * bug fixed
 *
 * Revision 2.19  2007/10/24 13:55:07  galli
 * publishes service fmc_version
 *
 * Revision 2.17  2007/10/23 10:31:29  galli
 * usage() prints FMC version
 *
 * Revision 2.14  2007/08/13 09:22:55  galli
 * usage() changed
 *
 * Revision 2.13  2007/08/09 22:12:38  galli
 * compatible with libFMCutils v 2
 *
 * Revision 2.12  2006/12/14 10:13:37  galli
 * added VERBOSE severity level
 *
 * Revision 2.11  2006/11/22 12:07:33  galli
 * if no UTGID specified in env use log_name as UTGID
 *
 * Revision 2.10  2006/11/22 08:53:27  galli
 * keep the last storLineN messages stored and publish them in SVC last_log
 * can filter using severityThreshold
 *
 * Revision 2.9  2006/11/06 14:22:57  galli
 * improved diagnostic messages
 *
 * Revision 2.8  2006/10/30 22:02:43  galli
 * added services log_filter and log_duplicate_suppression
 * added commands log_set_filter and log_set_duplicate_suppression
 * to change filter and suppression settings at runtime
 *
 * Revision 2.7  2006/10/20 08:04:32  galli
 * can print mark timestamps at regular intervals.
 *
 * Revision 2.6  2006/10/18 12:02:37  galli
 * uses the __func__ magic variable
 *
 * Revision 2.5  2006/10/16 10:19:33  galli
 * can suppress repeated message usind a fast word difference.
 * -R replaced by -L, -E and -W added
 *
 * Revision 2.4  2006/10/08 21:51:41  galli
 * Suppress repeated messages using Levenshtein distance
 * Added command line switches -R, -n, -c and -C
 *
 * Revision 2.3  2006/10/05 12:27:37  galli
 * get different regex severity filter from environment
 *
 * Revision 2.2  2006/09/29 09:30:03  galli
 * severity patterns in severityPatterns.h
 *
 * Revision 2.0  2006/09/26 14:50:09  galli
 * -N and -H command line switches added.
 * use regex to look for header.
 * use regex to identify severity.
 *
 * Revision 1.27  2006/09/24 21:02:13  galli
 * use signal_number_to_name() from libprocps
 *
 * Revision 1.26  2006/09/24 20:09:40  galli
 * changes in deBug level
 *
 * Revision 1.25  2006/09/24 19:43:30  galli
 * default pollPeriod set to 30 s for kernel >= 2.5.20
 * and to 0.1 s for kernel < 2.5.20
 *
 * Revision 1.24  2006/09/22 14:07:32  galli
 * can filter messages using wildcard pattern (-f switch)
 * can filter messages using regular expression (-F switch)
 *
 * Revision 1.23  2006/09/21 12:54:18  galli
 * low level read from FIFO
 * use asynchronous FIFO I/O notification via SIGIO/SIGPIPE to save CPU
 *
 * Revision 1.22  2006/09/17 23:01:18  galli
 * bug fixed
 *
 * Revision 1.21  2006/02/09 11:27:41  galli
 * x86_64 compatibility problems fixed
 *
 * Revision 1.20  2005/08/22 12:12:34  galli
 * open FIFO in O_RDONLY|O_NONBLOCK to avoid FIFO blocking
 *
 * Revision 1.18  2005/08/20 00:57:43  galli
 * opens the FIFO before starting DIM
 *
 * Revision 1.17  2005/08/20 00:23:59  galli
 * synchronous SIGINT/SIGTERM handling
 *
 * Revision 1.16  2005/08/18 14:21:17  galli
 * added headerPrepend() function
 * does not delete FIFO on exit
 *
 * Revision 1.15  2004/11/06 07:26:47  galli
 * DIM error handler added
 *
 * Revision 1.13  2004/10/26 10:19:29  galli
 * services "success" added
 * buffer increased up to 4096 butes (max for atomic read)
 * null-terminate truncated (>4096B) messages
 * send to DIM only the written buffer
 *
 * Revision 1.12  2004/10/26 00:19:48  galli
 * server/services name changed
 * added sensor_version service
 *
 * Revision 1.10  2004/10/09 23:59:19  galli
 * bug fixed
 * added cmdline switch -s
 *
 * Revision 1.6  2004/10/06 21:10:37  galli
 * high level FIFO read (1 line at a time)
 * nanosleep
 *
 * Revision 1.3  2004/10/05 00:02:29  galli
 * handle SIGTERM
 *
 * Revision 1.2  2004/10/04 23:20:46  galli
 * working version
 *
 * Revision 1.1  2004/10/04 22:56:45  galli
 * Initial revision
 * */
/*****************************************************************************/
#define _GNU_SOURCE                                  /* asprintf(), F_SETSIG */
#include <stdio.h>
#include <sys/types.h>                           /* mkfifo(), stat(), open() */
#include <unistd.h>                              /* access(), stat(), read() */
#include <sys/stat.h>                            /* mkfifo(), stat(), open() */
#include <fcntl.h>                                                 /* open() */
#include <errno.h>                                                  /* errno */
#include <string.h>                                            /* strerror() */
#include <syslog.h>                                              /* syslog() */
#include <libgen.h>                                            /* basename() */
#include <stdlib.h>                                      /* getenv(), exit() */
#include <envz.h>                                              /* envz_add() */
#include <time.h>                                             /* nanosleep() */
#include <math.h>                                        /* floor(), round() */
#include <regex.h>                                   /* regcomp(), regexec() */
#include <fnmatch.h>                                            /* fnmatch() */
#include <sys/param.h>                                           /* PIPE_BUF */
#include <sys/utsname.h>                                          /* uname() */
#include <signal.h>
#include <sys/wait.h>                                             /* wait(2) */
#include <getopt.h>                                         /* getopt_long() */
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>
/*---------------------------------------------------------------------------*/
#include <dis.h>                   /* dis_add_service(), dis_start_serving() */
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcMsgUtils.h"           /* mPrintf(), dfltLoggerOpen(), rPrintf() */
#include "fmcCmpUtils.h"                                        /* levDist() */
#include "fmcSrvNameUtils.h"                               /* getSrvPrefix() */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "fmcSigUtils.h"                                        /* sig2msg() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcRef.h"                                 /* FMC_URLS, FMC_AUTHORS */
#include "fmcPar.h"                                    /* DIM_CONF_FILE_NAME */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcMacro.h"     /* eExit(), likely(), unlikely(), printOutFuncOK() */
#include "fmcVersion.h"                                       /* FMC version */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*---------------------------------------------------------------------------*/
/* in logger/include */
#include "logSrv.h"                                    /* SRV_NAME, BUF_SIZE */
#include "severityPatterns.h"                    /* debugRegExpPattern, etc. */
/*****************************************************************************/
#define BUF_SIZE 4096                             /* size of logger messages */
#define PIPE_NAME_SIZE 128
#define SRV_NAME_SIZE 128
#define SVC_NAME_SIZE 256
#define DEF_FIFO_PATH "/tmp/logSrv.fifo"
#define PIPE_BUFFERS 16             /* linux/pipe_fs_i.h in kernel >= 2.6.17 */
/*****************************************************************************/
/* macro */
/*---------------------------------------------------------------------------*/
#define FREE_SETPARAMS_CMD_HANDLER_HEAP \
do{ \
  if(cmnd) \
  { \
    free(cmnd); \
    cmnd=NULL; \
  } \
  if(argz) \
  { \
    free(argz); \
    argz=NULL; \
    argz_len=0; \
  } \
  if(rArgv) \
  { \
    free(rArgv); \
    rArgv=NULL; \
    rArgc=0; \
  } \
}while(0)
/*****************************************************************************/
/* globals */
/* running parameters */
char *pName=NULL;                                            /* program name */
char *utgid=NULL;
int deBug=0;
int errU=L_STD|L_SYS;
int ptCmp=0;                                          /* punctual comparison */
int minLd=-1;                                /* minimum Levenshtein distance */
int minWd=-1;                                       /* minimum word distance */
int chkdLineN=1;
int storLineN=0;
size_t cutCol=0;
char *cutStr=NULL;
char pipePath[PIPE_NAME_SIZE]=DEF_FIFO_PATH;
int fifoFD=-1;
char srvPath[SRV_NAME_SIZE]="";
int severityThreshold=0;
int sysLogSkip=0;
pthread_t mainPtid;                  /* thread identifier of the main thread */
/* data to be published */
char *stString=NULL;
int stStringL=0;
/* message buffers */
char **mBuf=NULL;           /* buffer containing the last chkdLineN messages */
char *cBuf=NULL;                                         /* alias of mBuf[0] */
char **mStor=NULL;          /* buffer containing the last storLineN messages */
char *cStor=NULL;                                       /* alias of mStor[0] */
char *storBuf=NULL;
/* DIM SVC/CMD IDs */
int logSvcID=0;
int lastLogSvcID=0;
int versionSvcID=0;
int fmcVersionSvcID=0;
int successSvcID=0;
int getPropertiesSvcID=0;
int setPropertiesCmdID=0;
/* Regular Expressions / Wildcard Patterns */
regex_t verbRegExp;
regex_t debugRegExp;
regex_t infoRegExp;
regex_t warnRegExp;
regex_t errorRegExp;
regex_t fatalRegExp;
char *filterWildCardPattern=NULL;
char *xFilterWildCardPattern=NULL;
char *filterRegExpPattern=NULL;
char *xFilterRegExpPattern=NULL;
regex_t filterRegExp;
regex_t xFilterRegExp;
/* syslog date format (for strptime(3)) */
char *sysLogDateFmt=NULL;
/* kernel version */
static struct utsname osBuf;
char *kernelVersion=NULL;
/* oom adjust */
int logOomAdjVal;
int logOomScoreAdjVal;
char logOomAdjStr[8]="";                                         /* [-17,15] */
char logOomScoreAdjStr[8]="";                                /* [-1000,1000] */
/* mutexes */
pthread_mutex_t settingsLock=PTHREAD_MUTEX_INITIALIZER;
/* labels */
static char *sl[7]={"ALL","VERB","DEBUG","INFO","WARN","ERROR","FATAL"};
/* Version IDs */
static char rcsid[]="$Id: logSrv.c,v 3.34 2012/12/07 15:48:07 galli Exp galli $";
char *rcsidP=rcsid;
/*****************************************************************************/
/* function prototype */
/* Signal handlers */
void signalHandler(int sig);
/* DIM SVC handlers */
void logSvcHandler(long *tag,int **address,int *size);
void lastLogSvcHandler(long *tag,int **address,int *size);
void getPropertiesSvcHandler(long *tag,int **address,int *size);
/* DIM CMD handlers */
void setPropertiesCmdHandler(long *tag,int *cmd,int *size);
/* DIM Error handlers */
void disErrorHandler(int severity,int errorCode,char *message);
/* utilities */
int fifoCreate(char *pipePath);
static ssize_t bufCharRead(int fd,char *ptr);
ssize_t readLine(void *bufPtr,size_t bufLen,int fd);
int sysLogProcHdr(char *buf);
int headerPrepend(char *buf,int severity);
void printChangedSettings(void);
/* usage */
void usage(int mode);
void shortUsage(void);
/*****************************************************************************/
int main(int argc,char **argv,char **envp)
{
  /* counters */
  int i=0,j=0;
  unsigned long long suppressedMsgN=0;
  unsigned long long oSuppressedMsgN=0;
  /* pointers */
  char *p;
  /* DIM configuration */
  char *dimDnsNode=NULL;
  /* DIM names */
  char *srvBaseName=SRV_NAME;
  char srvSpecName[SRV_NAME_SIZE]="fmc";
  char svcPath[SVC_NAME_SIZE]="";
  char cmdPath[SVC_NAME_SIZE]="";
  /* signals handling */
  sigset_t signalMask;
  int signo=0;
  /* running parameters */
  char *deBugS=NULL;
  int addHeader=1;                         /* 0=never, 1=if absent, 2=always */
  int markPeriod=20;                                              /* minutes */
  time_t pollPeriodSec=0;
  long pollPeriodNSec=0;
  char *pollPeriodStr=NULL;
  double pollPeriod=30.0;
  struct timespec pollPeriodTS={0,0};
  struct timespec nullTS={0,0};
  struct timespec delay={0,500000000};                              /* 0.5 s */
  struct utsname unameBuf;                            /* kernel version data */
  /* time variables */
  time_t now=0,lastTime=0;
  /* returned values */
  int statusFlag=-1;
  int rv=0;
  /* booleans */
  int isRepeated=0;
  /* message parameters */
  int msgLen=0,severity=0;
  /* start-up messages */
  char *svMsg=NULL,*headMsg=NULL;
  char *reMsg=NULL,*xReMsg=NULL,*wpMsg=NULL,*xWpMsg=NULL;
  char *verbMsg="",*debugMsg="",*infoMsg="",*warnMsg="",*errorMsg="",
       *fatalMsg="";
  char *sysLogFmtMsg="";
  char *cmpMsg="",*ldMsg="",*wdMsg="",*clMsg="",*cutColMsg="",*cutStrMsg="";
  char *slMsg="";
  /* regular expressions */
  char *headerRegExpPattern="[a-zA-Z]{3}[0-9]{2}-[0-9]{6}\\[(VERB|DEBUG|INFO|"
                            "WARN|ERROR|FATAL)\\][[:blank:]]?\\w+:";
  regex_t headerRegExp;
  char *verbRegExpPattern=NULL;
  char *debugRegExpPattern=NULL;
  char *infoRegExpPattern=NULL;
  char *warnRegExpPattern=NULL;
  char *errorRegExpPattern=NULL;
  char *fatalRegExpPattern=NULL;
  /* data to be published */
  int success=1;
  /* getopt */
  int flag=0;
  char *shortOptions="N:p:s:S:Hvl:F:X:f:x:E:L:W:n:c:C:m:yh::";
  static struct option longOptions[]=
  {
    {"dim-dns",required_argument,NULL,'N'},
    {"fifo-path",required_argument,NULL,'p'},
    {"logger-name",required_argument,NULL,'s'},
    {"stored-messages",required_argument,NULL,'S'},
    {"always-header",no_argument,NULL,'H'},
    {"never-header",no_argument,NULL,'v'},
    {"severity-threshold",required_argument,NULL,'l'},
    {"filter-regex",required_argument,NULL,'F'},
    {"filter-out-regex",required_argument,NULL,'X'},
    {"filter-wildcard",required_argument,NULL,'f'},
    {"filter-out-wildcard",required_argument,NULL,'x'},
    {"suppress-ident",required_argument,NULL,'E'},
    {"suppress-levenshtein-threshold",required_argument,NULL,'L'},
    {"suppress-word-threshold",required_argument,NULL,'W'},
    {"compared-message-num",required_argument,NULL,'n'},
    {"skip-char-num",required_argument,NULL,'c'},
    {"skip-char-list",required_argument,NULL,'C'},
    {"mark-timestamp-interval",required_argument,NULL,'m'},
    {"syslog",no_argument,NULL,'y'},
    {"help",optional_argument,NULL,'h'},
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsidP,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  pName=strdup(basename(argv[0]));
  cutStr=strdup("");
  /*-------------------------------------------------------------------------*/
  /* default pollPeriod */
  if(uname(&unameBuf)==-1)eExit("uname()");
  if(strverscmp(unameBuf.release,"2.5.20")<0)
    pollPeriod=0.1;                                       /* kernel < 2.5.20 */
  else
    pollPeriod=30.0;                                     /* kernel >= 2.5.20 */
  /*-------------------------------------------------------------------------*/
  /* read environment variables */
  deBugS=getenv("deBug");
  if(deBugS)deBug=(int)strtol(deBugS,(char**)NULL,0);
  pollPeriodStr=getenv("pollPeriod");
  if(pollPeriodStr)pollPeriod=strtod(pollPeriodStr,(char**)NULL);
  pollPeriodSec=(time_t)floor(pollPeriod);
  pollPeriodNSec=(long)round((pollPeriod-floor(pollPeriod))*1e9);
  if(pollPeriodNSec>999999999)pollPeriodNSec=999999999;
  utgid=getenv("UTGID");
  /* default in severityPatterns.h */
  verbRegExpPattern=getenv("verbRegExp");
  debugRegExpPattern=getenv("debugRegExp");
  infoRegExpPattern=getenv("infoRegExp");
  warnRegExpPattern=getenv("warnRegExp");
  errorRegExpPattern=getenv("errorRegExp");
  fatalRegExpPattern=getenv("fatalRegExp");
  if(!verbRegExpPattern)verbRegExpPattern=dfltVerbRegExpPattern;
  if(!debugRegExpPattern)debugRegExpPattern=dfltDebugRegExpPattern;
  if(!infoRegExpPattern)infoRegExpPattern=dfltInfoRegExpPattern;
  if(!warnRegExpPattern)warnRegExpPattern=dfltWarnRegExpPattern;
  if(!errorRegExpPattern)errorRegExpPattern=dfltErrorRegExpPattern;
  if(!fatalRegExpPattern)fatalRegExpPattern=dfltFatalRegExpPattern;
  /* default in logSrv.h */
  sysLogDateFmt=getenv("sysLogDateFmt");
  if(!sysLogDateFmt)sysLogDateFmt=SYS_LOG_DATE_FMT;
  /*-------------------------------------------------------------------------*/
  /* process command line options */
  opterr=0;                  /* do not print default error message to stderr */
  //while((flag=getopt(argc,argv,"h::p:s:F:f:l:HvE:L:W:n:c:C:m:S:X:x:N:"))!=EOF)
  while((flag=getopt_long(argc,argv,shortOptions,longOptions,NULL))!=EOF)
  {
    char *p=NULL;
    switch(flag)
    {
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'p':
        snprintf(pipePath,PIPE_NAME_SIZE,"%s",optarg);
        break;
      case 's':
        snprintf(srvSpecName,SRV_NAME_SIZE,"%s",optarg);
        break;
      case 'S':
        storLineN=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'H':
        addHeader=2;
        break;
      case 'v':
        addHeader=0;
        break;
      case 'l':
        severityThreshold=(int)strtol(optarg,&p,0);
        if(!p||!*p)break;
        if(p==optarg)
        {
          if(!strncasecmp(optarg,"ALL",5))severityThreshold=0;
          else if(!strncasecmp(optarg,"VERB",4))severityThreshold=1;
          else if(!strncasecmp(optarg,"DEBUG",5))severityThreshold=2;
          else if(!strncasecmp(optarg,"INFO",4))severityThreshold=3;
          else if(!strncasecmp(optarg,"WARN",4))severityThreshold=4;
          else if(!strncasecmp(optarg,"ERROR",5))severityThreshold=5;
          else if(!strncasecmp(optarg,"FATAL",5))severityThreshold=6;
          else
          {
            mPrintf(errU,FATAL,__func__,0,"Invalid SEVERITY_THRESHOLD: "
                    "\"%s\"!",optarg);
            shortUsage();
          }
        }
        else
        {
          mPrintf(errU,FATAL,__func__,0,"Invalid SEVERITY_THRESHOLD: "
                  "\"%s\"!",optarg);
          shortUsage();
        }
        break;
      case 'F':
        filterRegExpPattern=strdup(optarg);
        break;
      case 'X':
        xFilterRegExpPattern=strdup(optarg);
        break;
      case 'f':
        filterWildCardPattern=strdup(optarg);
        break;
      case 'x':
        xFilterWildCardPattern=strdup(optarg);
        break;
      case 'E':
        ptCmp=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'L':
        minLd=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'W':
        minWd=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'n':
        chkdLineN=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'c':
        cutCol=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'C':
        cutStr=strdup(optarg);
        break;
      case 'm':
        markPeriod=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'y':
        sysLogSkip=1;
        break;
      case 'h':
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        mPrintf(errU,FATAL,__func__,0,"getopt(): invalid option \"-%c\"!",
                optopt);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* check cmdline options */
  if(optind!=argc && argv[optind] && argv[optind][0]!='\0')
  {
    fprintf(stderr,"\nUnrequired non-option "
            "argument: \"%s\"!",argv[optind]);
    mPrintf((errU&~L_STD)|L_SYS,FATAL,__func__,0,"Unrequired non-option "
            "argument: \"%s\"!",argv[optind]);
    shortUsage();
  }
  /*-------------------------------------------------------------------------*/
  if(!utgid)
  {
    utgid=srvSpecName;
    setenv("UTGID",utgid,0);
  }
  /*-------------------------------------------------------------------------*/
  /* set error unit */
  if(!strcmp(pipePath,DEF_FIFO_PATH))               /* default logger server */
  {
    errU=L_STD|L_SYS;
  }
  else                                            /* secondary logger server */
  {
    /* try to open the default logger FIFO for sending logger error messages */
    if(dfltLoggerOpen(1,0,INFO,DEBUG,1)==-1)                       /* failed */
    {
      mPrintf(errU,INFO,__func__,0,"Can't open the default logger \"%s\" to "
              "send logger error messages.",DEF_FIFO_PATH);
      mPrintf(errU,INFO,__func__,0,"Sending logger error messages to syslog "
              "and stderr.");
    }
    else                                                          /* success */
    {
      mPrintf(errU,INFO,__func__,0,"Sending logger error messages to the "
              "default logger \"%s\".",DEF_FIFO_PATH);
      errU=L_DIM;
    }
  }
  if(sysLogSkip)errU&=~L_SYS;
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
      dimDnsNode=getDimDnsNode(DIM_CONF_FILE_NAME,0,1);
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
  /* check command line options (contd) */
  if(severityThreshold<0||severityThreshold>6)
  {
    mPrintf(errU,FATAL,__func__,0,"Invalid severity threshold: %d! "
            "Severity threshold can only be: ALL=0, VERB=1, DEBUG=2, INFO=3, "
            "WARN=4, ERROR=5, FATAL=6!",severityThreshold);
    shortUsage();
  }
  if(ptCmp!=0 && ptCmp!=1)
  {
    mPrintf(errU,FATAL,__func__,0,"Invalid argument for -E switch: "
            "%d! -E argument must be either 0 or 1! ",ptCmp);
    shortUsage();
  }
  if(minLd<-1)
  {
    mPrintf(errU,FATAL,__func__,0,"Invalid value for minimum "
            "Levenshtein distance between messages: %d! Minimum Levenshtein "
            "distance between messages must be >=-1!",minLd);
    shortUsage();
  }
  if(minWd<-1)
  {
    mPrintf(errU,FATAL,__func__,0,"Invalid value for minimum word "
            "difference between messages: %d! Minimum word difference between "
            "messages must be >=-1!",minWd);
    shortUsage();
  }
  if(chkdLineN<1)
  {
    mPrintf(errU,FATAL,__func__,0,"Invalid value for the number of "
            "compared messages: %d! Minimum number of compared messages must "
            "be >=1!",chkdLineN);
    shortUsage();
  }
  if(cutCol<0)
  {
    mPrintf(errU,FATAL,__func__,0,"Invalid value for the number of "
            "characters skipped in comparison: %d! The number of characters "
            "skipped in comparison must be >=0!",cutCol);
    shortUsage();
  }
  if(storLineN<0)
  {
    mPrintf(errU,FATAL,__func__,0,"Invalid value for the number of "
            "messages stored in last_log: %d! The number of messages stored "
            "in last_log must be >=0!",storLineN);
    shortUsage();
  }
  /* setting -E, -L, -W and not -n force -n 2  */
  if((minLd>-1||minWd>-1||ptCmp>0) && chkdLineN==1)
  {
    mPrintf(errU,WARN,__func__,0,"Since duplicate suppression is required, "
            "the number of compared messages (set to %d) is forced to 2!",
            chkdLineN);
    chkdLineN=2;
  }
  /*-------------------------------------------------------------------------*/
  /* allocate memory for mBuf */
  mBuf=(char**)malloc(chkdLineN*sizeof(char*));
  for(i=0;i<chkdLineN;i++)mBuf[i]=(char*)malloc(BUF_SIZE*sizeof(char));
  cBuf=mBuf[0];
  /* allocate memory for mStor */
  if(storLineN)
  {
    mStor=(char**)malloc(storLineN*sizeof(char*));
    for(i=0;i<storLineN;i++)mStor[i]=(char*)malloc(BUF_SIZE*sizeof(char));
    cStor=mStor[0];
    storBuf=(char*)malloc(storLineN*BUF_SIZE*sizeof(char));
  }
  /*-------------------------------------------------------------------------*/
  /* starting message about message filter */
  if(severityThreshold)
    asprintf(&svMsg,"Severity filter: %s",sl[severityThreshold]);
  else
    asprintf(&svMsg,"no Severity filter");
  if(filterRegExpPattern)
    asprintf(&reMsg,"Regular Expression filter: \"%s\"",filterRegExpPattern);
  else
    asprintf(&reMsg,"no Regular Expression filter");
  if(xFilterRegExpPattern)
    asprintf(&xReMsg,"Regular Expression negate filter: \"%s\"",
             xFilterRegExpPattern);
  else
    asprintf(&xReMsg,"no Regular Expression negate filter");
  if(filterWildCardPattern)
    asprintf(&wpMsg,"Wildcard Pattern filter: \"%s\"",filterWildCardPattern);
  else
    asprintf(&wpMsg,"no Wildcard Pattern filter");
  if(xFilterWildCardPattern)
    asprintf(&xWpMsg,"Wildcard Pattern negate filter: \"%s\"",
             xFilterWildCardPattern);
  else
    asprintf(&xWpMsg,"no Wildcard Pattern negate filter");
  /*-------------------------------------------------------------------------*/
  /* starting message about header prepending */
  if(addHeader==0)headMsg="never preprend header";
  else if(addHeader==1)headMsg="preprend header if absent";
  else if(addHeader==2)headMsg="always preprend header";
  /*-------------------------------------------------------------------------*/
  /* starting message about changed severity regex */
  if(verbRegExpPattern!=dfltVerbRegExpPattern)
    asprintf(&verbMsg,", verbRegExp=\"%s\"",verbRegExpPattern);
  if(debugRegExpPattern!=dfltDebugRegExpPattern)
    asprintf(&debugMsg,", debugRegExp=\"%s\"",debugRegExpPattern);
  if(infoRegExpPattern!=dfltInfoRegExpPattern)
    asprintf(&infoMsg,", infoRegExp=\"%s\"",infoRegExpPattern);
  if(warnRegExpPattern!=dfltWarnRegExpPattern)
    asprintf(&warnMsg,", warnRegExp=\"%s\"",warnRegExpPattern);
  if(errorRegExpPattern!=dfltErrorRegExpPattern)
    asprintf(&errorMsg,", errorRegExp=\"%s\"",errorRegExpPattern);
  if(fatalRegExpPattern!=dfltFatalRegExpPattern)
    asprintf(&fatalMsg,", fatalRegExp=\"%s\"",fatalRegExpPattern);
  /*-------------------------------------------------------------------------*/
  /* starting message about syslog date format */
  if(sysLogSkip)
    asprintf(&sysLogFmtMsg,", syslog date format=\"%s\"",sysLogDateFmt);
  /*-------------------------------------------------------------------------*/
  /* starting message about repeated messages suppression */
  if(!ptCmp && minLd<0 && minWd<0)
    asprintf(&cmpMsg,", no repeated messages suppression");
  if(ptCmp)
    asprintf(&cmpMsg,", suppress identical messages");
  if(minLd>-1)
    asprintf(&ldMsg,", minimum Levenshtein distance between messages=%d",minLd);
  if(minWd>-1)
    asprintf(&wdMsg,", minimum word difference between messages=%d",minWd);
  if(chkdLineN>1)
    asprintf(&clMsg,", number of compared messages=%d",chkdLineN);
  if(cutCol>0)
    asprintf(&cutColMsg,", skip %zu characters in comparison",cutCol);
  if(cutStr[0]!=0)
    asprintf(&cutStrMsg,", skip characters in comparison until \"%s\" passed",
             cutStr);
  /*-------------------------------------------------------------------------*/
  /* starting message about messages storage */
  if(storLineN>0)
    asprintf(&slMsg,", number of stored messages=%d",storLineN);
  /*-------------------------------------------------------------------------*/
  /* define server name */
  snprintf(srvPath,SRV_NAME_SIZE,"%s/%s/%s",getSrvPrefix(),srvBaseName,
           srvSpecName);
  /*-------------------------------------------------------------------------*/
  /* compile regular expression */
  if(filterRegExpPattern)
  {
    rv=regcomp(&filterRegExp,filterRegExpPattern,REG_EXTENDED|REG_NOSUB);
    if(rv!=0)
    {
      char errMsg[4096]="";
      regerror(rv,&filterRegExp,errMsg,4096);
      mPrintf(errU,FATAL,__func__,0,"regcomp(): Cannot compile regular "
              "expression \"%s\". Reported error: \"%s\"!",
              filterRegExpPattern,errMsg);
      exit(1);
    }
  }
  if(xFilterRegExpPattern)
  {
    rv=regcomp(&xFilterRegExp,xFilterRegExpPattern,REG_EXTENDED|REG_NOSUB);
    if(rv!=0)
    {
      char errMsg[4096]="";
      regerror(rv,&xFilterRegExp,errMsg,4096);
      mPrintf(errU,FATAL,__func__,0,"regcomp(): Cannot compile regular "
              "expression \"%s\". Reported error: \"%s\"!",
              xFilterRegExpPattern,errMsg);
      exit(1);
    }
  }
  rv=regcomp(&headerRegExp,headerRegExpPattern,REG_EXTENDED|REG_NOSUB);
  if(rv!=0)
  {
    char errMsg[4096]="";
    regerror(rv,&headerRegExp,errMsg,4096);
    mPrintf(errU,FATAL,__func__,0,"regcomp(): Cannot compile regular "
            "expression \"%s\". Reported error: \"%s\"!",
            headerRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&verbRegExp,verbRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    char errMsg[4096]="";
    regerror(rv,&verbRegExp,errMsg,4096);
    mPrintf(errU,FATAL,__func__,0,"regcomp(): Cannot compile regular "
            "expression \"%s\". Reported error: \"%s\"!",
            verbRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&debugRegExp,debugRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    char errMsg[4096]="";
    regerror(rv,&debugRegExp,errMsg,4096);
    mPrintf(errU,FATAL,__func__,0,"regcomp(): Cannot compile regular "
            "expression \"%s\". Reported error: \"%s\"!",
            debugRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&infoRegExp,infoRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    char errMsg[4096]="";
    regerror(rv,&infoRegExp,errMsg,4096);
    mPrintf(errU,FATAL,__func__,0,"regcomp(): Cannot compile regular "
            "expression \"%s\". Reported error: \"%s\"!",
            infoRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&warnRegExp,warnRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    char errMsg[4096]="";
    regerror(rv,&warnRegExp,errMsg,4096);
    mPrintf(errU,FATAL,__func__,0,"regcomp(): Cannot compile regular "
            "expression \"%s\". Reported error: \"%s\"!",
            warnRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&errorRegExp,errorRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    char errMsg[4096]="";
    regerror(rv,&errorRegExp,errMsg,4096);
    mPrintf(errU,FATAL,__func__,0,"regcomp(): Cannot compile regular "
            "expression \"%s\". Reported error: \"%s\"!",
            errorRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&fatalRegExp,fatalRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    char errMsg[4096]="";
    regerror(rv,&fatalRegExp,errMsg,4096);
    mPrintf(errU,FATAL,__func__,0,"regcomp(): Cannot compile regular "
            "expression \"%s\". Reported error: \"%s\"!",
            fatalRegExpPattern,errMsg);
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* create FIFO */
  if(fifoCreate(pipePath))
  {
    mPrintf(errU,FATAL,__func__,0,"fifoCreate() returned a fatal error!");
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* POSIX.1 says that O_RDONLY|O_NONBLOCK means return with the FIFO opened */
  /* even when there is no process writing the FIFO.                         */
  /* do not open with flag O_ASYNC, otherwise O_ASYNC doesn't work (!?!)     */
  /* open FIFO */
  fifoFD=open(pipePath,O_RDONLY|O_NONBLOCK);
  if(fifoFD==-1)
  {
    mPrintf(errU,FATAL,__func__,0,"open(\"%s\"): %s!",pipePath,
            strerror(errno));
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* setting file descriptor for asynchronous notification via SIGIO/SIGPOLL */
  /* this still does not work with kernel 2.6.9-42.0.2.EL.1.cernsmp          */
  if(fcntl(fifoFD,F_SETOWN,getpid())==-1)
  {
    mPrintf(errU,FATAL,__func__,0,"fcntl(F_SETOWN): %s!",strerror(errno));
    exit(1);
  }
  statusFlag=fcntl(fifoFD,F_GETFL);
  if(statusFlag<0)
  {
    mPrintf(errU,FATAL,__func__,0,"fcntl(F_GETFL): %s!",strerror(errno));
    exit(1);
  }
  if(fcntl(fifoFD,F_SETFL,statusFlag|O_ASYNC)==-1)
  {
    mPrintf(errU,FATAL,__func__,0,"fcntl(F_SETFL): %s!",strerror(errno));
    exit(1);
  }
  if(fcntl(fifoFD,F_SETSIG,0)==-1)
  {
    mPrintf(errU,FATAL,__func__,0,"fcntl(F_SETSIG): %s!",strerror(errno));
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* get kernel version */
  if(uname(&osBuf))
  {
    mPrintf(errU,ERROR,__func__,0,"uname(): %s",strerror(errno));
    kernelVersion=(char*)"N/A";
  }
  else
  {
    kernelVersion=osBuf.release;
  }
  /*-------------------------------------------------------------------------*/
  /* Control the behavior of the OOM killer on logSrv process,               */
  /* if run as root */
  if(getuid()==0)
  {
    if(!access(LOG_OOM_SCORE_ADJ_INODE,W_OK))            /* kernel >= 2.6.36 */
    {
      int logOomScoreAdjFD=-1;
      logOomScoreAdjVal=LOG_OOM_SCORE_ADJ;
      snprintf(logOomScoreAdjStr,8,"%d",logOomScoreAdjVal);
      /* write value */
      logOomScoreAdjFD=open(LOG_OOM_SCORE_ADJ_INODE,O_WRONLY);
      if(logOomScoreAdjFD==-1)
      {
        mPrintf(errU,WARN,__func__,0,"Cannot set oom_score_adj for the "
                "Logger: open(2): %s!",strerror(errno));
      }
      else
      {
        int wB=-1;
        wB=write(logOomScoreAdjFD,logOomScoreAdjStr,strlen(logOomScoreAdjStr));
        if(wB==-1)
        {
          mPrintf(errU,WARN,__func__,0,"Cannot set oom_score_adj for the "
                  "Logger: write(2): %s!",strerror(errno));
        }
        else
        {
          mPrintf(errU,DEBUG,__func__,0,"Logger oom_score_adj set to %s.",
                  logOomScoreAdjStr);
        }
        close(logOomScoreAdjFD);
      }
      /* read written value */
      memset(logOomScoreAdjStr,0,8);
      logOomScoreAdjFD=open(LOG_OOM_SCORE_ADJ_INODE,O_RDONLY);
      read(logOomScoreAdjFD,logOomScoreAdjStr,8);
      close(logOomScoreAdjFD);
      p=strchr(logOomScoreAdjStr,'\n');
      if(p)*p='\0';
    }
    else if(!access(LOG_OOM_ADJ_INODE,W_OK))              /* kernel < 2.6.36 */
    {
      int logOomAdjFD=-1;
      logOomAdjVal=LOG_OOM_ADJ;
      snprintf(logOomAdjStr,8,"%d",logOomAdjVal);
      /* write value */
      logOomAdjFD=open(LOG_OOM_ADJ_INODE,O_WRONLY);
      if(logOomAdjFD==-1)
      {
        mPrintf(errU,WARN,__func__,0,"Cannot set oom_adj for the Logger: "
                "open(2): %s!",strerror(errno));
      }
      else
      {
        int wB=-1;
        wB=write(logOomAdjFD,logOomAdjStr,strlen(logOomAdjStr));
        if(wB==-1)
        {
          mPrintf(errU,WARN,__func__,0,"Cannot set oom_adj for the "
                  "Logger: write(2): %s!",strerror(errno));
        }
        else
        {
          mPrintf(errU,DEBUG,__func__,0,"Logger oom_adj set to %s.",
                  logOomAdjStr);
        }
        close(logOomAdjFD);
      }
      /* read written value */
      memset(logOomAdjStr,0,8);
      logOomAdjFD=open(LOG_OOM_ADJ_INODE,O_RDONLY);
      read(logOomAdjFD,logOomAdjStr,8);
      close(logOomAdjFD);
      p=strchr(logOomAdjStr,'\n');
      if(p)*p='\0';
    }
    else
    {
      mPrintf(errU,WARN,__func__,0,"OOM Killer Score Adjustment not foreseen "
              "in this OS version (%s)!",kernelVersion);
    }
  }
  /*-------------------------------------------------------------------------*/
  /* Block SIGINT, SIGTERM & SIGIO/SIGPOLL, to be handled synchronously by   */
  /* sigtimedwait().                                                         */
  /* Signals must be blocked after the FIFO has been opened, because open()  */
  /* call may block (if there are no writers to the FIFO).                   */
  /* Signals must be blocked before the dim_init() call, which creates 2 new */
  /* threads, to keep the signals blocked in all the 3 threads.              */
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigaddset(&signalMask,SIGIO);
  if(pthread_sigmask(SIG_BLOCK,&signalMask,NULL))eExit("pthread_sigmask()");
  /*-------------------------------------------------------------------------*/
  /* Start DIM. Here threads becomes 3 */
  dim_init();
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  dis_add_error_handler(disErrorHandler);
  /*-------------------------------------------------------------------------*/
  /* clear the chkdLineN message buffers */
  for(i=0;i<chkdLineN;i++)memset(mBuf[i],0,BUF_SIZE);
  /* clear the storLineN message buffers */
  for(i=0;i<storLineN;i++)memset(mStor[i],0,BUF_SIZE);
  memset(storBuf,0,storLineN*BUF_SIZE);
  /*-------------------------------------------------------------------------*/
  /* define DIM services */
  snprintf(svcPath,SVC_NAME_SIZE,"%s/log",srvPath);
  logSvcID=dis_add_service(svcPath,"C",0,0,logSvcHandler,1);
  snprintf(svcPath,SVC_NAME_SIZE,"%s/last_log",srvPath);
  lastLogSvcID=dis_add_service(svcPath,"C",0,0,lastLogSvcHandler,1);
  snprintf(svcPath,SVC_NAME_SIZE,"%s/server_version",srvPath);
  versionSvcID=dis_add_service(svcPath,"C",rcsidP,1+strlen(rcsidP),0,0);
  snprintf(svcPath,SVC_NAME_SIZE,"%s/fmc_version",srvPath);
  fmcVersionSvcID=dis_add_service(svcPath,"C",FMC_VERSION,
                                  1+strlen(FMC_VERSION),0,0);
  snprintf(svcPath,SVC_NAME_SIZE,"%s/success",srvPath);
  successSvcID=dis_add_service(svcPath,"I",&success,sizeof(int),0,0);
  snprintf(svcPath,SVC_NAME_SIZE,"%s/get_properties",srvPath);
  getPropertiesSvcID=dis_add_service(svcPath,"C",0,0,getPropertiesSvcHandler,0);
  /*-------------------------------------------------------------------------*/
  /* define DIM commands */
  snprintf(cmdPath,SVC_NAME_SIZE,"%s/set_properties",srvPath);
  setPropertiesCmdID=dis_add_cmnd(cmdPath,"C",setPropertiesCmdHandler,0);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  dis_start_serving(srvPath);
  nanosleep(&delay,NULL);    /* wait 0.5 s for the other threads to be ready */
  /*-------------------------------------------------------------------------*/
  /* save the thread identifier of the main thread, to send it a SIGIO       */
  /* using pthread_kill().                                                   */
  mainPtid=pthread_self();
  /*-------------------------------------------------------------------------*/
  /* catch SIGTERM, SIGINT and SIGIO/SIGPOLL without waiting */
  nullTS.tv_sec=0;
  nullTS.tv_nsec=0;
  signo=sigtimedwait(&signalMask,NULL,&nullTS);
  if((deBug & 0x1) && signo!=-1)
    mPrintf(errU,DEBUG,__func__,0,"Signal %d received.",signo);
  if(signo==SIGTERM||signo==SIGINT)signalHandler(signo);
  /*-------------------------------------------------------------------------*/
  /* print starting message */
  mPrintf(errU|L_STD,INFO,__func__,0,"FMC Message Logger Server started. "
          "Using software \"%s\", \"FMC-%s\". TGID=%d, FIFO=\"%s\", DIM "
          "Service Path=\"%s/log\", deBug=%#x, polling period=%.2f s, %s, %s, "
          "%s, %s, %s, %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s.",rcsidP,FMC_VERSION,
          getpid(),pipePath,srvPath,deBug,
          (double)pollPeriodSec+(double)pollPeriodNSec/1e9,
          svMsg,reMsg,xReMsg,wpMsg,xWpMsg,headMsg,verbMsg,debugMsg,infoMsg,
          warnMsg,errorMsg,fatalMsg,sysLogFmtMsg,cmpMsg,ldMsg,wdMsg,clMsg,
          cutColMsg,cutStrMsg,slMsg);
  /*-------------------------------------------------------------------------*/
  /* print starting message as the first message to the logger */
  snprintf(cBuf,BUF_SIZE,"%s(%s): %s(): FMC Message Logger Server started. "
           "Using software \"%s\", \"FMC-%s\". TGID=%d, FIFO=\"%s\", DIM "
           "Service Path=\"%s/log\", deBug=%#x, polling period=%.2f s, %s, "
           "%s, %s, %s, %s, %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s.",pName,
           utgid,__func__,rcsidP,FMC_VERSION,getpid(),pipePath,srvPath,deBug,
           (double)pollPeriodSec+(double)pollPeriodNSec/1e9,svMsg,reMsg,xReMsg,
           wpMsg,xWpMsg,headMsg,verbMsg,debugMsg,infoMsg,warnMsg,errorMsg,
           fatalMsg,sysLogFmtMsg,cmpMsg,ldMsg,wdMsg,clMsg,cutColMsg,cutStrMsg,
           slMsg);
  headerPrepend(cBuf,INFO);
  dis_update_service(successSvcID);
  dis_update_service(logSvcID);
  /*-------------------------------------------------------------------------*/
  /* rotate the mStor storLineN buffers and save message in mStor */
  pthread_mutex_lock(&settingsLock);
  if(storLineN)
  {
    cStor=mStor[storLineN-1];
    for(j=storLineN-1;j>0;j--)mStor[j]=mStor[j-1];
    mStor[0]=cStor;
    strcpy(cStor,cBuf);
  }
  pthread_mutex_unlock(&settingsLock);
  dis_update_service(lastLogSvcID);
  /* rotate the chkdLineN buffers */
  pthread_mutex_lock(&settingsLock);
  cBuf=mBuf[chkdLineN-1];
  for(j=chkdLineN-1;j>0;j--)mBuf[j]=mBuf[j-1];
  mBuf[0]=cBuf;
  pthread_mutex_unlock(&settingsLock);
  /*-------------------------------------------------------------------------*/
  /* main loop: in each iteration read a message from the FIFO and publish   */
  /* it using DIM                                                            */
  for(i=0,suppressedMsgN=0;;i++)
  {
    if(deBug & 0x4)mPrintf(errU,DEBUG,__func__,0,"updates: %d.\n",i);
    /* try to read one line from the FIFO */
    memset(cBuf,0,BUF_SIZE);
    /*.......................................................................*/
    if(readLine(cBuf,BUF_SIZE,fifoFD)>0)         /* message(s) found in FIFO */
    {
      /* deBug printout */
      if(deBug & 0x2)mPrintf(errU,DEBUG,__func__,0,"message(s) found.");
      if(deBug & 0x8)
      {
        for(j=0;j<chkdLineN;j++)
          rPrintf(errU,2,"mBuf[%02d]=\"%s\"",j,mBuf[j]);
          rPrintf(errU,2,"---------------------------");
      }
      /* skip empty lines, included those genetated by '\r' conversion */
      if(cBuf[0]=='\n')continue;
      /* set last char to '\n' if not present */
      if(!strchr(cBuf,'\n'))
      {
        int cBufLen=strlen(cBuf);
        if(cBufLen<BUF_SIZE-1)strcat(cBuf,"\n");
        else cBuf[cBufLen-1]='\n';
      }
      if(sysLogSkip)
      {
        sysLogProcHdr(cBuf);
      }
      /* add header if required */
      if(addHeader==2 || 
         (addHeader==1 && regexec(&headerRegExp,cBuf,(size_t)0,NULL,0)))
        headerPrepend(cBuf,-1);
      /* rotate the mStor storLineN buffers and save message in mStor */
      pthread_mutex_lock(&settingsLock);
      if(storLineN)
      {
        cStor=mStor[storLineN-1];
        for(j=storLineN-1;j>0;j--)mStor[j]=mStor[j-1];
        mStor[0]=cStor;
        strcpy(cStor,cBuf);
      }
      pthread_mutex_unlock(&settingsLock);
      dis_update_service(lastLogSvcID);
      /* filter using severity */
      pthread_mutex_lock(&settingsLock);
      if(severityThreshold)
      {
        msgLen=strlen(cBuf);
        if(msgLen>18 && !strncmp(cBuf+12,"[VERB]",6))severity=1;
        else if(msgLen>19 && !strncmp(cBuf+12,"[DEBUG]",7))severity=2;
        else if(msgLen>18 && !strncmp(cBuf+12,"[INFO]",6))severity=3;
        else if(msgLen>18 && !strncmp(cBuf+12,"[WARN]",6))severity=4;
        else if(msgLen>19 && !strncmp(cBuf+12,"[ERROR]",7))severity=5;
        else if(msgLen>19 && !strncmp(cBuf+12,"[FATAL]",7))severity=6;
        else severity=2;
        if(severity<severityThreshold)
        {
          pthread_mutex_unlock(&settingsLock);
          continue;
        }
      }
      pthread_mutex_unlock(&settingsLock);
      /* filter using IEEE/POSIX regular expression */
      pthread_mutex_lock(&settingsLock);
      if(filterRegExpPattern && regexec(&filterRegExp,cBuf,(size_t)0,NULL,0))
      {
        pthread_mutex_unlock(&settingsLock);
        continue;
      }
      pthread_mutex_unlock(&settingsLock);
      /* filter using IEEE/POSIX negate regular expression */
      pthread_mutex_lock(&settingsLock);
      if(xFilterRegExpPattern && !regexec(&xFilterRegExp,cBuf,(size_t)0,NULL,0))
      {
        pthread_mutex_unlock(&settingsLock);
        continue;
      }
      pthread_mutex_unlock(&settingsLock);
      /* filter using IEEE/POSIX wildcard pattern */
      pthread_mutex_lock(&settingsLock);
      if(filterWildCardPattern && fnmatch(filterWildCardPattern,cBuf,0))
      {
        pthread_mutex_unlock(&settingsLock);
        continue;
      }
      pthread_mutex_unlock(&settingsLock);
      /* filter using IEEE/POSIX negate wildcard pattern */
      pthread_mutex_lock(&settingsLock);
      if(xFilterWildCardPattern && !fnmatch(xFilterWildCardPattern,cBuf,0))
      {
        pthread_mutex_unlock(&settingsLock);
        continue;
      }
      pthread_mutex_unlock(&settingsLock);
      /* suppress repeated messages if required */
      isRepeated=0;
      pthread_mutex_lock(&settingsLock);
      if(minLd>-1||minWd>-1||ptCmp)
      {
        for(j=1;j<chkdLineN;j++)
        {
          char *cStr=cBuf,*oStr=mBuf[j];
          int cutStrLen=0;
          int k=0;
          /* shorten string to be compared using -c */
          if(cutCol)
          {
            if(strlen(cStr)>cutCol)cStr+=cutCol;
            else cStr="";
            if(strlen(oStr)>cutCol)oStr+=cutCol;
            else oStr="";
          }
          /* shorten string to be compared using -C */
          if((cutStrLen=strlen(cutStr)))
          {
            for(k=0;k<cutStrLen;k++)
            {
              char *p=NULL;
              if((p=strchr(cStr,cutStr[k]))!=NULL)cStr=p;
              else{cStr="";break;}
              if((p=strchr(oStr,cutStr[k]))!=NULL)oStr=p;
              else{oStr="";break;}
            }
          }
          /* compare strings */
          if(ptCmp)                                   /* punctual comparison */
          {
            if(!strcmp(cStr,oStr))
            {
              isRepeated=1;
              break;
            }
          }
          if(minLd>-1)                             /* Levenshtein comparison */
          {
            int d;
            d=levDist(cStr,oStr);
            if(deBug & 0x10)rPrintf(errU,2,"curr=\"%s\", old%d=\"%s\", "
                                    "levDist=%d\n",cStr,j,oStr,d);
            if(d>=0 && d<=minLd)
            {
              isRepeated=1;
              break;
            }
          }
          if(minWd>-1)                           /* punctual word comparison */
          {
            int res;
            res=wordCmp(cStr,oStr,minWd);
            if(deBug & 0x10)rPrintf(errU,2,"curr=\"%s\", old%d=\"%s\", "
                                    "wordCmp=%d\n",cStr,j,oStr,res);
            if(res)
            {
              isRepeated=1;
              break;
            }
          }
          /* end strings comparison */
        }
      }
      pthread_mutex_unlock(&settingsLock);
      /* publish message */
      if(!isRepeated)dis_update_service(logSvcID);
      else suppressedMsgN++;
      /* rotate the chkdLineN buffers */
      pthread_mutex_lock(&settingsLock);
      cBuf=mBuf[chkdLineN-1];
      for(j=chkdLineN-1;j>0;j--)mBuf[j]=mBuf[j-1];
      mBuf[0]=cBuf;
      pthread_mutex_unlock(&settingsLock);
      /* catch SIGTERM and SIGINT without waiting */
      nullTS.tv_sec=0;
      nullTS.tv_nsec=0;
      signo=sigtimedwait(&signalMask,NULL,&nullTS);
      if((deBug & 0x1) && signo!=-1)
        mPrintf(errU,DEBUG,__func__,0,"Signal %d received.",signo);
      if(signo==SIGTERM||signo==SIGINT)signalHandler(signo);
    }
    /*.......................................................................*/
    else                                        /* no messages found in FIFO */
    /*.......................................................................*/
    {
      /* Reduce the CPU load if no new messages are available in the FIFO.   */
      /* Wait pollPeriod (default 30 seconds), catching SIGTERM, SIGINT      */
      /* and SIGIO/SIGPOLL.                                                  */
      /* If new data are written to the FIFO in the meantime, the signal     */
      /* SIGIO/SIGPOLL is raised and sigtimedwait() stop sleeping            */
      /* immediately.                                                        */
      if(deBug & 0x2)mPrintf(errU,DEBUG,__func__,0,"no message(s) found.");
      printChangedSettings();
      if(suppressedMsgN>oSuppressedMsgN+1000)
      {
        memset(cBuf,0,BUF_SIZE);
        snprintf(cBuf,BUF_SIZE,"%s(%s): %s(): (%llu repeated messages "
                 "suppressed).",pName,utgid,__func__,suppressedMsgN);
        headerPrepend(cBuf,DEBUG);
        dis_update_service(logSvcID);
        /* rotate the mStor storLineN buffers and save message in mStor */
        pthread_mutex_lock(&settingsLock);
        if(storLineN)
        {
          cStor=mStor[storLineN-1];
          for(j=storLineN-1;j>0;j--)mStor[j]=mStor[j-1];
          mStor[0]=cStor;
          strcpy(cStor,cBuf);
        }
        pthread_mutex_unlock(&settingsLock);
        dis_update_service(lastLogSvcID);
        mPrintf(errU,DEBUG,__func__,0,"(%llu repeated messages suppressed).",
                suppressedMsgN);
       oSuppressedMsgN=suppressedMsgN;
      }
      now=time(NULL);
      if(markPeriod && now>lastTime+markPeriod*60)
      {
        snprintf(cBuf,BUF_SIZE,"%s(%s): %s(): -- MARK --",pName,utgid,__func__);
        headerPrepend(cBuf,DEBUG);
        dis_update_service(logSvcID);
        /* rotate the mStor storLineN buffers and save message in mStor */
        pthread_mutex_lock(&settingsLock);
        if(storLineN)
        {
          cStor=mStor[storLineN-1];
          for(j=storLineN-1;j>0;j--)mStor[j]=mStor[j-1];
          mStor[0]=cStor;
          strcpy(cStor,cBuf);
        }
        pthread_mutex_unlock(&settingsLock);
        dis_update_service(lastLogSvcID);
        lastTime=now;
      }
      pollPeriodTS.tv_sec=pollPeriodSec;
      pollPeriodTS.tv_nsec=pollPeriodNSec;
      signo=sigtimedwait(&signalMask,NULL,&pollPeriodTS);
      if((deBug & 0x1) && signo!=-1)
        mPrintf(errU,DEBUG,__func__,0,"Signal %d received.",signo);
      if(signo==SIGTERM||signo==SIGINT)signalHandler(signo);
    }
    /*.......................................................................*/
  }
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
/* DIM handler for log service */
void logSvcHandler(long *tag,int **address,int *size)
{
  *address=(int*)cBuf;
  *size=1+strlen(cBuf);
  return;
}
/*****************************************************************************/
/* DIM handler for lastLog service */
void lastLogSvcHandler(long *tag,int **address,int *size)
{
  static char *emptyBuf="";
  pthread_mutex_lock(&settingsLock);
  if(storLineN)
  {
    int i,j;
    int isRepeated=0;
    int msgLen=0,severity=0;
    memset(storBuf,0,storLineN*BUF_SIZE);
    for(i=storLineN-1;i>=0;i--)
    {
      /*---------------------------------------------------------------------*/
      /* filter using severity */
      if(severityThreshold)
      {
        msgLen=strlen(mStor[i]);
        if(msgLen>18 && !strncmp(mStor[i]+12,"[VERB]",6))severity=1;
        else if(msgLen>19 && !strncmp(mStor[i]+12,"[DEBUG]",7))severity=2;
        else if(msgLen>18 && !strncmp(mStor[i]+12,"[INFO]",6))severity=3;
        else if(msgLen>18 && !strncmp(mStor[i]+12,"[WARN]",6))severity=4;
        else if(msgLen>19 && !strncmp(mStor[i]+12,"[ERROR]",7))severity=5;
        else if(msgLen>19 && !strncmp(mStor[i]+12,"[FATAL]",7))severity=6;
        else severity=2;
        if(severity<severityThreshold)continue;
      }
      /* filter using IEEE/POSIX regular expression */
      if(filterRegExpPattern && regexec(&filterRegExp,mStor[i],(size_t)0,NULL,
         0))
        continue;
      /* filter using IEEE/POSIX negate regular expression */
      if(xFilterRegExpPattern && !regexec(&xFilterRegExp,mStor[i],(size_t)0,
         NULL,0))
        continue;
      /* filter using IEEE/POSIX wildcard pattern */
      if(filterWildCardPattern && fnmatch(filterWildCardPattern,mStor[i],0))
        continue;
      /* filter using IEEE/POSIX negate wildcard pattern */
      if(xFilterWildCardPattern && !fnmatch(xFilterWildCardPattern,mStor[i],0))
        continue;
      /*---------------------------------------------------------------------*/
      /* suppress duplicate strings */
      isRepeated=0;
      if(minLd>-1||minWd>-1||ptCmp)
      {
        for(j=i+1;j<i+chkdLineN;j++)
        {
          if(j<storLineN)
          {
            char *cStr=mStor[i],*oStr=mStor[j];
            int cutStrLen=0;
            int k=0;
            /* shorten string to be compared using -c */
            if(cutCol)
            {
              if(strlen(cStr)>cutCol)cStr+=cutCol;
              else cStr="";
              if(strlen(oStr)>cutCol)oStr+=cutCol;
              else oStr="";
            }
            /* shorten string to be compared using -C */
            if((cutStrLen=strlen(cutStr)))
            {
              for(k=0;k<cutStrLen;k++)
              {
                char *p=NULL;
                if((p=strchr(cStr,cutStr[k]))!=NULL)cStr=p;
                else{cStr="";break;}
                if((p=strchr(oStr,cutStr[k]))!=NULL)oStr=p;
                else{oStr="";break;}
              }
            }
            /* compare strings */
            if(ptCmp)                                 /* punctual comparison */
            {
              if(!strcmp(cStr,oStr))
              {
                isRepeated=1;
                break;
              }
            }
            if(minLd>-1)                           /* Levenshtein comparison */
            {
              int d;
              d=levDist(cStr,oStr);
              if(deBug & 0x10)rPrintf(errU,2,"curr=\"%s\", old%d=\"%s\", "
                                      "levDist=%d\n",cStr,j,oStr,d);
              if(d>=0 && d<=minLd)
              {
                isRepeated=1;
                break;
              }
            }
            if(minWd>-1)                         /* punctual word comparison */
            {
              int res;
              res=wordCmp(cStr,oStr,minWd);
              if(deBug & 0x10)rPrintf(errU,2,"curr=\"%s\", old%d=\"%s\", "
                                      "wordCmp=%d\n",cStr,j,oStr,res);
              if(res)
              {
                isRepeated=1;
                break;
              }
            }
            /* end strings comparison */
          }
        }
      }
      /*---------------------------------------------------------------------*/
      if(!isRepeated)strcat(storBuf,mStor[i]);
    }                                         /* for(i=storLineN-1;i>=0;i--) */
    *address=(int*)storBuf;
    *size=1+strlen(storBuf);
  }
  else
  {
    *address=(int*)emptyBuf;
    *size=1+strlen(emptyBuf);
  }
  pthread_mutex_unlock(&settingsLock);
  return;
}
/*****************************************************************************/
void getPropertiesSvcHandler(long *tag,int **address,int *size)
{
  /* compose settings string */
  stStringL=1;
  stString=(char*)realloc(stString,stStringL*sizeof(char));
  stString[0]='\0';
  /*-------------------------------------------------------------------------*/
  if(severityThreshold)
  {
    stStringL+=snprintf(NULL,0,"-l %s ",sl[severityThreshold]);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-l %s ",
            sl[severityThreshold]);
  }
  if(filterRegExpPattern)
  {
    stStringL+=snprintf(NULL,0,"-F %s ",filterRegExpPattern);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-F %s ",
            filterRegExpPattern);
  }
  if(xFilterRegExpPattern)
  {
    stStringL+=snprintf(NULL,0,"-X %s ",xFilterRegExpPattern);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-X %s ",
            xFilterRegExpPattern);
  }
  if(filterWildCardPattern)
  {
    stStringL+=snprintf(NULL,0,"-f %s ",filterWildCardPattern);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-f %s ",
            filterWildCardPattern);
  }
  if(xFilterWildCardPattern)
  {
    stStringL+=snprintf(NULL,0,"-x %s ",xFilterWildCardPattern);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-x %s ",
            xFilterWildCardPattern);
  }
  /*-------------------------------------------------------------------------*/
  if(ptCmp)
  {
    stStringL+=snprintf(NULL,0,"-E 1 ");
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-E 1 ");
  }
  if(minWd>-1)
  {
    stStringL+=snprintf(NULL,0,"-W %d ",minWd);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-W %d ",minWd);
  }
  if(minLd>-1)
  {
    stStringL+=snprintf(NULL,0,"-L %d ",minLd);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-L %d ",minLd);
  }
  if(chkdLineN>1)
  {
    stStringL+=snprintf(NULL,0,"-n %d ",chkdLineN);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-n %d ",chkdLineN);
  }
  if(cutCol>0)
  {
    stStringL+=snprintf(NULL,0,"-c %zu ",cutCol);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-c %zu ",cutCol);
  }
  if(cutStr[0]!='\0')
  {
    stStringL+=snprintf(NULL,0,"-C %s ",cutStr);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-C %s ",cutStr);
  }
  /*-------------------------------------------------------------------------*/
  if(storLineN>0)
  {
    stStringL+=snprintf(NULL,0,"-S %d",storLineN);
    stString=(char*)realloc(stString,stStringL*sizeof(char));
    sprintf(stString+strlen(stString),"-S %d",storLineN);
  }
  /*-------------------------------------------------------------------------*/
  /* chop final space */
  if(stStringL>1 && stString[stStringL-2]==' ')
  {
    stString[stStringL-2]='\0';
    stStringL--;
  }
  *address=(int*)stString;
  *size=stStringL;
  return;
}
/*****************************************************************************/
void setPropertiesCmdHandler(long *tag,int *cmd,int *size)
{
  /*.........................................................................*/
  /* counters */
  int i;
  /*.........................................................................*/
  /* global options */
  int doReset=0;
  int dupSupForeseen=0;
  /*.........................................................................*/
  /* command processing variables */
  char *cmnd=NULL;
  char *argz=NULL;
  size_t argz_len=0;
  int rArgc=0;
  char **rArgv=NULL;
  int flag;
  int rv=0;
  /*.........................................................................*/
  /* filter parameters */
  char *nFilterRegExpPattern=NULL;
  char *nXFilterRegExpPattern=NULL;
  char *nFilterWildCardPattern=NULL;
  char *nXFilterWildCardPattern=NULL;
  int nSeverityThreshold=INT_MIN;
  /*.........................................................................*/
  /* duplicate suppression parameters */
  int nPtCmp=INT_MIN;
  int nMinLd=INT_MIN;
  int nMinWd=INT_MIN;
  int nChkdLineN=INT_MIN;
  int nCutCol=INT_MIN;
  char *nCutStr=NULL;
  /*.........................................................................*/
  /* last message buffer parameters */
  int nStorLineN=INT_MIN;
  /*-------------------------------------------------------------------------*/
  asprintf(&cmnd,"setProperties %s",(char*)cmd);
  mPrintf(errU,DEBUG,__func__,0,"Received command \"%s\".",cmnd);
  /*-------------------------------------------------------------------------*/
  /* convert the command string in an argz vector */
  if(argz_create_sep(cmnd,32,&argz,&argz_len))
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": argz_create_sep(): %s!",cmnd,
            strerror(errno));
    FREE_SETPARAMS_CMD_HANDLER_HEAP;
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* convert the argz vector into argv format */
  rArgc=argz_count(argz,argz_len);
  rArgv=(char**)malloc((rArgc+1)*sizeof(char*));
  argz_extract(argz,argz_len,rArgv);
  /*-------------------------------------------------------------------------*/
  /* process DIM command arguments */
  if(rArgc>1)
  {
    optind=0;
    opterr=0;
    while((flag=getopt(rArgc,rArgv,"f:F:l:x:X:E:W:L:n:c:C:S:r"))!=EOF)
    {
      char *p=NULL;
      switch(flag)
      {
        /*...................................................................*/
        /* global options */
        case 'r':                                 /* reset values to default */
          doReset=1;
          break;
        /*...................................................................*/
        /* filter options */
        case 'l':
        nSeverityThreshold=(int)strtol(optarg,&p,0);
        if(!p||!*p)break;
        if(p==optarg)
        {
          if(!strncasecmp(optarg,"ALL",3))nSeverityThreshold=0;
          else if(!strncasecmp(optarg,"VERB",4))nSeverityThreshold=1;
          else if(!strncasecmp(optarg,"DEBUG",5))nSeverityThreshold=2;
          else if(!strncasecmp(optarg,"INFO",4))nSeverityThreshold=3;
          else if(!strncasecmp(optarg,"WARN",4))nSeverityThreshold=4;
          else if(!strncasecmp(optarg,"ERROR",5))nSeverityThreshold=5;
          else if(!strncasecmp(optarg,"FATAL",5))nSeverityThreshold=6;
          else 
          {
            mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid "
                    "severity threshold %s!",cmnd,optarg);
            FREE_SETPARAMS_CMD_HANDLER_HEAP;
            return;
          }
        }
        else 
        {
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid "
                  "severity threshold %s!",cmnd,optarg);
          FREE_SETPARAMS_CMD_HANDLER_HEAP;
          return;
        }
        break;
        case 'F':
          nFilterRegExpPattern=optarg;
          break;
        case 'f':
          nFilterWildCardPattern=optarg;
          break;
        case 'X':
          nXFilterRegExpPattern=optarg;
          break;
        case 'x':
          nXFilterWildCardPattern=optarg;
          break;
        /*...................................................................*/
        /* duplicate suppression options */
        case 'E':
          nPtCmp=(int)strtol(optarg,(char**)NULL,0);
          break;
        case 'L':
          nMinLd=(int)strtol(optarg,(char**)NULL,0);
          break;
        case 'W':
          nMinWd=(int)strtol(optarg,(char**)NULL,0);
          break;
        case 'n':
          nChkdLineN=(int)strtol(optarg,(char**)NULL,0);
          break;
        case 'c':
          nCutCol=(int)strtol(optarg,(char**)NULL,0);
          break;
        case 'C':
          nCutStr=optarg;
          break;
        /*...................................................................*/
        /* last message buffer options */
        case 'S':
          nStorLineN=(int)strtol(optarg,(char**)NULL,0);
          break;
        /*...................................................................*/
        default:
          mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid "
                  "option -%c!",cmnd,optopt);
          FREE_SETPARAMS_CMD_HANDLER_HEAP;
          return;
        /*...................................................................*/
      }                                                      /* switch(flag) */
    }                                         /* while((flag=getopt())!=EOF) */
  }                                                           /* if(rArgc>1) */
  else                                                           /* rArgc<=1 */
  {
    FREE_SETPARAMS_CMD_HANDLER_HEAP;
    return;
  }                                                              /* rArgc<=1 */
  /*-------------------------------------------------------------------------*/
  /* check cmdline options */
  if(optind!=rArgc && rArgv[optind] && rArgv[optind][0]!='\0')
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid "
            "non-option argument: \"%s\"!",cmnd,rArgv[optind]);
    FREE_SETPARAMS_CMD_HANDLER_HEAP;
    return;
  }
  if(nSeverityThreshold!=INT_MIN&&(nSeverityThreshold<0||nSeverityThreshold>6))
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid "
            "severity threshold: %d! Severity threshold can only be: ALL=0, "
            "VERB=1, DEBUG=2, INFO=3, WARN=4, ERROR=5, FATAL=6!",cmnd,
            nSeverityThreshold);
    FREE_SETPARAMS_CMD_HANDLER_HEAP;
    return;
  }
  if(nPtCmp!=INT_MIN && nPtCmp!=0 && nPtCmp!=1)
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid argument "
            " for -E switch: %d! -E argument must be either 0 or 1!",cmnd,
            nPtCmp);
    FREE_SETPARAMS_CMD_HANDLER_HEAP;
    return;
  }
  if(nMinLd!=INT_MIN && nMinLd<-1)
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid value for "
            "minimum Levenshtein distance between messages: %d! Minimum "
            "Levenshtein distance between messages must be >=-1!",cmnd,nMinLd);
    FREE_SETPARAMS_CMD_HANDLER_HEAP;
    return;
  }
  if(nMinWd!=INT_MIN && nMinWd<-1)
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid value for "
            "minimum word difference between messages: %d! Minimum word "
            "difference between messages must be >=-1!",cmnd,nMinWd);
    FREE_SETPARAMS_CMD_HANDLER_HEAP;
    return;
  }
  if(nChkdLineN!=INT_MIN && nChkdLineN<1)
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid value for "
            "the number of compared messages: %d! Minimum number of compared"
            "messages must be >=1!",cmnd,nChkdLineN);
    FREE_SETPARAMS_CMD_HANDLER_HEAP;
    return;
  }
  if(nCutCol!=INT_MIN && nCutCol<0)
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid value for "
            "the number of characters skipped in comparison: %d! The number "
            "of characters skipped in comparison must be >=0!",cmnd,nCutCol);
    FREE_SETPARAMS_CMD_HANDLER_HEAP;
    return;
  }
  if(nStorLineN!=INT_MIN && nStorLineN<0)
  {
    mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": getopt(): invalid value for "
            "the number of messages stored in last_log: %d! The number "
            "of messages stored in last_log must be >=0!",cmnd,nStorLineN);
    FREE_SETPARAMS_CMD_HANDLER_HEAP;
    return;
  }
  /* if duplicate suppression is foreseen, set chkdLineN at least to 2 */
  dupSupForeseen=0;
  if(nMinLd!=INT_MIN && nMinLd>-1)dupSupForeseen=1;
  if(nMinLd==INT_MIN && minLd>-1)dupSupForeseen=1;
  if(nMinWd!=INT_MIN && nMinWd>-1)dupSupForeseen=1;
  if(nMinWd==INT_MIN && minWd>-1)dupSupForeseen=1;
  if(nPtCmp!=INT_MIN && nPtCmp==1)dupSupForeseen=1;
  if(nPtCmp==INT_MIN && ptCmp==1)dupSupForeseen=1;
  if(dupSupForeseen && (nChkdLineN!=INT_MIN && nChkdLineN<2))
  {
    memset(cBuf,0,BUF_SIZE);
    snprintf(cBuf,BUF_SIZE,"%s(%s): %s(): CMD: \"%s\": Since duplicate "
            "suppression is required, the number of compared messages (set "
            "to %d) is forced to 2!",pName,utgid,__func__,cmnd,nChkdLineN);
    headerPrepend(cBuf,WARN);
    dis_update_service(logSvcID);
    nChkdLineN=2;
  }
  if(dupSupForeseen && (nChkdLineN==INT_MIN && chkdLineN<2))
  {
    memset(cBuf,0,BUF_SIZE);
    snprintf(cBuf,BUF_SIZE,"%s(%s): %s(): CMD: \"%s\": Since duplicate "
            "suppression is required, the number of compared messages (set "
            "to %d) is forced to 2!",pName,utgid,__func__,cmnd,chkdLineN);
    headerPrepend(cBuf,WARN);
    dis_update_service(logSvcID);
    nChkdLineN=2;
  }
  /*-------------------------------------------------------------------------*/
  /* reset to default */
  if(doReset)
  {
    nFilterRegExpPattern="";
    nXFilterRegExpPattern="";
    nFilterWildCardPattern="";
    nXFilterWildCardPattern="";
    nSeverityThreshold=0;
    nPtCmp=0;
    nMinLd=-1;
    nMinWd=-1;
    nChkdLineN=1;
    nCutCol=0;
    nCutStr="";
    nStorLineN=0;
  }
  /*-------------------------------------------------------------------------*/
  /* set severity threshold filter */
  if(nSeverityThreshold!=INT_MIN && nSeverityThreshold!=severityThreshold)
  {
    pthread_mutex_lock(&settingsLock);
    severityThreshold=nSeverityThreshold;
    pthread_mutex_unlock(&settingsLock);
  }
  /*-------------------------------------------------------------------------*/
  /* compile and set regular expression filter */
  if(nFilterRegExpPattern)
  {
    if(!strcmp(nFilterRegExpPattern,"")||                  /* null filtering */
       !strcmp(nFilterRegExpPattern,"''")||
       !strcmp(nFilterRegExpPattern,"\"\""))
    {
      pthread_mutex_lock(&settingsLock);
      if(filterRegExpPattern)
      {
        free(filterRegExpPattern);
        filterRegExpPattern=NULL;
        regfree(&filterRegExp);
      }
      pthread_mutex_unlock(&settingsLock);
    }                                                      /* null filtering */
    else                                                   /* real filtering */
    {
      pthread_mutex_lock(&settingsLock);
      if(filterRegExpPattern)
      {
        free(filterRegExpPattern);
        filterRegExpPattern=NULL;
        regfree(&filterRegExp);
      }
      rv=regcomp(&filterRegExp,nFilterRegExpPattern,REG_EXTENDED|REG_NOSUB);
      if(rv!=0)                                      /* regexp compile error */
      {
        char *errMsg=NULL;
        int errSz=0;
        errSz=regerror(rv,&filterRegExp,(char*)NULL,(size_t)0);
        errMsg=(char*)malloc(errSz*sizeof(char));
        regerror(rv,&filterRegExp,errMsg,errSz);
        mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": regcomp(): Cannot compile "
                "regular expression \"%s\". Reported error: \"%s\"!",cmnd,
                nFilterRegExpPattern,errMsg);
        if(errMsg)free(errMsg);
      }                                              /* regexp compile error */
      else                                        /* no regexp compile error */
      {
        filterRegExpPattern=strdup(nFilterRegExpPattern);
      }                                           /* no regexp compile error */
      pthread_mutex_unlock(&settingsLock);
    }                                                      /* real filtering */
  }                                              /* if(nFilterRegExpPattern) */
  /*-------------------------------------------------------------------------*/
  /* compile and set negate regular expression filter */
  if(nXFilterRegExpPattern)
  {
    if(!strcmp(nXFilterRegExpPattern,"")||                 /* null filtering */
       !strcmp(nXFilterRegExpPattern,"''")||
       !strcmp(nXFilterRegExpPattern,"\"\""))
    {
      pthread_mutex_lock(&settingsLock);
      if(xFilterRegExpPattern)
      {
        free(xFilterRegExpPattern);
        xFilterRegExpPattern=NULL;
        regfree(&xFilterRegExp);
      }
      pthread_mutex_unlock(&settingsLock);
    }                                                      /* null filtering */
    else                                                   /* real filtering */
    {
      pthread_mutex_lock(&settingsLock);
      if(xFilterRegExpPattern)
      {
        free(xFilterRegExpPattern);
        xFilterRegExpPattern=NULL;
        regfree(&xFilterRegExp);
      }
      rv=regcomp(&xFilterRegExp,nXFilterRegExpPattern,REG_EXTENDED|REG_NOSUB);
      if(rv!=0)                                      /* regexp compile error */
      {
        char *errMsg=NULL;
        int errSz=0;
        errSz=regerror(rv,&xFilterRegExp,(char*)NULL,(size_t)0);
        errMsg=(char*)malloc(errSz*sizeof(char));
        regerror(rv,&xFilterRegExp,errMsg,errSz);
        mPrintf(errU,ERROR,__func__,0,"CMD: \"%s\": regcomp(): Cannot compile "
                "regular expression \"%s\". Reported error: \"%s\"!",cmnd,
                nXFilterRegExpPattern,errMsg);
        if(errMsg)free(errMsg);
      }                                              /* regexp compile error */
      else                                        /* no regexp compile error */
      {
        xFilterRegExpPattern=strdup(nXFilterRegExpPattern);
      }                                           /* no regexp compile error */
      pthread_mutex_unlock(&settingsLock);
    }                                                      /* real filtering */
  }                                             /* if(nXFilterRegExpPattern) */
  /*-------------------------------------------------------------------------*/
  /* set wildcard pattern filter */
  if(nFilterWildCardPattern)
  {
    if(!strcmp(nFilterWildCardPattern,"")||                /* null filtering */
       !strcmp(nFilterWildCardPattern,"''")||
       !strcmp(nFilterWildCardPattern,"\"\""))
    {
      pthread_mutex_lock(&settingsLock);
      if(filterWildCardPattern)
      {
        free(filterWildCardPattern);
        filterWildCardPattern=NULL;
      }
      pthread_mutex_unlock(&settingsLock);
    }                                                      /* null filtering */
    else                                                   /* real filtering */
    {
      pthread_mutex_lock(&settingsLock);
      if(filterWildCardPattern)
      {
        free(filterWildCardPattern);
        filterWildCardPattern=NULL;
      }
      filterWildCardPattern=strdup(nFilterWildCardPattern);
      pthread_mutex_unlock(&settingsLock);
    }                                                      /* real filtering */
  }                                            /* if(nFilterWildCardPattern) */
  /*-------------------------------------------------------------------------*/
  /* set negate wildcard pattern filter */
  if(nXFilterWildCardPattern)
  {
    if(!strcmp(nXFilterWildCardPattern,"")||               /* null filtering */
       !strcmp(nXFilterWildCardPattern,"''")||
       !strcmp(nXFilterWildCardPattern,"\"\""))
    {
      pthread_mutex_lock(&settingsLock);
      if(xFilterWildCardPattern)
      {
        free(xFilterWildCardPattern);
        xFilterWildCardPattern=NULL;
      }
      pthread_mutex_unlock(&settingsLock);
    }                                                      /* null filtering */
    else                                                   /* real filtering */
    {
      pthread_mutex_lock(&settingsLock);
      if(xFilterWildCardPattern)
      {
        free(xFilterWildCardPattern);
        xFilterWildCardPattern=NULL;
      }
      xFilterWildCardPattern=strdup(nXFilterWildCardPattern);
      pthread_mutex_unlock(&settingsLock);
    }                                                      /* real filtering */
  }                                           /* if(nXFilterWildCardPattern) */
  /*-------------------------------------------------------------------------*/
  /* set duplicate suppression parameters */
  pthread_mutex_lock(&settingsLock);
  if(nPtCmp!=INT_MIN)ptCmp=nPtCmp;
  if(nMinLd!=INT_MIN)minLd=nMinLd;
  if(nMinWd!=INT_MIN)minWd=nMinWd;
  if(nCutCol!=INT_MIN)cutCol=nCutCol;
  if(nCutStr)
  {
    if(!strcmp(nCutStr,"")||                                     /* null cut */
       !strcmp(nCutStr,"''")||
       !strcmp(nCutStr,"\"\""))
    {
      if(cutStr)
      {
        free(cutStr);
        cutStr=NULL;
      }
      cutStr=strdup("");
    }                                                            /* null cut */
    else                                                         /* real cut */
    {
      if(cutStr)
      {
        free(cutStr);
        cutStr=NULL;
      }
      /* nCutStr is freed by FREE_SETPARAMS_CMD_HANDLER_HEAP */
      cutStr=strdup(nCutStr);
    }                                                            /* real cut */
  }                                                           /* if(nCutStr) */
  if(nChkdLineN!=INT_MIN)
  {
    if(nChkdLineN==chkdLineN){}
    else if(nChkdLineN>chkdLineN)
    {
      mBuf=(char**)realloc(mBuf,nChkdLineN*sizeof(char*));
      for(i=chkdLineN;i<nChkdLineN;i++)
      {
        mBuf[i]=(char*)malloc(BUF_SIZE*sizeof(char));
        memset(mBuf[i],0,BUF_SIZE);
      }
      chkdLineN=nChkdLineN;
    }
    else if(nChkdLineN<chkdLineN)
    {
      for(i=nChkdLineN;i<chkdLineN;i++)
      {
        if(mBuf[i])free(mBuf[i]);
      }
      mBuf=(char**)realloc(mBuf,nChkdLineN*sizeof(char*));
      chkdLineN=nChkdLineN;
    }
  }
  pthread_mutex_unlock(&settingsLock);
  /*-------------------------------------------------------------------------*/
  /* set last message buffer parameters */
  if(nStorLineN!=INT_MIN)
  {
    pthread_mutex_lock(&settingsLock);
    if(nStorLineN==storLineN){}
    else if(nStorLineN>storLineN)
    {
      /* add buffers after highest mStor[i] index */
      mStor=(char**)realloc(mStor,nStorLineN*sizeof(char*));
      for(i=storLineN;i<nStorLineN;i++)
      {
        mStor[i]=(char*)malloc(BUF_SIZE*sizeof(char));
        memset(mStor[i],0,BUF_SIZE);
      }
      storBuf=(char*)realloc(storBuf,nStorLineN*BUF_SIZE*sizeof(char));
      storLineN=nStorLineN;
    }
    else if(nStorLineN<storLineN)
    {
      /* remove buffers at higher mStor[i] indexes */
      for(i=nStorLineN;i<storLineN;i++)
      {
        if(mStor[i])free(mStor[i]);
      }
      mStor=(char**)realloc(mStor,nStorLineN*sizeof(char*));
      storBuf=(char*)realloc(storBuf,nStorLineN*BUF_SIZE*sizeof(char));
      storLineN=nStorLineN;
    }
    pthread_mutex_unlock(&settingsLock);
  }
  /*-------------------------------------------------------------------------*/
  dis_update_service(getPropertiesSvcID);
  /*-------------------------------------------------------------------------*/
  FREE_SETPARAMS_CMD_HANDLER_HEAP;
  pthread_kill(mainPtid,SIGIO);   /* speed-up call to printChangedSettings() */
  /*-------------------------------------------------------------------------*/
  return;
}
/*****************************************************************************/
void printChangedSettings(void)
{
  static int isFirstTime=1;
  static int oPtCmp=0,oMinLd=-1,oMinWd=-1,oChkdLineN=1;
  static int oStorLineN=0;
  static size_t oCutCol=0;
  static char *oCutStr=NULL;
  static char *oFilterRegExpPattern=NULL,*oFilterWildCardPattern=NULL;
  static char *oXFilterRegExpPattern=NULL,*oXFilterWildCardPattern=NULL;
  static int oSeverityThreshold=0;
  int filterSettingsChanged=0;
  int dupSupSettingsChanged=0;
  int storSettingsChanged=0;
  /*-------------------------------------------------------------------------*/
  if(isFirstTime)
  {
    oPtCmp=ptCmp;
    oMinLd=minLd;
    oMinWd=minWd;
    oCutCol=cutCol;
    oChkdLineN=chkdLineN;
    oStorLineN=storLineN;
    oSeverityThreshold=severityThreshold;
    if(cutStr)oCutStr=strdup(cutStr);
    if(filterRegExpPattern)oFilterRegExpPattern=strdup(filterRegExpPattern);
    if(xFilterRegExpPattern)oXFilterRegExpPattern=strdup(xFilterRegExpPattern);
    if(filterWildCardPattern)
      oFilterWildCardPattern=strdup(filterWildCardPattern);
    if(xFilterWildCardPattern)
      oXFilterWildCardPattern=strdup(xFilterWildCardPattern);
    isFirstTime=0;
  }
  else                                                     /* not first time */
  {
    filterSettingsChanged=0;
    dupSupSettingsChanged=0;
    storSettingsChanged=0;
    /*.......................................................................*/
    if(ptCmp!=oPtCmp||minLd!=oMinLd||minWd!=oMinWd||chkdLineN!=oChkdLineN||
       cutCol!=oCutCol)dupSupSettingsChanged=1;
    if((cutStr&&!oCutStr)||(!cutStr&&oCutStr))dupSupSettingsChanged=1;
    else if(cutStr&&oCutStr&&strcmp(cutStr,oCutStr))dupSupSettingsChanged=1;
    /*.......................................................................*/
    if(severityThreshold!=oSeverityThreshold)filterSettingsChanged=1;

    if((filterRegExpPattern&&!oFilterRegExpPattern)||
       (!filterRegExpPattern&&oFilterRegExpPattern))filterSettingsChanged=1;
    else if(filterRegExpPattern&&oFilterRegExpPattern&&
            strcmp(filterRegExpPattern,oFilterRegExpPattern))
      filterSettingsChanged=1;
    if((xFilterRegExpPattern&&!oXFilterRegExpPattern)||
       (!xFilterRegExpPattern&&oXFilterRegExpPattern))filterSettingsChanged=1;
    else if(xFilterRegExpPattern&&oXFilterRegExpPattern&&
            strcmp(xFilterRegExpPattern,oXFilterRegExpPattern))
      filterSettingsChanged=1;
    if((filterWildCardPattern&&!oFilterWildCardPattern)||
       (!filterWildCardPattern&&oFilterWildCardPattern))
      filterSettingsChanged=1;
    else if(filterWildCardPattern&&oFilterWildCardPattern&&
            strcmp(filterWildCardPattern,oFilterWildCardPattern))
      filterSettingsChanged=1;
    if((xFilterWildCardPattern&&!oXFilterWildCardPattern)||
       (!xFilterWildCardPattern&&oXFilterWildCardPattern))
      filterSettingsChanged=1;
    else if(xFilterWildCardPattern&&oXFilterWildCardPattern&&
            strcmp(xFilterWildCardPattern,oXFilterWildCardPattern))
      filterSettingsChanged=1;
    /*.......................................................................*/
    if(storLineN!=oStorLineN)storSettingsChanged=1;
    /*.......................................................................*/
    if(filterSettingsChanged)
    {
      char *reMsg=NULL,*xReMsg=NULL,*wpMsg=NULL,*xWpMsg=NULL,*svMsg=NULL;
      if(severityThreshold)
        asprintf(&svMsg,"Severity filter: %s",sl[severityThreshold]);
      else
        asprintf(&svMsg,"no Severity filter");
      if(filterRegExpPattern)
        asprintf(&reMsg,"Regular Expression filter: \"%s\"",
                 filterRegExpPattern);
      else
        asprintf(&reMsg,"no Regular Expression filter");
      if(xFilterRegExpPattern)
        asprintf(&xReMsg,"Regular Expression negate filter: \"%s\"",
                 xFilterRegExpPattern);
      else
        asprintf(&xReMsg,"no Regular Expression negate filter");
      if(filterWildCardPattern)
        asprintf(&wpMsg,"Wildcard Pattern filter: \"%s\"",
                 filterWildCardPattern);
      else
        asprintf(&wpMsg,"no Wildcard Pattern filter");
      if(xFilterWildCardPattern)
        asprintf(&xWpMsg,"Wildcard Pattern negate filter: \"%s\"",
                 xFilterWildCardPattern);
      else
        asprintf(&xWpMsg,"no Wildcard Pattern negate filter");
      memset(cBuf,0,BUF_SIZE);
      snprintf(cBuf,BUF_SIZE,"%s(%s): %s(): Filter settings changed: %s, %s, "
               "%s, %s, %s.",pName,utgid,__func__,svMsg,reMsg,xReMsg,wpMsg,
               xWpMsg);
      headerPrepend(cBuf,INFO);
      dis_update_service(logSvcID);
      /* rotate the mStor storLineN buffers and save message in mStor */
      int j;
      pthread_mutex_lock(&settingsLock);
      if(storLineN)
      {
        cStor=mStor[storLineN-1];
        for(j=storLineN-1;j>0;j--)mStor[j]=mStor[j-1];
        mStor[0]=cStor;
        strcpy(cStor,cBuf);
      }
      pthread_mutex_unlock(&settingsLock);
      dis_update_service(lastLogSvcID);
      free(reMsg);
      free(xReMsg);
      free(wpMsg);
      free(xWpMsg);
      free(svMsg);
      oSeverityThreshold=severityThreshold;
      if(filterRegExpPattern)
      {
        oFilterRegExpPattern=(char*)realloc(oFilterRegExpPattern,
                                            1+strlen(filterRegExpPattern));
        strcpy(oFilterRegExpPattern,filterRegExpPattern);
      }
      else
      {
        if(oFilterRegExpPattern)free(oFilterRegExpPattern);
        oFilterRegExpPattern=NULL;
      }
      if(xFilterRegExpPattern)
      {
        oXFilterRegExpPattern=(char*)realloc(oXFilterRegExpPattern,
                                            1+strlen(xFilterRegExpPattern));
        strcpy(oXFilterRegExpPattern,xFilterRegExpPattern);
      }
      else
      {
        if(oXFilterRegExpPattern)free(oXFilterRegExpPattern);
        oXFilterRegExpPattern=NULL;
      }
      if(filterWildCardPattern)
      {
        oFilterWildCardPattern=(char*)realloc(oFilterWildCardPattern,
                                              1+strlen(filterWildCardPattern));
        strcpy(oFilterWildCardPattern,filterWildCardPattern);
      }
      else
      {
        if(oFilterWildCardPattern)free(oFilterWildCardPattern);
        oFilterWildCardPattern=NULL;
      }
      if(xFilterWildCardPattern)
      {
        oXFilterWildCardPattern=(char*)realloc(oXFilterWildCardPattern,
                                              1+strlen(xFilterWildCardPattern));
        strcpy(oXFilterWildCardPattern,xFilterWildCardPattern);
      }
      else
      {
        if(oXFilterWildCardPattern)free(oXFilterWildCardPattern);
        oXFilterWildCardPattern=NULL;
      }
    }
    /*.......................................................................*/
    if(dupSupSettingsChanged)
    {
      char *cmpMsg=NULL,*ldMsg=NULL,*wdMsg=NULL;
      char *clMsg=NULL,*cutColMsg=NULL,*cutStrMsg=NULL;
      if(!ptCmp && minLd<0 && minWd<0)
        asprintf(&cmpMsg,"No repeated messages suppression. ");
      else if(ptCmp)
        asprintf(&cmpMsg,"Suppress identical messages. ");
      else cmpMsg=strdup("");
      if(minLd>-1)
        asprintf(&ldMsg,"Minimum Levenshtein distance between messages=%d. ",
                 minLd);
      else ldMsg=strdup("");
      if(minWd>-1)
        asprintf(&wdMsg,"Minimum word difference between messages=%d. ",minWd);
      else wdMsg=strdup("");
      if(chkdLineN>1)
        asprintf(&clMsg,"Number of compared messages=%d. ",chkdLineN);
      else clMsg=strdup("");
      if(cutCol>0)
        asprintf(&cutColMsg,"Skip %zu characters in comparison. ",cutCol);
      else cutColMsg=strdup("");
      if(cutStr[0]!=0)
        asprintf(&cutStrMsg,"Skip characters in comparison until \"%s\" "
                 "passed. ",cutStr);
      else cutStrMsg=strdup("");
      memset(cBuf,0,BUF_SIZE);
      snprintf(cBuf,BUF_SIZE,"%s(%s): %s(): Duplicate suppression settings "
               "changed: %s%s%s%s%s%s", pName,utgid,__func__,cmpMsg,ldMsg,
               wdMsg,clMsg,cutColMsg,cutStrMsg);
      headerPrepend(cBuf,INFO);
      dis_update_service(logSvcID);
      /* rotate the mStor storLineN buffers and save message in mStor */
      int j;
      pthread_mutex_lock(&settingsLock);
      if(storLineN)
      {
        cStor=mStor[storLineN-1];
        for(j=storLineN-1;j>0;j--)mStor[j]=mStor[j-1];
        mStor[0]=cStor;
        strcpy(cStor,cBuf);
      }
      pthread_mutex_unlock(&settingsLock);
      dis_update_service(lastLogSvcID);
      free(cmpMsg);free(ldMsg);free(wdMsg);
      free(clMsg);free(cutColMsg);free(cutStrMsg);
      oPtCmp=ptCmp;
      oMinLd=minLd;
      oMinWd=minWd;
      oCutCol=cutCol;
      oChkdLineN=chkdLineN;
      if(cutStr)
      {
        oCutStr=(char*)realloc(oCutStr,1+strlen(cutStr));
        strcpy(oCutStr,cutStr);
      }
      else
      {
        if(oCutStr)free(oCutStr);
        oCutStr=NULL;
      }
    }
    /*.......................................................................*/
    if(storSettingsChanged)
    {
      char *slMsg=NULL;
      int j;
      asprintf(&slMsg,"number of stored messages=%d.",storLineN);
      memset(cBuf,0,BUF_SIZE);
      snprintf(cBuf,BUF_SIZE,"%s(%s): %s(): Message storage settings "
               "changed: %s", pName,utgid,__func__,slMsg);
      headerPrepend(cBuf,INFO);
      dis_update_service(logSvcID);
      /* rotate the mStor storLineN buffers and save message in mStor */
      pthread_mutex_lock(&settingsLock);
      if(storLineN)
      {
        cStor=mStor[storLineN-1];
        for(j=storLineN-1;j>0;j--)mStor[j]=mStor[j-1];
        mStor[0]=cStor;
        strcpy(cStor,cBuf);
      }
      pthread_mutex_unlock(&settingsLock);
      dis_update_service(lastLogSvcID);
      free(slMsg);
      oStorLineN=storLineN;
    }
    /*.......................................................................*/
  }
  return;
}
/*****************************************************************************/
int fifoCreate(char *pipePath)
{
  struct stat statBuf;
  mode_t savedMask;
  /*-------------------------------------------------------------------------*/
  if(access(pipePath,R_OK)==-1)                        /* FIFO access denied */
  {
    if(errno!=ENOENT)                                         /* FIFO exists */
    {
      mPrintf(errU,FATAL,__func__,0,"access(%s): %s.",pipePath,
              strerror(errno));
      return 1;
    }
    else                                              /* FIFO does not exist */
    {
      mPrintf(errU,DEBUG,__func__,0,"Creating FIFO: \"%s\"...",pipePath);
      savedMask=umask(0);
      if(mkfifo(pipePath,0666)==-1)                    /* create FIFO i-node */
      {
        mPrintf(errU,FATAL,__func__,0,"mkfifo(%s): %s.",pipePath,
                strerror(errno));
        return 1;
      }
      umask(savedMask);
    }
  }
  if(stat(pipePath,&statBuf)==-1)                       /* check FIFO i-node */
  {
    mPrintf(errU,FATAL,__func__,0,"stat(%s): %s.",pipePath,strerror(errno));
    return 1;
  }
  if(!S_ISFIFO(statBuf.st_mode))                     /* i-node is not a FIFO */
  {
    mPrintf(errU,DEBUG,__func__,0,"I-node: \"%s\" is not a FIFO!",pipePath);
    mPrintf(errU,DEBUG,__func__,0,"Removing file: \"%s\"...",pipePath);
    if(unlink(pipePath)==-1)                                /* remove i-node */
    {
      mPrintf(errU,FATAL,__func__,0,"unlink(%s): %s.",pipePath,
              strerror(errno));
      return 1;
    }
    mPrintf(errU,DEBUG,__func__,0,"Creating FIFO: \"%s\"...",pipePath);
    savedMask=umask(0);
    if(mkfifo(pipePath,0666)==-1)                      /* create FIFO i-node */
    {
      mPrintf(errU,FATAL,__func__,0,"mkfifo(%s): %s.",pipePath,
              strerror(errno));
      return 1;
    }
    umask(savedMask);
  }
  return 0;
}
/*****************************************************************************/
int sysLogProcHdr(char *buf)
{
  static char tmpBuf[BUF_SIZE];
  char *p=NULL;
  struct tm lNow;
  char sNow[13]="";
  char hostName[128]="";
  int hostNameLen=0;
  int severity;
  enum severity{UNKNOWN=-1,OK,VERB,DEBUG,INFO,WARN,ERROR,FATAL};
  static char *sl[7]={"[OK]","[VERB]","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
                      "[FATAL]"};
  /*-------------------------------------------------------------------------*/
  strncpy(tmpBuf,buf,BUF_SIZE);
  p=strptime(tmpBuf,sysLogDateFmt,&lNow);
  if(unlikely(!p))return 1;                        /* date conversion failed */
  strftime(sNow,13,"%b%d-%H%M%S",&lNow);
  /*-------------------------------------------------------------------------*/
  if(unlikely(!strlen(p)))return 1;                    /* nothing after date */
  p+=strspn(p," \t");                                 /* skip blanks, if any */
  if(unlikely(!strlen(p)))return 1;                  /* nothing after blanks */
  hostNameLen=strcspn(p," \t");
  if(hostNameLen>128-1)hostNameLen=128-1;
  snprintf(hostName,hostNameLen+1,"%s",p);
  p+=hostNameLen;                                   /* skip hostname, if any */
  if(unlikely(!strlen(p)))return 1;                /* nothing after hostName */
  p+=strspn(p," \t");                                 /* skip blanks, if any */
  if(unlikely(!strlen(p)))return 1;                  /* nothing after blanks */
  /*-------------------------------------------------------------------------*/
  /* try to guess severity from message content */
  if(!regexec(&fatalRegExp,p,(size_t)0,NULL,0))severity=FATAL;
  else if(!regexec(&errorRegExp,p,(size_t)0,NULL,0))severity=ERROR;
  else if(!regexec(&warnRegExp,p,(size_t)0,NULL,0))severity=WARN;
  else if(!regexec(&infoRegExp,p,(size_t)0,NULL,0))severity=INFO;
  else if(!regexec(&debugRegExp,p,(size_t)0,NULL,0))severity=DEBUG;
  else if(!regexec(&verbRegExp,p,(size_t)0,NULL,0))severity=VERB;
  else severity=INFO;
  /*-------------------------------------------------------------------------*/
  snprintf(buf,BUF_SIZE,"%s%s%s: %s",sNow,sl[severity],hostName,p);
  /* NULL-terminate truncated messages */
  if(!memchr(buf,0,BUF_SIZE))buf[BUF_SIZE-1]='\0';
  /* append '\n' if not present */
  if(!strchr(buf,'\n'))
  {
    int bufLen=strlen(buf);
    if(bufLen<BUF_SIZE-1)strcat(buf,"\n");
    else buf[bufLen-1]='\n';
  }
  return 0;
}
/*****************************************************************************/
int headerPrepend(char *buf,int severity) 
{
  static char tmpBuf[BUF_SIZE];
  enum severity{UNKNOWN=-1,OK,VERB,DEBUG,INFO,WARN,ERROR,FATAL};
  static char *sl[7]={"[OK]","[VERB]","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
                      "[FATAL]"};
  time_t now;
  struct tm lNow;
  char sNow[13]="";
  char hostName[128]="";
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  gethostname(hostName,128);
  p=strchr(hostName,'.');
  if(p)*p='\0';
  /*-------------------------------------------------------------------------*/
  now=time(NULL);
  localtime_r(&now,&lNow);
  strftime(sNow,13,"%b%d-%H%M%S",&lNow);
  /*-------------------------------------------------------------------------*/
  if(severity==UNKNOWN)
  {
    /* try to guess severity from message content */
    if(!regexec(&fatalRegExp,buf,(size_t)0,NULL,0))severity=FATAL;
    else if(!regexec(&errorRegExp,buf,(size_t)0,NULL,0))severity=ERROR;
    else if(!regexec(&warnRegExp,buf,(size_t)0,NULL,0))severity=WARN;
    else if(!regexec(&infoRegExp,buf,(size_t)0,NULL,0))severity=INFO;
    else if(!regexec(&debugRegExp,buf,(size_t)0,NULL,0))severity=DEBUG;
    else if(!regexec(&verbRegExp,buf,(size_t)0,NULL,0))severity=VERB;
    else severity=INFO;
  }
  /*-------------------------------------------------------------------------*/
  strncpy(tmpBuf,buf,BUF_SIZE);
  snprintf(buf,BUF_SIZE,"%s%s%s: %s",sNow,sl[severity],hostName,tmpBuf);
  /* NULL-terminate truncated messages */
  if(!memchr(buf,0,BUF_SIZE))buf[BUF_SIZE-1]='\0';
  /* append '\n' if not present */
  if(!strchr(buf,'\n'))
  {
    int bufLen=strlen(buf);
    if(bufLen<BUF_SIZE-1)strcat(buf,"\n");
    else buf[bufLen-1]='\n';
  }
  return 0;
}
/*****************************************************************************/
/* DIM error handler */
void disErrorHandler(int severity,int errorCode,char *message)
{
  if(severity>3||severity<0)severity=3;
  mPrintf(errU,severity+3,__func__,0,"%s! (error code = %d).",message,
          errorCode);
  if(severity==3)exit(1);
  return;
}
/*****************************************************************************/
/* signalHandler() called synchronously. Don't warry about async-signal-safe */
void signalHandler(int sig)
{
  mPrintf(errU|L_SYS|L_STD,WARN,__func__,0,"FMC Message Logger Server: "
          "TGID=%d, FIFO=\"%s\", DIM Service Path=\"%s/log\", terminated on "
          "signal %u (%s)!",getpid(),pipePath,srvPath,sig,
          sig2msg(sig));
  snprintf(cBuf,BUF_SIZE,"%s(%s): %s(): FMC Message Logger Server: TGID=%d, "
           "FIFO=\"%s\", DIM Service Path=\"%s/log\", terminated on signal "
           "%u (%s)!\n",pName,utgid,__func__,getpid(),pipePath,srvPath,sig,
           sig2msg(sig));
  headerPrepend(cBuf,WARN);
  dis_update_service(logSvcID);
  dis_remove_service(logSvcID);
  dis_remove_service(lastLogSvcID);
  dis_remove_service(versionSvcID);
  dis_remove_service(fmcVersionSvcID);
  dis_remove_service(successSvcID);
  dis_remove_service(getPropertiesSvcID);
  dis_remove_service(setPropertiesCmdID);
  dis_stop_serving();
  if(fifoFD)close(fifoFD);
  _exit(0);
}
/*****************************************************************************/
/* terminating '\0' is guaranteed                                            */
/* '\n' not guaranteed (not present if line truncated)                       */
/*****************************************************************************/
ssize_t readLine(void *bufPtr,size_t bufLen,int fd)
{
  int n,rc;
  char c,*ptr;
  /*-------------------------------------------------------------------------*/
  ptr=bufPtr;
  for(n=0;n<bufLen-1;n++)
  {
    rc=bufCharRead(fd,&c);
    if(rc==1)                                              /* character read */
    {
      if(c=='\r')c='\n';   /* ms-window carriage-return converted in newline */
      *ptr++=c;
      if(c=='\n')break;                   /* newline is stored, like fgets() */
    }
    else if(rc==0)                                           /* nothing read */
    {
      if(n==0)return 0;                                 /* EOF, no data read */
      else break;                                 /* EOF, some data was read */
    }
    else return -1;                /* rc=-1: read error; errno set by read() */
  }
  *ptr=0;                           /* null-terminate the line, like fgets() */
  return n+1;
}
/*****************************************************************************/
static ssize_t bufCharRead(int fd,char *ptr)
{
  /* Function called only in the main thread: don't worry about static */
  static int readCnt=0;
  static char *readPtr;
  static char readBuf[PIPE_BUF*PIPE_BUFFERS];
  /*-------------------------------------------------------------------------*/
  if(readCnt<=0)
  {
    while(1)
    {
      readCnt=read(fd,readBuf,sizeof(readBuf));
      if(readCnt>0)
      {
        readPtr=readBuf;
        break;
      }
      else if(readCnt==0)return 0;
      else if(readCnt<0 && errno!=EINTR)return -1;
    }
  }
  readCnt--;
  *ptr=*readPtr++;
  return 1;
}
/*****************************************************************************/
void shortUsage(void)
{
  char *shortUsageStr=
"SYNOPSIS\n"
"logSrv [-N | --dim-dns DIM_DNS_NODE] [-p | --fifo-path FIFO_PATH]\n"
"       [-s | --logger-name LOG_NAME]\n"
"       [-S | --stored-messages STORED_MSGS] [-H | --always-header]\n"
"       [-v | --never-header]\n"
"       [-l | --severity-threshold SEVERITY_THRESHOLD]\n"
"       [-F | --filter-regex REGULAR_EXPRESSION]\n"
"       [-X | --filter-out-regex REGULAR_EXPRESSION]\n"
"       [-f | --filter-wildcard WILDCARD_PATTERN]\n"
"       [-x | --filter-out-wildcard WILDCARD_PATTERN]\n"
"       [-E | --suppress-ident 0|1]\n"
"       [-L | --suppress-levenshtein-threshold LEV_DIST]\n"
"       [-W | --suppress-word-threshold WORD_DIST]\n"
"       [-n | --compared-message-num COMP_MSGS]\n"
"       [-c | --skip-char-num SKIP_NUM_COMP]\n"
"       [-C | --skip-char-list SKIP_CHAR_COMP]\n"
"       [-m | --mark-timestamp-interval INTERVAL] [-y | --syslog]\n"
"logSrv { -h | --help }\n"
"\n"
"Try \"logSrv -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,INFO,__func__,0,"\n%s",shortUsageStr);
  exit(1);
}
/*****************************************************************************/
void usage(int mode)
{
  FILE *fpOut;
  int status;
  char *formatter;
  char *pattern[7]={dfltVerbRegExpPattern,dfltDebugRegExpPattern,
                    dfltInfoRegExpPattern,dfltWarnRegExpPattern,
                    dfltErrorRegExpPattern,dfltFatalRegExpPattern,
                    sysLogDateFmt};
  char *pPattern[7];
  int i;
  char *pi,*pf;
  /*-------------------------------------------------------------------------*/
  for(i=0;i<7;i++)
  {
    pPattern[i]=(char*)malloc(1+2*strlen(pattern[i]));
    memset(pPattern[i],0,1+2*strlen(pattern[i]));
    for(pi=pattern[i],pf=pPattern[i];*pi;pi++)
    {
      if(*pi=='\\')
      {
        *pf++='\\';
        *pf++='[';
        *pf++='r';
        *pf++='s';
        *pf++=']';
      }
      else if(*pi=='|' && *(pi+1)=='\\')
      {
        *pf++='|';
        *pf++='\\';
        *pf++=':';
      }
      else
      {
        *pf++=*pi;
      }
    }
    *pf='\0';
  }
  /*-------------------------------------------------------------------------*/
  if(!mode)formatter="/usr/bin/groff -man -Tascii | /usr/bin/less -iRs";
  else formatter="/bin/cat";
  fpOut=popen(formatter,"w");
  fprintf(fpOut,
".ig\n"
"logSrv.man\n"
"\n"
"..\n"
"%s"
".hw WILDCARD_PATTERN INTERVAL /tmp/logGaudi.fifo logGetProperties\n"
".hw bexpired bnetwork synchronized already bunavailable baddress\n"
".hw representable permitted welcome stored\\[hy]messages\n"
".hw SEVERITY_THRESHOLD filter\\[hy]regex suppress\\[hy]ident\n"
".hw suppress\\[hy]levenshtein\\[hy]threshold compared\\[hy]message\\[hy]num\n"
".hw mark\\[hy]timestamp\\[hy]interval strptime\n"
".TH logSrv 8  %s \"FMC-%s\" \"FMC Servers\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis logSrv\\ \\-\n"
"FMC Message Logger Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis logSrv\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] p fifo\\[hy]path FIFO_PATH\n"
".DoubleOpt[] s logger\\[hy]name LOG_NAME\n"
".DoubleOpt[] S stored\\[hy]messages STORED_MSGS\n"
".DoubleOpt[] H always\\[hy]header\n"
".DoubleOpt[] v never\\[hy]header\n"
".DoubleOpt[] l severity\\[hy]threshold SEVERITY_THRESHOLD\n"
".DoubleOpt[] F filter\\[hy]regex REGULAR_EXPRESSION\n"
".DoubleOpt[] X filter\\[hy]out\\[hy]regex REGULAR_EXPRESSION\n"
".DoubleOpt[] f filter\\[hy]wildcard WILDCARD_PATTERN\n"
".DoubleOpt[] x filter\\[hy]out\\[hy]wildcard WILDCARD_PATTERN\n"
".DoubleOpt[] E suppress\\[hy]ident 0|1\n"
".DoubleOpt[] L suppress\\[hy]levenshtein\\[hy]threshold LEV_DIST\n"
".DoubleOpt[] W suppress\\[hy]word\\[hy]threshold WORD_DIST\n"
".DoubleOpt[] n compared\\[hy]message\\[hy]num COMP_MSGS\n"
".DoubleOpt[] c skip\\[hy]char\\[hy]num SKIP_NUM_COMP\n"
".DoubleOpt[] C skip\\[hy]char\\[hy]list SKIP_CHAR_COMP\n"
".DoubleOpt[] m mark\\[hy]timestamp\\[hy]interval INTERVAL\n"
".DoubleOpt[] y syslog\n"
".EndSynopsis\n"
".sp\n"
".Synopsis logSrv\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
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
"This feature is especially useful to intercept the error messages sent to "
"the standard error stream by the Linux dynamic loader at the application "
"start-up, otherwise not retrievable.\n"
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
"Look at section \\fIWRITING TO THE FMC LOGGER\\fP below for a comprehensive "
"review of the ways of sending messages to the FMC Message Logger.\n"
".PP\n"
"Since the FMC Message Logger uses DIM to move messages throughout the "
"network and DIM is based on the TCP protocol, it takes advantage of the TCP "
"built-in mechanisms to \\fBavoid\\fP to contribute to an incidental "
"\\fBnetwork congestion\\fP. Moreover, it can completely avoid the message "
"drop (no-drop mode) if this behavior is desired.\n"
".PP\n"
"Messages are pulled off the FIFO by the FMC Message Logger Server, "
"\\fBdriven by\\fP the \\fIIO/POLL\\fP \\fBsignal\\fP. When a burst of "
"messages terminates and no more messages can be read from the FIFO, the "
"process stops polling the FIFO and waits for an \\fIIO\\fP signal "
"(\\fBsigtimedwait\\fP(2)) without consuming CPU resources. As soon as new "
"messages are pushed into the FIFO, the kernel raises an \\fIIO\\fP signal "
"and the FMC Message Logger Server starts again polling "
"the FIFO.\n"
".\n"
".\n"
".SS No-Drop and Congestion-Proof Behavior\n"
".\n"
"The FMC Message Logger can work either in \\fBno-drop\\fP or in "
"\\fBcongestion-proof\\fP mode.  If the application opens directly the FIFO "
"for writing, the behavior can be chosen by specifying or not the "
"\\fIO_NONBLOCK\\fP flag in the \\fBopen\\fP(2) system call.  If the "
"application writes to the standard output/error stream, and the Task Manager "
"redirects the stream to the FIFO, then the behavior can be chosen by means "
"of the flag \\fB-A\\fP in the Task Manager \\fBtmStart\\fP(1) command.\n"
".PP\n"
"If the FIFO is completely filled (this could be due to a network congestion "
"which prevents the FIFO drain), any attempt to write to the FIFO in "
"\\fBno-drop\\fP mode \\fBblocks\\fP the writer application until a message "
"is pulled out of the FIFO, while any attempt to write to the FIFO in "
"\\fBcongestion-proof\\fP mode returns immediately the \\fIEAGAIN\\fP "
"\\fBerror\\fP, without blocking, without writing (dropping therefore the "
"message) and without injecting additional traffic into the network.\n"
".PP\n"
"The \\fIno-drop\\fP behavior can be useful for debugging purposes: no "
"message are lost, but the applications could block waiting to send messages "
"in case of network congestion or if an application is sending messages at a "
"rate greater than the rate of message displaying.\n"
".PP\n"
"The \\fIcongestion-proof\\fP behavior is more useful in normal run "
"operation: in case of network congestion, messages are dropped, but "
"applications do not block and no additional message traffic is injected "
"into the network (except the TCP retransmissions, which however are slowed "
"down by the congestion control mechanism), since the messages are dropped "
"in the FIFO, before they reach the network.\n"
".\n"
".\n"
".SS The Message Filter\n"
".\n"
"Since very often an excess of messages hides the messages of interest, it is "
"often desirable to filter the message flow. The FMC Message Logger allows to "
"filter the message flow both on the \\fIserver side\\fP (\\fBlogSrv\\fP(8)) "
"and on the \\fIclient side\\fP (\\fBlogViewer\\fP(1)). The filtering can be "
"based, on both sides, on:\n"
".TP 2\n*\n"
"the \\fIseverity\\fP of the message, "
"classified in six classes, (VERB, DEBUG, INFO, WARN, ERROR and FATAL) and "
"recognized either by a conventional message header or by means of a "
"heuristic method based on regular expressions, which can be changed at the "
"server start-up;\n"
".TP 2\n*\n"
"an arbitrary \\fIwildcard pattern\\fP;\n"
".TP 2\n*\n"
"an arbitrary \\fIextended regular expression\\fP.\n"
".PP\n"
"The settings concerning the Message Filter (severity threshold, wildcard "
"pattern and regular expression) on the \\fIserver side\\fP (i.e. on the "
"single farm node) can be \\fBchanged at run-time\\fP, without interrupting "
"the message flow, by means of the \\fBlogSetProperties\\fP(1) command.\n"
".\n"
".\n"
".SS The Suppression of Duplicates\n"
".\n"
"While running processes on a large farm, an anomalous condition can trigger "
"the same error message (or the same set of few error messages) on all the "
"nodes of the farm. If the same messages is sent more than once by each node, "
"the result is a \\fBmessage storm\\fP of thousands of messages which differ "
"at most for a few details (e.g. the name of the sender, a serial number, "
"etc.).\n"
".PP\n"
"To avoid such a condition, the FMC Message Logger can suppress the "
"duplicated messages both on the \\fIserver side\\fP and on the \\fIclient "
"side\\fP. To recognize duplicates, it can compare the last \\fIn\\fP "
"received messages (i.e. can suppress repeated messages which are "
"\\fBinterleaved\\fP by zero to \\fIn-1\\fP different messages) where the "
"parameter \\fIn\\fP can be set and modified at run-time.\n"
".PP\n"
"Three different comparison criteria can be used to distinguish duplicates "
"from different messages:\n"
".TP 2\n*\n"
"\\fIexact comparison\\fP: two messages are considered different if an exact "
"string comparison between the two messages fails;\n"
".TP 2\n*\n"
"\\fInumber of different tokens\\fP: two messages are considered different "
"if the number of tokens which are different in the two messages exceeds a "
"given threshold;\n"
".TP 2\n*\n"
"\\fILevenshtein distance\\fP: two message are considered different if the "
"Levenshtein distance between the two messages exceeds a given threshold. The "
"Levenshtein distance (also known as \\fIedit distance\\fP) is a measurement "
"of the similarity of the two strings, defined as the number of single "
"character edit operations (deletion, insertion or substitution) required to "
"transform one string into the other.\n"
".PP\n"
"Moreover, the \\fBheader\\fP of the message -- which contains the time and "
"the node name, which are very often different -- can be \\fBskipped\\fP in "
"the comparison to distinguish duplicates from different messages. Indeed, "
"the FMC Message Logger allows to start the comparison either after a settled "
"number of characters from the beginning of the messages or after the "
"occurrence of a given sequence of (not necessarily contiguous) characters "
"of the message.\n"
".PP\n"
"All the settings concerning the suppression of duplicates (the type of "
"comparison, the threshold in the number of different tokens, the threshold "
"in the Levenshtein distance, the number of messages to compare, the number "
"of characters to skip and the sequence of characters to skip) on the "
"\\fIserver side\\fP (i.e. on the single farm node) can be \\fBchanged at "
"run-time\\fP, without interrupting the message flow, by means of the "
"\\fBlogSetProperties\\fP(1) command.\n"
".\n"
".\n"
".SS The Last Message Buffer\n"
".\n"
"When the message filter is tight, only the severe messages are displayed on "
"the console, while all the other messages are discarded. If something "
"severe does happen, sometimes is desirable to \\fBretrieve also the less "
"severe messages already discarded\\fP.\n"
".PP\n"
"To this aim the FMC Message Logger Server implements a buffer, on the "
"\\fIserver side\\fP, containing the "
"\\fBlast\\fP \\fIm\\fP \\fBmessages\\fP (before filtering and duplicate "
"suppression), where the parameter \\fIm\\fP can be set and modified at "
"\\fBrun-time\\fP.  The whole buffer can be retrieved -- filtered and "
"reduced with the current settings -- through the DIM service "
"\\fIlast_log\\fP.  While in normal operation the FMC Message Logger Client "
"subscribes only, in MONITORED mode, the DIM service \\fIlog\\fP "
"containing the last single message, however, every time it detects a change "
"in the filter or message reduction settings it accesses also the "
"\\fIlast_log\\fP DIM service, in ONCE_ONLY mode, to download the last "
"message buffer.\n"
".PP\n"
"If something severe happens, the user can make the filter and reduction cuts "
"looser and the client, detecting a change in the settings, automatically "
"downloads the Last Message Buffer. This way, even less severe messages among "
"the last \\fIm\\fP ones can be displayed to better understand the fault.\n"
".PP\n"
"The size of the Last Message Buffer (on the \\fIserver side\\fP, i.e. on the "
"single farm node) can be \\fBchanged at run-time\\fP, without interrupting "
"the message flow, by means of the \\fBlogSetProperties\\fP(1) command.\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The FMC Message Logger Server (\\fBlogSrv\\fP(8)) collects the messages sent "
"by the applications runnings on the same computer though a \\fBFIFO\\fP "
"(named pipe) and makes them available to the network through a \\fBDIM "
"Service\\fP.\n"
".PP\n"
"The \\fBlogViewer\\fP(1) application can then access the \\fBlogSrv\\fP(8) "
"DIM Services through the network, retrieve the messages, and, either "
"display them on the screen, or forward them to another \\fBlogSrv\\fP(8).\n"
".PP\n"
"More than one instance of \\fBlogSrv\\fP(8) can be run on the same node to "
"set-up multiple separated message streams. Different \\fBlogSrv\\fP(8) "
"instances on the same node must have different \\fIFIFO_PATH\\fP (input) and "
"different \\fILOG_NAME\\fP (output). The default \\fBlogSrv\\fP(8) (started "
"without the \\fB-p\\fP and \\fB-s\\fP options) has "
"\\fIFIFO_PATH\\fP\\~=\\~\"/tmp/logSrv.fifo\" and "
"\\fILOG_NAME\\fP\\~=\\~\"fmc\". A secondary \\fBlogSrv\\fP(8) instance could "
"have, e.g., \\fIFIFO_PATH\\fP\\~=\\~\"/tmp/logGaudi.fifo\" and "
"\\fILOG_NAME\\fP\\~=\\~\"gaudi\".\n"
"Three separated \\fBlogSrv\\fP(8) instances could be started, e.g., with the "
"commands:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/logSrv -S 200\n"
".ShellCommand /opt/FMC/sbin/logSrv -S 200 -p /tmp/logGaudi.fifo -s gaudi\n"
".ShellCommand /opt/FMC/sbin/logSrv -S 200 -p /tmp/logThird.fifo -s third\n"
".\n"
".PP\n"
"Filter Options, Duplicate Suppression Options and Message Buffer Options\n"
"(see \\fBOPTIONS\\fP below) can be set at the process start-up, but can\n"
"be also changed at \\fBruntime\\fP (without interrupting the message flow)\n"
"by means of the \\fBlogSetProperties\\fP(1) command. The current running\n"
"settings can be retrieved by means of the \\fBlogGetProperties\\fP(1)\n"
"command.\n"
".\n"
".PP\n"
"The DIM Name Server, looked up by \\fBlogSrv\\fP to register the provided\n"
"DIM services, can be chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\"\\fB/etc/sysconfig/dim\\fP\".\n"
".RE\n"
".\n"
".\n"
".SH OPTIONS\n"
".\n"
".SS General Options\n"
".\n"
".OptDef h help\n"
"Get basic usage help from the command line.\n"
".\n"
".OptDef N dim-dns DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef p fifo-path FIFO_PATH (string)\n"
"Read messages from the FIFO (named pipe) named \\fIFIFO_PATH\\fP\n"
"(more precisely \\fIFIFO_PATH\\fP is the file system path to the FIFO\n"
"special file). \\fBDefault\\fP:\n"
"\\fIFIFO_PATH\\fP\\~=\\~\"/tmp/logSrv.fifo\".\n"
"This option is needed to set-up multiple separated message streams (see\n"
"\\fBDESCRIPTION\\fP above).\n"
".\n"
".OptDef s logger-name LOG_NAME (string)\n"
"Set the name of the current \\fBlogSrv\\fP instance to \\fILOG_NAME\\fP.\n"
"The \\fILOG_NAME\\fP is used to compose the corresponding DIM \\fBServer\n"
"name\\fP, which is set to: \"%s/\\fIHOST_NAME\\fP/logger/\\fILOG_NAME\\fP\"\n"
"and the corresponding DIM \\fBService name\\fP, which is set to:\n"
"\"%s/\\fIHOST_NAME\\fP/logger/\\fILOG_NAME\\fP/log\".\n"
"\\fBDefault\\fP:\\ \\fILOG_NAME\\fP\\~=\\~\"fmc\". This option is needed to\n"
"set-up multiple separated message streams (see \\fBDESCRIPTION\\fP above).\n"
".\n"
".OptDef H always-header\n"
"Always prefix to the message a header contining date, time, severity and\n"
"hostname. \\fBDefault\\fP: prefix the header only if it is absent.\n"
".\n"
".OptDef v never-header\n"
"Never prefix to the message a header contining date, time, severity and\n"
"hostname. \\fBDefault\\fP: prefix the header only if it is absent.\n"
".\n"
".OptDef m mark-timestamp-interval INTERVAL (integer)\n"
"Log a mark timestamp regularly every \\fIINTERVAL\\fP minutes.\n"
"\\fBDefault\\fP: \\fIINTERVAL\\fP\\~=\\~20 minutes. Setting the\n"
"\\fIINTERVAL\\fP to zero turns it off entirely.\n"
".\n"
".OptDef y syslog\n"
"Correctly format the messages coming from \\fBsyslogd\\fP(8), by reading the "
"heading date using the format \\fBsysLogDateFmt\\fP (see ENVIRONMENT section "
"below) and the following blank-separated hostname and reformatting them in "
"the standard FMC message format.\n"
".br\n"
".RS\n"
"To enable redirection from \\fBsyslogd\\fP(8) to FMC logger:\n"
".br\n"
"Add to \\fB/etc/syslog.conf\\fP the line:\n"
".br\n"
"*.*		|/tmp/logSys.fifo\n"
".br\n"
"Check in \\fB/etc/selinux/targeted/booleans\\fP that the following variables "
"have the following values:\n"
".br\n"
"\\fBallow_syslog_to_console\\fP=\\fI0\\fP\n"
".br\n"
"\\fBsyslogd_disable_trans\\fP=\\fI1\\fP\n"
".br\n"
"\\fBuse_syslogng\\fP=\\fI0\\fP\n"
".br\n"
"Use \\fBgetsebool(8)\\fP and \\fBtogglesebool\\fP(8) to check current "
"settings and activate changes immediately.\n"
".RE\n"
".\n"
".SS Filter Options\n"
".\n"
".OptDef l severity-threshold SEVERITY_THRESHOLD (integer or string)\n"
"Filter out messages which have\n"
"\\fBseverity\\fP\\~<\\~\\fISEVERITY_THRESHOLD\\fP.\n"
"As string, \\fBALL\\fP\\~=\\~0, "
"\\fBVERB\\fP\\~=\\~1, \\fBDEBUG\\fP\\~=\\~2, \\fBINFO\\fP\\~=\\~3, "
"\\fBWARN\\fP\\~=\\~4, \\fBERROR\\fP\\~=\\~5, \\fBFATAL\\fP\\~=6\\~.\n"
"\\fBDefault\\fP: \\fISEVERITY_THRESHOLD\\fP\\~=\\~1\\~=\\~\\fBVERB\\fP (no\n"
"message filtered out due to severity cut).\n"
".\n"
".OptDef F filter-regex REGULAR_EXPRESSION (string)\n"
"Filter \\fBthrough\\fP messages by using the extended regular expression\n"
"\\fIREGULAR_EXPRESION\\fP. \\fBDefault\\fP: do not filter.\n"
".\n"
".OptDef f filter-wildcard WILDCARD_PATTERN (string)\n"
"Filter \\fBthrough\\fP messages by using the wildcard pattern\n"
"\\fIWILDCARD_PATTERN\\fP. \\fBDefault\\fP: do not filter.\n"
".\n"
".OptDef X filter-out-regex REGULAR_EXPRESSION (string)\n"
"Filter \\fBout\\fP messages by using the extended regular expression\n"
"\\fIREGULAR_EXPRESION\\fP. \\fBDefault\\fP: do not filter.\n"
".\n"
".OptDef x filter-out-wildcard WILDCARD_PATTERN (string)\n"
"Filter \\fBout\\fP messages by using the wildcard pattern\n"
"\\fIWILDCARD_PATTERN\\fP. \\fBDefault\\fP: do not filter.\n"
".\n"
".SS Duplicate Suppression Options\n"
".\n"
".OptDef E suppress-ident 0|1\n"
"Suppress repeated messages using exact comparison, i.e.\n"
"suppress messages which are identical to at least one of the\n"
"previous \\fICOMP_MSGS\\fP-1 messages. \\fBDefault\\fP: do not suppress\n"
"repeated messages. \\fIN.B.\\fP: the behaviour with this option is\n"
"affected by \\fB-c\\fP, \\fB-C\\fP and \\fB-n\\fP options.\n"
".\n"
".OptDef L suppress-levenshtein-threshold LEV_DIST (integer)\n"
"Suppress repeated messages using Levenshtein difference, i.e.\n"
"suppress messages which differ less than or equal to \\fILEV_DIST\\fP\n"
"(inserted, deleted or substituted) characters from at least\n"
"one of the previous \\fICOMP_MSGS\\fP-1 messages. \\fBDefault\\fP: do not\n"
"suppress repeated messages. \\fIN.B.\\fP: the behaviour with this\n"
"option is affected by \\fB-c\\fP, \\fB-C\\fP and \\fB-n\\fP options.\n"
".\n"
".OptDef W suppress-word-threshold WORD_DIST (integer)\n"
"Suppress repeated messages using word difference, i.e.\n"
"suppress messages which differ less than or equal to\n"
"\\fIWORD_DIST\\fP subsituted or appended word, with respect to at least one\n"
"of the previous \\fICOMP_MSGS\\fP-1 messages. \\fBDefault\\fP: do not\n"
"suppress repeated messages. \\fIN.B.\\fP: the behaviour with this option is\n"
"affected by \\fB-c\\fP, \\fB-C\\fP and \\fB-n\\fP options.\n"
".\n"
".OptDef n compared-message-num COMP_MSGS (integer)\n"
"To suppress repeated messages, consider the last \\fICOMP_MSGS\\fP\n"
"messages, i.e. compare each message with the previous\n"
"\\fICOMP_MSGS\\fP-1 messages. \\fBDefault\\fP: 1 if repeated message\n"
"suppression is not active, 2 if it is active. \\fIN.B.\\fP: it affects the\n"
"behaviour with \\fB-E\\fP, \\fB-L\\fP and \\fB-W\\fP options.\n"
".\n"
".OptDef c skip-char-num SKIP_NUM_COMP (integer)\n"
"In the comparison for the repeated message suppression, skip\n"
"\\fISKIP_NUM_COMP\\fP characters in each string, i.e. start the comparison\n"
"between a couple of strings from the \\fISKIP_NUM_COMP\\fP-th character.\n"
"It affects the behaviour with \\fB-E\\fP, \\fB-L\\fP and \\fB-W\\fP\n"
"options.\n"
".\n"
".OptDef C skip-char-list SKIP_CHAR_COMP (string)\n"
"In the comparison for the repeated message suppression, skip\n"
"characters in each string until all the characters listed in\n"
"the string \\fISKIP_CHAR_COMP\\fP are passed. It affect the behaviour\n"
"with \\fB-E\\fP, \\fB-L\\fP and \\fB-W\\fP options.\n"
".\n"
".SS Message Buffer Options\n"
".\n"
".OptDef S stored-messages STORED_MSGS (integer)\n"
"Keep the last \\fISTORED_MSGS\\fP messages stored. Default: 0.\n"
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
"Must include the path to the libraries \"libdim\"\n"
"and \"libFMCutils\".\n"
".\n"
".TP\n"
".EnvVar UTGID \\ (string,\\ optional)\n"
"Printed in diagnostic messages, used to identify the \\fBlogSrv\\fP\n"
"instance. If not set, \\fILOG_NAME\\fP is used.\n"
".\n"
".TP\n"
".EnvVar pollPeriod \\ (float,\\ optional)\n"
"Period of FIFO polling in seconds if no new messages are written to the\n"
"FIFO. Default: 30.0 s (kernel >= 2.5.20) or 0.1 s (kernel < 2.5.20). Must\n"
"be decreased down to 0.1 s if asynchronous I/O notification for FIFOs and\n"
"pipes (via SIGIO/SIGPOLL) does not work in the running Linux kernel.\n"
".\n"
".TP\n"
".EnvVar deBug \\ (integer,\\ optional)\n"
"Debug mask. Can be set to 0x00...0x1f. The meaning of the mask bits is:\n"
".RS\n"
"0x01: print signal received.\n"
".br\n"
"0x02: print message found/not found.\n"
".br\n"
"0x04: print update number.\n"
".br\n"
"0x08: print all the \\fICOMP_MSGS\\fP message buffers.\n"
".br\n"
"0x10: print diagnostics on repeated msgs suppression.\n"
".RE\n"
".\n"
".TP\n"
".EnvVar verbRegExp \\ (string,\\ optional)\n"
"Regular expression used to identify unformatted \\fBVERB\\fP messages.\n"
".br\n"
"\\fBDefault\\fP: \"%s\".\n"
".\n"
".TP\n"
".EnvVar debugRegExp \\ (string,\\ optional)\n"
"Regular expression used to identify unformatted \\fBDEBUG\\fP messages.\n"
".br\n"
"\\fBDefault\\fP: \"%s\".\n"
".\n"
".TP\n"
".EnvVar infoRegExp \\ (string,\\ optional)\n"
"Regular expression used to identify unformatted \\fBINFO\\fP messages.\n"
".br\n"
"\\fBDefault\\fP: \"%s\".\n"
".\n"
".TP\n"
".EnvVar warnRegExp \\ (string,\\ optional)\n"
"Regular expression used to identify unformatted \\fBWARNING\\fP messages.\n"
".br\n"
".ad l\n"
"\\fBDefault\\fP: \"%s\".\n"
".ad b\n"
".\n"
".TP\n"
".EnvVar errorRegExp \\ (string,\\ optional)\n"
"Regular expression used to identify unformatted \\fBERROR\\fP messages.\n"
".br\n"
".ad l\n"
"\\fBDefault\\fP: \"%s\".\n"
".ad b\n"
".\n"
".TP\n"
".EnvVar fatalRegExp \\ (string,\\ optional)\n"
"Regular expression used to identify unformatted \\fBFATAL\\fP messages.\n"
".br\n"
".ad l\n"
"\\fBDefault\\fP: \"%s\".\n"
".ad b\n"
".\n"
".TP\n"
".EnvVar sysLogDateFmt \\ (string,\\ optional)\n"
"Format of the date in the messages coming from \\fBsyslogd\\fP(8). Used only "
"when the command-line option \\fB-y\\fP or \\fB--syslog\\fP is specified. "
"The format conversion specification are described in the man page "
"\\fBstrptime\\fP(3).\n"
".br\n"
".ad l\n"
"\\fBDefault\\fP: \"%s\".\n"
".ad b\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Run without filter:\n"
".\n"
".PP\n"
".ShellCommand logSrv\n"
".ShellCommand logSrv -p /tmp/log_1.fifo -s log_1\n"
".ShellCommand logSrv --fifo /tmp/log_1.fifo --logger log_1\n"
".\n"
".PP\n"
"Display messages from the above servers:\n"
".\n"
".PP\n"
".ShellCommand logViewer\n"
".ShellCommand logViewer -s \"log_1\"\n"
".ShellCommand logViewer --logger \"log_1\"\n"
".\n"
".PP\n"
"Keep the last 20 messages stored:\n"
".\n"
".PP\n"
".ShellCommand logSrv -p /tmp/log_1.fifo -s log_1 -S 20\n"
".ShellCommand logSrv --fifo /tmp/log_1.fifo --logger log_1 --stored 20\n"
".\n"
".PP\n"
"Filter out the lines with severity < ERROR:\n"
".\n"
".PP\n"
".ShellCommand logSrv -p /tmp/log_1.fifo -s log_1 -l 4\n"
".ShellCommand logSrv -p /tmp/log_1.fifo -s log_1 -l ERROR\n"
".ShellCommand logSrv --fifo /tmp/log_1.fifo --logger log_1 --severity 4\n"
".ShellCommand logSrv --fifo /tmp/log_1.fifo --log log_1 --severity ERROR\n"
".\n"
".PP\n"
"Filter through the lines containing the string \"Srv\":\n"
".\n"
".PP\n"
".ShellCommand logSrv -p /tmp/log_1.fifo -s log_1 -f \\[dq]*Srv*\\[dq]\n"
".ShellCommand logSrv -p /tmp/log_1.fifo -s log_1 -F \\[dq]Srv\\[dq]\n"
".ShellCommand logSrv --fifo /tmp/log_1.fifo --log log_1 --filter-w "
"\\[dq]*Srv*\\[dq]\n"
".ShellCommand logSrv --fifo /tmp/log_1.fifo --log log_1 --filter-r "
"\\[dq]Srv\\[dq]\n"
".\n"
".PP\n"
"Filter out the lines containing the string \"Srv\":\n"
".\n"
".PP\n"
".ShellCommand logSrv -p /tmp/log_1.fifo -s log_1 -x \\[dq]*Srv*\\[dq]\n"
".ShellCommand logSrv -p /tmp/log_1.fifo -s log_1 -X \\[dq]Srv\\[dq]\n"
".ShellCommand logSrv --fifo /tmp/log_1.fifo --log log_1 --filter-out-w "
"\\[dq]*Srv*\\[dq]\n"
".ShellCommand logSrv --fifo /tmp/log_1.fifo --log log_1 --filter-out-r "
"\\[dq]Srv\\[dq]\n"
".\n"
".PP\n"
"Filter through the lines containing the word \"word1\" near and\n"
"before the word \"word2\" (at most 4 words between):\n"
".\n"
".PP\n"
".ShellCommand logSrv -F \\[dq]\\[rs]bword1\\[rs]W+(\\[rs]w+\\[rs]W+)\\:{0,4}"
"\\:word2\\[rs]b\\[dq]\n"
".ShellCommand logSrv --filter-r "
"\\[dq]\\[rs]bword1\\[rs]W+(\\[rs]w+\\[rs]W+)\\:{0,4}\\:word2\\[rs]b\\[dq]\n"
".\n"
".PP\n"
"Filter through the lines containing the word \"word1\" near the\n"
"word \"word2\" (at most 4 words between) in any order:\n"
".\n"
".PP\n"
".ShellCommand logSrv -F \\[dq]\\[rs]bword1\\[rs]W+(\\[rs]w+\\[rs]W+)\\:{0,4}"
"\\:word2\\[rs]b|\\[rs]bword2\\[rs]W+(\\[rs]w+\\[rs]W+)\\:{0,4}\\:"
"word1\\[rs]b\\[dq]\n"
".ShellCommand logSrv --filter-r \\[dq]\\[rs]bword1\\[rs]W+(\\[rs]w+\\[rs]W+)"
"{0,4}\\:word2\\[rs]b|\\[rs]bword2\\[rs]W+\\:(\\[rs]w+\\:\\[rs]W+)\\:{0,4}\\:"
"word1\\[rs]b\\[dq]\n"
".\n"
".PP\n"
"Filter through the lines containing a floating point number:\n"
".\n"
".PP\n"
".ShellCommand logSrv -F \\[dq][-+]?[0-9]*\\[rs].[0-9]+([eE][-+]?[0-9]+)?"
"\\[dq]\n"
".ShellCommand logSrv --filter-r \\[dq][-+]?[0-9]*\\[rs].[0-9]+([eE][-+]?"
"[0-9]+)?\\[dq]\n"
".\n"
".PP\n"
"Suppress the messages which are identical to the previous one:\n"
".\n"
".PP\n"
".ShellCommand logSrv -E 1\n"
".ShellCommand logSrv --suppress-ident 1\n"
".\n"
".PP\n"
"Suppress the messages which are identical after the 30-th character\n"
"to the previous one:\n"
".\n"
".PP\n"
".ShellCommand logSrv -E 1 -c 30\n"
".ShellCommand logSrv --suppress-i 1 --skip-char-num 30\n"
".\n"
".PP\n"
"Suppress the messages which are identical after the 30-th character\n"
"to one of the previous 10 messages:\n"
".\n"
".PP\n"
".ShellCommand logSrv -E 1 -c 30 -n 10\n"
".ShellCommand logSrv  --suppress-i 1 --skip-char-n 30 --compared-message-num "
"10\n"
".\n"
".PP\n"
"Suppress the messages which are identical, after the second \":\" (colon) in\n"
"the message, to one of the previous 10 messages:\n"
".\n"
".PP\n"
".ShellCommand logSrv -E 1 -C \\[dq]::\\[dq] -n 10\n"
".ShellCommand logSrv --suppress-i 1 --skip-char-list \\[dq]::\\[dq] "
"--compared-message 10\n"
".\n"
".PP\n"
"Suppress the messages which differs, in at most 2 words after the\n"
"second \":\" (colon) with respect to one of the previous 10 messages:\n"
".\n"
".PP\n"
".ShellCommand logSrv -W 2 -C \\[dq]::\\[dq] -n 10\n"
".ShellCommand logSrv --suppress-word-threshold 2 --skip-char-l \\[dq]::\\[dq] --co 10\n"
".\n"
".PP\n"
"Suppress the messages which differs, in at most 6 character \n"
"substituted, inserted or deleted after the second \":\" (colon) with\n"
"respect to one of the previous 10 messages:\n"
".\n"
".PP\n"
".ShellCommand logSrv -L 6 -C \\[dq]::\\[dq] -n 10\n"
".ShellCommand logSrv --suppress-levenshtein-thresh 6 --skip-char-l \\[dq]::\\[dq] -co 10\n"
".\n"
".PP\n"
"Start a logger server which manages the messages coming from "
"\\fBsyslogd\\fP(8) (see above, in the description of \\fB-y\\fP option, how "
"to redirect messages)\n"
".\n"
".PP\n"
".ShellCommand logSrv -S 100 -p /tmp/logSys.fifo -s sys -y\n"
".ShellCommand logSrv --stored 100 --fifo /tmp/logSys.fifo --logger sys "
"--syslog\n"
".\n"
".\n"
".\n"
".SH WRITING TO THE FMC LOGGER\n"
".PP\n"
".SS Redirecting stdout/stderr from outside the application\n"
".\n"
"The simplest way for a process to use the FMC Message Logger consists in "
"redirecting the standard error and/or the standard output of the process to "
"the FMC Message Logger FIFO (\\fI/tmp/logSrv.fifo\\fP for the default "
"logger). This allows using the FMC Message Logger with the \\fBexisting "
"applications without modifying them\\fP.\n"
".PP\n"
"Redirection can be achieved either by starting the process by the "
"\\fBshell\\fP command-line or by starting the process using the FMC Task "
"Manager (see \\fBtmStart\\fP(1)).\n"
".PP\n"
"The drawback of this method is the lack of control on the severity level "
"and, in the case of the shell, on the logging policy. The \\fBseverity "
"level\\fP cannot be set by the writer application but is guessed by the FMC "
"Logger heuristic method based on the regular expressions. The \\fBlogging "
"policy\\fP is always set to \\fBno-drop\\fP using the shell redirection "
"while can be set also to \\fBcongestion-free\\fP using the FMC Task "
"Manager.\n"
".PP\n"
"To redirect the stdout/stderr to the FMC Message Logger using the shell:\n"
".PP\n"
".ShellCommand myapp >/tmp/logSrv.fifo &\n"
".ShellCommand myapp 2>/tmp/logSrv.fifo &\n"
".ShellCommand myapp >/tmp/logGaudi.fifo 2>&1 &\n"
".ShellCommand myapp >/tmp/my_out.fifo 2>/tmp/my_err.fifo &\n"
".PP\n"
"To redirect the stdout/stderr to the FMC Message Logger using the FMC Task "
"Manager and the \\fBcongestion-proof\\fP policy:\n"
".PP\n"
".ShellCommand tmStart -o myapp\n"
".ShellCommand tmStart -e myapp\n"
".ShellCommand tmStart -o -e myapp\n"
".ShellCommand tmStart -O /tmp/my_out.fifo -E /tmp/my_err.fifo myapp\n"
".PP\n"
"To redirect the stdout/stderr to the FMC Message Logger using the FMC Task "
"Manager and the \\fBno-drop\\fP policy:\n"
".PP\n"
".ShellCommand tmStart -A -O /tmp/my_out.fifo -E /tmp/my_err.fifo myapp\n"
".\n"
".\n"
".SS Redirecting stdout/stderr from inside the application\n"
".\n"
"The redirection of the stdout/stderr to the FMC Message Logger can be "
"performed also from inside the application. In this case it is possible to "
"choose either the no-drop or the congestion-proof \\fBpolicy\\fP, but the "
"\\fBseverity level\\fP still can only be guessed by the FMC Logger "
"heuristic method.\n"
".PP\n"
"To redirect the stdout/stderr to the FMC Message Logger using the "
"\\fBno-drop\\fP policy:\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBfreopen(\"/tmp/logSrv.fifo\",\"a\",stdout);\\fP\n"
".br\n"
"\\fBsetlinebuf(stdout);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBprintf(\"i=%%d j=%%d\",j,k);\\fP\n"
".PP\n"
"To redirect the stdout/stderr to the FMC Message Logger using the "
"\\fBcongestion-proof\\fP policy:\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
"\\fB#include <sys/types.h>\\fP\n"
".br\n"
"\\fB#include <sys/stat.h>\\fP\n"
".br\n"
"\\fB#include <fcntl.h>\\fP\n"
".br\n"
"\\fB#include <unistd.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBfifoFD=open(\"/tmp/logSrv.fifo\",O_RDWR|O_NONBLOCK|O_APPEND);\\fP\n"
".br\n"
"\\fBdup2(fifoFD,STDOUT_FILENO);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBprintf(\"i=%%d j=%%d\",j,k);\\fP\n"
".\n"
".\n"
".SS Opening the FMC Logger FIFO from inside the application\n"
".\n"
"Instead of redirecting stdout/stderr, a process can open directly the logger "
"FIFO and send messages to it. Also in this case it is possible to choose "
"either the no-drop or the congestion-proof \\fBpolicy\\fP, but the "
"\\fBseverity level\\fP can only be guessed by the FMC Logger heuristic "
"method.\n"
".PP\n"
"A FIFO can be opened in \\fBblocking mode\\fP to achieve a \\fBno-drop\\fP "
"logging policy. For example, by using the buffered streams "
"(\\fBfprintf\\fP(3)):\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBfifoFP=fopen(\"/tmp/logSrv.fifo\",\"a\");\\fP\n"
".br\n"
"\\fBsetlinebuf(fifoFP);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBfprintf(fifoFP,\"i=%%d j=%%d\",j,k);\\fP\n"
".PP\n"
"Alternatively, by using the low-level \\fBwrite\\fP(2) system call:\n"
".PP\n"
"\\fB#include <sys/types.h>\\fP\n"
".br\n"
"\\fB#include <sys/stat.h>\\fP\n"
".br\n"
"\\fB#include <fcntl.h>\\fP\n"
".br\n"
"\\fB#include <unistd.h>\\fP\n"
".br\n"
"\\fBfifoFD=open(\"/tmp/logSrv.fifo\",O_WRONLY|O_APPEND);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBwrite(fifoFD,message,1+strlen(message));\\fP\n"
".PP\n"
"A FIFO can also be opened in \\fBnon-blocking mode\\fP to achieve a "
"\\fBcongestion-proof\\fP logging policy. For example, by using the buffered "
"streams (\\fBfprintf\\fP(3)):\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
"\\fB#include <sys/types.h>\\fP\n"
".br\n"
"\\fB#include <sys/stat.h>\\fP\n"
".br\n"
"\\fB#include <fcntl.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBfifoFD=open(\"/tmp/logSrv.fifo\",O_RDWR|O_NONBLOCK|O_APPEND);\\fP\n"
".br\n"
"\\fBfifoFP=fdopen(fifoFD,\"a\");\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBfprintf(fifoFP,\"i=%%d j=%%d\",j,k);\\fP\n"
".PP\n"
"Alternatively, by using the low-level \\fBwrite\\fP(2) system call:\n"
".PP\n"
"\\fB#include <sys/types.h>\\fP\n"
".br\n"
"\\fB#include <sys/stat.h>\\fP\n"
".br\n"
"\\fB#include <fcntl.h>\\fP\n"
".br\n"
"\\fB#include <unistd.h>\\fP\n"
".br\n"
"\\fBfifoFD=open(\"/tmp/logSrv.fifo\",O_RDWR|O_NONBLOCK|O_APPEND);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBwrite(fifoFD,message,1+strlen(message));\\fP\n"
".\n"
".\n"
".SS Using the FMC Logger Utility API\n"
".\n"
"To write formatted messages to the FMC Logger, with or without a message "
"header (which includes date, time, severity level, hostname, command, UTGID "
"and function), the calls \\fBdfltLoggerOpen\\fP(3), \\fBmPrintf\\fP(3), "
"\\fBrPrintf\\fP(3), \\fBdfltLoggerClose\\fP(3), \\fBloggerOpen\\fP(3), "
"\\fBmfPrintf\\fP(3), \\fBrfPrintf\\fP(3), and \\fBloggerClose\\fP(3) are "
"provided in the library \\fIlibFMCutils\\fP.\n"
".PP\n"
"The \\fIformat description string\\fP used by the calls \\fBmPrintf\\fP(3), "
"\\fBrPrintf\\fP(3), \\fBmfPrintf\\fP(3) and \\fBrfPrintf\\fP(3) has exactly "
"the same syntax of the format description string of the standard library "
"calls \\fBprintf\\fP(3) and \\fBfprintf\\fP(3).\n"
".PP\n"
"The functions \\fBmPrintf\\fP(1) and \\fBmfPrintf\\fP(1) in the FMC Logger "
"Utility API allow to set both the \\fBlogging policy\\fP and the "
"\\fBseverity level\\fP.\n"
".PP\n"
"To write a message \\fIwith\\fP header using \\fBno-drop\\fP policy to the "
"\\fIdefault\\fP FMC Logger:\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
"\\fB#include <fmcMsgUtils.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBdfltLoggerOpen(10,NO_DROP,ERROR,DEBUG,0);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBmPrintf(L_DIM,INFO,__func__,0,\"i=%%d j=%%d\",j,k);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBdfltLoggerClose();\\fP\n"
".PP\n"
"To write a message \\fIwith\\fP header using \\fBcongestion-proof\\fP policy "
"to the \\fIdefault\\fP FMC Logger:\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
"\\fB#include <fmcMsgUtils.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBdfltLoggerOpen(10,CONG_PROOF,ERROR,DEBUG,0);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBmPrintf(L_DIM,INFO,__func__,0,\"i=%%d j=%%d\",j,k);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBdfltLoggerClose();\\fP\n"
".PP\n"
"To write a message \\fIwith\\fP header using \\fBno-drop\\fP policy to an "
"\\fIarbitrary\\fP FMC Logger:\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
"\\fB#include <fmcMsgUtils.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBld=loggerOpen(\"/tmp/logGaudi.fifo\",NO_DROP,L_STD,ERROR);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBmfPrintf(ld,INFO,__func__,0,\"i=%%d j=%%d\",j,k);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBloggerClose(ld);\\fP\n"
".PP\n"
"To write a message \\fIwith\\fP header using \\fBcongestion-proof\\fP policy "
"to an \\fIarbitrary\\fP FMC Logger:\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
"\\fB#include <fmcMsgUtils.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBloggerOpen(\"/tmp/logGaudi.fifo\",CONG_PROOF,L_STD,ERROR);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBmfPrintf(ld,INFO,__func__,0,\"i=%%d j=%%d\",j,k);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBloggerClose(ld);\\fP\n"
".PP\n"
"To write a message \\fIwithout\\fP header using \\fBno-drop\\fP policy to "
"the \\fIdefault\\fP FMC Logger:\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
"\\fB#include <fmcMsgUtils.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBdfltLoggerOpen(10,NO_DROP,ERROR,DEBUG,0);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBrPrintf(L_DIM,5,\"i=%%d j=%%d\",j,k);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBdfltLoggerClose();\\fP\n"
".PP\n"
"To write a message \\fIwithout\\fP header using \\fBcongestion-proof\\fP "
"policy to the \\fIdefault\\fP FMC Logger:\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
"\\fB#include <fmcMsgUtils.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBdfltLoggerOpen(10,CONG_PROOF,ERROR,DEBUG,0);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBrPrintf(L_DIM,5,\"i=%%d j=%%d\",j,k);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBdfltLoggerClose();\\fP\n"
".PP\n"
"To write a message \\fIwithout\\fP header using \\fBno-drop\\fP policy to an "
"\\fIarbitrary\\fP FMC Logger:\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
"\\fB#include <fmcMsgUtils.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBld=loggerOpen(\"/tmp/logGaudi.fifo\",NO_DROP,L_STD,ERROR);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBrfPrintf(ld,5,\"i=%%d j=%%d\",j,k);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBloggerClose(ld);\\fP\n"
".PP\n"
"To write a message \\fIwithout\\fP header using \\fBcongestion-proof\\fP "
"policy to an \\fIarbitrary\\fP FMC Logger:\n"
".PP\n"
"\\fB#include <stdio.h>\\fP\n"
".br\n"
"\\fB#include <fmcMsgUtils.h>\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBloggerOpen(\"/tmp/logGaudi.fifo\",CONG_PROOF,L_STD,ERROR);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBrfPrintf(ld,5,\"i=%%d j=%%d\",j,k);\\fP\n"
".br\n"
" \\fB...................\\fP\n"
".br\n"
"\\fBloggerClose(ld);\\fP\n"
".PP\n"
"To get more details about the FMC Logger Utility API calls, look at their "
"man pages, in section 3 of FMC man pages.\n"
".\n"
".\n"
".SS Writing to the FMC Logger from a shell prompt or a shell script\n"
".\n"
"The simplest way to use the FMC Message Logger from a \\fBshell prompt\\fP "
"or from a \\fBshell script\\fP consists in redirecting the standard output "
"of the \\fBecho\\fP command to a FMC Logger FIFO:\n"
".PP\n"
".ShellCommand echo 'Error: blah blah blah!' >/tmp/logSrv.fifo\n"
".br\n"
".ShellCommand echo 'Error: blah blah blah!' >/tmp/logGaudi.fifo\n"
".PP\n"
"The limit of this approach is the lack of control on the severity level and "
"on the logging policy. The \\fBseverity level\\fP cannot be set by the "
"writing script but is guessed by the FMC Logger heuristic method based on "
"the regular expressions. The \\fBlogging policy\\fP is always set to "
"\\fBno-drop\\fP.\n"
".PP\n"
"To gain control on severity level and logging policy, the command "
"\\fBmPrint\\fP(1) is provided by the FMC package. A few usage example are "
"listed here, but more details on the command syntax can be found in the "
"\\fBmPrint\\fP(1) man page:\n"
".PP\n"
".ShellCommand mPrint 'blah blah blah!'\n"
".br\n"
".ShellCommand mPrint -s WARN 'blah blah blah!'\n"
".br\n"
".ShellCommand mPrint -s FATAL -l FMC -l STD -l SYS 'blah blah blah!'\n"
".br\n"
".ShellCommand mPrint -s INFO -p /tmp/logGaudi.fifo 'blah blah blah!'\n"
".br\n"
".ShellCommand mPrint -s WARN -f main 'blah blah blah!'\n"
".br\n"
".ShellCommand mPrint -s WARN -P my_program 'blah blah blah!'\n"
".br\n"
".ShellCommand mPrint -s WARN -u my_utgid 'blah blah blah!'\n"
".\n"
".\n"
".SH DIM INFORMATION\n"
".\n"
".SS DIM Server Name\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/logger/\\fILOG_NAME\\fP\n"
"where \\fIHOST_NAME\\fP is the host name of the current PC, as returned by\n"
"the command \"hostname -s\", but written in upper case characters, and\n"
"\\fILOG_NAME\\fP is set by default to the string \"fmc\" (but the string\n"
"\\fILOG_NAME\\fP can be changed by means of the \\fB-s\\fP \\fILOG_NAME\\fP\n"
"command line option).\n"
".\n"
".\n"
".SS Published DIM Services\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/logger/\\fILOG_NAME\\fP/log\n"
"Publishes a NULL-terminated string containing the last message (to be\n"
"subscribed in MONITORED DIM mode).\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/logger/\\fILOG_NAME\\fP/last_log\n"
"Publishes a NULL-terminated string containing the \\fISTORED_MSGS\\fP\n"
"messages kept in the last message buffer (to be subscribed in ONCE_ONLY\n"
"DIM mode and accessed from time to time to retrieve the old messages).\n"
"Messages are separated by a 'newline' character (LF, '\\[rs]n', ascii 0x0A).\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/logger/\\fILOG_NAME\\fP/get_properties\n"
"Publishes a NULL-terminated string containing the Filter Options, the\n"
"Duplicate Suppression Options and the Message Buffer Options of the current\n"
"\\fBlogSrv\\fP instance, in the format:\n"
".IP\n"
".ad l\n"
"[\\fB-l\\fP \\fISEVERITY_THRESHOLD\\fP]\n"
"[\\fB-F\\fP \\fIREGULAR_EXPRESSION\\fP]\n"
"[\\fB-X\\fP \\fIREGULAR_EXPRESSION\\fP]\n"
"[\\fB-f\\fP \\fIWILDCARD_PATTERN\\fP] [\\fB-x\\fP \\fIWILDCARD_PATTERN\\fP]\n"
"[\\fB-E\\fP \\fI0|1\\fP] [\\fB-L\\fP \\fILEV_DIST\\fP]\n"
"[\\fB-W\\fP \\fIWORD_DIST\\fP] [\\fB-n\\fP \\fICOMP_MSGS\\fP]\n"
"[\\fB-c\\fP \\fISKIP_NUM_COMP\\fP] [\\fB-C\\fP \\fISKIP_CHAR_COMP\\fP]\n"
"[\\fB-S\\fP \\fISTORED_MSGS\\fP]\n"
".ad b\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/logger/\\fILOG_NAME\\fP/success\n"
"Always publishes the integer 1. This dummy services can be used to check\n"
"whether the current instance of \\fBlogSrv\\fP is up and running.\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/logger/\\fILOG_NAME\\fP/server_version\n"
"Publishes a NULL-terminated string containing the RCS revision label of\n"
"the current \\fBlogSrv\\fP instance, as returned by the command\n"
"\"ident /opt/FMC/sbin/logSrv\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/logger/\\fILOG_NAME\\fP/fmc_version\n"
"Publishes a NULL-terminated string containing the revision label of the FMC\n"
"package which includes the current \\fBlogSrv\\fP executable, in the\n"
"present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".\n"
".SS Published DIM Commands\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/logger/\\fILOG_NAME\\fP/set_properties\n"
"Accepts, as argument, a NULL-terminated string containing the Filter\n"
"Options, the Duplicate Suppression Options and the Message Buffer Options\n"
"to be set in the the current running \\fBlogSrv\\fP instance, in the format:\n"
".IP\n"
".ad l\n"
"[\\fB-l\\fP \\fISEVERITY_THRESHOLD\\fP]\n"
"[\\fB-F\\fP \\fIREGULAR_EXPRESSION\\fP]\n"
"[\\fB-X\\fP \\fIREGULAR_EXPRESSION\\fP]\n"
"[\\fB-f\\fP \\fIWILDCARD_PATTERN\\fP] [\\fB-x\\fP \\fIWILDCARD_PATTERN\\fP]\n"
"[\\fB-E\\fP \\fI0|1\\fP] [\\fB-L\\fP \\fILEV_DIST\\fP]\n"
"[\\fB-W\\fP \\fIWORD_DIST\\fP] [\\fB-n\\fP \\fICOMP_MSGS\\fP]\n"
"[\\fB-c\\fP \\fISKIP_NUM_COMP\\fP] [\\fB-C\\fP \\fISKIP_CHAR_COMP\\fP]\n"
"[\\fB-S\\fP \\fISTORED_MSGS\\fP]\n"
".ad b\n"
"%s"
"%s"
".SH VERSION\n"
".\n"
"Source:  %s\n"
".br\n"
"Package: FMC-%s\n"
".\n"
".\n"
".SH SEE ALSO\n"
".ad l\n"
".SS FMC Message Logger User Commands\n"
".BR \\%%logViewer (1),\n"
".BR \\%%logGetProperties (1),\n"
".BR \\%%logSetProperties (1).\n"
".SS UI for writing to an arbitrary FMC Message Logger from a shell script\n"
".BR \\%%mPrint (1).\n"
".SS API for formatted writing to the default FMC Message Logger\n"
".BR \\%%dfltLoggerOpen (3),\n"
".BR \\%%mPrintf (3),\n"
".BR \\%%rPrintf (3),\n"
".BR \\%%dfltLoggerClose (3),\n"
".BR \\%%getDfltFifoFD (3).\n"
".SS API for formatted writing to an arbitrary FMC Message Logger\n"
".BR \\%%loggerOpen (3),\n"
".BR \\%%mfPrintf (3),\n"
".BR \\%%rfPrintf (3),\n"
".BR \\%%loggerClose (3).\n"
".SS Regular expression and wildcard pattern syntax\n"
".BR \\%%regex (7),\n"
".BR \\%%glob (7).\n"
".SS Date format\n"
".BR \\%%strptime (3).\n"
".SS Syslog daemon\n"
".BR \\%%syslogd (8),\n"
".BR \\%%syslog.conf (5).\n"
".SS Named pipes\n"
".BR \\%%fifo (4),\n"
".BR \\%%pipe (7),\n"
".BR \\%%open (2),\n"
".BR \\%%write (2).\n"
".br\n"
"\\fBhttp://lwn.net/Articles/118750/\\fP.\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro,FMC_DATE,FMC_VERSION
,SVC_HEAD,SVC_HEAD
,pPattern[0],pPattern[1],pPattern[2],pPattern[3],pPattern[4],pPattern[5]
,pPattern[6]
,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD,SVC_HEAD
,rcsidP,SVC_HEAD,FMC_VERSION,SVC_HEAD
,FMC_URLS,FMC_AUTHORS
,rcsidP,FMC_VERSION
       );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/*****************************************************************************/
