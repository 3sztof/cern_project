/* ************************************************************************* */
/*
 * $Log: tmStart.C,v $
 * Revision 6.5  2012/12/13 15:59:49  galli
 * minor changes
 *
 * Revision 6.4  2012/12/03 16:20:52  galli
 * Send -k option only if Task Manager Server version >= 5.1
 *
 * Revision 6.3  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 6.2  2012/10/09 11:49:02  galli
 * Bug fixed
 *
 * Revision 6.1  2012/10/09 11:05:02  galli
 * Man page updated
 *
 * Revision 6.0  2012/10/08 14:03:56  galli
 * Makes some check on command line options and parameters
 *
 * Revision 5.2  2012/08/30 15:06:34  galli
 * minor changes
 *
 * Revision 5.0  2011/11/29 13:42:36  galli
 * Compatible with tmSrv v 5.
 * Can set the oom_adj or oom_score_adj of started processes.
 *
 * Revision 2.7  2008/08/26 10:26:36  galli
 * groff manual
 *
 * Revision 2.4  2008/07/07 11:21:49  galli
 * service names starts with /FMC
 *
 * Revision 2.3  2007/12/14 09:20:44  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 2.1  2007/10/23 15:54:52  galli
 * usage() prints FMC version
 *
 * Revision 2.0  2007/09/29 22:21:27  galli
 * compatible with tmSrv 2.2
 * authentication
 * command-line switch --no-authentication for compatibility with
 * tmSrv version < 2
 *
 * Revision 1.20  2007/08/16 12:43:08  galli
 * usage() modified
 *
 * Revision 1.17  2007/06/28 13:31:58  galli
 * usage() updated for tmSrv.c v1.53 and tmUtils.c v1.39
 *
 * Revision 1.15  2006/09/07 12:15:56  galli
 * more usage() examples
 *
 * Revision 1.14  2006/09/07 10:21:54  galli
 * -m option is repeatable
 * usage and messages improved
 *
 * Revision 1.13  2005/09/02 13:01:31  galli
 * more diagnostics
 *
 * Revision 1.12  2005/09/02 12:51:15  galli
 * -m option commutes with option to tmSrv
 *
 * Revision 1.11  2005/08/26 07:35:06  galli
 * more restrictive DIM wildcard pattern to speed-up browsing
 *
 * Revision 1.7  2004/11/21 20:13:25  galli
 * option -m hostname_pattern substitutes option -s svc_pattern
 * avoid use of "--" (end of option-scanning)
 *
 * Revision 1.3  2004/10/15 00:17:46  galli
 * bug fixed
 *
 * Revision 1.1  2004/10/13 14:19:56  galli
 * Initial revision
 */
/* ************************************************************************* */
#include <vector>
#include <string>
#include <set>
using namespace std;
/* ------------------------------------------------------------------------- */
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <argz.h>                         /* argz_create(), argz_stringify() */
#include <ctype.h>                                              /* toupper() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
#include <pwd.h>                                              /* getpwuid(3) */
#include <sys/wait.h>                                             /* wait(2) */
#include <getopt.h>                                         /* getopt_long() */
/* ------------------------------------------------------------------------- */
/* DIM */
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                                             /* itos() */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcRef.h"                                  /* FMC_URLS FMC_AUTHORS */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/* ------------------------------------------------------------------------- */
#define _CLIENT 1
#include "tmSrv.h"                             /* SRV_NAME, TMLL_FIELD_N_ERR */
/* ************************************************************************* */
/* function prototype */
void usage(int mode);
void shortUsage(void);
void getServerVersion(vector<string>hostPttnLs);
int getLocalOpt(int *argcP,char ***argvP,char* optS,int withArg,char **optArg);
char *getAuthString(void);
/* ************************************************************************* */
int deBug=0;
static char rcsid[]="$Id: tmStart.C,v 6.5 2012/12/13 15:59:49 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
/* ************************************************************************* */
int main(int argc,char **argv)
{
  /* counters */
  int i=0,j=0;
  int sc1,sc2;
  unsigned k;
  size_t b,e;
  /* pointers */
  char *p=NULL;
  /* service browsing */
  char *dimDnsNode=NULL;
  vector<string> hostPttnLs;                          /* host pattern vector */
  vector<string>::iterator hpIt;             /* host pattern vector iterator */
  string hostPttn;
  string startRawCmdPttn;
  string startCmdPttn;
  string startCmd;
  string tmVerSvc;
  string tmVersion,tmVersionStr;
  bool oomAdjSupported;
  DimBrowser br;
  char *format=NULL;
  int type=0;
  int nodeFoundN=0;
  string nodeFound;
  /* command line switch */
  int doPrintServerVersion=0;
  int authenticated=1;
  /* command line arguments */
  char *tail=NULL;
  int scheduler=0;
  const char *ss[3]={"SCHED_OTHER","SCHED_FIFO","SCHED_RR"};
  int niceLevel=0;
  int rtPriority=0;
  int cpuNum=0;
  string envDef;
  size_t eqPos;
  string envName;
  string::iterator eNIt;
  int oomScoreAdj=0;
  string userName;
  string::iterator uNIt;
  string groupName;
  string::iterator gNIt;
  string outFifo;
  string errFifo;
  string::iterator ptIt;
  /* DIM command */
  string dimCmdOpts;
  string compatDimCmdOpts;
  string dimCmdArgs;
  string dimCmd;
  string compatDimCmd;
  /* related to authentication */
  string authString;
  /* getopt */
  int flag;
  /* short command line options */
  const char *shortOptions="vN:Xm:cD:s:p:r:a:k:n:g:eoE:O:Adu:w:Vh::";
  /* long command line options */
  static struct option longOptions[]=
  {
    /* ..................................................................... */
    /* General Options */
    {"debug",no_argument,NULL,'v'},
    {"dim-dns",required_argument,NULL,'N'},
    /* ..................................................................... */
    /* Authentication Options */
    {"no-authentication",no_argument,NULL,'X'},
    /* ..................................................................... */
    /* Node Selection Options */
    {"hostname",required_argument,NULL,'m'},
    /* ..................................................................... */
    /* Process Environment Options */
    {"clr-env",no_argument,NULL,'c'},
    {"set-env",required_argument,NULL,'D'},
    /* ..................................................................... */
    /* Process Scheduling Options */
    {"scheduler",required_argument,NULL,'s'},
    {"nice",required_argument,NULL,'p'},
    {"rtprio",required_argument,NULL,'r'},
    {"cpu",required_argument,NULL,'a'},
    /* ..................................................................... */
    /* Process OOK-killer Options */
    {"oom-score-adj",required_argument,NULL,'k'},
    /* ..................................................................... */
    /* Process Owner Options */
    {"user",required_argument,NULL,'n'},
    {"group",required_argument,NULL,'g'},
    /* ..................................................................... */
    /* Process Output Redirection Options */
    {"err-to-stdlog",no_argument,NULL,'e'},
    {"out-to-stdlog",no_argument,NULL,'o'},
    {"err-to-log",required_argument,NULL,'E'},
    {"out-to-log",required_argument,NULL,'O'},
    {"no-drop",no_argument,NULL,'A'},
    /* ..................................................................... */
    /* Other Process Options */
    {"daemon",no_argument,NULL,'d'},
    {"utgid",required_argument,NULL,'u'},
    {"wd",required_argument,NULL,'w'},
    /* ..................................................................... */
    /* Accessory Options */
    {"version",no_argument,NULL,'V'},
    {"help",no_argument,NULL,'h'},
    /* ..................................................................... */
    {NULL,0,NULL,0}
  };
  /* ----------------------------------------------------------------------- */
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /* ----------------------------------------------------------------------- */
  opterr=0;                  /* do not print default error message to stderr */
  optind=0;
  /* stops option processing as soon as a non-option argument is encountered */
  setenv("POSIXLY_CORRECT","1",1);
  do
  {
    int curind=optind;
    flag=getopt_long(argc,argv,shortOptions,longOptions,NULL);
    switch(flag)
    {
      /* ................................................................... */
      /* General Options */
      case 'v': /* -v, --debug */
        deBug++;
        break;
      case 'N': /* -N, --dim-dns */
        dimDnsNode=optarg;
        break;
      /* ................................................................... */
      /* Authentication Options */
      case 'X': /* -X, --no-authentication */
        authenticated=0;
        break;
      /* ................................................................... */
      /* Node Selection Options */
      case 'm': /* -m, --hostname */
        hostPttnLs.push_back(optarg);
        break;
      /* ................................................................... */
      /* Process Environment Options */
      case 'c': /* -c, --clr-env */
        dimCmdOpts+=" -c";
        compatDimCmdOpts+=" -c";
        break;
      case 'D': /* -D, --set-env */
        envDef=optarg;
        eqPos=envDef.find_first_of("=");
        if(eqPos==string::npos)                                      /* NAME */
        {
          envName=envDef;
        }
        else                                                   /* NAME=VALUE */
        {
          envName=envDef.substr(0,eqPos);
        }
        if(isdigit(envName[0]))                      /* IEEE Std 1003.1-2001 */
        {
          fprintf(stderr,"\nERROR: INVALID ENVIRONMENT VARIABLE NAME: "
                         "\"%s\"!\n",envName.c_str());
          fprintf(stderr,"\nEnvironment variable name may not begin with a "
                         "digit!\n");
          shortUsage();
        }
        for(eNIt=envName.begin();eNIt<envName.end();eNIt++)
        {
          if(!isgraph(*eNIt))                                    /* extended */
          {
            fprintf(stderr,"\nERROR: INVALID ENVIRONMENT VARIABLE NAME: "
                           "\"%s\"!\n",envName.c_str());
            fprintf(stderr,"\nInvalid character '%c' (%#2.2x) in Environment "
                           "variable name \"%s\"!\n",*eNIt,*eNIt,
                           envName.c_str());
            shortUsage();
          }
          else if(!isalnum(*eNIt) && *eNIt!='_')     /* IEEE Std 1003.1-2001 */
          {
            fprintf(stdout,"\nWARNING: Environment Variable Name: "
                           "\"%s\" NOT Conforming with IEEE Std "
                           "1003.1-2001!\n",envName.c_str());
            fprintf(stdout,"Character '%c' (%#2.2x) in Environment variable "
                           "name \"%s\" is not allowed in IEEE Std "
                           "1003.1-2001!\n",*eNIt,*eNIt,envName.c_str());
            fprintf(stdout,"Going on...\n");
          }
        }
        dimCmdOpts+=" -D ";
        dimCmdOpts+=optarg;
        compatDimCmdOpts+=" -D ";
        compatDimCmdOpts+=optarg;
        break;
      /* ................................................................... */
      /* Process Scheduling Options */
      case 's': /* -s, --scheduler */
        if(strcasestr(optarg,"other"))scheduler=SCHED_OTHER;
        else if(strcasestr(optarg,"ts"))scheduler=SCHED_OTHER;
        else if(strcasestr(optarg,"shar"))scheduler=SCHED_OTHER;
        else if(strcasestr(optarg,"fifo"))scheduler=SCHED_FIFO;
        else if(strcasestr(optarg,"ff"))scheduler=SCHED_FIFO;
        else if(strcasestr(optarg,"rr"))scheduler=SCHED_RR;
        else if(strcasestr(optarg,"round"))scheduler=SCHED_RR;
        else if(strcasestr(optarg,"robin"))scheduler=SCHED_RR;
        else
        {
          tail=NULL;
          scheduler=(int)strtol(optarg,&tail,0);
          if(*tail)
          {
            fprintf(stderr,"\nERROR: UNKNOWN SCHEDULER: \"%s\"!\n",tail);
            fprintf(stderr,"\nOnly values 0, 1, 2, \"ts\", \"time-sharing\", "
                           "\"fifo\", \"rr\", \"round-robin\" are allowed for "
                           "SCHEDULER.\n");
            shortUsage();
          }
        }
        dimCmdOpts+=" -s ";
        dimCmdOpts+=FmcUtils::itos(scheduler);
        compatDimCmdOpts+=" -s ";
        compatDimCmdOpts+=FmcUtils::itos(scheduler);
        break;
      case 'p': /* -p, --nice */
        tail=NULL;
        niceLevel=(int)strtol(optarg,&tail,0);
        if(*tail)
        {
          fprintf(stderr,"\nERROR: NICE_LEVEL: \"%s\" NOT ALLOWED!\n",tail);
          fprintf(stderr,"\nOnly values -20..19 allowed for NICE_LEVEL!\n");
          shortUsage();
        }
        dimCmdOpts+=" -p ";
        dimCmdOpts+=FmcUtils::itos(niceLevel);
        compatDimCmdOpts+=" -p ";
        compatDimCmdOpts+=FmcUtils::itos(niceLevel);
        break;
      case 'r': /* -r, --rtprio */
        tail=NULL;
        rtPriority=(int)strtol(optarg,&tail,0);
        if(*tail)
        {
          fprintf(stderr,"\nERROR: RT_PRIORITY: \"%s\" NOT ALLOWED!\n",tail);
          fprintf(stderr,"\nOnly values 1..99 allowed for RT_PRIORITY!\n");
          fprintf(stderr,"Maximum RT_PRIORITY is also limited by Task Manager "
                         "IO-thread priority which -- by default -- is 93!\n");
          shortUsage();
        }
        dimCmdOpts+=" -r ";
        dimCmdOpts+=FmcUtils::itos(rtPriority);
        compatDimCmdOpts+=" -r ";
        compatDimCmdOpts+=FmcUtils::itos(rtPriority);
        break;
      case 'a': /* -a, --cpu */
        tail=NULL;
        cpuNum=(int)strtol(optarg,&tail,0);
        if(*tail)
        {
          fprintf(stderr,"\nERROR: CPU_NUM: \"%s\" NOT ALLOWED!\n",tail);
          fprintf(stderr,"\nOnly positive integer values allowed for "
                         "CPU_NUM!\n");
          shortUsage();
        }
        if(cpuNum<0)
        {
          fprintf(stderr,"\nERROR: CPU_NUM: %d NOT ALLOWED!\n",cpuNum);
          fprintf(stderr,"\nOnly positive integer values allowed for "
                         "CPU_NUM!\n");
          shortUsage();
        }
        dimCmdOpts+=" -a ";
        dimCmdOpts+=FmcUtils::itos(cpuNum);
        compatDimCmdOpts+=" -a ";
        compatDimCmdOpts+=FmcUtils::itos(cpuNum);
        break;
      /* ................................................................... */
      /* Process OOK-killer Options */
      case 'k': /* -k, --oom-score-adj */
        tail=NULL;
        oomScoreAdj=(int)strtol(optarg,&tail,0);
        if(*tail)
        {
          fprintf(stderr,"\nERROR: OOM_SCORE_ADJ: \"%s\" NOT ALLOWED!\n",tail);
          fprintf(stderr,"\nOnly values -999..1000 allowed for "
                         "OOM_SCORE_ADJ!\n");
          shortUsage();
        }
        dimCmdOpts+=" -k ";
        dimCmdOpts+=FmcUtils::itos(oomScoreAdj);
        break;
      /* ................................................................... */
      /* Process Owner Options */
      case 'n': /* -n, --user */
        userName=optarg;
        dimCmdOpts+=" -n ";
        dimCmdOpts+=userName;
        compatDimCmdOpts+=" -n ";
        compatDimCmdOpts+=userName;
        break;
      case 'g': /* -g, --group */
        groupName=optarg;
        dimCmdOpts+=" -g ";
        dimCmdOpts+=groupName;
        compatDimCmdOpts+=" -g ";
        compatDimCmdOpts+=groupName;
        break;
      /* ................................................................... */
      /* Process Output Redirection Options */
      case 'e': /* -e, --err-to-stdlog */
        dimCmdOpts+=" -e";
        compatDimCmdOpts+=" -e";
        break;
      case 'o': /* -o, --out-to-stdlog */
        dimCmdOpts+=" -o";
        compatDimCmdOpts+=" -o";
        break;
      case 'E': /* -E, --err-to-log */
        errFifo=optarg;
        dimCmdOpts+=" -E ";
        dimCmdOpts+=errFifo;
        compatDimCmdOpts+=" -E ";
        compatDimCmdOpts+=errFifo;
        break;
      case 'O': /* -O, --out-to-log */
        outFifo=optarg;
        dimCmdOpts+=" -O ";
        dimCmdOpts+=outFifo;
        compatDimCmdOpts+=" -O ";
        compatDimCmdOpts+=outFifo;
        break;
      case 'A': /* -A, --no-drop */
        dimCmdOpts+=" -A";
        compatDimCmdOpts+=" -A";
        break;
      /* ................................................................... */
      /* Other Process Options */
      case 'd': /* -d, --daemon */
        dimCmdOpts+=" -d";
        compatDimCmdOpts+=" -d";
        break;
      case 'u': /* -u, --utgid */
        dimCmdOpts+=" -u ";
        dimCmdOpts+=optarg;
        compatDimCmdOpts+=" -u ";
        compatDimCmdOpts+=optarg;
        break;
      case 'w': /* -w, --wd */
        dimCmdOpts+=" -w ";
        dimCmdOpts+=optarg;
        compatDimCmdOpts+=" -w ";
        compatDimCmdOpts+=optarg;
        break;
      /* ................................................................... */
      /* accessory options */
      case 'V': /* -V, --version */
        doPrintServerVersion=1;
        break;
      case 'h': /* -h, -hh, --help */
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
        break;
      /* ................................................................... */
      case EOF:
        break;
      /* ................................................................... */
      /* unrecognized option */
      case '?':
        if(optopt)
        {
          fprintf(stdout,"\nERROR: Invalid short option \"-%c\"!\n",
                  optopt);
        }
        else
        {
          if(curind==0)curind++;
          fprintf(stdout,"\nERROR: Invalid long option \"%s\"!\n",
                  argv[curind]);
        }
        shortUsage();
        break;
      /* ................................................................... */
      default:
        fprintf(stderr,"\nERROR: getopt_long() error %d!\n",optopt);
        shortUsage();
        break;
      /* ................................................................... */
    }
  }
  while(flag!=EOF && flag!='?');
  /* ----------------------------------------------------------------------- */
  /* default wildcard for hostPttnLs */
  if(hostPttnLs.empty())hostPttnLs.push_back("*");
  /* ----------------------------------------------------------------------- */
  /* check hostPttnLs */
  for(hpIt=hostPttnLs.begin();hpIt!=hostPttnLs.end();hpIt++)
  {
    if(hpIt->find_first_of('/')!=string::npos)
    {
      fprintf(stderr,"\nERROR: Illegal NODE_PATTERN "
              "\"%s\"! A NODE_PATTERN specified with the command-line option "
              "\"-m NODE_PATTERN\" or \"--hostname NODE_PATTERN\" must not "
              "contain the '/' character!\n",hpIt->c_str());
      shortUsage();
    }
  }
  /* ----------------------------------------------------------------------- */
  if(doPrintServerVersion)getServerVersion(hostPttnLs);
  /* ----------------------------------------------------------------------- */
  /* check for the presence of at least 1 non-option argument */
  if(optind==argc)
  {
    fprintf(stderr,"\nERROR: Executable file PATH (non-option argument) "
                   "required!\n");
    shortUsage();
  }
  /* ----------------------------------------------------------------------- */
  /* check command line options */
  if(scheduler<SCHED_OTHER || scheduler>SCHED_RR)
  {
    fprintf(stderr,"\nERROR: UNKNOWN SCHEDULER: %d!\n", scheduler);
    fprintf(stderr,"\nOnly values 0, 1, 2, \"ts\", \"time-sharing\", "
                   "\"fifo\", \"rr\", \"round-robin\" are allowed for "
                   "SCHEDULER.\n");
    shortUsage();
  }
  if(niceLevel<-20 || niceLevel>19)
  {
    fprintf(stderr,"\nERROR: NICE_LEVEL = %d NOT ALLOWED!\n",niceLevel);
    fprintf(stderr,"\nOnly values -20..19 allowed for NICE_LEVEL!\n");
    shortUsage();
  }
  if(scheduler==SCHED_OTHER && rtPriority!=0)
  {
    fprintf(stderr,"\nERROR: RT_PRIORITY = %d NOT ALLOWED for SCHED_OTHER "
                   "time sharing scheduler!\n",rtPriority);
    fprintf(stderr,"\nOnly RT_PRIORITY = 0 allowed for SCHED_OTHER time "
                   "sharing scheduler!\n");
    shortUsage();
  }
  if((scheduler==SCHED_FIFO||scheduler==SCHED_RR)&&
     (rtPriority<1||rtPriority>99))
  {
    fprintf(stderr,"\nERROR: RT_PRIORITY = %d NOT ALLOWED for %s "
                   "real time scheduler!\n",rtPriority,ss[scheduler]);
    fprintf(stderr,"\nOnly values 1..99 allowed for RT_PRIORITY!\n");
    fprintf(stderr,"Maximum RT_PRIORITY is also limited by Task Manager "
                   "IO-thread priority which -- by default -- is 93!\n");
    shortUsage();
  }
  if(oomScoreAdj<-999 || oomScoreAdj>1000)
  {
    fprintf(stderr,"\nERROR: OOM_SCORE_ADJ: %d NOT ALLOWED!\n",oomScoreAdj);
    fprintf(stderr,"\nOnly values -999..1000 allowed for OOM_SCORE_ADJ!\n");
    shortUsage();
  }
  /* NAME_REGEX="^[a-z][-a-z0-9]*\$" */
  if(!userName.empty())
  {
    if(!islower(userName[0]))
    {
      fprintf(stderr,"\nERROR: INVALID USER_NAME: \"%s\"!\n",userName.c_str());
      fprintf(stderr,"\nUSER_NAME must begin with a lower case character!\n");
      shortUsage();
    }
    for(uNIt=userName.begin();uNIt<userName.end();uNIt++)
    {
      if(!islower(*uNIt) && !isdigit(*uNIt) && *uNIt!='-')
      {
        fprintf(stderr,"\nERROR: INVALID USER_NAME: \"%s\"!\n",
                userName.c_str());
        fprintf(stderr,"\nInvalid character '%c' (%#2.2x) in USER_NAME "
                       "\"%s\"!\n",*uNIt,*uNIt, userName.c_str());
        shortUsage();
      }
    }
  }
  if(!groupName.empty())
  {
    if(!islower(groupName[0]))
    {
      fprintf(stderr,"\nERROR: INVALID GROUP_NAME: \"%s\"!\n",
              groupName.c_str());
      fprintf(stderr,"\nGROUP_NAME must begin with a lower case character!\n");
      shortUsage();
    }
    for(gNIt=groupName.begin();gNIt<groupName.end();gNIt++)
    {
      if(!islower(*gNIt) && !isdigit(*gNIt) && *gNIt!='-')
      {
        fprintf(stderr,"\nERROR: INVALID GROUP_NAME: \"%s\"!\n",
                groupName.c_str());
        fprintf(stderr,"\nInvalid character '%c' (%#2.2x) in GROUP_NAME "
                       "\"%s\"!\n",*gNIt,*gNIt, groupName.c_str());
        shortUsage();
      }
    }
  }
  if(!errFifo.empty())
  {
    if(errFifo.at(0)=='-')
    {
      fprintf(stderr,"\nERROR: INVALID ERR_FIFO: \"%s\"!\n",
              errFifo.c_str());
      fprintf(stderr,"\nERR_FIFO may not begin with a hyphen!\n");
      shortUsage();
    }
    for(ptIt=errFifo.begin();ptIt<errFifo.end();ptIt++)
    {
      if(!isalpha(*ptIt) && !isdigit(*ptIt) && !index("-_/.~",*ptIt))
      {
        fprintf(stderr,"\nERROR: INVALID ERR_FIFO: \"%s\"!\n",
                errFifo.c_str());
        fprintf(stderr,"\nInvalid character '%c' (%#2.2x) in ERR_FIFO \"%s\"!"
                       "\n",*ptIt,*ptIt,errFifo.c_str());
        shortUsage();
      }
    }
  }
  /* ----------------------------------------------------------------------- */
  /* set the DIM DNS node */
  printf("\n");
  if(dimDnsNode)
  {
    printf("DIM_DNS_NODE: \"%s\" (from -N command-line option).\n",
           dimDnsNode);
  }
  else
  {
    dimDnsNode=getenv("DIM_DNS_NODE");
    if(dimDnsNode)
    {
      printf("DIM_DNS_NODE: \"%s\" (from DIM_DNS_NODE environment variable).\n",
             dimDnsNode);
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
        fprintf(stderr,"\nERROR: DIM Name Server (DIM_DNS_NODE) not defined!\n"
                "DIM Name Server can be defined (in decreasing order of "
                "priority):\n"
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
    fprintf(stderr,"setenv(): %s!\n",strerror(errno));
    exit(1);
  }
  /* ----------------------------------------------------------------------- */
  printf("Node patterns list: ");
  for(hpIt=hostPttnLs.begin();hpIt<hostPttnLs.end();hpIt++)
  {
    printf("\"%s\"",hpIt->c_str());
    if(*hpIt!=hostPttnLs.back())printf(", ");
  }
  printf(".\n");
  /* ----------------------------------------------------------------------- */
  for(i=optind;i<argc;i++)
  {
    dimCmdArgs+=" ";
    dimCmdArgs+=argv[i];
  }
  dimCmd=dimCmdOpts+dimCmdArgs;
  compatDimCmd=compatDimCmdOpts+dimCmdArgs;
  /* ----------------------------------------------------------------------- */
  if(deBug)
  {
    printf("---------------------------------\n");
    printf("dimCmdOpts = \"%s\"\n",dimCmdOpts.c_str());
    printf("dimCmdArgs = \"%s\"\n",dimCmdArgs.c_str());
    printf("dimCmd     = \"%s\"\n",dimCmd.c_str());
    printf("---------------------------------\n");
  }
  else
  {
    printf("DIM command: \"start%s\"\n",dimCmd.c_str());
  }
  /* ----------------------------------------------------------------------- */
  /* authentication */
  authString=string(getAuthString());
  if(authenticated)
  {
    dimCmd=authString+dimCmd;
    compatDimCmd=authString+compatDimCmd;
  }
  /* ----------------------------------------------------------------------- */
  /* loop over node patterns */
  for(hpIt=hostPttnLs.begin(),j=0;hpIt<hostPttnLs.end();hpIt++)
  {
    hostPttn=*hpIt;
    printf("* Node pattern: \"%s\":\n",hostPttn.c_str());
    hostPttn=FmcUtils::toUpper(hostPttn);
    startRawCmdPttn=string("/FMC/*/")+SRV_NAME+"/start";
    startCmdPttn=string("/FMC/")+hostPttn+"/"+SRV_NAME+"/start";
    if(deBug)printf("  Command pattern: \"%s\"\n",startCmdPttn.c_str());
    /* ..................................................................... */
    /* count the number of the slashes */
    for(k=0,sc1=0;k<startCmdPttn.size();k++)
    {
      if(startCmdPttn.at(k)=='/')sc1++;
    }
    /* ..................................................................... */
    br.getServices(startRawCmdPttn.c_str());
    nodeFoundN=0;
    while((type=br.getNextService(p,format))!=0)          /* loop over nodes */
    {
      if(!fnmatch(startCmdPttn.c_str(),p,0))
      {
        startCmd=p;
        /* ................................................................. */
        /* count the number of the slashes */
        for(k=0,sc2=0;k<startCmd.size();k++)
        {
          if(startCmd.at(k)=='/')sc2++;
        }
        if(sc2!=sc1)continue;
        /* ................................................................. */
        /* extract node hostname from "success" service name */
        e=startCmd.find_last_of('/');                          /* last slash */
        e=startCmd.find_last_of('/',e-1);              /* last-but-one slash */
        for(k=0;k<8;k++)e=startCmd.find_last_of('/',e-1);
        b=startCmd.find_last_of('/',e-1)+1;
        nodeFound=startCmd.substr(b,e-b);
        nodeFound=FmcUtils::toLower(nodeFound);
        /* ................................................................. */
        tmVerSvc=startCmd;
        b=tmVerSvc.rfind("/start");                            /* last slash */
        tmVerSvc=tmVerSvc.replace(b,6,"/server_version");
        /* ................................................................. */
        nodeFoundN++;
        /* ................................................................. */
        DimCurrentInfo TmVersion(tmVerSvc.c_str(),(char*)"N/A");
        tmVersionStr=string((char*)TmVersion.getData());
        b=tmVersionStr.find_first_of(' ')+1;
        e=tmVersionStr.find_first_of(' ',b);
        tmVersion=tmVersionStr.substr(b,e-b);
        if(strverscmp(tmVersion.c_str(),"5.1")>=0)oomAdjSupported=true;
        else oomAdjSupported=false;
        /* ................................................................. */
        printf("  - Sending DIM start command to node: %3d: %s\n",j,
               nodeFound.c_str());
        if(deBug)
        {
          printf("    ==> %3d Contacting Dim Command: \"%s\"...\n",j,
                 startCmd.c_str());
        }
        /* ................................................................. */
        if(oomScoreAdj && !oomAdjSupported)
        {
          printf("      WARN: Node \"%s\": Found Task Manager Server version "
                 "%s < 5.1 !\n",nodeFound.c_str(),tmVersion.c_str());
          printf("      WARN: Node \"%s\": OOM Adjusting NOT supported!\n",
                 nodeFound.c_str());
          printf("      WARN: Node \"%s\": -k|--oom-score-adj option "
                 "removed on this node!\n",nodeFound.c_str());
          DimClient::sendCommand(startCmd.c_str(),compatDimCmd.c_str());
        }
        else
        {
          DimClient::sendCommand(startCmd.c_str(),dimCmd.c_str());
        }
        /* ................................................................. */
        j++;
        /* ................................................................. */
      }
    }
    if(!nodeFoundN)
    {
      printf("  - No node found for pattern: \"%s\"!\n",hpIt->c_str());
    }
  }
  /* ----------------------------------------------------------------------- */
  return 0;
}
/*****************************************************************************/
void getServerVersion(vector<string>hostPttnLs)
{
  printf("HOSTNAME     FMC      SERVER\n");
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnLs,SVC_HEAD,SRV_NAME,deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnLs,"",SRV_NAME,deBug,12,8);
  /* older service name rule */
  FmcUtils::printServerVersion(hostPttnLs,"","mem",deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/*****************************************************************************/
char *getAuthString(void)
{
  char hostName[128]="";
  char *userName=NULL;
  char *authString=NULL;
  int authStringLen=0;
  struct passwd *pw;
  /*-------------------------------------------------------------------------*/
  gethostname(hostName,80);
  //userName=getlogin();
  pw=getpwuid(getuid());
  userName=pw->pw_name;
  authStringLen=snprintf(NULL,0,"%s\f%s",userName,hostName);
  authString=(char*)malloc(1+authStringLen);
  snprintf(authString,1+authStringLen,"%s\f%s",userName,hostName);
  return authString;
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"       tmStart [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"               [-m | --hostname NODE_PATTERN...]  [-X | --no-authentication]\n"
"               [-c | --clr-env] [-D | --set-env NAME=VALUE...]\n"
"               [-s | --scheduler SCHEDULER] [-p | --nice NICE_LEVEL]\n"
"               [-r | --rtprio RT_PRIORITY] [-a | --cpu CPU_NUM...]\n"
"               [-k | --oom-score-adj OOM_SCORE_ADJ] [-d | --daemon]\n"
"               [-n | --user USER_NAME] [-g | --group GROUP_NAME]\n"
"               [-u | --utgid UTGID] [-w | --wd WD] [-o | --out-to-stdlog]\n"
"               [-e | --err-to-stdlog] [-O | --out-to-log OUT_FIFO]\n"
"               [-E | --err-to-log ERR_FIFO] [-A | --no-drop] PATH [ARG...]\n"
"\n"
"       tmStart { -h | --help }\n"
"\n"
"Try \"tmStart -h\" for more information.\n";
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
"tmStart.man\n"
"\n"
"..\n"
"%s"
".hw no\\[hy]authentication\n"
".TH tmStart 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis tmStart\\ \\-\n"
"Start a process using the FMC Task Manager\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis tmStart\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".br\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".DoubleOpt[] X no\\[hy]authentication\n"
".DoubleOpt[] c clr\\[hy]env\n"
".DoubleOpt[] D set\\[hy]env NAME=VALUE...\n"
".br\n"
".DoubleOpt[] s scheduler SCHEDULER\n"
".DoubleOpt[] p nice NICE_LEVEL\n"
".DoubleOpt[] r rtprio RT_PRIORITY\n"
".DoubleOpt[] a cpu CPU_NUM...\n"
".DoubleOpt[] k oom\\[hy]score\\[hy]adj OOM_SCORE_ADJ\n"
".DoubleOpt[] d daemon\n"
".DoubleOpt[] n user USER_NAME\n"
".DoubleOpt[] g group GROUP_NAME\n"
".DoubleOpt[] u utgid UTGID\n"
".DoubleOpt[] w wd WD\n"
".DoubleOpt[] o out\\[hy]to\\[hy]stdlog\n"
".DoubleOpt[] e err\\[hy]to\\[hy]stdlog\n"
".DoubleOpt[] O out\\[hy]to\\[hy]log OUT_FIFO\n"
".DoubleOpt[] E err\\[hy]to\\[hy]log ERR_FIFO\n"
".DoubleOpt[] A no\\[hy]drop\n"
"\\fIPATH\\fP [\\fIARG\\fP...]\n"
".EndSynopsis\n"
".sp\n"
".Synopsis tmStart\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The command \\fBtmStart\\fP starts a \\fBnew process\\fP on all the nodes\n"
"whose hostname matches at least one of the wildcard patterns\n"
"\\fINODE_PATTERN\\fP, using the executable file located in \\fIPATH\\fP and\n"
"the arguments specified in \\fIARG\\fP. In the environment of the started\n"
"process the server \\fBtmSrv\\fP puts a new string variable, called\n"
"\\fBUTGID\\fP (User assigned unique Thread Group Identifier), which is\n"
"used as a tag by the Task Manager System.\n"
".PP\n"
"By \\fBdefault\\fP, before starting the process, all file descriptors are\n"
"closed and standard file descriptors (STDIN_FILENO, STDOUT_FILENO and\n"
"STDERR_FILENO) are re-opened on /dev/null.\n"
".PP\n"
"The \\fBUTGID\\fP can be either defined by the user (using the\n"
"\\fB-u\\fP\\~\\fIUTGID\\fP option) or can be generated automatically\n"
"(omitting \\fB-u\\fP\\~\\fIUTGID\\fP options) by appending to the command\n"
"name (the name of the executable image) an underscore followed by an\n"
"instance counter.\n"
".PP\n"
"The DIM Name Server, looked up to seek for Task Manager Servers, can be\n"
"chosen (in decreasing order of priority) by:\n"
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
".OptDef v debug\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef N dim\\[hy]dns DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".SS Authentication Options\n"
".\n"
".OptDef X no-authentication\n"
"Do not send authentication string (compatible with \\fBtmSrv\\fP,\n"
"version\\~<\\~2.0). Works also with \\fBtmSrv\\fP version\\~\\[>=]\\~2.0 if\n"
"\\fBtmSrv\\fP is started with the \\fB--no-authentication\\fP command-line\n"
"switch.\n"
".\n"
".SS Node Selection Options\n"
".\n"
".OptDef m hostname NODE_PATTERN (string, repeatable)\n"
"Start process only at the nodes whose hostname matches the wildcard pattern\n"
"\\fINODE_PATTERN\\fP (\\fBdefault\\fP: Start process at all the\n"
"nodes).\n"
".\n"
".SS Process Environment Options\n"
".\n"
".OptDef c clr\\[hy]env\n"
"Clear the \\fBenvironment\\fP inherited by the Task Manager.\n"
"\\fBDefault\\fP: inherit the Task Manager environment.\n"
".\n"
".OptDef D set\\[hy]env NAME=VALUE (string, repeatable)\n"
"Set the \\fBenvironment\\fP variable \\fINAME\\fP to the value\n"
"\\fIVALUE\\fP.\n"
".\n"
".SS Process Scheduling Options\n"
".\n"
".OptDef s scheduler SCHEDULER (string or integer in the range 0..2)\n"
"Set the process scheduler to \\fISCHEDULER\\fP, which can be: \n"
".RS\n"
".TP\n"
"\\fB0\\fP\\ | \\fBts\\fP | \\fBtimesharing\\fP | \\fBSCHED_OTHER\\fP\n"
"The default Linux \\fBtime-sharing\\fP scheduler, with a\n"
"\\fBdynamic priority\\fP based on the \\fINICE_LEVEL\\fP.\n"
".TP\n"
"\\fB1\\fP\\ | \\fBff\\fP | \\fBfifo\\fP | \\fBSCHED_FIFO\\fP\n"
"The \\fBstatic-priority\\fP Linux \\fBreal-time\\fP \\fBfifo\\fP scheduler,\n"
"without time slicing. A \\fBSCHED_FIFO\\fP process runs until either it is\n"
"blocked by an I/O request, it is preempted by a higher "
"priority process, or it calls sched_yield (2).\n"
".TP\n"
"\\fB2\\fP\\ | \\fBrr\\fP | \\fBroundrobin\\fP | \\fBSCHED_RR\\fP\n"
"The \\fBstatic-priority\\fP Linux \\fBreal-time Round-Robin\\fP scheduler.\n"
"It differs from \\fBSCHED_FIFO\\fP because each process is only allowed to\n"
"run for a maximum time quantum. If a \\fBSCHED_RR\\fP process has been\n"
"running for a time period equal to or longer than the time quantum, it\n"
"will be put at the end of the list for its priority.\n"
".PP\n"
"\\fBDefault\\fP: 0 (SCHED_OTHER).\n"
".RE\n"
".\n"
".OptDef p nice NICE_LEVEL (integer in the range -20..19)\n"
"If \\fISCHEDULER\\fP=\\fBSCHED_OTHER\\fP, set the \\fBnice level\\fP of the\n"
"process to \\fINICE_LEVEL\\fP. The \\fBnice level\\fP is used by the\n"
"\\fBSCHED_OTHER\\fP time-sharing Linux scheduler to compute the\n"
"\\fBdynamic priority\\fP. Allowed values for \\fINICE_LEVEL\\fP are in the\n"
"range -20..19 (-20 corresponds to the most favorable scheduling; 19\n"
"corresponds to the least favorable scheduling). The \\fBnice level\\fP\n"
"can be lowered by the Task Manager even for processes which run as a user\n"
"different from root.\n"
".\n"
".OptDef r rtprio RT_PRIORITY (integer in the range 0..99)\n"
"Set the \\fBstatic\\fP (real-time) \\fBpriority\\fP of the process to\n"
"\\fIRT_PRIORITY\\fP. Only value 0 (zero) for \\fIRT_PRIORITY\\fP is\n"
"allowed for scheduler \\fBSCHED_OTHER\\fP. For \\fBSCHED_FIFO\\fP and\n"
"\\fBSCHED_RR\\fP real-time schedulers, allowed values are in the range\n"
"1..99 (1 is the least favorable priority, 99 is the most favorable\n"
"priority).\n"
".\n"
".OptDef a cpu CPU_NUM (integer, repeatable)\n"
"Add the CPU number \\fICPU_NUM\\fP to the \\fBprocess-to-CPU affinity\n"
"mask\\fP. More than one of these options can be present in the same command\n"
"to add more than one CPU to the affinity mask. Started process is allowed\n"
"to run only on the CPUs specified in the affinity mask. Omitting this\n"
"option, process is allowed to run on any CPU of the node. Allowed\n"
"\\fICPU_NUM\\fP depend on the PC architecture\n"
".\n"
".\n"
".SS Process OOM-killer Options\n"
".\n"
".OptDef k oom\\[hy]score\\[hy]adj OOM_SCORE_ADJ (integer in the range "
"-999..1000)\n"
"Set the \\fBOut Of Memory Killer Adjust Parameter\\fP of the started process "
"(\\fBoom_score_adj\\fP) to \\fIOOM_SCORE_ADJ\\fP. If the kernel version is "
"less than 2.6.36 then the value is converted in the range -16..15 for the "
"\\fBoom_adj\\fP parameter.  The process to be killed in an out-of-memory "
"situation is selected based on its badness score. The badness score is "
"reflected in /proc/<pid>/oom_score. This value is determined on the basis "
"that the system loses the minimum amount of work done, recovers a large "
"amount of memory, doesn't kill any innocent process eating tons of memory, "
"and kills the minimum number of processes (if possible limited to one). The "
"badness score is computed using the original memory size of the process, its "
"CPU time (utime + stime), the run time (uptime - start time) and its "
"oom_adj/oom_score_adj value. The \\fBhigher\\fP the parameter "
"\\fIOOM_SCORE_ADJ\\fP, \\fBmore likely\\fP the process is to be "
"\\fBkilled\\fP by \\fBOOM-killer\\fP. The value \\fIOOM_SCORE_ADJ\\fP=-1000 "
"is reserved for the Task Manager process and is not allowed for the started "
"processes.\n"
".\n"
".SS Process Owner Options\n"
".\n"
".OptDef n user USER_NAME (string)\n"
"Set the \\fBeffective UID\\fP (User IDentifier), the \\fBreal UID\\fP and\n"
"the \\fBsaved UID\\fP of the process to the \\fBUID\\fP corresponding to\n"
"the user \\fIUSER_NAME\\fP.\n"
".\n"
".OptDef g group GROUP_NAME (string)\n"
"Set the \\fBeffective GID\\fP (Group IDentifier), the \\fBreal GID\\fP and\n"
"the \\fBsaved GID\\fP of the process to the \\fBGID\\fP corresponding to\n"
"the group \\fIGROUP_NAME\\fP.\n"
".\n"
".SS Process Output Redirection Options\n"
".\n"
".OptDef e err\\[hy]to\\[hy]stdlog\n"
"Redirect the process \\fBstderr\\fP (standard error stream) to the default\n"
"FMC Message Logger. Omitting \\fB-e\\fP or \\fB-E\\fP \\fIERR_FIFO\\fP\n"
"options, the standard error stream is thrown in /dev/null.\n"
".\n"
".OptDef o out\\[hy]to\\[hy]stdlog\n"
"Redirect the process \\fBstdout\\fP (standard output stream) to the default\n"
"FMC Message Logger. Omitting \\fB-o\\fP or \\fB-O\\fP \\fIOUT_FIFO\\fP\n"
"options, the standard output stream is thrown in /dev/null.\n"
".\n"
".OptDef E err\\[hy]to\\[hy]log ERR_FIFO (string)\n"
"Redirect the process \\fBstderr\\fP to the FMC Message Logger which uses\n"
"the FIFO (named pipe) \\fIERR_FIFO\\fP. Omitting \\fB-e\\fP or \\fB-E\\fP\n"
"\\fIERR_FIFO\\fP options, the standard error stream is thrown in\n"
"/dev/null.\n"
".\n"
".OptDef O out\\[hy]to\\[hy]log OUT_FIFO (string)\n"
"Redirect the process \\fBstdout\\fP to the FMC Message Logger which uses\n"
"the FIFO (named pipe) \\fIOUT_FIFO\\fP. Omitting \\fB-o\\fP or \\fB-O\\fP\n"
"\\fIOUT_FIFO\\fP options, the standard output stream is thrown in\n"
"/dev/null.\n"
".\n"
".OptDef A no\\[hy]drop\n"
"In conjunction with \\fB-E\\fP \\fIERR_FIFO\\fP and\n"
"\\fB-O\\fP \\fIOUT_FIFO\\fP options, this option forces the \\fBno-drop\\fP\n"
"policy for the FIFO. \\fBDefault\\fP: the \\fBcongestion-proof\\fP policy\n"
"is used for the FIFO.\n"
".\n"
".SS Other Process Options\n"
".\n"
".OptDef d daemon\n"
"Start the process as \\fBdaemon\\fP: the process \\fBumask\\fP (user\n"
"file-creation mask) is changed into 0 (zero) and the program is run in a\n"
"new \\fBsession\\fP as \\fBprocess group leader\\fP.\n"
".\n"
".OptDef u utgid UTGID (string)\n"
"Set the process \\fButgid\\fP (User assigned unique Thread Group\n"
"Identifier) to \\fIUTGID\\fP.\n"
".\n"
".OptDef w wd WD (string)\n"
"Set the process \\fBworking directory\\fP to \\fIWD\\fP. File open by the\n"
"process without path specification are sought by the process in this\n"
"directory. Process \\fBrelative file path\\fP start from this directory.\n"
".\n"
".SS Accessory Options\n"
".\n"
".OptDef h help\n"
"Get basic usage help from the command line, then terminate.\n"
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
".\n"
".SH EXAMPLES\n"
".\n"
"Start the process \"counter\" at all the nodes running the Task\n"
"Manager Server registered with the current DIM DNS:\n"
".\n"
".PP\n"
".ShellCommand tmStart /opt/FMC/tests/counter\n"
".ShellCommand tmStart -m \\[dq]*\\[dq] /opt/FMC/tests/counter\n"
".ShellCommand tmStart -m \\[rs]* /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Redirect stdout and stderr to the default FMC Message "
"Logger:\n"
".\n"
".PP\n"
".ShellCommand tmStart -e -o /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Set the utgid to \"my_counter\":\n"
".\n"
".PP\n"
".ShellCommand tmStart -e -o -u my_counter /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Start the process \"counter\" only at nodes \"farm0101\" and \"farm0102\":\n"
".\n"
".PP\n"
".ShellCommand tmStart -m farm0101 -m farm0102 /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Use wildcard patterns to select the nodes:\n"
".\n"
".PP\n"
".ShellCommand tmStart -m \\[dq]farm0*\\[dq] -m \\[dq]farm1*\\[dq] "
"/opt/FMC/tests/counter\n"
".ShellCommand tmStart -m farm0\\[rs]* -m farm1\\[rs]* "
"/opt/FMC/tests/counter\n"
".ShellCommand tmStart -m \\[dq]farm010?\\[dq] /opt/FMC/tests/counter\n"
".ShellCommand tmStart -m farm010\\[rs]? /opt/FMC/tests/counter\n"
".ShellCommand tmStart -m \\[dq]farm0[3-7]01\\[dq] /opt/FMC/tests/counter\n"
".ShellCommand tmStart -m \\[dq]farm0[3-7]??\\[dq] -m \\[dq]ctrl0[1-4]\\[dq] "
"/opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Start the process as daemon and set the working directory:\n"
".\n"
".PP\n"
".ShellCommand tmStart -m farm003 -d -w /opt/FMC/tests ./counter\n"
".\n"
".PP\n"
"Clear the environment, set the utgid and the working "
"directory:\n"
".\n"
".PP\n"
".ShellCommand tmStart -m farm003 -c -u my_ps -w /bin ps -e -f\n"
".\n"
".PP\n"
"Start the process as daemon:\n"
".\n"
".PP\n"
".ShellCommand tmStart -d /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Use the scheduler SCHED_FIFO with static priority set to 1:\n"
".\n"
".PP\n"
".ShellCommand tmStart -d -s 1 -r 1 /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Use the scheduler SCHED_OTHER with nice level set to -10:\n"
".\n"
".PP\n"
".ShellCommand tmStart -d -p -10 -n galli /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Decrease the chance for the process to be killed by the OOM Killer:\n"
".\n"
".PP\n"
".ShellCommand tmStart -k -100 -n galli /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Set the user to \"galli\" and the group to \"users\":\n"
".\n"
".PP\n"
".ShellCommand tmStart -d -p -10 -n galli -g users /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Clear the environment and set two new environment variables:\n"
".\n"
".PP\n"
".ShellCommand tmStart -c -d "
"-D LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib\\: "
"-D DIM_DNS_NODE=ctrl00.daq.lhcb\\: /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Clear the environment, set two new environment variables, run as daemon,\n"
"with scheduler SCHED_FIFO and static priority set to 1,\n"
"set the user, the utgid and the working directory, redirect stdout\n"
"and stderr:\n"
".\n"
".PP\n"
".ShellCommand tmStart -m farm0101 -c -d\\: "
"-D LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib\\: "
"-D DIM_DNS_NODE=ctrl00.daq.lhcb\\: "
"-s 1\\: -r 1\\: -n galli\\: -u my_counter\\: -w /opt/FMC/tests\\: -e\\: "
"-o\\: ./counter 123\n"
".\n"
".PP\n"
"Set the CPU affinity of the process to CPU 0 and 2:\n"
".\n"
".PP\n"
".ShellCommand tmStart -m farm0141 -c -d\\: "
"-D LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib\\: "
"-D DIM_DNS_NODE=ctrl00.daq.lhcb\\: -s 1\\: -r 1\\: -a 0\\: -a 2\\: "
"-n galli\\: -u my_counter\\: -w /opt/FMC/tests\\: -e -o \\:./counter 123\n"
".\n"
".PP\n"
"Using a specific logger (non-default) for both stdout and \n"
"stderr:\n"
".\n"
".PP\n"
".ShellCommand tmStart -m farm0101 -u counter_logger\\: "
"/opt/FMC/sbin/logSrv\\: -p /tmp/counter_logger.fifo\\: "
"-s counter_logger\\: \n"
".ShellCommand logViewer -s counter_logger\n"
".ShellCommand tmStart -m farm0101 -u my_counter\\: "
"-E /tmp/counter_logger.fifo\\: -O /tmp/counter_logger.fifo\\: -A\\: "
"/opt/FMC/tests/counter\n"
"%s"
"%s"
".\n"
".\n"
".SH VERSION\n"
".\n"
"Source:  %s\n"
".PP\n"
"Package: FMC-%s\n"
".\n"
".\n"
".SH SEE ALSO\n"
".ad l\n"
".BR \\%%tmLs (1),\n"
".BR \\%%tmLl (1),\n"
".BR \\%%tmKill (1),\n"
".BR \\%%tmStop (1).\n"
".br\n"
".BR \\%%pcAdd (1),\n"
".BR \\%%pcRm (1),\n"
".BR \\%%pcLs (1),\n"
".BR \\%%pcLl (1),\n"
".BR \\%%pcLss (1).\n"
".br\n"
".BR \\%%tmSrv (8),\n"
".BR \\%%pcSrv (8).\n"
".br\n"
".BR \\%%glob (7).\n"
"\n"
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
