/*****************************************************************************/
/*
 * $Log: logViewer.C,v $
 * Revision 3.35  2012/12/07 15:48:07  galli
 * Minor change in man page reference
 *
 * Revision 3.34  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 3.33  2009/02/16 10:19:32  galli
 * minor changes
 *
 * Revision 3.32  2009/02/11 14:15:59  galli
 * Use <vector> for hostPttnLs, logPttnLs and svcPttnLs
 *
 * Revision 3.31  2009/02/10 12:59:55  galli
 * Output FIFO opened by LogUtils::fifoOpen()
 * Signal SIGPIPE blocked
 *
 * Revision 3.30  2009/02/10 09:57:05  galli
 * Classes moved to separate files
 * Handle SIGHUP (re-open output file for logrotate)
 *
 * Revision 3.28  2009/01/22 10:17:26  galli
 * do not depend on kernel-devel headers
 *
 * Revision 3.17  2008/10/13 10:55:20  galli
 * uses FmcUtils::printLogServerVersion()
 *
 * Revision 3.11  2008/08/26 08:45:20  galli
 * groff manual
 *
 * Revision 3.9  2008/07/03 15:49:23  galli
 * getServerVersion() can catch old logSrv versions
 *
 * Revision 3.8  2008/07/01 12:28:29  galli
 * services starts with /FMC
 * bug fixed
 *
 * Revision 3.5  2008/06/24 11:02:25  galli
 * -E options takes 0|1 as argument
 *
 * Revision 3.4  2008/06/24 09:05:07  galli
 * Compatible with logSrv version >= 3.0
 *
 * Revision 3.3  2008/06/20 12:38:31  galli
 * In cmd-line separate host (-m) and logger name (-s)
 *
 * Revision 3.2  2008/06/19 14:17:49  galli
 * bug fixed
 *
 * Revision 3.0  2008/06/19 12:40:16  galli
 * Can subscribe to new servers after start-up
 *
 * Revision 2.25  2008/05/15 09:58:27  galli
 * getServerVersion() compatible with new SVC base name
 *
 * Revision 2.23  2008/05/15 08:25:18  galli
 * service base name changed
 * diagnostic message severity lowered
 *
 * Revision 2.21  2007/12/13 12:02:06  galli
 * bug fixed
 *
 * Revision 2.20  2007/12/13 11:57:57  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 2.16  2007/10/24 14:41:01  galli
 * cmd-line option -v to print server version and FMC version
 *
 * Revision 2.14  2007/10/23 12:05:31  galli
 * usage() prints FMC version
 *
 * Revision 2.12  2007/08/13 12:44:52  galli
 * usage() changed
 *
 * Revision 2.10  2007/08/09 21:58:16  galli
 * compatible with libFMCutils v 2
 *
 * Revision 2.9  2006/12/14 10:13:07  galli
 * added VERBOSE severity level
 *
 * Revision 2.8  2006/11/24 13:49:35  galli
 * added command line option -S
 * bug fixed
 *
 * Revision 2.7  2006/11/23 16:10:59  galli
 * bug fixed
 *
 * Revision 2.6  2006/11/23 15:09:51  galli
 * print old messages from servers at start-up and after change in
 * a server settings
 *
 * Revision 2.5  2006/11/17 11:23:32  galli
 * try to read last_log message buffer at start-up
 *
 * Revision 2.3  2006/10/19 14:33:46  galli
 * can suppress repeated message using a fast word difference.
 * -R replaced by -L, -E and -W added
 * uses the __func__ magic variable
 *
 * Revision 2.2  2006/10/08 21:53:55  galli
 * Suppress repeated messages using Levenshtein distance
 * Added command line switches -R, -n, -c and -C
 *
 * Revision 2.1  2006/09/28 14:24:56  galli
 * DIM error handler added
 *
 * Revision 2.0  2006/09/27 10:21:57  galli
 * Can send output to FIFO or to file.
 * -o, -O and -A command line switches added.
 * Guess severity using regex if header not found.
 *
 * Revision 1.9  2006/09/16 14:11:14  galli
 * -s option is repeatable
 * can filter messages using wildcard pattern (-f switch)
 * can filter messages using regular expression (-F switch)
 *
 * Revision 1.8  2005/09/15 14:41:58  galli
 * fflush message output, for output reditection to a log file
 *
 * Revision 1.7  2005/08/20 00:24:42  galli
 * does not print strings beginning with ffffffff
 *
 * Revision 1.6  2004/10/26 00:30:43  galli
 * comply with logSrv version 1.12
 *
 * Revision 1.5  2004/10/09 10:25:52  galli
 * Command line options added.
 *
 * Revision 1.2  2004/10/06 14:00:55  galli
 * working version
 */
/*****************************************************************************/
#include <string>                                             /* std::string */
#include <list>                                                 /* std::list */
#include <vector>                                               /* std::list */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <fnmatch.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>                                              /* tolower() */
#include <getopt.h>                                         /* getopt_long() */
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcMsgUtils.h"           /* dfltLoggerOpen(), mPrintf(), rPrintf() */
#include "fmcCmpUtils.h"                                        /* levDist() */
#include "fmcSrvNameUtils.h"                               /* getSrvPrefix() */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                            /* printLogServerVersion() */
#include "fmcSigUtils.h"                                        /* sig2msg() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcRef.h"                                  /* FMC_URLS FMC_AUTHORS */
#include "fmcPar.h"                                    /* DIM_CONF_FILE_NAME */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcMacro.h"     /* eExit(), likely(), unlikely(), printOutFuncOK() */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*---------------------------------------------------------------------------*/
/* in logger/include */
#include "severityPatterns.h"                    /* debugRegExpPattern, etc. */
#include "LogDimErrorHandler.IC"                 /* class LogDimErrorHandler */
#include "LogSettingsWatcher.IC"                 /* class LogSettingsWatcher */
#include "LogSrvSeeker.IC"                             /* class LogSrvSeeker */
#include "LogUtils.IC"                                     /* class LogUtils */
#include "logSrv.h"                                    /* BUF_SIZE, SRV_NAME */
/*****************************************************************************/
using namespace std;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
void getServerVersion(vector<string> hostPttnLs, vector<string> logPttnLs);
void signalHandler(int sig);
/*****************************************************************************/
/* globals */
/* counters */
unsigned long long suppressedMsgN=0;
/* Running Parameters */
char *pName=NULL;
char *utgid=NULL;
int deBug=0;
FILE *outFP=NULL;
int errU=L_STD|L_SYS;
int lStd=L_STD;
int viewOutput=1;
int minLd=-1;                                /* minimum Levenshtein distance */
int minWd=-1;                                       /* minimum word distance */
int ptCmp=0;                                          /* punctual comparison */
int chkdLineN=1;
size_t cutCol=0;
const char *cutStr="";
int noDrop=0;
char *outFifo=NULL;
char *outFile=NULL;
pthread_t mainPtid;                  /* thread identifier of the main thread */
int severityThreshold=1;
int color=1;
int doPrintLast=1;
/* variables */
list<string> csList;               /* list of servers which changed settings */
/* start-up messages */
string propertiesMsg;
string lvFunction;
/* message buffers */
char **mBuf=NULL;           /* buffer containing the last chkdLineN messages */
char *cBuf=NULL;                                         /* alias of mBuf[0] */
/* mutexes */
pthread_mutex_t csLock=PTHREAD_MUTEX_INITIALIZER;    /* change settings lock */
/* prLock: print lock.                                                       */
/* Locked by: LogUtils::printLastLog(),                                      */
/*            LogDimErrorHandler::errorHandler(),                            */
/*            LogWriter::infoHandler()                                       */
pthread_mutex_t prLock=PTHREAD_MUTEX_INITIALIZER;              /* print lock */
/* Regular Expressions / Wildcard Patterns */
regex_t filterRegExp;
regex_t xFilterRegExp;
char *filterWildCardPattern=NULL;
char *filterRegExpPattern=NULL;
char *xFilterWildCardPattern=NULL;
char *xFilterRegExpPattern=NULL;
char *verbRegExpPattern=NULL;
char *debugRegExpPattern=NULL;
char *infoRegExpPattern=NULL;
char *warnRegExpPattern=NULL;
char *errorRegExpPattern=NULL;
char *fatalRegExpPattern=NULL;
regex_t verbRegExp;
regex_t debugRegExp;
regex_t infoRegExp;
regex_t warnRegExp;
regex_t errorRegExp;
regex_t fatalRegExp;
/* Version IDs */
static char rcsid[]="$Id: logViewer.C,v 3.35 2012/12/07 15:48:07 galli Exp galli $";
char *rcsidP;
/* ######################################################################### */
int main(int argc,char **argv)
{
  /* counters */
  int i=0;
  /* pointers */
  char *p;
  const char *ls[7]={"ALL","VERB","DEBUG","INFO","WARN","ERROR","FATAL"};
  int rv=0;
  char errMsg[4096]="";
  char *deBugS=NULL;
  int inFromDfltLog=0;
  int outToDfltLog=0;
  char *dfltSvcPath=NULL;
  int newErrU=0;
  sigset_t signalMask;
  int signo=0;
  /* service browsing */
  vector<string> hostPttnLs;                          /* host pattern vector */
  vector<string> logPttnLs;                         /* logger pattern vector */
  vector<string> svcPttnLs;                        /* service pattern vector */
  vector<string>::iterator hvIt;             /* host pattern vector iterator */
  vector<string>::iterator lvIt;           /* logger pattern vector iterator */
  vector<string>::iterator svIt;          /* service pattern vector iterator */
  DimBrowser br;
  char *dimDnsNode=NULL;
  /* command line switch */
  int doPrintServerVersion=0;
  /* getopt */
  char flag;
  const char *shortOptions="h::Vbl:s:F:f:O:o:AE:L:W:n:c:C:Sm:N:X:x:";
  static struct option longOptions[]=
  {
    {"dim-dns",required_argument,NULL,'N'},
    {"no-color",no_argument,NULL,'b'},
    {"no-old-messages",no_argument,NULL,'S'},
    {"severity-threshold",required_argument,NULL,'l'},
    {"logger-name",required_argument,NULL,'s'},
    {"out-fifo",required_argument,NULL,'o'},
    {"out-file",required_argument,NULL,'O'},
    {"no-drop",no_argument,NULL,'A'},
    {"filter-regex",required_argument,NULL,'F'},
    {"filter-wildcard",required_argument,NULL,'f'},
    {"filter-out-regex",required_argument,NULL,'X'},
    {"filter-out-wildcard",required_argument,NULL,'x'},
    {"suppress-ident",required_argument,NULL,'E'},
    {"suppress-levenshtein-threshold",required_argument,NULL,'L'},
    {"suppress-word-threshold",required_argument,NULL,'W'},
    {"compared-message-num",required_argument,NULL,'n'},
    {"skip-char-num",required_argument,NULL,'c'},
    {"skip-char-list",required_argument,NULL,'C'},
    {"version",required_argument,NULL,'V'},
    {"hostname",required_argument,NULL,'m'},
    {"help",optional_argument,NULL,'h'},
    {NULL,0,NULL,0}
  };
  /* messages */ 
  string startUpMsg;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  pName=strdup(basename(argv[0]));
  utgid=getenv("UTGID");
  if(!utgid)utgid=(char*)"no UTGID";
  /*-------------------------------------------------------------------------*/
  /* process command line options */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt_long(argc,argv,shortOptions,longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 'b': /* -b, --no-color */
        color=0;
        break;
      case 'F': /* -F, --filter-regex */
        filterRegExpPattern=optarg;
        break;
      case 'f': /* -f, --filter-wildcard */
        filterWildCardPattern=optarg;
        break;
      case 'l': /* -l, --severity-threshold */
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
            fprintf(stderr,"\nInvalid"
                    "SEVERITY_THRESHOLD: \"%s\"!\n",optarg);
            mPrintf((errU&~L_STD)|L_SYS,FATAL,__func__,0,"Invalid"
                    "SEVERITY_THRESHOLD: \"%s\"!",optarg);
            shortUsage();
          }
        }
        else
        {
          fprintf(stderr,"\nInvalid"
                  "SEVERITY_THRESHOLD: \"%s\"!\n",optarg);
          mPrintf((errU&~L_STD)|L_SYS,FATAL,__func__,0,"Invalid"
                  "SEVERITY_THRESHOLD: \"%s\"!",optarg);
          shortUsage();
        }
        break;
      case 's': /* -s, --logger-name */
        logPttnLs.push_back(optarg);
        break;
      case 'o': /* -o, --out-fifo */
        outFifo=optarg;
        color=0;
        break;
      case 'O': /* -O, --out-file */
        outFile=optarg;
        color=0;
        break;
      case 'A': /* -A, --no-drop */
        noDrop=1;
        break;
      case 'E': /* -E, --suppress-ident */
        ptCmp=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'L': /* -L, --suppress-levenshtein-threshold */
        minLd=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'W': /* -W, --suppress-word-threshold */
        minWd=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'n': /* -n, --compared-message-num */
        chkdLineN=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'c': /* -c, --skip-char-num */
        cutCol=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'C': /* -C, --skip-char-list */
        cutStr=optarg;
        break;
      case 'S': /* -S, --no-old-messages */
        doPrintLast=0;
        break;
      case 'V': /* -V, --version */
        doPrintServerVersion=1;
        color=0;
        break;
      case 'm': /* -m, --hostname */                /* only togrther with -V */
        hostPttnLs.push_back(optarg);
        break;
      case 'N': /* -N, --dim-dns */
        dimDnsNode=optarg;
        break;
      case 'X': /* -X, --filter-out-regex */
        xFilterRegExpPattern=optarg;
        break;
      case 'x': /* -x, --filter-out-wildcard */
        xFilterWildCardPattern=optarg;
        break;
      case 'h': /* -h, --help */
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        fprintf(stderr,"\ngetopt_long(): invalid "
                "option \"%s\"!\n",argv[optind-1]);
        mPrintf((errU&~L_STD)|L_SYS,FATAL,__func__,0,"getopt_long(): invalid "
                "option \"%s\"!",argv[optind-1]);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  if(outFifo||outFile)viewOutput=0;
  if(color)lStd=L_STD_C;
  else lStd=L_STD;
  errU=lStd|L_SYS;
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
      mPrintf(errU,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from "
              "\""DIM_CONF_FILE_NAME"\" file).",dimDnsNode);
    }
    else
    {
      dimDnsNode=getDimDnsNode(DIM_CONF_FILE_NAME,0,1);
      if(dimDnsNode)
      {
        printf("DIM_DNS_NODE: \"%s\" (from \""DIM_CONF_FILE_NAME" file).\n",
               dimDnsNode);
      }
      else
      {
        mPrintf(errU,FATAL,__func__,0,"DIM Name Server (DIM_DNS_NODE) not "
                "defined!\nDIM Name Server can be defined (in decreasing "
                "order of priority):\n"
                "  1. Specifying the -N DIM_DNS_NODE command-line option;\n"
                "  2. Specifying the DIM_DNS_NODE environment variable;\n"
                "  3. Defining the DIM_DNS_NODE in the file "
                "\""DIM_CONF_FILE_NAME"\".\n");
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
  /* read environment variables */
  deBugS=getenv("deBug");
  if(deBugS)deBug=(int)strtol(deBugS,(char**)NULL,0);
  /*-------------------------------------------------------------------------*/
  /* default wildcard for hostPttnLs and logPttnLs */
  if(hostPttnLs.empty())hostPttnLs.push_back("*");
  if(logPttnLs.empty())logPttnLs.push_back("*");
  /*-------------------------------------------------------------------------*/
  /* check hostPttnLs and logPttnLs */
  for(hvIt=hostPttnLs.begin();hvIt!=hostPttnLs.end();hvIt++)
  {
    if(hvIt->find_first_of('/')!=string::npos)
    {
      fprintf(stderr,"Illegal NODE_PATTERN "
              "\"%s\"! A NODE_PATTERN specified with the command-line option "
              "\"-m NODE_PATTERN\" or \"--hostname NODE_PATTERN\" must not "
              "contain the '/' character!\n",hvIt->c_str());
      mPrintf((errU&~lStd)|L_SYS,FATAL,__func__,0,"Illegal NODE_PATTERN "
              "\"%s\"! A NODE_PATTERN specified with the command-line option "
              "\"-m NODE_PATTERN\" or \"--hostname NODE_PATTERN\" must not "
              "contain the '/' character!",hvIt->c_str());
      shortUsage();
    }
  }
  for(lvIt=logPttnLs.begin();lvIt!=logPttnLs.end();lvIt++)
  {
    if(lvIt->find_first_of('/')!=string::npos)
    {
      fprintf(stderr,"Illegal LOG_PATTERN "
              "\"%s\"! A LOG_PATTERN specified with the command-line option "
              "\"-s LOG_PATTERN\" or \"--logger-name LOG_PATTERN\" must not "
              "contain the '/' character!\n",lvIt->c_str());
      mPrintf((errU&~lStd)|L_SYS,FATAL,__func__,0,"Illegal LOG_PATTERN "
              "\"%s\"! A LOG_PATTERN specified with the command-line option "
              "\"-s LOG_PATTERN\" or \"--logger-name LOG_PATTERN\" must not "
              "contain the '/' character!",lvIt->c_str());
      shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* compose svcPttnLs */
  for(hvIt=hostPttnLs.begin();hvIt!=hostPttnLs.end();hvIt++)
  {
    for(lvIt=logPttnLs.begin();lvIt!=logPttnLs.end();lvIt++)
    {
      svcPttnLs.push_back(string(SVC_HEAD)+"/"+FmcUtils::toUpper(*hvIt)+
                          "/"+SRV_NAME+"/"+*lvIt+"/log");
    }
  }
  /*-------------------------------------------------------------------------*/
  /* handling of -V option */
  if(doPrintServerVersion)
  {
    getServerVersion(hostPttnLs,logPttnLs);
  }
  /*-------------------------------------------------------------------------*/
  /* check command line options */
  /* setting -E, -L, -W and not -n force -n 2  */
  if((minLd>-1||minWd>-1||ptCmp>0) && chkdLineN==1)chkdLineN=2;
  /*-------------------------------------------------------------------------*/
  /* allocate memory for mBuf (buffer for duplicate suppression) */
  mBuf=(char**)malloc(chkdLineN*sizeof(char*));
  for(i=0;i<chkdLineN;i++)mBuf[i]=(char*)malloc(BUF_SIZE*sizeof(char));
  cBuf=mBuf[0];
  /* clear the chkdLineN message buffers */
  for(i=0;i<chkdLineN;i++)memset(mBuf[i],0,BUF_SIZE);
  /*-------------------------------------------------------------------------*/
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
  /*-------------------------------------------------------------------------*/
  /* Set error unit for logViewer's own error messages:                      */
  /* If default logger not in input/output, send errors to it;               */
  /* If default logger in input or output, send errors to stderr and syslog. */
  /* Default logger is, by definition, the FMC logger which has:             */
  /* - fifo:          /tmp/logSrv.fifo                                       */
  /* - DIM SVC name:  /FMC/<HOST>/logger/fmc                                 */ 
  /*-------------------------------------------------------------------------*/
  /* exclude default logger if I/O is default logger */
  newErrU=0;
  /* check if output to default logger /tmp/logSrv.fifo */
  if(outFifo && !strcmp(outFifo,"/tmp/logSrv.fifo"))outToDfltLog=1;
  /* check if input from default logger /<HOSTNAME>/logger/fmc/log */
  asprintf(&dfltSvcPath,"%s/%s/%s/fmc/log",SVC_HEAD,getSrvPrefix(),SRV_NAME);
  for(svIt=svcPttnLs.begin();svIt!=svcPttnLs.end();svIt++)
  {
    if(!fnmatch(svIt->c_str(),dfltSvcPath,0))inFromDfltLog=1;
  }
  /* if default logger in input or output, send errors to stderr and syslog  */
  if(inFromDfltLog||outToDfltLog)
  {
    newErrU|=(L_SYS|lStd);
  }
  else           /* if default logger not in input/output, send errors to it */
  {
    if(dfltLoggerOpen(1,0,WARN,DEBUG,1)!=-1)newErrU|=L_DIM;
    else
    {
      mPrintf(errU,INFO,__func__,0,"Can't initialize standard FMC Message "
              "Logger. Using syslog/stderr instead.");
      newErrU|=(L_SYS|lStd);
    }
  }
  /* if output to terminal include stderr in error destinations */
  if(viewOutput)newErrU|=lStd;
  /* print error unit */
  if(newErrU&L_DIM)
  {
    mPrintf(errU,DEBUG,__func__,0,"logViewer own error messages sent to the "
            "default FMC Logger \"/tmp/logSrv.fifo\".");
  }
  if(newErrU&lStd)
  {
    mPrintf(errU,DEBUG,__func__,0,"logViewer own error messages sent to its "
            "own stderr.");
  }
  if(newErrU&L_SYS)
  {
    mPrintf(errU,DEBUG,__func__,0,"logViewer own error messages sent to "
            "syslog facility.");
  }
  /* activate the new error unit */
  errU=newErrU;
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  new LogDimErrorHandler(color);
  /*-------------------------------------------------------------------------*/
  /* check command line options */
  if(severityThreshold>FATAL)
  {
    fprintf(stderr,"\nSEVERITY_THRESHOLD must be"
            "less than or equal to %d (FATAL)!\n",FATAL);
    mPrintf((errU&~lStd)|L_SYS,FATAL,__func__,0,"SEVERITY_THRESHOLD must be "
            "less than or equal to %d (FATAL)!",FATAL);
    shortUsage();
  }
  /* avoid getting msgs from dflt logger & sending msgs to dflt logger */
  if(inFromDfltLog && outToDfltLog)
  {
    mPrintf(errU|lStd|L_SYS,FATAL,__func__,0,"Can't get input from the "
            "default logger \"%s\" and send output to the default logger "
            "\"%s\"! Exiting!",dfltSvcPath,"/tmp/logSrv.fifo");
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* set output destination */
  if(outFifo) outFP=LogUtils::fifoOpen(outFifo,noDrop);
  else if(outFile) outFP=LogUtils::fileOpen(outFile);
  else outFP=stdout;
  if(!outFP)exit(1);
  setlinebuf(outFP);
  /*-------------------------------------------------------------------------*/
  /* compose properties messages lvFunction & propertiesMsg */
  if(outFile)lvFunction="Archiver";
  else if(outFifo)lvFunction="Forwarder";
  else lvFunction="Viewer";
  propertiesMsg=string("TGID = ")+FmcUtils::utos(getpid())+". ";
  propertiesMsg+=string("Software version: \"");
  propertiesMsg+=string(rcsidP)+"\", \"FMC-"+FMC_VERSION+"\". ";
  propertiesMsg+="Severity threshold = \"";
  propertiesMsg+=string(ls[severityThreshold]);
  propertiesMsg+="\"; Subscribed DIM SVC pattern list = (";
  for(svIt=svcPttnLs.begin();svIt!=svcPttnLs.end();svIt++)
  {
    if(svIt!=svcPttnLs.begin())propertiesMsg+=", ";
    propertiesMsg+=*svIt;
  }
  propertiesMsg+="); ";
  if(filterRegExpPattern)
  {
    propertiesMsg+=string("Filter IN Regular Expression = \"")+
                   filterRegExpPattern+"\"; ";
  }
  if(filterWildCardPattern)
  {
    propertiesMsg+=string("Filter IN Wildcard Pattern = \"")+
                   filterWildCardPattern+"\"; ";
  }
  if(xFilterRegExpPattern)
  {
    propertiesMsg+=string("Filter OUT Regular Expression = \"")+
                   xFilterRegExpPattern+"\"; ";
  }
  if(xFilterWildCardPattern)
  {
    propertiesMsg+=string("Filter OUT Wildcard Pattern = \"")+
                   xFilterWildCardPattern+"\"; ";
  }
  if(outFifo && noDrop)
  {
    propertiesMsg+=string("Output sent to FIFO: \"")+
                   outFifo+"\", No-drop policy; ";
  }
  else if(outFifo && !noDrop)
  {
    propertiesMsg+=string("Output sent to FIFO: \"")+
                   outFifo+"\", Congestion-proof policy; ";
  }
  else if(outFile)
  {
    propertiesMsg+=string("Output sent to File: \"")+outFile+"\"; ";
  }
  else
  {
    propertiesMsg+=string("Output sent to STDOUT; ");
  }
  if(verbRegExpPattern!=dfltVerbRegExpPattern)
  {
    propertiesMsg+=string("Regular Expression for identifying VERB messages "
                          "= \"")+verbRegExpPattern+"\"; ";
  }
  if(debugRegExpPattern!=dfltDebugRegExpPattern)
  {
    propertiesMsg+=string("Regular Expression for identifying DEBUG messages "
                          "= \"")+debugRegExpPattern+"\"; ";
  }
  if(infoRegExpPattern!=dfltInfoRegExpPattern)
  {
    propertiesMsg+=string("Regular Expression for identifying INFO messages "
                          "= \"")+infoRegExpPattern+"\"; ";
  }
  if(warnRegExpPattern!=dfltWarnRegExpPattern)
  {
    propertiesMsg+=string("Regular Expression for identifying WARN messages "
                          "= \"")+warnRegExpPattern+"\"; ";
  }
  if(errorRegExpPattern!=dfltErrorRegExpPattern)
  {
    propertiesMsg+=string("Regular Expression for identifying ERROR messages "
                          "= \"")+errorRegExpPattern+"\"; ";
  }
  if(fatalRegExpPattern!=dfltFatalRegExpPattern)
  {
    propertiesMsg+=string("Regular Expression for identifying FATAL messages "
                          "= \"")+fatalRegExpPattern+"\"; ";
  }
  if(!ptCmp && minLd<0 && minWd<0)
  {
    propertiesMsg+=string("NO repeated messages suppression; ");
  }
  if(ptCmp)
  {
    propertiesMsg+=string("Suppress identical messages; ");
  }
  if(minLd>-1)
  {
    propertiesMsg+=string("Minimum Levenshtein distance between messages = ")+
                   FmcUtils::itos(minLd)+"; ";
  }
  if(minWd>-1)
  {
    propertiesMsg+=string("Minimum word difference between messages = ")+
                   FmcUtils::itos(minWd)+"; ";
  }
  if(chkdLineN>1)
  {
    propertiesMsg+=string("Number of compared messages = ")+
                   FmcUtils::itos(chkdLineN)+"; ";
  }
  if(cutCol>0)
  {
    propertiesMsg+=string("Skip ")+FmcUtils::itos(cutCol)+
                   " characters in comparison; ";
  }
  if(cutStr[0]!=0)
  {
    propertiesMsg+=string("Skip characters in comparison until \"")+
                   string(cutStr)+"\" passed; ";
  }
  propertiesMsg.erase(propertiesMsg.length()-2,2);       /* esase final "; " */
  /*-------------------------------------------------------------------------*/
  /* compile regular expressions */
  if(filterRegExpPattern)
  {
    rv=regcomp(&filterRegExp,filterRegExpPattern,REG_EXTENDED|REG_NOSUB);
    if(rv!=0)
    {
      mPrintf(errU|lStd|L_SYS,FATAL,__func__,0,"regcomp(): Cannot compile "
              "regular expression \"%s\": Reported error: %s! Exiting!\n",
              filterRegExpPattern,strerror(errno));
      return 1;
    }
  }
  if(xFilterRegExpPattern)
  {
    rv=regcomp(&xFilterRegExp,xFilterRegExpPattern,REG_EXTENDED|REG_NOSUB);
    if(rv!=0)
    {
      mPrintf(errU|lStd|L_SYS,FATAL,__func__,0,"regcomp(): Cannot compile "
              "regular expression \"%s\": Reported error: %s! Exiting!\n",
              xFilterRegExpPattern,strerror(errno));
      return 1;
    }
  }
  rv=regcomp(&verbRegExp,verbRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    regerror(rv,&verbRegExp,errMsg,4096);
    mPrintf(errU|lStd|L_SYS,FATAL,__func__,0,"regcomp(): Cannot compile "
            "regular expression \"%s\". Reported error: \"%s\"! Exiting!",
            verbRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&debugRegExp,debugRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    regerror(rv,&debugRegExp,errMsg,4096);
    mPrintf(errU|lStd|L_SYS,FATAL,__func__,0,"regcomp(): Cannot compile "
            "regular expression \"%s\". Reported error: \"%s\"! Exiting!",
            debugRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&infoRegExp,infoRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    regerror(rv,&infoRegExp,errMsg,4096);
    mPrintf(errU|lStd|L_SYS,FATAL,__func__,0,"regcomp(): Cannot compile "
            "regular expression \"%s\". Reported error: \"%s\"! Exiting!",
            infoRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&warnRegExp,warnRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    regerror(rv,&warnRegExp,errMsg,4096);
    mPrintf(errU|lStd|L_SYS,FATAL,__func__,0,"regcomp(): Cannot compile "
            "regular expression \"%s\". Reported error: \"%s\"! Exiting!",
            warnRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&errorRegExp,errorRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    regerror(rv,&errorRegExp,errMsg,4096);
    mPrintf(errU|lStd|L_SYS,FATAL,__func__,0,"regcomp(): Cannot compile "
            "regular expression \"%s\". Reported error: \"%s\"! Exiting!",
            errorRegExpPattern,errMsg);
    exit(1);
  }
  rv=regcomp(&fatalRegExp,fatalRegExpPattern,REG_EXTENDED|REG_NOSUB|REG_ICASE);
  if(rv!=0)
  {
    regerror(rv,&fatalRegExp,errMsg,4096);
    mPrintf(errU|lStd|L_SYS,FATAL,__func__,0,"regcomp(): Cannot compile "
            "regular expression \"%s\". Reported error: \"%s\"! Exiting!",
            fatalRegExpPattern,errMsg);
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* Block SIGINT & SIGTERM, to be handled synchronously by                  */
  /* sigtimedwait().                                                         */
  /* Signals must be blocked before the dis_start_serving() call, which      */
  /* creates new threads, to keep the signals blocked in all the 3 threads.  */
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGUSR1);         /* used to signal changed settings */
  sigaddset(&signalMask,SIGHUP);                       /* used for logrotate */
  sigaddset(&signalMask,SIGPIPE);      /* could arrive if outFifo and noDrop */
  sigfillset(&signalMask);
  if(pthread_sigmask(SIG_BLOCK,&signalMask,NULL))eExit("pthread_sigmask()");
  /*-------------------------------------------------------------------------*/
  /* save the thread identifier of the main thread, to send it a SIGIO       */
  /* using pthread_kill().                                                   */
  mainPtid=pthread_self();
  /*-------------------------------------------------------------------------*/
  new LogSrvSeeker("DIS_DNS/SERVER_LIST",svcPttnLs);
  /*-------------------------------------------------------------------------*/
  /* print start-up message */
  startUpMsg=string("FMC Message Logger ")+lvFunction+" started. "+
             propertiesMsg+".";
  mPrintf(errU|L_SYS|lStd,INFO,__func__,0,"%s",startUpMsg.c_str());
  /*-------------------------------------------------------------------------*/
  dtq_sleep(1);       /* leave time to LogSrvSeeker for initial subscription */
  /*-------------------------------------------------------------------------*/
  /* main loop */
  while(1)
  {
    /*.......................................................................*/
    signo=sigwaitinfo(&signalMask,NULL);
    if((deBug & 0x1) && signo!=-1)
      mPrintf(errU,DEBUG,__func__,0,"Signal %d received.",signo);
    /*.......................................................................*/
    if(signo==SIGUSR1 &&            /* settings changed on at least 1 server */
            doPrintLast)
    {
      /* csList is the list of servers which changed their settings */
      /* csList is written by LogSettingsWatcher::infoHandler() */
      /* copy csList to unlock csLock mutex before accessing last_log */
      pthread_mutex_lock(&csLock);
      list<string>csListCopy=csList;
      csList.clear();
      pthread_mutex_unlock(&csLock);
      list<string>::iterator it;
      /* print last_log of csList elements */
      for(it=csListCopy.begin();it!=csListCopy.end();it++)
      {
        LogUtils::printLastLog((char*)it->c_str(),color,severityThreshold);
      }
    }
    /*.......................................................................*/
    else if(signo==SIGHUP && outFile)   /* re-open output file for logrotate */
    {
      char msg[BUF_SIZE]="";
      /* ................................................................... */
      mPrintf(errU,INFO,__func__,0,"SIGHUP received. Trying to re-open output "
              "file \"%s\"...",outFile);
      snprintf(msg,BUF_SIZE,"%s(%s): %s(): SIGHUP received. Trying to re-open "
               "output file \"%s\"...",pName,utgid,__func__,outFile);
      LogUtils::headerPrepend(msg,INFO);
      LogUtils::printC(outFP,INFO,-1,color,msg);
      /* ................................................................... */
      pthread_mutex_lock(&prLock);
      outFP=freopen(outFile,"w",outFP);
      pthread_mutex_unlock(&prLock);
      /* ................................................................... */
      if(!outFP)
      {
        mPrintf(errU|lStd|L_SYS,FATAL,__func__,0,"freopen(): %s! Exiting!",
                strerror(errno));
        exit(1);
      }
      else
      {
        mPrintf(errU,INFO,__func__,0,"SIGHUP received. Output file \"%s\" "
                "re-opened.",outFile);
        snprintf(msg,BUF_SIZE,"%s(%s): %s(): SIGHUP received. Output file "
                 "\"%s\" re-opened.",pName,utgid,__func__,outFile);
        LogUtils::headerPrepend(msg,INFO);
        LogUtils::printC(outFP,INFO,-1,color,msg);
      }
      /* ................................................................... */
    }                  /* SIGHUP received: re-open output file for logrotate */
    /*.......................................................................*/
    else if(signo==SIGPIPE && outFifo)      /* write to FIFO with no readers */
    {
      mPrintf(errU,ERROR,__func__,0,"SIGPIPE received. Output FIFO \"%s\" "
              "has not a reader process connected to the other end!",outFifo);
    }                                       /* write to FIFO with no readers */
    /*.......................................................................*/
    else signalHandler(signo);
    /*.......................................................................*/
  }                                                             /* main loop */
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
/* signalHandler() called synchronously. Don't warry about async-signal-safe */
void signalHandler(int sig)
{
  string stopMsg;
  /*-------------------------------------------------------------------------*/
  /* print terminating message */
  stopMsg=string("FMC Message Logger ")+lvFunction+" TEMINATED on Signal "+
          FmcUtils::itos(sig)+" ("+sig2msg(sig)+")! "+propertiesMsg+".";
  if(sig==SIGINT || sig==SIGTERM)
  {
    mPrintf(errU|L_SYS|lStd,WARN,__func__,0,"%s",stopMsg.c_str());
    _exit(0);
  }
  else
  {
    mPrintf(errU|L_SYS|lStd,FATAL,__func__,0,"%s",stopMsg.c_str());
    _exit(sig);
  }
  /*-------------------------------------------------------------------------*/
}
/*****************************************************************************/
void getServerVersion(vector<string>hostPttnLs, vector<string>logPttnLs)
{
  printf("HOSTNAME(LOGGER)          FMC      SERVER\n");
  FmcUtils::printLogServerVersion(hostPttnLs,logPttnLs,SVC_HEAD,deBug,25,8);
  printf("HOSTNAME                  FMC      CLIENT\n");
  printf("%-25s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"logViewer [-N|--dim-dns DIM_DNS_NODE]\n"
"          [-m|--hostname NODE_PATTERN...]\n"
"          [-s|--logger-name LOG_PATTERN...]  [-b|--no-color]\n"
"          [-S|--no-old-messages]\n"
"          [-l|--severity-threshold SEVERITY_THRESHOLD]\n"
"          [-o|--out-fifo FIFO_NAME]  [-O|--out-file FILE_NAME]\n"
"          [-A|--no-drop]  [-F|--filter-regex REGULAR_EXPRESSION]\n"
"          [-f|--filter-wildcard WILDCARD_PATTERN]\n"
"          [-X|--filter-out-regex REGULAR_EXPRESSION]\n"
"          [-x|--filter-out-wildcard WILDCARD_PATTERN]\n"
"          [-E|--suppress-ident 0|1]\n"
"          [-L|--suppress-levenshtein-threshold LEV_DIST]\n"
"          [-W|--suppress-word-threshold WORD_DIST]\n"
"          [-n|--compared-message-num COMP_MSGS]\n"
"          [-c|--skip-char-num SKIP_NUM_COMP]\n"
"          [-C|--skip-char-list SKIP_CHAR_COMP]\n"
"logViewer { -V | --version } [-N | --dim-dns DIM_DNS_NODE]\n"
"          [-m | --hostname NODE_PATTERN...]\n"
"          [-s | --logger-name LOG_PATTERN...]\n"
"logViewer { -h | --help }\n"
"\n"
"       SEVERITY_THRESHOLD = VERB | DEBUG | INFO | WARN | ERROR | FATAL\n"
"\n"
"Try \"logViewer -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  fprintf(stderr,"\n%s\n",shortUsageStr);
  rPrintf((errU&~lStd)|L_SYS,6,"%s",shortUsageStr);
  exit(1);
}
/*****************************************************************************/
void usage(int mode)
{
  FILE *fpOut;
  int status;
  const char *formatter;
  char *pattern[6]={dfltVerbRegExpPattern,dfltDebugRegExpPattern,
                    dfltInfoRegExpPattern,dfltWarnRegExpPattern,
                    dfltErrorRegExpPattern,dfltFatalRegExpPattern};
  char *pPattern[6];
  int i;
  char *pi,*pf;
  /*-------------------------------------------------------------------------*/
  for(i=0;i<6;i++)
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
"logViewer.man\n"
"\n"
"..\n"
"%s"
".hw dim\\[hy]dns hostname logger\\[hy]name no\\[hy]color\n"
".hw severity\\[hy]threshold out\\[hy]fifo out\\[hy]file no\\[hy]drop \n"
".hw filter\\[hy]regex filter\\[hy]wildcard filter\\[hy]out\\[hy]regex\n"
".hw filter\\[hy]out\\[hy]wildcard suppress\\[hy]ident\n"
".hw suppress\\[hy]levenshtein\\[hy]threshold\n"
".hw suppress\\[hy]word\\[hy]threshold compared\\[hy]message\\[hy]num\n"
".hw skip\\[hy]char\\[hy]num skip\\[hy]char\\[hy]list version help\n"
".hw DIM_DNS_NODE NODE_PATTERN LOG_PATTERN SEVERITY_THRESHOLD\n"
".hw FIFO_NAME FILE_NAME REGULAR_EXPRESSION WILDCARD_PATTERN LEV_DIST\n"
".hw WORD_DIST COMP_MSGS SKIP_NUM_COMP SKIP_CHAR_COMP\n"
".hw brepeated bexpired synchronized binappropriate bnetwork bunavailable\n"
".hw baddress representable permitted\n"
".hw \n"
".TH logViewer 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis logViewer\\ \\-\n"
"FMC Message Logger displayer, archiver and forwarder\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis logViewer\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".DoubleOpt[] s logger\\[hy]name LOG_PATTERN...\n"
".DoubleOpt[] b no\\[hy]color\n"
".DoubleOpt[] S no\\[hy]old\\[hy]messages\n"
".DoubleOpt[] l severity\\[hy]threshold SEVERITY_THRESHOLD\n"
".DoubleOpt[] o out\\[hy]fifo FIFO_NAME\n"
".DoubleOpt[] O out\\[hy]file FILE_NAME\n"
".DoubleOpt[] A no\\[hy]drop\n"
".DoubleOpt[] F filter\\[hy]regex REGULAR_EXPRESSION\n"
".DoubleOpt[] f filter\\[hy]wildcard WILDCARD_PATTERN\n"
".DoubleOpt[] X filter\\[hy]out\\[hy]regex REGULAR_EXPRESSION\n"
".DoubleOpt[] x filter\\[hy]out\\[hy]wildcard WILDCARD_PATTERN\n"
".DoubleOpt[] E suppress\\[hy]ident 0|1\n"
".DoubleOpt[] L suppress\\[hy]levenshtein\\[hy]threshold LEV_DIST\n"
".DoubleOpt[] W suppress\\[hy]word\\[hy]threshold WORD_DIST\n"
".DoubleOpt[] n compared\\[hy]message\\[hy]num COMP_MSGS\n"
".DoubleOpt[] c skip\\[hy]char\\[hy]num SKIP_NUM_COMP\n"
".DoubleOpt[] C skip\\[hy]char\\[hy]list SKIP_CHAR_COMP\n"
".EndSynopsis\n"
".sp\n"
".Synopsis logViewer\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".DoubleOpt[] s logger\\[hy]name LOG_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis logViewer\n"
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
".SS No-Drop and Congestion-Proof Behavior\n"
".\n"
"The FMC Message Logger can work either in \\fBno-drop\\fP or in "
"\\fBcongestion-proof\\fP mode.\n"
".PP\n"
"If the FIFO is completely filled (this could be due to a network congestion "
"which prevents the FIFO drain), any attempt to write to the FIFO in "
"\\fBno-drop\\fP mode \\fBblocks\\fP the writer application until a message "
"is pulled out of the FIFO, while any attempt to write to the FIFO in "
"\\fBcongestion-proof\\fP mode returns immediately the \\fIEAGAIN\\fP "
"\\fBerror\\fP, without blocking, without writing (dropping therefore the "
"message) and without injecting additional traffic into the network.\n"
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
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"The \\fBlogViewer\\fP is an application of the \\fIFMC Message Logger\\fP "
"which is able:\n"
".RS\n"
".TP\n"
"1. to \\fBdisplay\\fP on the \\fBcurrent terminal\\fP the messages retrieved "
"from one or more selected FMC Message Logger Servers (\\fBlogSrv\\fP(8));\n"
".TP\n"
"2. to \\fBsave\\fP in a \\fBfile\\fP the messages retrieved  from one or "
"more selected FMC Message Logger Servers (\\fBlogSrv\\fP(8));\n"
".TP\n"
"3. to \\fBforward\\fP the messages retrieved from one or more selected FMC "
"Message Logger Servers (\\fBlogSrv\\fP(8)), through a \\fBFIFO\\fP (a.k.a. "
"named pipe), to another FMC Message Logger Servers (\\fBlogSrv\\fP(8)), "
"which act as \"collector\".\n"
".RE\n"
".\n"
".PP\n"
"The \\fBlogViewer\\fP application terminates on \\fICtrl-C\\fP.\n"
".PP\n"
"The DIM Name Server, looked up to seek for Message Logger Servers, can be\n"
"chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\"\\fB"DIM_CONF_FILE_NAME"\\fP\".\n"
".RE\n"
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
".OptDef V version\n"
"For each selected node print the FMC Message Logger\n"
"Server version and the FMC version, than exit.\n"
".\n"
".OptDef m hostname NODE_PATTERN (string, repeatable)\n"
"Contact FMC Message Logger Servers only at the nodes whose hostname\n"
"matches the wildcard pattern \\fINODE_PATTERN\\fP (\\fBdefault\\fP: contact\n"
"the FMC Logger Servers at all the nodes).\n"
".\n"
".OptDef s logger-name LOG_PATTERN (string, repeatable)\n"
"Contact only FMC Message Logger Servers whose logger name matches the\n"
"wildcard pattern \\fILOG_PATTERN\\fP. \\fBDefault\\fP: contact the FMC\n"
"Logger Servers with whatever logger name).\n"
".\n"
".OptDef b no-color\n"
"Print output in \\fIblack & white\\fP (do not include ANSI color\n"
"escape sequences). \\fBDefault\\fP: include ANSI color escape\n"
"sequences. Option \\fB-b\\fP is implicitely activated setting options\n"
"\\fB-o\\fP or \\fB-O\\fP.\n"
".\n"
".OptDef S no-old-messages\n"
"Do not print the old messages stored on servers and published\n"
"in the DIM service \\fIlast_log\\fP.\n"
".\n"
".SS Output Options\n"
".\n"
".OptDef O out-file FILE_NAME (string)\n"
"Send output to \\fBfile\\fP \\fIFILE_NAME\\fP. \\fBDefault\\fP: send output\n"
"to stdout.\n"
".\n"
".OptDef o out-fifo FIFO_NAME (string)\n"
"Send output to \\fBFIFO\\fP (named pipe) \\fIFIFO_NAME\\fP. \n"
"\\fBDefault\\fP: send output to stdout.\n"
".\n"
".OptDef A no-drop\n"
"Use the \\fBno-drop policy\\fP in writing to a \\fBFIFO\\fP.\n"
"\\fBDefault\\fP: use the \\fBcongestion-proof\\fP policy.\n"
".\n"
".SS Filter Options\n"
".\n"
".OptDef l severity-threshold SEVERITY_THRESHOLD (integer or string)\n"
"Send to output only messages of \\fBseverity\\fP \\[>=]\n"
"\\fISEVERITY_THRESHOLD\\fP. As string, \\fBALL\\fP\\~=\\~0, "
"\\fBVERB\\fP\\~=\\~1, \\fBDEBUG\\fP\\~=\\~2, \\fBINFO\\fP\\~=\\~3, "
"\\fBWARN\\fP\\~=\\~4, \\fBERROR\\fP\\~=\\~5, \\fBFATAL\\fP\\~=6\\~.\n"
"\\fBDefault\\fP: \\fISEVERITY_THRESHOLD\\fP\\~=\\~1\\~=\\~\\fBVERB\\fP.\n"
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
"\\fIWORD_DIST\\fP subsituted word, with respect to at least one of\n"
"the previous \\fICOMP_MSGS\\fP-1 messages. \\fBDefault\\fP: do not suppress\n"
"repeated messages. \\fIN.B.\\fP: the behaviour with this option is\n"
"affected by \\fB-c\\fP, \\fB-C\\fP and \\fB-n\\fP options.\n"
".\n"
".OptDef n compared-message-num COMP_MSGS (integer)\n"
"To suppress repeated messages, consider the last \\fICOMP_MSGS\\fP\n"
"messages, i.e. compare each message with the previous\n"
"\\fICOMP_MSGS\\fP-1 messages. \\fBDefault\\fP: 1 if repeated message\n"
"suppression is not active, 2 if it is active. \\fIN.B.\\fP: affect the\n"
"behaviour with \\fB-E\\fP, \\fB-L\\fP and \\fB-W\\fP options.\n"
".\n"
".OptDef c skip-char-num SKIP_NUM_COMP (integer)\n"
"In the comparison for the repeated message suppression, skip\n"
"\\fISKIP_NUM_COMP\\fP characters in each string, i.e. start the\n"
"comparison between a couple of string from the\n"
"\\fISKIP_NUM_COMP\\fP-th character. Affect the behaviour with \\fB-E\\fP, \\fB-L\\fP\n"
"and \\fB-W\\fP options.\n"
".\n"
".OptDef C skip-char-list SKIP_CHAR_COMP (string)\n"
"In the comparison for the repeated message suppression, skip\n"
"characters in each string until all the characters listed in\n"
"the string \\fISKIP_CHAR_COMP\\fP are passed. Affect the behaviour\n"
"with \\fB-E\\fP, \\fB-L\\fP and \\fB-W\\fP options.\n"
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
"Printed in diagnostic messages, used to identify the \\fBlogViewer\\fP\n"
"instance.\n"
".\n"
".TP\n"
".EnvVar deBug \\ (int,\\ optional)\n"
"Debug level. Can be set to 0..0x2.\n"
".RS\n"
".TP\n"
"0x1: print signal received.\n"
".TP\n"
"0x2: print diagnostics on repeated message suppression.\n"
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
"\\fBDefault\\fP: \"%s\".\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Read messages from all the loggers:\n"
".\n"
".PP\n"
".ShellCommand logViewer\n"
".ShellCommand logViewer -m \\[dq]*\\[dq] -s \\[dq]*\\[dq]\n"
".ShellCommand logViewer -m \\[rs]* -s \\[rs]*\n"
".\n"
".PP\n"
"Read messages only from the logger \"gaudi\" at the node \"farm0101\":\n"
".\n"
".PP\n"
".ShellCommand logViewer -m farm0101 -s gaudi\n"
".\n"
".PP\n"
"Read messages from the loggers \"gaudi\" and \"fmc\" at the nodes\n"
"\"farm0101\" and \"farm0102\":\n"
".\n"
".PP\n"
".ShellCommand logViewer -m farm0101 -m farm0102 -s gaudi -s fmc\n"
".\n"
".PP\n"
"Use wildcard patterns to select names and nodes:\n"
".\n"
".PP\n"
".ShellCommand logViewer -m \\[dq]farm010[123]\\[dq] -m \\[dq]farm020?\\[dq] "
"-s \\[dq]*au*\\[dq] -s \\[dq]*mc\\[dq]\n"
".ShellCommand logViewer -m \\[dq]farm0[0-2]0[0-9]\\[dq] "
"-m \\[dq]farm120?\\[dq] -s \\[dq]*au*\\[dq] -s \\[dq]*mc\\[dq]\n"
".\n"
".PP\n"
"Filter through the lines containing the string \"Srv\":\n"
".\n"
".PP\n"
".ShellCommand logViewer -s gaudi -f \\[dq]*Srv*\\[dq]\n"
".ShellCommand logViewer -s gaudi -F \\[dq]Srv\\[dq]\n"
".\n"
".PP\n"
"Filter out the lines containing the string \"Srv\":\n"
".\n"
".PP\n"
".ShellCommand logViewer -s gaudi -x \\[dq]*Srv*\\[dq]\n"
".ShellCommand logViewer -s gaudi -X \\[dq]Srv\\[dq]\n"
".\n"
".PP\n"
"Filter through the lines containing the word \"word1\" near and before\n"
"the word \"word2\" (at most 4 words between):\n"
".\n"
".PP\n"
".ShellCommand logViewer -F \\[dq]\\[rs]bword1\\[rs]W+(\\[rs]w+\\[rs]W+){0,4}"
"word2\\[rs]b\\[dq]\n"
".\n"
".PP\n"
"Filter through the lines containing the word \"word1\" near the word\n"
"\"word2\" (at most 4 words between) in any order:\n"
".\n"
".PP\n"
".ShellCommand logViewer -F \\[dq]\\[rs]bword1\\[rs]W+(\\[rs]w+\\[rs]W+){0,4}"
"word2\\[rs]b|\\[rs]bword2\\[rs]W+(\\[rs]w+\\[rs]W+)\\:{0,4}word1\\[rs]b"
"\\[dq]\n"
".\n"
".PP\n"
"Filter through the lines containing a floating point number:\n"
".\n"
".PP\n"
".ShellCommand logViewer -F \\[dq][-+]?[0-9]*\\[rs].[0-9]+([eE][-+]?[0-9]+)?\\[dq]\n"
".\n"
".PP\n"
"Filter and send output to another FIFO:\n"
".\n"
".PP\n"
".ShellCommand logViewer -m \\[dq]farm01*\\[dq] -s fmc -o /tmp/log_f.fifo "
"-F \\[dq]ipmiSrv\\[dq]\n"
".\n"
".PP\n"
"Merge several message flows, filter and forward output to another FIFO:\n"
".\n"
".PP\n"
".ShellCommand logViewer -m \\[dq]farm01*\\[dq] -m \\[dq]farm02*\\[dq] "
"-s fmc -s gaudi\\ \\:-o /tmp/log_t.fifo -F \\[dq]ipmiSrv\\[dq]\n"
".\n"
".PP\n"
"Forward severe Task Manager Messages on node \"farm0101\" involving "
"\"gaudi.exe\" also to the Gaudi Logger:\n"
".\n"
".PP\n"
".ShellCommand logViewer -m farm0101 -s fmc -l WARN -o /tmp/logGaudi.fifo\\ "
"\\:-F \\[dq]gaudi.exe\\[dq]\n"
".\n"
".PP\n"
"Suppress messages which are identical to the previous one:\n"
".\n"
".PP\n"
".ShellCommand logViewer -E 1\n"
".\n"
".PP\n"
"Suppress messages which are identical after the 30-th character to the"
"\n"
"previous one:\n"
".\n"
".PP\n"
".ShellCommand logViewer -E 1 -c 30\n"
".\n"
".PP\n"
"Suppress messages which are identical after the 30-th character to one"
"\n"
"of the previous 10 messages:\n"
".\n"
".PP\n"
".ShellCommand logViewer -E 1 -c 30 -n 10\n"
".\n"
".PP\n"
"Suppress messages which are identical, after the second \":\" in the\n"
"message, to one of the previous 10 messages:\n"
".\n"
".PP\n"
".ShellCommand logViewer -E 1 -C \\[dq]::\\[dq] -n 10\n"
".\n"
".PP\n"
"Suppress messages which differ, in at most 2 words substituted after\n"
"the second \":\" with respect to one of the previous 10 messages:\n"
".\n"
".PP\n"
".ShellCommand logViewer -W 2 -C \\[dq]::\\[dq] -n 10\n"
".\n"
".PP\n"
"Suppress messages which differs, in at most 6 character substituted,\n"
"inserted or deleted after the second \":\" with respect to one of the"
"\n"
"previous 10 messages:\n"
".\n"
".PP\n"
".ShellCommand logViewer -L 6 -C \\[dq]::\\[dq] -n 10\n"
".\n"
".PP\n"
"%s"
"%s"
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
".SS FMC Message Logger Server and User Commands\n"
".BR \\%%logSrv (8),\n"
".BR \\%%logGetProperties (1),\n"
".BR \\%%logSetProperties (1).\n"
".SS UI for writing to an arbitrary FMC Message Logger from a shell script\n"
".BR \\%%mPrint (1).\n"
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
".SS Regular expression and wildcard pattern syntax\n"
".BR \\%%regex (7),\n"
".BR \\%%glob (7).\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro,FMC_DATE,FMC_VERSION
,pPattern[0],pPattern[1],pPattern[2],pPattern[3],pPattern[4],pPattern[5]
,FMC_URLS,FMC_AUTHORS
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/*****************************************************************************/
