/*****************************************************************************/
/*
 * $Log: pcLl.C,v $
 * Revision 1.20  2012/12/14 13:50:40  galli
 * Minor changes in man page references
 *
 * Revision 1.19  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.16  2008/10/03 15:13:38  galli
 * DIM_CONF_FILE_NAME got from fmcPar.h
 *
 * Revision 1.12  2008/10/01 07:52:32  galli
 * groff manual
 *
 * Revision 1.9  2008/07/07 15:38:24  galli
 * service names starts with /FMC
 *
 * Revision 1.8  2007/12/14 11:21:08  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.7  2007/08/16 13:38:51  galli
 * usage() modified
 * compact (-c) and very compact (-cc) output
 *
 * Revision 1.5  2006/09/07 06:51:13  galli
 * output in more readable format
 * old output with -c option
 *
 * Revision 1.4  2006/09/06 14:04:25  galli
 * -C and -m options repeatable
 * usage and messages improved
 *
 * Revision 1.2  2005/05/28 22:41:21  galli
 * working version
 *
 * Revision 1.1  2005/05/28 22:34:58  galli
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
#include <sys/wait.h>                                             /* wait(2) */
/* ------------------------------------------------------------------------- */
/* DIM */
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcRef.h"                                  /* FMC_URLS FMC_AUTHORS */
#include "fmcPar.h"                                    /* DIM_CONF_FILE_NAME */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
#include "fmcDate.h"                                     /* compilation date */
/* ------------------------------------------------------------------------- */
/* in pc/include */
#include "pcSrv.H"                                               /* SRV_NAME */
#include "FmcUtils.IC"                               /* printServerVersion() */
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: pcLl.C,v 1.20 2012/12/14 13:50:40 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
void getServerVersion(int nodeHostNameC,char **nodeHostNameV);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i=0,j=0,k=0,l=0,m=0;
  /* pointers */
  char *p=NULL;
  /* service browsing */
  int ctrlPcHostNameC=0;
  char **ctrlPcHostNameV=NULL;
  char *srvPattern=NULL;
  char *svcPattern=NULL;
  int svcPatternLen=0;
  char *rawSvcPattern=NULL;
  int rawSvcPatternLen=0;
  DimBrowser br;
  int type=0;
  char *format=NULL;
  char *llSvc=NULL;
  char *llSvcDup=NULL;
  int cpcFoundN=0;
  char cpcFound[128]="";
  char *dimDnsNode=NULL;
  /* retrieved quantities */
  int llBufSz=0;
  char *llBuf=NULL;
  char *llBufP=NULL;
  char nodeName[128]="";
  /* command line switch */
  int doPrintServerVersion=0;
  int nodeHostNameC=0;
  char **nodeHostNameV=NULL;
  int compact=0;
  /* getopt */
  int flag;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  ctrlPcHostNameC=0;
  nodeHostNameC=0;
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt(argc,argv,"+h::VC:m:vcN:"))!=EOF)
  {
    switch(flag)
    {
      case 'm':
        nodeHostNameC++;
        nodeHostNameV=(char**)realloc(nodeHostNameV,nodeHostNameC*
                                      sizeof(char*));
        nodeHostNameV[nodeHostNameC-1]=optarg;
        break;
      case 'C':
        ctrlPcHostNameC++;
        ctrlPcHostNameV=(char**)realloc(ctrlPcHostNameV,ctrlPcHostNameC*
                                      sizeof(char*));
        ctrlPcHostNameV[ctrlPcHostNameC-1]=optarg;
        break;
      case 'V':
        doPrintServerVersion=1;
        break;
      case 'v':
        deBug++;
        break;
      case 'c':
        compact++;
        break;
      case 'N':
        dimDnsNode=optarg;
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
  /* default: all the nodes */
  if(!nodeHostNameC)
  {
    nodeHostNameC=1;
    nodeHostNameV=(char**)realloc(nodeHostNameV,sizeof(char*));
    nodeHostNameV[nodeHostNameC-1]=(char*)"*";
  }
  /* default: all the ctrl PCs */
  if(!ctrlPcHostNameC)
  {
    ctrlPcHostNameC=1;
    ctrlPcHostNameV=(char**)realloc(ctrlPcHostNameV,sizeof(char*));
    ctrlPcHostNameV[ctrlPcHostNameC-1]=(char*)"*";
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
  if(doPrintServerVersion)getServerVersion(ctrlPcHostNameC,ctrlPcHostNameV);
  /*-------------------------------------------------------------------------*/
  printf("Ctrl Pc patterns: ");
  for(i=0;i<ctrlPcHostNameC;i++)                /* loop over ctrlPc patterns */
  {
    printf("\"%s\"",ctrlPcHostNameV[i]);
    if(i<ctrlPcHostNameC-1)printf(", ");
  }
  printf(".\n");
  printf("Node patterns:    ");
  for(l=0;l<nodeHostNameC;l++)                    /* loop over node patterns */
  {
    printf("\"%s\"",nodeHostNameV[l]);
    if(l<nodeHostNameC-1)printf(", ");
  }
  printf(".\n");
  /*-------------------------------------------------------------------------*/
  for(i=0,j=0;i<ctrlPcHostNameC;i++)           /* loop over ctrl PC patterns */
  {
    printf("  Ctrl PC pattern: \"%s\"\n",ctrlPcHostNameV[i]);
    srvPattern=(char*)realloc(srvPattern,1+strlen(ctrlPcHostNameV[i]));
    strcpy(srvPattern,ctrlPcHostNameV[i]);
    /* convert to upper case */
    for(p=srvPattern;*p;p++)*p=toupper(*p);
    svcPatternLen=snprintf(NULL,0,"%s/%s/%s/ll",SVC_HEAD,srvPattern,SRV_NAME);
    svcPattern=(char*)realloc(svcPattern,1+svcPatternLen);
    snprintf(svcPattern,1+svcPatternLen,"%s/%s/%s/ll",SVC_HEAD,srvPattern,
             SRV_NAME);
    if(deBug)printf("  Service pattern: \"%s\"\n",svcPattern);
    /*.......................................................................*/
    rawSvcPatternLen=snprintf(NULL,0,"%s/*/%s/ll",SVC_HEAD,SRV_NAME);
    rawSvcPattern=(char*)realloc(rawSvcPattern,1+rawSvcPatternLen);
    snprintf(rawSvcPattern,1+rawSvcPatternLen,"%s/*/%s/ll",SVC_HEAD,SRV_NAME);
    /*.......................................................................*/
    br.getServices(rawSvcPattern);
    cpcFoundN=0;
    while((type=br.getNextService(llSvc,format))!=0)   /* loop over ctrl PCs */
    {
      if(!fnmatch(svcPattern,llSvc,0))
      {
        cpcFoundN++;
        llSvcDup=(char*)realloc(llSvcDup,1+strlen(llSvc));
        strcpy(llSvcDup,llSvc);
        p=strrchr(llSvcDup,'/');
        *p='\0';
        p=strrchr(llSvcDup,'/');
        *p='\0';
        p=strrchr(llSvcDup,'/');
        snprintf(cpcFound,sizeof(cpcFound),"%s",1+p);
        for(p=cpcFound;*p;p++)
        {
          if(*p=='/')
          {
            *p='\0';
            break;
          }
          *p=tolower(*p);
        }
        if(deBug)printf("  %3d Contacting Process Controller Service: \"%s\""
                        "...\n",j,llSvc);
        DimCurrentInfo ll(llSvc,(char*)"(N/A)");
        /*...................................................................*/
        llBufSz=ll.getSize();
        llBuf=(char*)ll.getData();
        llBufSz--;                               /* suppress the second '\0' */
        /*...................................................................*/
        /* change \n into \0 */
        for(k=0;k<llBufSz;k++)
        {
          if(llBuf[k]=='\n')llBuf[k]='\0';
        }
        /*...................................................................*/
        printf("    Processes scheduled by Process Controller %2d: \"%s\":\n",
               j,cpcFound);
        llBufP=llBuf;
        for(k=0,m=0;;k++)                       /* loop over found nodeNames */
        {
          memset(nodeName,0,sizeof(nodeName));
          strncpy(nodeName,llBufP,strcspn(llBufP,":"));
          for(l=0;l<nodeHostNameC;l++)            /* loop over node patterns */
          {
            if(!fnmatch(nodeHostNameV[l],nodeName,0))
            {
              if(compact==2)printf("    %-4d  %s\n",k,llBufP);/* very compact */
              else if(compact==1)                                 /* compact */
              {
                char *line=NULL;
                char *lP=NULL;
                char *bpP=NULL;
                char *epP=NULL;
                char *bP=NULL;
                char *eP=NULL;
                int pN=0;
                line=strdup(llBufP);
                lP=line;
                lP+=(1+strcspn(lP,":"));
                pN=atoi(lP);
                printf("    %s: %d scheduled process(es)\n",nodeName,pN);
                if(pN)
                {
                  lP+=(1+strcspn(lP,":"));
                  for(bpP=lP;;bpP=epP+2)
                  {
                    int ii=0;
                    epP=strstr(bpP,"),");
                    if(!epP)epP=strstr(bpP,").");
                    if(!epP)break;
                    *(epP+1)='\0';
                    bP=bpP;
                    eP=strchr(bP,'(');
                    if(!eP)continue;
                    *eP='\0';
                    printf("      UTGID: %s (",bP);
                    for(bP=eP+1,ii=0;;bP=eP+1,ii++)
                    {
                      eP=strchr(bP,',');
                      if(!eP)eP=strchr(bP,')');
                      if(!eP)break;
                      *eP='\0';
                      if(ii)printf(", ");
                      printf("%s",bP);
                    }
                    printf(")\n");
                  }
                }
                if(line)free(line);
              }                                                   /* compact */
              else                                           /*  not compact */
              {
                char *line=NULL;
                char *lP=NULL;
                char *bpP=NULL;
                char *epP=NULL;
                char *bP=NULL;
                char *eP=NULL;
                int pN=0;
                line=strdup(llBufP);
                lP=line;
                lP+=(1+strcspn(lP,":"));
                pN=atoi(lP);
                printf("    %s: %d scheduled process(es)\n",nodeName,pN);
                if(pN)
                {
                  lP+=(1+strcspn(lP,":"));
                  for(bpP=lP;;bpP=epP+2)
                  {
                    epP=strstr(bpP,"),");
                    if(!epP)epP=strstr(bpP,").");
                    if(!epP)break;
                    *(epP+1)='\0';
                    bP=bpP;
                    eP=strchr(bP,'(');
                    if(!eP)continue;
                    *eP='\0';
                    printf("      UTGID: %s\n",bP);
                    for(bP=eP+1;;bP=eP+1)
                    {
                      eP=strchr(bP,',');
                      if(!eP)eP=strchr(bP,')');
                      if(!eP)break;
                      *eP='\0';
                      printf("        %s\n",bP);
                    }
                  }
                }
                if(line)free(line);
              }                                            /* if not compact */
              m++;
            }
          }
          llBufP+=(1+strlen(llBufP));
          if(llBufP>=llBuf+llBufSz)break;
        }                                       /* loop over found nodeNames */
        if(!m)
        {
          printf("    no nodes found matching at least one of the patterns: ");
          for(l=0;l<nodeHostNameC;l++)            /* loop over node patterns */
          {
            printf("\"%s\"",nodeHostNameV[l]);
            if(l<nodeHostNameC-1)printf(", ");
          }
          printf("!\n");
        }
        j++;
      }                                  /* if(!fnmatch(svcPattern,llSvc,0)) */
    }                                                  /* loop over ctrl PCs */
    if(!cpcFoundN)
    {
      printf("    no Ctrl PC found for pattern: \"%s\"\n",ctrlPcHostNameV[i]);
    }
  }                                            /* loop over ctrl PC patterns */
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
void getServerVersion(int nodeHostNameC,char **nodeHostNameV)
{
  printf("HOSTNAME     FMC      SERVER\n");
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(nodeHostNameC,nodeHostNameV,SVC_HEAD,SRV_NAME,
                               deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(nodeHostNameC,nodeHostNameV,"",SRV_NAME,
                               deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(1);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"pcLl [-v] [-N DIM_DNS_NODE] [-c|-cc] [-C CTRL_PC_PATTERN...]\n"
"     [-m NODE_PATTERN...]\n"
"pcLl -V [-N DIM_DNS_NODE] [-C CTRL_PC_PATTERN...]\n"
"pcLl -h\n"
"\n"
"Try \"pcLl -h\" for more information.\n";
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
"pcLl.man\n"
"\n"
"..\n"
"%s"
".hw no\\[hy]authentication NODE_PATTERN RT_PRIORITY\n"
".TH pcLl 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis pcLl\\ \\-\n"
"Show the processes in the list of the FMC Process Controller with their "
"start parameters\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis pcLl\n"
".ShortOpt[] v\n"
".ShortOpt[] N DIM_DNS_NODE\n"
"[\\fB-c\\fP|\\fB-cc\\fP]\n"
".ShortOpt[] C CTRL_PC_PATTERN...\n"
".ShortOpt[] m NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis pcLl\n"
".ShortOpt V\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] C CTRL_PC_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis pcLl\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH INTRODUCTION\n"
"The FMC \\fBProcess Controller\\fP is a tool in charge of \\fIkeeping a "
"dynamically manageable list of applications up and running\\fP on the farm "
"nodes.\n"
".PP\n"
"The Process Controller Server (\\fBpcSrv\\fP(8)) tipically runs on a few "
"control PCs (CTRL_PC), each one watching up to ~200 farm nodes (NODE), and "
"performs its task by maintaining the \\fIlist of the scheduled "
"applications\\fP for each controlled farm node and by interacting with the "
"FMC Task Manager Servers (\\fBtmSrv\\fP(8)) running on each farm node to "
"start processes, to obtain the notification of the process termination, to "
"re-start the untimely terminated processes (\"respawn\"), and to stop "
"processes.\n"
".PP\n"
"The list of the controlled farm nodes is read by each Process Controller "
"Server (\\fBpcSrv\\fP(8)) from the configuration file \\fICONF_FILE\\fP "
"(by default: \"/etc/pcSrv.conf\").\n"
".PP\n"
"The Process Controller Clients (\\fBpcAdd\\fP(1), \\fBpcRm\\fP(1), "
"\\fBpcLs\\fP(1), \\fBpcLl\\fP(1) and \\fBpcLss\\fP(1)) can contact more than "
"one Process Controller Servers at a time. They can use the option "
"\\fB-C\\fP \\fICTRL_PC_PATTERN\\fP to choose the Control PC and the option "
"\\fB-m\\fP \\fINODE_PATTERN\\fP to choose the worker node.\n"
".PP\n"
"At runtime, processes can be added to the list for one or more nodes by "
"means of the \\fBpcAdd\\fP(1) command and removed from the list for one or "
"more nodes by means of the \\fBpcRm\\fP(1) command. The commands "
"\\fBpcLs\\fP(1), \\fBpcLl\\fP(1) and \\fBpcLss\\fP(1) provide several "
"information about the controlled processes.\n"
".PP\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The command \\fBpcLl\\fP(1) lists the processes - together with their "
"\\fBstart options\\fP, \\fBarguments\\fP and \\fBparameters\\fP - in the "
"Process Controller list for the nodes whose hostname "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP, which "
"are controlled by the control PCs whose hostname matches at least one of "
"the wildcard patterns \\fICTRL_PC_PATTERN\\fP.\n"
".PP\n"
"The DIM Name Server, looked up to seek for Process Controller Servers, can\n"
"be chosen (in decreasing order of priority) by:\n"
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
"Get basic usage help from the command line.\n"
".\n"
".OptDef V \"\"\n"
"For each selected node print the FMC Process Controller Server version and "
"the FMC version, than exit.\n"
".\n"
".OptDef v \"\"\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef c \"\"\n"
"Compact output (one line per process).\n"
".\n"
".OptDef cc \"\"\n"
"Very compact output (one line per node).\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef C \"\" CTRL_PC_PATTERN (string, repeatable)\n"
"Contact the Process Controller Server (\\fBpcSrv\\fP(8)) of the control "
"PCs whose hostname matches the wildcard pattern \\fICTRL_PC_PATTERN\\fP "
"(\\fBdefault\\fP: contact all the control PCs).\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"List the process(es) in the Process Controller list for the nodes "
"(controlled by the control PCs whose hostname matches the wildcard pattern "
"\\fICTRL_PC_PATTERN\\fP) whose hostname matches the wildcard pattern "
"\\fINODE_PATTERN\\fP (\\fBdefault\\fP: list processes from the Process "
"Controller list for all the nodes controlled by the control PCs whose "
"hostname matches the wildcard pattern \\fICTRL_PC_PATTERN\\fP).\n"
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
"List the processes scheduled at all the farm nodes, controlled by all the "
"Control PCs running a Process Controller servers registered with the current "
"DIM DNS:\n"
".PP\n"
".ShellCommand pcLl \n"
".PP\n"
".\n"
"Show the output in compact format:\n"
".PP\n"
".ShellCommand pcLl -c\n"
".PP\n"
".\n"
"Show the output in very compact format:\n"
".PP\n"
".ShellCommand pcLl -cc\n"
".PP\n"
".\n"
"List only the processes scheduled at the farm node \"farm0101\":\n"
".PP\n"
".ShellCommand pcLl -m farm0101\n"
".ShellCommand pcLl -c -m farm0101\n"
".ShellCommand pcLl -cc -m farm0101\n"
".PP\n"
".\n"
"Use a wildcard pattern to select the farm nodes:\n"
".PP\n"
".ShellCommand pcLl -m farm010\\[rs]*\n"
".ShellCommand pcLl -m \\[dq]farm010*\\[dq]\n"
".ShellCommand pcLl -m \\[dq]farm0[2-5]???\\[dq] -m "
"\\[dq]far[a-z,A-Z]0101\\[dq]\n"
".PP\n"
".\n"
"List only the processes scheduled by the Process Controller running at the "
"Control PC \"ctrl01\":\n"
".PP\n"
".ShellCommand pcLl -C ctrl01\n"
".PP\n"
".\n"
"Use a wildcard pattern to select the control PC:\n"
".PP\n"
".ShellCommand pcLl -C ctrl0\\[rs]*\n"
".ShellCommand pcLl -C \\[dq]ctrl0*\\[dq]\n"
".PP\n"
".\n"
"List only the processes scheduled at the farm node \"farm0101\" by the "
"Process Controller running at the Control PC \"ctrl01\":\n"
".PP\n"
".ShellCommand pcLl -C ctrl01 -m farm0101\n"
".PP\n"
".\n"
"Use a wildcard pattern to select the control PC and the farm node:\n"
".PP\n"
".ShellCommand pcLl -C ctrl0\\[rs]* -m farm010\\[rs]*\n"
".ShellCommand pcLl -C \\[dq]ctrl0*\\[dq] -m \\[dq]farm010*\\[dq]\n"
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
".BR \\%%pcAdd (1),\n"
".BR \\%%pcRm (1),\n"
".BR \\%%pcLs (1),\n"
".BR \\%%pcLss (1),\n"
".BR \\%%pcSrv (8).\n"
".br\n"
".BR \\%%tmStart (1),\n"
".BR \\%%tmStop (1),\n"
".BR \\%%tmLs (1),\n"
".BR \\%%tmLl (1),\n"
".BR \\%%tmSrv (8).\n"
".br\n"
".BR \\%%logViewer (1),\n"
".BR \\%%logSrv (8).\n"
".br\n"
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
