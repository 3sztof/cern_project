/*****************************************************************************/
/*
 * $Log: pwStatus.C,v $
 * Revision 2.10  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 2.9  2008/10/16 10:14:05  galli
 * minor changes
 *
 * Revision 2.6  2008/10/15 10:18:04  galli
 * groff manual
 *
 * Revision 2.5  2008/10/15 07:07:43  galli
 * condition variable to wait for the I/O thread
 *
 * Revision 2.3  2007/12/14 14:33:22  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 2.2  2007/10/23 16:05:44  galli
 * usage() prints FMC version
 *
 * Revision 2.0  2007/10/10 13:55:59  galli
 * rewritten: uses DimInfo instead of DimCurrentInfo
 * 20 times faster
 *
 * Revision 1.4  2006/02/10 14:20:41  galli
 * added timestamps for power status readings
 *
 * Revision 1.3  2006/02/09 12:18:55  galli
 * print error codes
 *
 * Revision 1.1  2005/04/18 23:19:56  galli
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#include <getopt.h>                                         /* getopt_long() */
#include <time.h>                                            /* nanosleep(2) */
#include <errno.h>
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                               /* printServerVersion() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*---------------------------------------------------------------------------*/
/* in ipmi/include */
#include "ipmiSrv.h"                                             /* SRV_NAME */
#include "ipmiUtils.h"                                        /* error codes */
/*****************************************************************************/
/* globals */
int deBug=0;
/* configuration */
const char *dimConfFile=DIM_CONF_FILE_NAME;
/* revision */
static char rcsid[]="$Id: pwStatus.C,v 2.10 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
/* inter-thread communication */
int totNodeFoundC=0;
pthread_mutex_t totNodeFoundMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t totNodeFoundCond=PTHREAD_COND_INITIALIZER;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
class PwInfo:public DimClient
{
  char *hostName;
  int nodeSvcC;
  DimInfo *PwStatus;
  int pwStatus;
  DimInfo *PwTs;
  time_t pwTs;
  char *pwStatusSvc;
  int pwStatusSvcLen;
  char *pwTsSvc;
  int pwTsSvcLen;
  /*-------------------------------------------------------------------------*/
  public:
  /*-------------------------------------------------------------------------*/
  PwInfo(char *hostName)
  {
    /*.......................................................................*/
    this->hostName=hostName;
    nodeSvcC=0;
    /*.......................................................................*/
    pwStatusSvcLen=snprintf(NULL,0,"%s/%s/power_status",SVC_HEAD,hostName);
    pwStatusSvc=(char*)malloc(1+pwStatusSvcLen);
    sprintf(pwStatusSvc,"%s/%s/power_status",SVC_HEAD,hostName);
    if(deBug)printf("Contacting Service: \"%s\"...\n",pwStatusSvc);
    PwStatus=new DimInfo(pwStatusSvc,-1,this);
    /*.......................................................................*/
    pwTsSvcLen=snprintf(NULL,0,"%s/%s/power_status_timestamp",SVC_HEAD,
                        hostName);
    pwTsSvc=(char*)malloc(1+pwTsSvcLen);
    sprintf(pwTsSvc,"%s/%s/power_status_timestamp",SVC_HEAD,hostName);
    if(deBug)printf("Contacting Service: \"%s\"...\n",pwTsSvc);
    PwTs=new DimInfo(pwTsSvc,-1,this);
    /*.......................................................................*/
  }
  /*-------------------------------------------------------------------------*/
  void infoHandler()
  {
    nodeSvcC++;
    /*.......................................................................*/
    DimInfo *curr=getInfo();
    if(curr==PwStatus)
    {
      pwStatus=(int)curr->getInt();
      if(deBug)printf("Service \"%s\" replied.\n",pwStatusSvc);
    }
    else if(curr==PwTs)
    {
      pwTs=(time_t)curr->getInt();
      if(deBug)printf("Service \"%s\" replied.\n",pwTsSvc);
    }
    /*.......................................................................*/
    if(nodeSvcC==2)
    {
      char *p;
      char *hostname;
      struct tm lPwTs;
      char sPwTs[25]="";
      hostname=(char*)malloc(2+strlen(hostName));
      sprintf(hostname,"%s:",hostName);
      for(p=hostname;*p;p++)*p=tolower(*p);
      if(deBug)printf("Node %s done.\n",hostname);
      localtime_r(&pwTs,&lPwTs);
      strftime(sPwTs,26,"%a %b %d %H:%M:%S %Y",&lPwTs);
      printf("%-12s ",hostname);
      if(pwStatus>=0)
        printf("%s",pwStatus==0?"OFF":"ON");
      else if(pwStatus==IPMI_NOT_YET_AVAIL)
        printf("%s",IPMI_NOT_YET_AVAIL_SL);
      else if(pwStatus==IPMI_SENSOR_UNREADABLE)
        printf("%s",IPMI_SENSOR_UNREADABLE_SL);
      else if(pwStatus==IPMI_INVALID_RESP)
        printf("%s",IPMI_INVALID_RESP_SL);
      else if(pwStatus==IPMI_NOT_RESP)
        printf("%s",IPMI_NOT_RESP_SL);
      else
        printf("%s","Unknown Error!");
      printf(" (%s).\n",sPwTs);
      /*.....................................................................*/
      /* increase the node counter */
      pthread_mutex_lock(&totNodeFoundMutex);
      totNodeFoundC++;
      pthread_mutex_unlock(&totNodeFoundMutex);
      /*.....................................................................*/
      /* inform the main thread of the change in node counter */
      pthread_cond_broadcast(&totNodeFoundCond);
      /*.....................................................................*/
    }
    /*.......................................................................*/
  }
  /*-------------------------------------------------------------------------*/
};
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j;
  /* pointers */
  char *p=NULL;
  /* service browsing */
  int hostPttnC=0;
  char **hostPttnV=NULL;
  char *srvPattern=NULL;                                       /* upper case */
  int svcPatternLen=0;
  char *svcPattern=NULL;
  int rawSvcPatternLen=0;
  char *rawSvcPattern=NULL;
  int nodeFoundN=0;
  int totNodeFoundN=0;
  char nodeFound[128]="";
  char ucNodeFound[128]="";
  DimBrowser br;
  char *format=NULL;
  int type=0;
  char *pwStatusSvc=NULL;
  char *pwStatusSvcDup=NULL;
  char *dimDnsNode=NULL;
  /* command line switch */
  int doPrintServerVersion=0;
  /* getopt */
  int flag;
  /* long command line options */
  static struct option longOptions[]=
  {
    {"hostname",required_argument,NULL,'m'},
    {"dim-dns",required_argument,NULL,'N'},
    {"debug",no_argument,NULL,'v'},
    {"version",no_argument,NULL,'V'},
    {"help",optional_argument,NULL,'h'},
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt_long(argc,argv,"m:vN:Vh::",longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 'm': /* -m, --hostname */
        hostPttnC++;
        hostPttnV=(char**)realloc(hostPttnV,hostPttnC*sizeof(char*));
        hostPttnV[hostPttnC-1]=optarg;
        break;
      case 'v': /* -v, --debug */
        deBug++;
        break;
      case 'N': /* -N, --dim-dns */
        dimDnsNode=optarg;
        break;
      case 'V': /* -V, --version */
        doPrintServerVersion=1;
        break;
      case 'h': /* -h, -hh, --help */
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        fprintf(stderr,"\ngetopt(): Invalid option \"%s\"!\n",argv[optind-1]);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* default: all the nodes */
  if(!hostPttnC)
  {
    hostPttnC=1;
    hostPttnV=(char**)realloc(hostPttnV,sizeof(char*));
    hostPttnV[hostPttnC-1]=(char*)"*";
  }
  /*-------------------------------------------------------------------------*/
  /* check command line non-option argument(s) */
  if(optind<argc)
  {
    i=optind;
    string msg="Unrequired non-option argument(s): ";
    for(i=optind;i<argc;i++)
    {
      if(i!=optind)msg+=", ";
      msg+="\""+string(argv[i])+"\"";
    }
    msg+="!";
    fprintf(stderr,"\n%s\n",msg.c_str());
    shortUsage();
  }
  if(argc-optind!=0)shortUsage();
  /*-------------------------------------------------------------------------*/
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
      dimDnsNode=getDimDnsNode(dimConfFile,0,1);
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
  /*-------------------------------------------------------------------------*/
  /* handling of -V option */
  if(doPrintServerVersion)getServerVersion(hostPttnC,hostPttnV);
  /*-------------------------------------------------------------------------*/
  printf("Node patterns list:  ");
  for(i=0;i<hostPttnC;i++)                    /* loop over node patterns */
  {
    printf("\"%s\"",hostPttnV[i]);
    if(i<hostPttnC-1)printf(", ");
  }
  printf(".\n");
  printf("Power status of Farm Nodes:\n");
  /*-------------------------------------------------------------------------*/
  totNodeFoundN=0;
  for(i=0,j=0;i<hostPttnC;i++)                    /* loop over node patterns */
  {
    if(deBug)printf("  Node pattern: \"%s\":\n",hostPttnV[i]);
    srvPattern=(char*)realloc(srvPattern,1+strlen(hostPttnV[i]));
    strcpy(srvPattern,hostPttnV[i]);
    /* convert to upper case */
    for(p=srvPattern;*p;p++)*p=toupper(*p);
    svcPatternLen=snprintf(NULL,0,"%s/%s/power_status",SVC_HEAD,srvPattern);
    svcPattern=(char*)realloc(svcPattern,1+svcPatternLen);
    snprintf(svcPattern,1+svcPatternLen,"%s/%s/power_status",SVC_HEAD,
             srvPattern);
    if(deBug)printf("  Service pattern: \"%s\"\n",svcPattern);
    /*.......................................................................*/
    rawSvcPatternLen=snprintf(NULL,0,"%s/*/power_status",SVC_HEAD);
    rawSvcPattern=(char*)realloc(rawSvcPattern,1+rawSvcPatternLen);
    snprintf(rawSvcPattern,1+rawSvcPatternLen,"%s/*/power_status",
             SVC_HEAD);
    /*.......................................................................*/
    br.getServices(rawSvcPattern);
    nodeFoundN=0;
    while((type=br.getNextService(pwStatusSvc,format))!=0)/* loop over nodes */
    {
      if(!fnmatch(svcPattern,pwStatusSvc,0))
      {
        totNodeFoundN++;
        nodeFoundN++;
        pwStatusSvcDup=(char*)realloc(pwStatusSvcDup,1+strlen(pwStatusSvc));
        strcpy(pwStatusSvcDup,pwStatusSvc);
        p=strrchr(pwStatusSvcDup,'/');
        *p='\0';
        p=strrchr(pwStatusSvcDup,'/');
        snprintf(nodeFound,sizeof(nodeFound),"%s",1+p);
        strcpy(ucNodeFound,nodeFound);
        for(p=nodeFound;*p;p++)
        {
          if(*p=='/')
          {
            *p='\0';
            break;
          }
          *p=tolower(*p);
        }
        /*...................................................................*/
        if(deBug)printf("Subscribing to host: \"%s\"...\n",nodeFound);
        new PwInfo(strdup(ucNodeFound));
        /*...................................................................*/
      }                                                     /* if !fnmatch() */
    }                                                     /* loop over nodes */
    if(!nodeFoundN)
    {
      printf("    No node found for pattern: \"%s\"!\n",hostPttnV[i]);
    }
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  /* wait until all service data are printed */
  if(totNodeFoundN)
  {
    for(;;)
    {
      /* wait for a change in node counter set by the I/O thread */
      pthread_mutex_lock(&totNodeFoundMutex);
      pthread_cond_wait(&totNodeFoundCond,&totNodeFoundMutex);
      pthread_mutex_unlock(&totNodeFoundMutex);
      if(totNodeFoundC>=totNodeFoundN)break;
    }
  }
  /*-------------------------------------------------------------------------*/
  if(deBug)printf("All the %d found nodes have done.\n",totNodeFoundN);
  return 0;
}
/*****************************************************************************/
void getServerVersion(int hostPttnC,char **hostPttnV)
{
  printf("HOSTNAME     FMC      SERVER\n");
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,SVC_HEAD,SRV_NAME,
                               deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"",SRV_NAME,
                               deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"pwStatus [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"         [-m | --hostname NODE_PATTERN...]\n"
"pwStatus { -V | --version } [-v | --debug]\n"
"         [-N | --dim-dns DIM_DNS_NODE]\n"
"         [-m | --hostname NODE_PATTERN...]\n"
"pwStatus { -h | --help }\n"
"\n"
"Try \"pwStatus -h\" for more information.\n";
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
"pwStatus.man\n"
"\n"
"..\n"
"%s"
".hw hostname NODE_PATTERN\n"
".TH pwStatus 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".SH NAME\n"
".Synopsis pwStatus\\ \\-\n"
"Print the Power Status (on/off) of the farm PC(s), got by means of "
"the FMC Power Manager Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis pwStatus\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis pwStatus\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis pwStatus\n"
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
"The \\fBpwStatus\\fP(1) command prints to the standard output stream the "
"\\fBpower status\\fP (\\fIon/off\\fP) of the farm nodes, got from the Power "
"Manager Servers registered with the DIM name server \\fIDIM_DNS_NODE\\fP.\n"
".PP\n"
"The DIM Name Server \\fIDIM_DNS_NODE\\fP, looked up to seek for FMC Power "
"Manager Servers, can be chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\"\\fB%s\\fP\".\n"
".RE\n"
".\n"
".\n"
".SH OPTIONS\n"
".\n"
".OptDef h help\n"
"Get basic usage help from the command line, then terminate.\n"
".\n"
".OptDef v debug\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef N dim\\[hy]dns DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef m hostname NODE_PATTERN (string, repeatable)\n"
"Print only the status of the PCs whose hostname matches at least one of the "
"wildcard patterns \\fINODE_PATTERN\\fP (\\fBdefault\\fP: print the status "
"of all the controlled PCs.\n"
".\n"
".OptDef V version\n"
"For each selected node print the FMC Power Manager Server version and the "
"FMC version, than terminate.\n"
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
"Print the status of all the controlled PCs:\n"
".PP\n"
".ShellCommand pwStatus\n"
".PP\n"
".\n"
"Print the status of the PC farm0101:\n"
".PP\n"
".ShellCommand pwStatus -m farm0101\n"
".PP\n"
".\n"
"Use wildcard patterns to select PCs:\n"
".PP\n"
".ShellCommand pwStatus -m \\[dq]farm01*\\[dq]\n"
".ShellCommand pwStatus -m \\[dq]farm01*\\[dq] -m \\[dq]farm02*\\[dq]\n"
".ShellCommand pwStatus -m \\[dq]farm010[1357]\\[dq]\n"
".PP\n"
".\n"
".\n"
". SH AVAILABILITY\n"
".\n"
"Information on how to get the FMC (Farm Monitoring and Control System)\n"
"package and related information is available at the web sites:\n"
".PP\n"
".URL https://\\:lhcbweb.bo.infn.it/\\:twiki/\\:bin/\\:view.cgi/\\:"
"LHCbBologna/\\:FmcLinux \"FMC Linux Development Homepage\".\n"
".PP\n"
".URL http://\\:itcobe.web.cern.ch/\\:itcobe/\\:Projects/\\:Framework/\\:"
"Download/\\:Components/\\:SystemOverview/\\:fwFMC/\\:welcome.html "
"\"CERN ITCOBE: FW Farm Monitor and Control\".\n"
".PP\n"
".URL http://\\:lhcb-daq.web.cern.ch/\\:lhcb-daq/\\:online-rpm-repo/\\:"
"index.html \"LHCb Online RPMs\".\n"
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
"Package: FMC-%s\n"
".\n"
".\n"
".SH SEE ALSO:\n"
".BR \\%%pwSwitch (1),\n"
".BR \\%%ipmiViewer (1),\n"
".BR \\%%ipmiSrv (8).\n"
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
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(0);
}
/*****************************************************************************/
