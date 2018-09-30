/*****************************************************************************/
/*
 * $Log: logSetProperties.C,v $
 * Revision 1.18  2012/12/07 15:48:07  galli
 * Minor change in man page reference
 *
 * Revision 1.17  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.16  2008/10/27 08:27:22  galli
 * minor changes
 *
 * Revision 1.9  2008/10/13 14:18:35  galli
 * can print server version
 *
 * Revision 1.4  2008/08/27 21:37:09  galli
 * groff manual
 *
 * Revision 1.2  2008/07/01 14:11:14  galli
 * services starts with /FMC
 *
 * Revision 1.1  2008/06/24 11:12:19  galli
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
#include <argz.h>                         /* argz_create(), argz_stringify() */
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                            /* printLogServerVersion() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcRef.h"                                  /* FMC_URLS FMC_AUTHORS */
#include "fmcPar.h"                                    /* DIM_CONF_FILE_NAME */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
int getLocalOpt(int *argcP,char ***argvP,const char* optS,int withArg,
                char **optArg);
void getServerVersion(int hostPatternC,char **hostPatternV,int logPatternC,
                      char **logPatternV);
/*****************************************************************************/
static char rcsid[]="$Id: logSetProperties.C,v 1.18 2012/12/07 15:48:07 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
int deBug=0;
/*****************************************************************************/
int main(int argc,char **argv)
{
  DimBrowser br;
  char *srvPattern=NULL;
  char *cmdPattern=NULL;
  int cmdPatternLen=0;
  char *rawCmdPattern=NULL;
  int rawCmdPatternLen=0;
  int hostPatternC=0;
  char **hostPatternV=NULL;
  int logPatternC=0;
  char **logPatternV=NULL;
  char nodeFound[128]="";
  char loggerFound[128]="";
  int svcFoundN=0;
  char *s=NULL;
  char *p=NULL;
  char *sN=NULL;
  char *sNdup=NULL;
  char *format=NULL;
  int type=0;
  char *argz=0;
  size_t argz_len=0;
  int found=0;
  int i=0,j=0,k=0;
  char *dimDnsNode=NULL;
  int doPrintServerVersion=0;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process local command-line options (-h, --help, -hh, -m, -v, -V) */
  if(argc==1)                                                /* no arguments */
  {
    fprintf(stderr,"\nERROR: at least one setting option required (-l, -f, "
            "-F, -x, -X, -E, -L, -W, -n, -c, -C, -S, -V)!\n");
    shortUsage();
  }
  if(getLocalOpt(&argc,&argv,"-h",0,&s))usage(0);
  if(getLocalOpt(&argc,&argv,"--help",0,&s))usage(0);
  if(getLocalOpt(&argc,&argv,"-hh",0,&s))usage(1);
  if(getLocalOpt(&argc,&argv,"-v",0,&s))deBug++;
  if(getLocalOpt(&argc,&argv,"-V",0,&s))doPrintServerVersion=1;
  if(getLocalOpt(&argc,&argv,"--version",0,&s))doPrintServerVersion=1;
  found=getLocalOpt(&argc,&argv,"-N",1,&s);
  if(found==1)dimDnsNode=s;
  else if(found==-1)shortUsage();
  for(hostPatternC=0,found=0;;)
  {
    found=getLocalOpt(&argc,&argv,"-m",1,&s);
    if(found==-1)shortUsage();
    else if(found==0)break;
    else
    {
      hostPatternC++;
      hostPatternV=(char**)realloc(hostPatternV,hostPatternC*sizeof(char*));
      hostPatternV[hostPatternC-1]=s;
    }
  }
  for(logPatternC=0,found=0;;)
  {
    found=getLocalOpt(&argc,&argv,"-s",1,&s);
    if(found==-1)shortUsage();
    else if(found==0)break;
    else
    {
      logPatternC++;
      logPatternV=(char**)realloc(logPatternV,logPatternC*sizeof(char*));
      logPatternV[logPatternC-1]=s;
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
  if(doPrintServerVersion)
  {
    getServerVersion(hostPatternC,hostPatternV,logPatternC,logPatternV);
  }
  /*-------------------------------------------------------------------------*/
  /* prepare command line to be passed to logSrv */
  argz_create(argv+1,&argz,&argz_len);
  argz_stringify(argz,argz_len,32);
  if(argz==NULL)
  {
    fprintf(stderr,"\nERROR: at least one setting option required (-l, -f, "
            "-F, -x, -X, -E, -L, -W, -n, -c, -C, -S)!\n");
    shortUsage();
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
      cmdPatternLen=snprintf(NULL,0,"%s/%s/logger/%s/%s",SVC_HEAD,srvPattern,
                             logPatternV[k],"set_properties");
      cmdPattern=(char*)realloc(cmdPattern,1+cmdPatternLen);
      snprintf(cmdPattern,1+cmdPatternLen,"%s/%s/logger/%s/%s",SVC_HEAD,
               srvPattern,logPatternV[k],"set_properties");
      if(deBug)printf("  Command pattern: \"%s\"\n",cmdPattern);
      /*.....................................................................*/
      rawCmdPatternLen=snprintf(NULL,0,"%s/*/logger/*/%s",SVC_HEAD,
                                "set_properties");
      rawCmdPattern=(char*)realloc(rawCmdPattern,1+rawCmdPatternLen);
      snprintf(rawCmdPattern,1+rawCmdPatternLen,"%s/*/logger/*/%s",SVC_HEAD,
               "set_properties");
      /*.....................................................................*/
      br.getServices(rawCmdPattern);
      svcFoundN=0;
      while((type=br.getNextService(sN,format))!=0)       /* loop over nodes */
      {
        if(!fnmatch(cmdPattern,sN,0))
        {
          svcFoundN++;
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
          printf("    found %3d: node: \"%s\", logger: \"%s\"\n",j,nodeFound,
                 loggerFound);
          if(deBug)printf("  %3d Contacting Task Manager Service: \"%s\""
                          "...\n",j,sN);
          DimClient::sendCommand(sN,argz);
          j++;
        }
      }                                                   /* loop over nodes */
      if(!svcFoundN)
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
/* Look for the switch optS (e.g.: -C, --help) in the command line, return   */
/* its eventual argument in optArg and strip the switch and its eventual     */
/* argument from the command line.                                           */
/* If withArg=0 no argument is expected to follow the switch.                */
/* If withArg=1 an argument is expected to follow the switch.                */
/* Return value:                                                             */
/*   0 option not found                                                      */
/*   1 option found                                                          */
/*  -1 error                                                                 */
/* Example:                                                                  */
/*   main(int argc,char **argv)                                              */
/*   {                                                                       */
/*     int found=0;                                                          */
/*     char *pcSrvHostName=NULL;                                             */
/*     found=getLocalOpt(&argc,&argv,"-C",1,&pcSrvHostName);                 */
/*   }                                                                       */
/*****************************************************************************/
int getLocalOpt(int *argcP,char ***argvP,const char* optS,int withArg,
                char **optArg)
{
  int newArgc=*argcP;
  char **newArgv=*argvP;
  int i,j;
  int found=0;
  int found2=0;
  /*-------------------------------------------------------------------------*/
  /* find option */
  for(i=0;i<*argcP;i++)                               /* loop over arguments */
  {
    if(!strcmp((*argvP)[i],optS))
    {
      found++;
      if(!withArg)break;
      if(!(i+1<*argcP))                                 /* no more arguments */
      {
        fprintf(stderr,"\nERROR: option \"%s\" require an argument!\n\n",optS);
        return -1;
      }
      else if((*argvP)[i+1][0]=='-')           /* following arg start with - */
      {
        fprintf(stderr,"\nERROR: option \"%s\" require an argument!\n\n",optS);
        return -1;
      }
      else 
      {
        *optArg=strdup((*argvP)[i+1]);
      }
      break;
    }
  }                                                   /* loop over arguments */
  /*-------------------------------------------------------------------------*/
  if(!found)return found;
  /*-------------------------------------------------------------------------*/
  /* strip found option from command line */
    newArgc=*argcP-1-!!withArg;
    newArgv=(char**)malloc((1+newArgc)*sizeof(char*));
    newArgv[newArgc]=NULL;
    for(i=0,j=0,found2=0;;)
    {
      if(i>=*argcP)break;
      if(!found2 && !strcmp((*argvP)[i],optS))
      {
        i+=(1+!!withArg);
        found2++;
      }
      newArgv[j]=(*argvP)[i];
      i++;
      j++;
    }
  *argcP=newArgc;
  *argvP=newArgv;
  return found;
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"logSetProperties [-v] [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"                 [-s LOG_PATTERN...]  [-l SEVERITY_THRESHOLD]\n"
"                 [-F REGULAR_EXPRESSION] [-f WILDCARD_PATTERN]\n"
"                 [-X REGULAR_EXPRESSION] [-x WILDCARD_PATTERN] [-E 0|1]\n"
"                 [-L LEV_DIST] [-W WORD_DIST] [-n COMP_MSGS]\n"
"                 [-c SKIP_NUM_COMP] [-C SKIP_CHAR_COMP]\n"
"                 [-S STORED_MSGS]\n"
"logSetProperties [-v] [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"                 [-s LOG_PATTERN...] -r\n"
"logSetProperties {-V | --version} [-v] [-N DIM_DNS_NODE] "
"[-m NODE_PATTERN...]\n"
"                 [-s LOG_PATTERN...]\n"
"logSetProperties -h\n"
"\n"
"Try \"logSetProperties -h\" for more information.\n";
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
"logSetProperties.man\n"
"\n"
"..\n"
"%s"
".hw REGULAR_EXPRESSION\n"
".TH logSetProperties 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis logSetProperties\\ \\-\n"
"Set the Properties of one or more FMC Message Logger Server(s)\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis logSetProperties\n"
".ShortOpt[] v\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] s LOG_PATTERN...\n"
".ShortOpt[] l SEVERITY_THRESHOLD\n"
".ShortOpt[] F REGULAR_EXPRESSION\n"
".ShortOpt[] f WILDCARD_PATTERN\n"
".ShortOpt[] X REGULAR_EXPRESSION\n"
".ShortOpt[] x WILDCARD_PATTERN\n"
".ShortOpt[] E 0|1\n"
".ShortOpt[] L LEV_DIST\n"
".ShortOpt[] W WORD_DIST\n"
".ShortOpt[] n COMP_MSGS\n"
".ShortOpt[] c SKIP_NUM_COMP\n"
".ShortOpt[] C SKIP_CHAR_COMP\n"
".ShortOpt[] S STORED_MSGS\n"
".EndSynopsis\n"
".sp\n"
".Synopsis logSetProperties\n"
".ShortOpt[] v\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] s LOG_PATTERN...\n"
".ShortOpt r\n"
".EndSynopsis\n"
".sp\n"
".Synopsis logSetProperties\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] s LOG_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis logSetProperties\n"
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
"The \\fBlogSetProperties\\fP command sets and changes, at \\fBruntime\\fP,\n"
"the \\fBproperties\\fP\n"
"of the selected FMC Message Logger Servers, i.e. the \\fBfilter\\fP\n"
"settings (severity threshold, filter regular expressions and wildcard\n"
"patterns), the \\fBduplicate suppression\\fP settings (exact comparison,\n"
"word distance threshold, Levenshtein distance threshold, compared message\n"
"number, header skipping parameters) and the \\fBmessage buffer\\fP\n"
"setting (number of stored messages).\n"
".PP\n"
"The current \\fBproperties\\fP of any FMC Message Logger Server can be\n"
"retrieved through the \\fBlogGetProperties\\fP(1) command.\n"
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
".\n"
".SH OPTIONS\n"
".\n"
".SS General Options\n"
".\n"
".OptDef h \"\"\n"
"Get basic usage help from the command line.\n"
".\n"
".OptDef r \"\"\n"
"Reset all the properties to their default values.\n"
".\n"
".OptDef v \"\"\n"
"Increase output verbosity for debugging.\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef V version\n"
"For each selected node print the FMC Message Logger\n"
"Server version and the FMC version, than exit.\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"Set the properties of FMC Logger Servers running on nodes whose\n"
"hostname matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP\n"
"(\\fBdefault\\fP: set the properties of FMC Logger Servers\n"
"running at all the nodes).\n"
".\n"
".OptDef s \"\" LOG_PATTERN (string, repeatable)\n"
"Set the properties of the FMC Logger Servers whose name\n"
"matches the wildcard pattern \\fILOG_PATTERN\\fP (\\fBdefault\\fP: all the\n"
"names). The name of a FMC Logger is set using the \\fB-s\\fP\n"
"option in the \\fBlogSrv(8)\\fP command line. The name of the\n"
"default logger is \\fBfmc\\fP. The name of the Gaudi logger is\n"
"\\fBgaudi\\fP.\n"
".\n"
".\n"
".SS Filter Options\n"
".\n"
".OptDef l \"\" SEVERITY_THRESHOLD (integer or string)\n"
"Filter out messages with \\fBseverity\\fP <\n"
"\\fISEVERITY_THRESHOLD\\fP. As string, \\fBALL\\fP\\~=\\~0, "
"\\fBVERB\\fP\\~=\\~1, \\fBDEBUG\\fP\\~=\\~2, \\fBINFO\\fP\\~=\\~3, "
"\\fBWARN\\fP\\~=\\~4, \\fBERROR\\fP\\~=\\~5, \\fBFATAL\\fP\\~=6\\~.\n"
"\\fBDefault\\fP: \\fISEVERITY_THRESHOLD\\fP\\~=\\~1\\~=\\~\\fBVERB\\fP.\n"
".\n"
".OptDef F \"\" REGULAR_EXPRESSION (string)\n"
"Filter \\fBthrough\\fP messages by using the extended regular expression\n"
"\\fIREGULAR_EXPRESION\\fP. \\fBDefault\\fP: no regular expression filter.\n"
".\n"
".OptDef f \"\" WILDCARD_PATTERN (string)\n"
"Filter \\fBthrough\\fP messages by using the wildcard pattern\n"
"\\fIWILDCARD_PATTERN\\fP. \\fBDefault\\fP: no wildcard pattern filter.\n"
".\n"
".OptDef X \"\" REGULAR_EXPRESSION (string)\n"
"Filter \\fBout\\fP messages by using the extended regular expression\n"
"\\fIREGULAR_EXPRESION\\fP. \\fBDefault\\fP: no regular expression filter.\n"
".\n"
".OptDef x \"\" WILDCARD_PATTERN (string)\n"
"Filter \\fBout\\fP messages by using the wildcard pattern\n"
"\\fIWILDCARD_PATTERN\\fP. \\fBDefault\\fP: no wildcard pattern filter.\n"
".\n"
".\n"
".SS Duplicate Suppression Options\n"
".\n"
".OptDef E \"\" 0|1\n"
"Suppress repeated messages using exact comparison, i.e.\n"
"suppress messages which are identical to at least one of the\n"
"previous \\fICOMP_MSGS\\fP-1 messages. \\fBDefault\\fP: do not suppress\n"
"repeated messages. \\fIN.B.\\fP: the behaviour with this option is\n"
"affected by \\fB-c\\fP, \\fB-C\\fP and \\fB-n\\fP options.\n"
".\n"
".OptDef L \"\" LEV_DIST (integer)\n"
"Suppress repeated messages using Levenshtein difference, i.e.\n"
"suppress messages which differ less than or equal to \\fILEV_DIST\\fP\n"
"(inserted, deleted or substituted) characters from at least\n"
"one of the previous \\fICOMP_MSGS\\fP-1 messages. \\fBDefault\\fP: do not\n"
"suppress repeated messages. \\fIN.B.\\fP: the behaviour with this\n"
"option is affected by \\fB-c\\fP, \\fB-C\\fP and \\fB-n\\fP options.\n"
".\n"
".OptDef W \"\" WORD_DIST (integer)\n"
"Suppress repeated messages using word difference, i.e.\n"
"suppress messages which differ less than or equal to \\fIWORD_DIST\\fP\n"
"subsituted or appended words, with respect to at least one of\n"
"the previous \\fICOMP_MSGS\\fP-1 messages. \\fBDefault\\fP: do not suppress\n"
"repeated messages. \\fIN.B.\\fP: the behaviour with this option is\n"
"affected by \\fB-c\\fP, \\fB-C\\fP and \\fB-n\\fP options.\n"
".\n"
".OptDef n \"\" COMP_MSGS (integer)\n"
"To suppress repeated messages, consider the last \\fICOMP_MSGS\\fP\n"
"messages, i.e. compare each message with the previous\n"
"\\fICOMP_MSGS\\fP-1 messages. \\fBDefault\\fP: 1 if repeated message\n"
"suppression is not active, 2 if it is active. \\fIN.B.\\fP: affect the\n"
"behaviour with \\fB-E\\fP, \\fB-L\\fP and \\fB-W\\fP options.\n"
".\n"
".OptDef c \"\" SKIP_NUM_COMP (integer)\n"
"In the comparison for the repeated message suppression, skip\n"
"\\fISKIP_NUM_COMP\\fP characters in each string, i.e. start the\n"
"comparison between a couple of string from the\n"
"\\fISKIP_NUM_COMP\\fP-th character. Affect the behaviour with \\fB-E\\fP, \\fB-L\\fP\n"
"and \\fB-W\\fP options.\n"
".\n"
".OptDef C \"\" SKIP_CHAR_COMP (string)\n"
"In the comparison for the repeated message suppression, skip\n"
"characters in each string until all the characters listed in\n"
"the string \\fISKIP_CHAR_COMP\\fP are passed. Affect the behaviour\n"
"with \\fB-E\\fP, \\fB-L\\fP and \\fB-W\\fP options.\n"
".\n"
".\n"
".SS Message Buffer Options\n"
".OptDef S \"\" STORED_MSGS (integer)\n"
"Keep the last \\fISTORED_MSGS\\fP messages stored at the \\fBsource\\fP\n"
"node, accessible through the \\fBlast_log\\fP DIM service of the\n"
"\\fBlogSrv\\fP process. \\fBDefault\\fP: 0.\n"
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
"Reset all the \\fBlogSrv\\fP properties to their default values in all\n"
"nodes and all loggers registered with the current DIM DNS:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -r\n"
".ShellCommand logSetProperties -m \\[rs]* -s \\[rs]* -r\n"
".ShellCommand logSetProperties -m \\[dq]*\\[dq] -s \\[dq]*\\[dq] -r\n"
".\n"
".PP\n"
"Reset all the \\fBlogSrv\\fP properties to their default values in the\n"
"loggers named \"fmc\" running on wildcard selected nodes:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m \\[dq]*01*\\[dq] -s fmc -r\n"
".\n"
".PP\n"
"Filter out messages with severity < ERROR:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -m farm0102 -s fmc -s gaudi -l 5\n"
".ShellCommand logSetProperties -m farm0101 -m farm0102 -s fmc -s gaudi "
"-l ERROR\n"
".\n"
".PP\n"
"Filter through only messages matching the regular expression \"Srv\":\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -m farm0102 -s fmc -s gaudi -F "
"\\[dq]Srv\\[dq]\n"
".ShellCommand logSetProperties -m \\[dq]farm01*\\[dq] -s fmc -s gaudi -F "
"\\[dq]Srv\\[dq]\n"
".\n"
".PP\n"
"Filter out messages matching the regular expression \"Srv\":\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -m farm0102 -s fmc -s gaudi -X "
"\\[dq]Srv\\[dq]\n"
".ShellCommand logSetProperties -m \\[dq]farm01*\\[dq] -s fmc -s gaudi -X "
"\\[dq]Srv\\[dq]\n"
".\n"
".PP\n"
"Filter through only messages matching the wildcard pattern \"*Srv*\":\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m \\[dq]farm01*\\[dq] -s fmc -s gaudi "
"-f \\[dq]*Srv*\\[dq]\n"
".\n"
".PP\n"
"Filter out messages matching the wildcard pattern \"*Srv*\":\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m \\[dq]farm01*\\[dq] -s fmc -s gaudi "
"-x \\[dq]*Srv*\\[dq]\n"
".\n"
".PP\n"
"Do \\fBnot\\fP filter at all:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m \\[dq]*01*\\[dq] -s fmc \\[dq]-F '' -f '' "
"-X '' -x ''\\[dq]\n"
".\n"
".PP\n"
"Select nodes and loggers using wildcard pattern:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m \\[dq]farm01[1-5]?\\[dq] "
"-s \\[dq]logger_[0-3]\\[dq] -F \\[dq]Srv\\[dq]\n"
".\n"
".PP\n"
"Filter through only the lines containing the word \"word1\" near and\n"
"before the word \"word2\" (at most 4 words between):\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -s gaudi "
"-F \\[dq]\\[rs]bword1\\[rs]W+(\\[rs]w+\\[rs]W+)\\:{0,4}\\:word2\\[rs]b\\[dq]\n"
".\n"
".PP\n"
"Filter through only the lines containing a floating point number:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -s gaudi "
"-F \\[dq][-+]?[0-9]*\\[rs].[0-9]+([eE][-+]?\\:[0-9]+)?\\[dq]\n"
".\n"
".PP\n"
"Filter through only the lines containing the string \"ERROR\":\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -F ERROR\n"
".\n"
".PP\n"
"Suppress messages which are identical to the previous one:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -s gaudi -E 1\n"
".\n"
".PP\n"
"Suppress messages which are identical after the 30-th character\n"
"to the previous one:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -s gaudi -E 1 -c 30\n"
".\n"
".PP\n"
"Suppress messages which are identical after the 30-th character\n"
"to one of the previous 9 messages:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -s gaudi -E 1 -c 30 -n 10\n"
".\n"
".PP\n"
"Suppress messages which are identical, after the second \":\" in\n"
"the message, to one of the previous 9 messages:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -s gaudi -E 1 -C \\[dq]::\\[dq] -n 10\n"
".\n"
".PP\n"
"Suppress messages which differs, in at most 2 words after the\n"
"second \":\" with respect to one of the previous 9 messages:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -s gaudi -W 2 -C \\[dq]::\\[dq] -n 10\n"
".\n"
".PP\n"
"Suppress messages which differs, in at most 6 character\n"
"substituted, inserted or deleted after the second \":\" with\n"
"respect to one of the previous 9 messages:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -s gaudi -L 6 -C \\[dq]::\\[dq] -n 10\n"
".\n"
".PP\n"
"Do \\fBnot\\fP suppress repeated messages:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -s gaudi -E 0 -L -1 -W -1 -n 1\n"
".\n"
".PP\n"
"Keep the last 20 messages stored:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -s gaudi -S 20\n"
".\n"
".PP\n"
"Do \\fBnot\\fP keep messages stored:\n"
".\n"
".PP\n"
".ShellCommand logSetProperties -m farm0101 -s gaudi -S 0"
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
".BR \\%%logViewer (1),\n"
".BR \\%%logGetProperties (1).\n"
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
  exit(0);
}
/*****************************************************************************/
