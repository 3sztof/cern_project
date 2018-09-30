/*****************************************************************************/
/*
 * $Log: fmcSrvViewer.C,v $
 * Revision 1.10  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.9  2009/02/18 13:31:05  galli
 * added logger(reco) and logger(sys) to checklist
 *
 * Revision 1.7  2008/10/14 15:05:17  galli
 * groff manual
 *
 * Revision 1.2  2007/11/02 14:27:41  galli
 * can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.1  2006/10/27 13:24:45  galli
 * Initial revision
 */
/*****************************************************************************/
#include <string>
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>                                       /* DimCurrentInfo() */
/*---------------------------------------------------------------------------*/
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*---------------------------------------------------------------------------*/
#define SRV_SZ 256
#define SRV_TY_SZ 20
/*****************************************************************************/
using namespace std;
/*****************************************************************************/
typedef struct server
{
  const char *srvName;
  const char *exeName;
  const char *shortName;
  const char *longName;
}server_t;
/*****************************************************************************/
static char rcsid[]="$Id: fmcSrvViewer.C,v 1.10 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
int sCmp(const void *str1,const void *str2);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i=0;
  unsigned j=0;
  /* pointers */
  char *p=NULL;
  /* service browsing */
  char *dimDnsNode=NULL;
  int hostPttnC=0;
  char **hostPttnV=NULL;
  char *serverList=NULL;
  /* service processing */
  int l;
  char srv[SRV_SZ];
  char *node=NULL;
  char nodeBase[SRV_SZ];
  int match=0;
  int nodeC=0;
  char **nodeV=NULL;
  int found=0;
  char s[SRV_SZ+SRV_TY_SZ];
  /* getopt */
  int flag;
  bool printCtrl=true;
  bool printMoni=true;
  /* server list */
  server_t ctrlSrvList[]={
    /* srvName            exeName      shortName      longName */
    {"logger/fmc",        "logSrv",    "log (fmc)",   "Event Logger (fmc)"},
    {"logger/gaudi",      "logSrv",    "log (gaudi)", "Event Logger (gaudi)"},
    {"logger/reco",       "logSrv",    "log (reco)",  "Event Logger (reco)"},
    {"logger/sys",        "logSrv",    "log (sys)",   "Event Logger (sys)"},
    {"task_manager",      "tmSrv",     "tm",          "Task Manager"},
    {"config_manager",    "cmSrv",     "cm",          "Configuration Manager"},
    {"process_controller","pcSrv",     "pc",          "Process Controller"},
    {"power_manager",     "ipmiSrv",   "ipmi",        "Power Manager"},
  };
  server_t moniSrvList[]={
    /* srvName      exeName      shortName  longName */
    {"Coalescence", "coalSrv",   "coal",    "Coalescence Monitor"},
    {"cpu/info",    "cpuinfoSrv","cpuinfo", "CPU info Monitor"},
    {"cpu/stat",    "cpustatSrv","cpustat", "CPU state Monitor"},
    {"filesystems", "fsSrv",     "fs",      "File System Monitor"},
    {"IRQ",         "irqSrv",    "irq",     "Interrupt Monitor"},
    {"memory",      "memSrv",    "mem",     "Memory Monitor"},
    {"net/ifs",     "nifSrv",    "nif",     "Netwotk Interface Monitor"},
    {"os",          "osSrv",     "os",      "Operating System Monitor"},
    {"ps",          "psSrv",     "ps",      "Process Monitor"},
    {"disk",        "smartSrv",  "smart",   "SMART Monitor"},
    {"tcpip",       "tcpipSrv",  "tcpip",   "TCP/IP Monitor"}
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  hostPttnC=0;
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt(argc,argv,"h::m:N:MC"))!=EOF)
  {
    switch(flag)
    {
      case 'm':
        hostPttnC++;
        hostPttnV=(char**)realloc(hostPttnV,hostPttnC*sizeof(char*));
        hostPttnV[hostPttnC-1]=optarg;
        break;
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'C':
        printMoni=false;
        break;
      case 'M':
        printCtrl=false;
        break;
      case 'h':
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        fprintf(stderr,"\ngetopt(): invalid option \"-%c\"!\n",optopt);
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
  /* check cmnd-line options */
  if(!printCtrl && !printMoni)
  {
    printCtrl=true;
    printMoni=true;
  }
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
  /* get server list from the DIM DNS */
  DimCurrentInfo DNSServerList(DIM_DNS_SRV_LIST,(char*)"N/A");
  serverList=(char*)DNSServerList.getString();
  /*-------------------------------------------------------------------------*/
  if(!strcmp(serverList,"N/A"))
  {
    fprintf(stderr,"\nDIM DNS unreachable!\n\n");
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* find nodes list nodeV[nodeC] */
  for(p=serverList;p<serverList+strlen(serverList);)
  {
    memset(srv,0,sizeof(srv));
    l=strcspn(p,"|");
    if(!l)break;
    if(l>SRV_SZ-1)l=SRV_SZ-1;
    strncpy(srv,p,l);
    node=1+strchr(srv,'@');
    strncpy(nodeBase,node,strcspn(node,"."));
    for(i=0,match=0;i<hostPttnC;i++)
    {
      if(!fnmatch(hostPttnV[i],nodeBase,0))
      {
        match=1;
        break;
      }
    }
    if(match)
    {
      for(i=0,found=0;i<nodeC;i++)
      {
        if(!strcmp(node,nodeV[i]))
        {
          found=1;
          break;
        }
      }
      if(!found)
      {
        nodeV=(char**)realloc(nodeV,++nodeC*sizeof(char*));
        nodeV[nodeC-1]=strdup(node);
      }
    }
    p+=(l+1);
  }
  /*-------------------------------------------------------------------------*/
  /* sort node list */
  qsort(nodeV,nodeC,sizeof(char*),sCmp);
  /*-------------------------------------------------------------------------*/
  for(i=0;i<nodeC;i++)                               /* loop over farm nodes */
  {
    printf("  NODE: %s\n",nodeV[i]);
    /*.......................................................................*/
    /* print control servers */
    if(printCtrl)
    {
      for(j=0;j<sizeof(ctrlSrvList)/sizeof(char*)/4;j++)
      {
        sprintf(s,"%s@%s",ctrlSrvList[j].srvName,nodeV[i]);
        if(strstr(serverList,s))
          printf("    %-12s   %-26s on\n",ctrlSrvList[j].shortName,
                                          ctrlSrvList[j].longName);
        else
          printf("    %-12s   %-26s off\n",ctrlSrvList[j].shortName,
                                           ctrlSrvList[j].longName);
      }
    }
    /*.......................................................................*/
    /* print monitor servers */
    if(printMoni)
    {
      for(j=0;j<sizeof(moniSrvList)/sizeof(char*)/4;j++)
      {
        sprintf(s,"%s@%s",moniSrvList[j].srvName,nodeV[i]);
        if(strstr(serverList,s))
          printf("    %-12s   %-26s on\n",moniSrvList[j].shortName,
                                          moniSrvList[j].longName);
        else
          printf("    %-12s   %-26s off\n",moniSrvList[j].shortName,
                                           moniSrvList[j].longName);
      }
    }
    /*.......................................................................*/
  }                                                  /* loop over farm nodes */
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
int sCmp(const void *str1,const void *str2)
{
  return strcasecmp(*(char**)str1,*(char**)str2);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"fmcSrvViewer [-N DIM_DNS_NODE] [-m NODE_PATTERN...] [-C] [-M]\n"
"fmcSrvViewer -h\n"
"\n"
"Try \"fmcSrvViewer -h\" for more information.\n";
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
"fmcSrvViewer.man\n"
"\n"
"..\n"
"%s"
".hw NODE_PATTERN \n"
".TH fmcSrvViewer 1  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis fmcSrvViewer\\ \\-\n"
"Print the execution status (on/off) of the FMC Servers\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis fmcSrvViewer\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] C\n"
".ShortOpt[] M\n"
".EndSynopsis\n"
".sp\n"
".Synopsis fmcSrvViewer\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The command \\fBfmcSrvViewer\\fP(1) is a FMC debugging tool which prints the "
"\\fBexecution status\\fP (on/off) of the FMC Servers which are supposed to "
"be running on the farm nodes registered with the DIM Name Server in "
"execution on the node \\fIDIM_DNS_NODE\\fP.\n"
".PP\n"
"FMC servers are classified in \\fIControl Servers\\fP [\\fBlogSrv\\fP(8), "
"\\fBtmSrv\\fP(8), \\fBpcSrv\\fP(8), \\fBipmiSrv\\fP(8), \\fBcmSrv\\fP(8)] "
"and \\fIMonitor Servers\\fP [\\fBcoalSrv\\fP(8), \\fBcpuinfoSrv\\fP(8), "
"\\fBcpustatSrv\\fP(8), \\fBfsSrv\\fP(8), \\fBirqSrv\\fP(8), "
"\\fBmemSrv\\fP(8), \\fBnifSrv\\fP(8), \\fBosSrv\\fP(8), \\fBpsSrv\\fP(8), "
"\\fBsmartSrv\\fP(8) and \\fBtcpipSrv\\fP(8)].\n"
".PP\n"
"The DIM Name Server \\fIDIM_DNS_NODE\\fP, looked up to seek for FMC Servers, "
"can be chosen (in decreasing order of priority) by:\n"
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
".\n"
".SH OPTIONS\n"
".\n"
".OptDef h \"\"\n"
"Get basic usage help from the command line, then terminate.\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"Print only the status of the servers running on the nodes whose hostname "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: print the status of the servers running on all the nodes "
"registered with the DIM Name Server \\fIDIM_DNS_NODE\\fP).\n"
".\n"
".OptDef C \"\"\n"
"Print only the status of the Control Servers [\\fBlogSrv\\fP(8), "
"\\fBtmSrv\\fP(8), \\fBpcSrv\\fP(8), \\fBipmiSrv\\fP(8), \\fBcmSrv\\fP(8)]. "
"\\fBDefault\\fP: print the status all the FMC servers.\n"
".\n"
".OptDef M \"\"\n"
"Print only the status of the Monitor Servers [\\fBcoalSrv\\fP(8), "
"\\fBcpuinfoSrv\\fP(8), \\fBcpustatSrv\\fP(8), \\fBfsSrv\\fP(8), "
"\\fBirqSrv\\fP(8), \\fBmemSrv\\fP(8), \\fBnifSrv\\fP(8), \\fBosSrv\\fP(8), "
"\\fBpsSrv\\fP(8), \\fBsmartSrv\\fP(8) and \\fBtcpipSrv\\fP(8)]. "
"\\fBDefault\\fP: print the status all the FMC servers.\n"
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
"Print the status of all the FMC servers registered with the DIM Name Server "
"\\fIDIM_DNS_NODE\\fP:\n"
".PP\n"
".ShellCommand fmcSrvViewer\n"
".PP\n"
".\n"
"Print the status of all the FMC control servers registered with the DIM "
"Name Server \\fIDIM_DNS_NODE\\fP:\n"
".PP\n"
".ShellCommand fmcSrvViewer -C\n"
".PP\n"
".\n"
"Print the status of all the FMC monitor servers registered with the DIM "
"Name Server \\fIDIM_DNS_NODE\\fP:\n"
".PP\n"
".ShellCommand fmcSrvViewer -M\n"
".PP\n"
".\n"
"Print the status of the FMC control servers running on nodes farm0101 and "
"farm0102:\n"
".PP\n"
".ShellCommand fmcSrvViewer -m farm0101 -m farm0102 -C\n"
".PP\n"
".\n"
"Use wildcard patterns to select the nodes:\n"
".PP\n"
".ShellCommand fmcSrvViewer -m \\[dq]farm01??\\[dq] -m "
"\\[dq]farm02??\\[dq] -C\n"
".ShellCommand fmcSrvViewer -m \\[dq]farm010[1-3]\\[dq]\n"
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
".SS FMC debugging tools:\n"
".BR \\%%fmcVersionViewer (1).\n"
".br\n"
".SS FMC Control Servers:\n"
".BR \\%%logSrv (8),\n"
".BR \\%%tmSrv (8),\n"
".BR \\%%pcSrv (8),\n"
".BR \\%%ipmiSrv (8),\n"
".BR \\%%cmSrv (8).\n"
".br\n"
".SS FMC Monitor Servers:\n"
".BR \\%%coalSrv (8),\n"
".BR \\%%cpuinfoSrv (8),\n"
".BR \\%%cpustatSrv (8),\n"
".BR \\%%fsSrv (8),\n"
".BR \\%%irqSrv (8),\n"
".BR \\%%memSrv (8),\n"
".BR \\%%nifSrv (8),\n"
".BR \\%%osSrv (8),\n"
".BR \\%%psSrv (8),\n"
".BR \\%%smartSrv (8),\n"
".BR \\%%tcpipSrv (8).\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(0);
}
/*****************************************************************************/
