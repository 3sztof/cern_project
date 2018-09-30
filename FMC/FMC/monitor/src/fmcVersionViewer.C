/*****************************************************************************/
/*
 * $Log: fmcVersionViewer.C,v $
 * Revision 1.20  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.15  2008/10/14 11:40:02  galli
 * groff manual
 *
 * Revision 1.14  2008/10/13 23:27:52  galli
 * uses FmcUtils::printServerVersion() & FmcUtils::printLogServerVersion()
 *
 * Revision 1.11  2008/01/29 15:52:55  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 1.10  2008/01/23 09:26:35  galli
 * added SRV osSrv
 *
 * Revision 1.4  2007/10/30 15:25:00  galli
 * can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.3  2007/10/27 22:54:15  galli
 * working version
 *
 * Revision 1.1  2007/10/26 08:13:44  galli
 * Initial revision
 */
/*****************************************************************************/
#include <string>                                             /* std::string */
#include <vector>                                               /* std::list */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcMsgUtils.h"                                            /* L_STD */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"      /* printServerVersion(), printLogServerVersion() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcPar.h"                                    /* DIM_CONF_FILE_NAME */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*****************************************************************************/
typedef struct server
{
  const char *srvName;
  const char *exeName;
  const char *shortName;
}server_t;
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: fmcVersionViewer.C,v 1.20 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
void getServerVersion(int hostPttnC,char **hostPttnV,server_t srvName);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  unsigned i,k;
  int j;
  /* pointers */
  char *p;
  /* service browsing */
  char *dimDnsNode=NULL;
  int hostPttnC=0;
  char **hostPttnV=NULL;
  int srvPttnC=0;
  char **srvPttnV=NULL;
  /* getopt */
  int flag;
  server_t srvList[]={
  /*  srvName              exeName         shortName                         */
    {"logger",            "logSrv",       "log"},      /*  1 */
    {"task_manager",      "tmSrv",        "tm"},       /*  2 */
    {"config_manager",    "cmSrv",        "cm"},       /*  3 */
    {"process_controller","pcSrv",        "pc"},       /*  4 */
    {"power_manager",     "ipmiSrv",      "ipmi"},     /*  5 */
    {"Coalescence",       "coalSrv",      "coal"},     /*  6 */
    {"coalescence",       "oldCoalSrv",   "coal"},     /*  6 old */
    {"cpu/info",          "cpuinfoSrv"   ,"cpuinfo"},  /*  7 */
    {"cpuinfo",           "oldCpuinfoSrv","cpuinfo"},  /*  7 old */
    {"cpu/stat",          "cpustatSrv",   "cpustat"},  /*  8 */
    {"cpustat",           "oldCpustatSrv","cpustat"},  /*  8 old */
    {"filesystems",       "fsSrv",        "fs"},       /*  9 */
    {"fs",                "oldFsSrv",     "fs"},       /*  9 old */
    {"IRQ",               "irqSrv",       "irq"},      /* 10 */
    {"irq",               "oldIrqSrv",    "irq"},      /* 10 old */
    {"memory",            "memSrv",       "mem"},      /* 11 */
    {"mem",               "oldMemSrv",    "mem"},      /* 11 old */
    {"net/ifs",           "nifSrv",       "nif"},      /* 12 */
    {"nif",               "oldNifSrv",    "nif"},      /* 12 old */
    {"os",                "osSrv",        "os"},       /* 13 */
    {"ps",                "psSrv",        "ps"},       /* 14 */
    {"procs",             "oldPsSrv",     "ps"},       /* 14 old */
    {"disk",              "smartSrv",     "smart"},    /* 15 */
    {"tcpip",             "tcpipSrv",     "tcpip"}     /* 16 */
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt(argc,argv,"h::m:s:CMvN:"))!=EOF)
  {
    switch(flag)
    {
      case 'm':
        hostPttnC++;
        hostPttnV=(char**)realloc(hostPttnV,hostPttnC*
                                      sizeof(char*));
        hostPttnV[hostPttnC-1]=optarg;
        break;
      case 's':
        srvPttnC++;
        srvPttnV=(char**)realloc(srvPttnV,srvPttnC*sizeof(char*));
        srvPttnV[srvPttnC-1]=optarg;
        break;
      case 'C':
        srvPttnC+=5;
        srvPttnV=(char**)realloc(srvPttnV,srvPttnC*sizeof(char*));
        srvPttnV[srvPttnC-5]=(char*)"log";
        srvPttnV[srvPttnC-4]=(char*)"tm";
        srvPttnV[srvPttnC-3]=(char*)"cm";
        srvPttnV[srvPttnC-2]=(char*)"pc";
        srvPttnV[srvPttnC-1]=(char*)"ipmi";
        break;
      case 'M':
        srvPttnC+=11;
        srvPttnV=(char**)realloc(srvPttnV,srvPttnC*sizeof(char*));
        srvPttnV[srvPttnC-11]=(char*)"coal";
        srvPttnV[srvPttnC-10]=(char*)"cpuinfo";
        srvPttnV[srvPttnC-9]=(char*)"cpustat";
        srvPttnV[srvPttnC-8]=(char*)"fs";
        srvPttnV[srvPttnC-7]=(char*)"irq";
        srvPttnV[srvPttnC-6]=(char*)"mem";
        srvPttnV[srvPttnC-5]=(char*)"nif";
        srvPttnV[srvPttnC-4]=(char*)"os";
        srvPttnV[srvPttnC-3]=(char*)"ps";
        srvPttnV[srvPttnC-2]=(char*)"smart";
        srvPttnV[srvPttnC-1]=(char*)"tcpip";
        break;
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'v':
        deBug++;
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
  /* default: all the server */
  if(!srvPttnC)
  {
    srvPttnC=1;
    srvPttnV=(char**)realloc(srvPttnV,sizeof(char*));
    srvPttnV[srvPttnC-1]=(char*)"*";
  }
  /*-------------------------------------------------------------------------*/
  /* check command line non-option argument(s) */
  if(optind<argc)
  {
    j=optind;
    string msg="Unrequired non-option argument(s): ";
    for(j=optind;j<argc;j++)
    {
      if(j!=optind)msg+=", ";
      msg+="\""+string(argv[j])+"\"";
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
  /* print header for compact format */
  printf("\nHOSTNAME(LOGGER)   FMC      SERVER\n");
  /*-------------------------------------------------------------------------*/
  for(j=0;j<srvPttnC;j++)
  {
    for(i=0,k=0;i<sizeof(srvList)/sizeof(char*)/3;i++)
    {
      if(fnmatch(srvPttnV[j],srvList[i].shortName,0)!=FNM_NOMATCH)
      {
        getServerVersion(hostPttnC,hostPttnV,srvList[i]);
        k++;
      }
    }
    if(!k)
    {
      printf("No server found for pattern \"%s\"!\n",srvPttnV[j]);
    }
  }
  /*-------------------------------------------------------------------------*/
  printf("\nHOSTNAME           FMC      CLIENT\n");
  printf("%-18s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
void getServerVersion(int hostPttnC,char **hostPttnV,server_t srv)
{
  unsigned i;
  unsigned slashInName=0;
  /*-------------------------------------------------------------------------*/
  if(!strcmp(srv.srvName,"logger"))
  {
    vector<string> hostPttn,logPttn;
    for(i=0;i<(unsigned)hostPttnC;i++)hostPttn.push_back(hostPttnV[i]);
    logPttn.push_back("*");
    FmcUtils::printLogServerVersion(hostPttn,logPttn,SVC_HEAD,deBug,18,8);
  }
  else
  {
    for(i=0;i<strlen(srv.srvName);i++)
    {
      if(srv.srvName[i]=='/')slashInName++;
    }
    /* current (>=FMC-3.9) service name rule */
    /* /FMC/<HOSTNAME>/process_controller/success */
    FmcUtils::printServerVersion(hostPttnC,hostPttnV,SVC_HEAD,
                                 string(srv.srvName), deBug,18,8,slashInName);
    /* old (<FMC-3.9) service name rule */
    /* /<HOSTNAME>/process_controller/success */
    FmcUtils::printServerVersion(hostPttnC,hostPttnV,"",
                                 string(srv.srvName), deBug,18,8,slashInName);
  }
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"fmcVersionViewer [-v...] [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"                 [-C] [-M] [-s SERVER_NAME...]\n"
"fmcVersionViewer -h\n"
"\n"
"Try \"fmcVersionViewer -h\" for more information.\n";
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
"fmcVersionViewer.man\n"
"\n"
"..\n"
"%s"
".hw NODE_PATTERN \n"
".TH fmcVersionViewer 1  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis fmcVersionViewer\\ \\-\n"
"Print the revision tag of the main source file and the version tag of the "
"FMC package of the FMC Servers currently running\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis fmcVersionViewer\n"
".ShortOpt[] v...\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] C\n"
".ShortOpt[] M\n"
".ShortOpt[] s SERVER_NAME...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis fmcVersionViewer\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The command \\fBfmcVersionViewer\\fP(1) is a FMC debugging tool which prints "
"to the standard output stream the \\fBrevision tag\\fP (RCS header) of the "
"\\fBmain source file\\fP and the \\fBversion tag\\fP of the "
"\\fBFMC package\\fP of the FMC servers registered with the DIM Name Server "
"running on the node \\fIDIM_DNS_NODE\\fP.\n"
".PP\n"
"Contacted FMC servers are: \\fBlogSrv\\fP(8), \\fBtmSrv\\fP(8), "
"\\fBpcSrv\\fP(8), \\fBipmiSrv\\fP(8), \\fBcmSrv\\fP(8), \\fBcoalSrv\\fP(8), "
"\\fBcpuinfoSrv\\fP(8), \\fBcpustatSrv\\fP(8), \\fBfsSrv\\fP(8), "
"\\fBirqSrv\\fP(8),  \\fBmemSrv\\fP(8), \\fBnifSrv\\fP(8), \\fBosSrv\\fP(8), "
"\\fBpsSrv\\fP(8), \\fBsmartSrv\\fP(8) and \\fBtcpipSrv\\fP(8).\n"
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
".OptDef v \"\"\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"Print only the version of the servers running on the nodes whose hostname "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: print the version of the servers running on all the nodes "
"registered with the DIM Name Server \\fIDIM_DNS_NODE\\fP).\n"
".\n"
".OptDef s \"\" SERVER_NAME (string, repeatable)\n"
"Print only the version of server \\fISERVER_NAME\\fP. More than one server "
"can be specified by repeating the \\fB-s\\fP option. \\fBDefault\\fP (if no "
"option \\fB-s\\fP is specified): print the version of all the FMC servers. "
"Available \\fISERVER_NAME\\fPs are:\n"
".PP\n"
".RS\n"
".RS\n"
".br\n"
"\\fBlog\\fP     FMC Message Logger Server;\n"
".br\n"
"\\fBtm\\fP      FMC Task Manager Server;\n"
".br\n"
"\\fBpc\\fP      FMC Process Controller Server;\n"
".br\n"
"\\fBipmi\\fP    FMC Power Manager Server;\n"
".br\n"
"\\fBcm\\fP      FMC Configuration Manager Server;\n"
".br\n"
"\\fBcoal\\fP    FMC Network Inteface Coalescence Monitor Server;\n"
".br\n"
"\\fBcpuinfo\\fP FMC CPU Static Information Monitor Server;\n"
".br\n"
"\\fBcpustat\\fP FMC CPU state information Monitor Server;\n"
".br\n"
"\\fBfs\\fP      FMC File System Monitor Server;\n"
".br\n"
"\\fBirq\\fP     FMC Interrupt Monitor Server;\n"
".br\n"
"\\fBmem\\fP     FMC Memory Monitor Server;\n"
".br\n"
"\\fBnif\\fP     FMC Network Interface Monitor Server;\n"
".br\n"
"\\fBos\\fP      FMC Operating System Monitor Server;\n"
".br\n"
"\\fBps\\fP      FMC Process Monitor Server;\n"
".br\n"
"\\fBsmart\\fP   FMC Disk health Monitor Server;\n"
".br\n"
"\\fBtcpip\\fP   FMC TCP/IP Stack Monitor Server.\n"
".br\n"
".RE\n"
".RE\n"
".\n"
".OptDef C \"\"\n"
"Print only the version of the Control Servers. Equivalent to \"\\fB-s\\fP "
"\\fIlog\\fP \\fB-s\\fP \\fItm\\fP \\fB-s\\fP \\fIpc\\fP \\fB-s\\fP "
"\\fIipmi\\fP \\fB-s\\fP \\fIcm\\fP\".\n"
".\n"
".OptDef M \"\"\n"
"Print only the version of the Monitor Servers. Equivalent to \"\\fB-s\\fP "
"\\fIcoal\\fP \\fB-s\\fP \\fIcpuinfo\\fP \\fB-s\\fP \\fIcpustat\\fP "
"\\fB-s\\fP \\fIfs\\fP \\fB-s\\fP \\fIirq\\fP \\fB-s\\fP \\fImem\\fP "
"\\fB-s\\fP \\fInif\\fP \\fB-s\\fP \\fIos\\fP \\fB-s\\fP \\fIps\\fP "
"\\fB-s\\fP \\fIsmart\\fP \\fB-s\\fP \\fItcpip\\fP\".\n"
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
"Print the version of all the FMC servers registered with the DIM Name Server "
"\\fIDIM_DNS_NODE\\fP:\n"
".PP\n"
".ShellCommand fmcVersionViewer\n"
".PP\n"
".\n"
"Print the version of all the FMC control servers registered with the DIM "
"Name Server \\fIDIM_DNS_NODE\\fP:\n"
".PP\n"
".ShellCommand fmcVersionViewer -C\n"
".PP\n"
".\n"
"Print the version of the FMC servers Task Manager servers and Configuration "
"Manager servers running on nodes farm0101 and farm0102:\n"
".PP\n"
".ShellCommand fmcVersionViewer -m farm0101 -m farm0102 -s tm -s cm\n"
".PP\n"
".\n"
"Use wildcard patterns to select the nodes:\n"
".PP\n"
".ShellCommand fmcVersionViewer -m \\[dq]farm01??\\[dq] -m "
"\\[dq]farm02??\\[dq] -s tm -s cm\n"
".ShellCommand fmcVersionViewer -m \\[dq]farm010[1-3]\\[dq]\n"
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
".BR \\%%fmcSrvViewer (1).\n"
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
