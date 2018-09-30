/*****************************************************************************/
/*
 * $Log: logGetProperties.C,v $
 * Revision 1.21  2012/12/07 15:48:07  galli
 * Minor change in man page reference
 *
 * Revision 1.20  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.19  2008/10/27 08:26:19  galli
 * minor changes
 *
 * Revision 1.11  2008/10/13 12:25:42  galli
 * uses FmcUtils::printLogServerVersion()
 *
 * Revision 1.4  2008/08/27 13:22:40  galli
 * groff manual
 *
 * Revision 1.2  2008/07/01 14:06:43  galli
 * services starts with /FMC
 *
 * Revision 1.1  2008/06/24 11:33:21  galli
 * Initial revision
 */
/*****************************************************************************/
#include <string>                                             /* std::string */
#include <vector>                                               /* std::list */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
#include <string.h>                                              /* memchr() */
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                            /* printLogServerVersion() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcRef.h"                                 /* FMC_URLS, FMC_AUTHORS */
#include "fmcPar.h"                                    /* DIM_CONF_FILE_NAME */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*****************************************************************************/
using namespace std;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
void getServerVersion(int hostPatternC,char **hostPatternV,int logPatternC,
                      char **logPatternV);
static char rcsid[]="$Id: logGetProperties.C,v 1.21 2012/12/07 15:48:07 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
int deBug=0;
/*****************************************************************************/
int main(int argc,char **argv)
{
  DimBrowser br;
  int hostPatternC=0;
  char **hostPatternV=NULL;
  int logPatternC=0;
  char **logPatternV=NULL;
  char *srvPattern=NULL;
  char *svcPattern=NULL;
  int svcPatternLen=0;
  char *rawSvcPattern=NULL;
  int rawSvcPatternLen=0;
  int nodeFoundN=0;
  char nodeFound[128]="";
  char loggerFound[128]="";
  char *p=NULL;
  char *sN=NULL;
  char *sNdup=NULL;
  char *format=NULL;
  int type=0;
  int i=0,j=0,k=0;
  char *buf=NULL;
  int flag;
  char *dimDnsNode=NULL;
  const char *baseSvc="get_properties";
  int doPrintServerVersion=0;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  hostPatternC=0;
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt(argc,argv,"+h::Vm:s:vN:"))!=EOF)
  {
    switch(flag)
    {
      case 'm':
        hostPatternC++;
        hostPatternV=(char**)realloc(hostPatternV,hostPatternC*
                                      sizeof(char*));
        hostPatternV[hostPatternC-1]=optarg;
        break;
      case 's':
        logPatternC++;
        logPatternV=(char**)realloc(logPatternV,logPatternC*sizeof(char*));
        logPatternV[logPatternC-1]=optarg;
        break;
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'v':
        deBug++;
        break;
      case 'V':
        doPrintServerVersion=1;
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
  if(!hostPatternC)
  {
    hostPatternC=1;
    hostPatternV=(char**)realloc(hostPatternV,sizeof(char*));
    hostPatternV[hostPatternC-1]=(char*)"*";
  }
  /* default: all the log services */
  if(!logPatternC)
  {
    logPatternC=1;
    logPatternV=(char**)realloc(logPatternV,sizeof(char*));
    logPatternV[logPatternC-1]=(char*)"*";
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
  for(int k=0;k<hostPatternC;k++)
  {
    if(strchr(hostPatternV[k],'/'))
    {
      fprintf(stderr,"\nA node pattern specified with the option \"-m\" "
              "(\"%s\") must not contain the '/' character!\n",
              hostPatternV[k]);
      shortUsage();
    }
  }
  for(int k=0;k<logPatternC;k++)
  {
    if(strchr(logPatternV[k],'/'))
    {
      fprintf(stderr,"\nA service pattern specified with the option \"-s\" "
              "(\"%s\") must not contain the '/' character!\n",logPatternV[k]);
      shortUsage();
    }
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
  printf("Node patterns:  ");
  for(i=0;i<hostPatternC;i++)                    /* loop over node patterns */
  {
    printf("\"%s\"",hostPatternV[i]);
    if(i<hostPatternC-1)printf(", ");
  }
  printf(".\n");
  printf("Logger patterns:  ");
  for(i=0;i<logPatternC;i++)                    /* loop over logger patterns */
  {
    printf("\"%s\"",logPatternV[i]);
    if(i<logPatternC-1)printf(", ");
  }
  printf(".\n");
  /*-------------------------------------------------------------------------*/
  /* handling of -V option */
  if(doPrintServerVersion)
  {
    getServerVersion(hostPatternC,hostPatternV,logPatternC,logPatternV);
  }
  /*-------------------------------------------------------------------------*/
  for(i=0,j=0;i<hostPatternC;i++)                /* loop over node patterns */
  {
    for(k=0;k<logPatternC;k++)                 /* loop over service patterns */
    {
      /*.....................................................................*/
      printf("  Node pattern: \"%s\", service pattern: \"%s\"\n",
             hostPatternV[i],logPatternV[k]);
      srvPattern=(char*)realloc(srvPattern,1+strlen(hostPatternV[i]));
      strcpy(srvPattern,hostPatternV[i]);
      /* convert to upper case */
      for(p=srvPattern;*p;p++)*p=toupper(*p);
      svcPatternLen=snprintf(NULL,0,"%s/%s/logger/%s/%s",SVC_HEAD,srvPattern,
                             logPatternV[k],baseSvc);
      svcPattern=(char*)realloc(svcPattern,1+svcPatternLen);
      snprintf(svcPattern,1+svcPatternLen,"%s/%s/logger/%s/%s",SVC_HEAD,
               srvPattern,logPatternV[k],baseSvc);
      if(deBug)printf("  Service pattern: \"%s\"\n",svcPattern);
      /*.....................................................................*/
      rawSvcPatternLen=snprintf(NULL,0,"%s/*/logger/*/%s",SVC_HEAD,baseSvc);
      rawSvcPattern=(char*)realloc(rawSvcPattern,1+rawSvcPatternLen);
      snprintf(rawSvcPattern,1+rawSvcPatternLen,"%s/*/logger/*/%s",SVC_HEAD,
               baseSvc);
      /*.....................................................................*/
      br.getServices(rawSvcPattern);
      nodeFoundN=0;
      while((type=br.getNextService(sN,format))!=0)       /* loop over nodes */
      {
        if(!fnmatch(svcPattern,sN,0))
        {
          nodeFoundN++;
          sNdup=(char*)realloc(sNdup,1+strlen(sN));
          strcpy(sNdup,sN);
          p=strrchr(sNdup,'/');
          *p='\0';
          p=strrchr(sNdup,'/');
          snprintf(loggerFound,sizeof(loggerFound),"%s",1+p);
          *p='\0';
          p=strrchr(sNdup,'/');
          *p='\0';
          p=strrchr(sNdup,'/');
          snprintf(nodeFound,sizeof(nodeFound),"%s",1+p);
          for(p=nodeFound;*p;p++)
          {
            if(*p=='/')
            {
              *p='\0';
              break;
            }
            *p=tolower(*p);
          }
          for(p=loggerFound;*p;p++)
          {
            if(*p=='/')
            {
              *p='\0';
              break;
            }
          }
          if(deBug)printf("  %3d Contacting FMC Logger Service: \"%s\"...\n",
                          j,sN);
          DimCurrentInfo Info(sN,(char*)"(N/A)");
          buf=Info.getString();
          /*.................................................................*/
          printf("    %-4d Node: \"%s\", logger: \"%s\": \"%s\".\n",
                 j,nodeFound,loggerFound,buf);
          j++;
        }
      }                                                   /* loop over nodes */
      if(!nodeFoundN)
      {
        printf("    no node found for node pattern: \"%s\" and service "
               "pattern: \"%s\"\n",hostPatternV[i],logPatternV[k]);
      }
    }                                          /* loop over service patterns */
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
/* globals svcPatternC and svcPatternV read from this function */
void getServerVersion(int hostPatternC,char **hostPatternV,int logPatternC,
                      char **logPatternV)
{
  vector<string> hostPttn,logPttn;
  int i;
  /*-------------------------------------------------------------------------*/
  for(i=0;i<hostPatternC;i++) hostPttn.push_back(hostPatternV[i]);
  for(i=0;i<logPatternC;i++)  logPttn.push_back(logPatternV[i]);
  /*-------------------------------------------------------------------------*/
  //printf("HOSTNAME     LOGGER       FMC      SERVER\n");
  printf("HOSTNAME(LOGGER)          FMC      SERVER\n");
  FmcUtils::printLogServerVersion(hostPttn,logPttn,SVC_HEAD,deBug,25,8);
  printf("HOSTNAME                  FMC      CLIENT\n");
  printf("%-25s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"logGetProperties [-v] [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"                 [-s LOG_PATTERN...]\n"
"logGetProperties -V [-v] [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"                 [-s LOG_PATTERN...]\n"
"logGetProperties -h\n"
"\n"
"Try \"logGetProperties -h\" for more information.\n";
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
"logGetProperties.man\n"
"\n"
"..\n"
"%s"
".hw no\\[hy]authentication UTGID_PATTERN\n"
".TH logGetProperties 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis logGetProperties\\ \\-\n"
"Get the Properties of one or more FMC Message Logger Server(s)\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis logGetProperties\n"
".ShortOpt[] v\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] s LOG_PATTERN\n"
".EndSynopsis\n"
".sp\n"
".Synopsis logGetProperties\n"
".ShortOpt V\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] s LOG_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis logGetProperties\n"
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
"The \\fBlogGetProperties\\fP command gets and prints the \\fBproperties\\fP\n"
"of the selected FMC Message Logger Servers, i.e. the \\fBfilter\\fP\n"
"settings (severity threshold, filter reguar expressions and wildcard\n"
"patterns), the \\fBduplicate suppression\\fP settings (exact comparison,\n"
"word distance threshold, Levenshtein distance threshold, compared message\n"
"number, header skipping parameters) and the \\fBold message storage\\fP\n"
"setting.\n"
".PP\n"
"The setting strings are described in manuals logSetProperties(1),\n"
"logViewer(1) and logSrv(8).\n"
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
".OptDef h \"\"\n"
"Get basic usage help from the command line.\n"
".\n"
".OptDef v \"\"\n"
"Increase output verbosity for debugging.\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef V \"\"\n"
"For each selected node print the FMC Message Logger\n"
"Server version and the FMC version, than exit.\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"Get the current properties from FMC Logger Servers running on nodes whose\n"
"hostname matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP\n"
"(\\fBdefault\\fP: Get the current properties from FMC Logger Servers\n"
"running at all the nodes).\n"
".\n"
".OptDef s \"\" LOG_PATTERN (string, repeatable)\n"
"Get the current properties from the FMC Logger Servers whose name\n"
"matches the wildcard pattern \\fILOG_PATTERN\\fP (\\fBdefault\\fP: all the\n"
"names). The name of a FMC Logger is set using the \\fB-s\\fP\n"
"option in the \\fBlogSrv(8)\\fP command line. The name of the\n"
"default logger is \\fBfmc\\fP. The name of the Gaudi logger is\n"
"\\fBgaudi\\fP.\n"
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
"Get properties from FMC Message Logger Server with names \"fmc\" and\n"
"\"gaudi\" running on nodes \"farm0101\" and \"farm0102\":\n"
".\n"
".PP\n"
".ShellCommand logGetProperties -m farm0101 -m farm0102 -s gaudi -s fmc\n"
".\n"
".PP\n"
"Use wildcard patterns to select nodes and loggers:\n"
".\n"
".PP\n"
".ShellCommand logGetProperties -m \\[dq]farm01*\\[dq] -s gaudi -s logger_1\n"
".ShellCommand logGetProperties -m \\[dq]farm01[1-5]?\\[dq] "
"-s \\[dq]logger_[0-3]\\[dq]\n"
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
".BR \\%%logSrv (8),\n"
".BR \\%%logViewer (1),\n"
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
,FMC_URLS,FMC_AUTHORS
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/*****************************************************************************/
