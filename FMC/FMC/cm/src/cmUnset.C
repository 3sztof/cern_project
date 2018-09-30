/* ######################################################################### */
/*
 * $Log: cmUnset.C,v $
 * Revision 1.7  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.6  2008/10/16 13:46:50  galli
 * minor changes
 *
 * Revision 1.5  2008/10/15 06:42:38  galli
 * bug fixed
 *
 * Revision 1.2  2008/10/07 22:46:50  galli
 * groff manual
 *
 * Revision 1.1  2008/10/06 13:34:31  galli
 * Initial revision
 */
/* ######################################################################### */
#include <stdio.h>
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>                                              /* PRIu64 */
#include <fnmatch.h>                                            /* fnmatch() */
#include <errno.h>
#include <string.h>                                           /* strerror(3) */
#include <ctype.h>                                              /* toupper() */
#include <termios.h>                           /* tcgetattr(3), tcsetattr(3) */
#include <pwd.h>                           /* uid_t, getpwnam(3) getpwuid(3) */
#include <grp.h>                           /* gid_t, getgrnam(3) getgrgid(3) */
#include <sys/stat.h>                      /* struct stat, lstat(2), open(2) */
#include <fcntl.h>                                      /* O_RDONLY, open(2) */
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>
/*---------------------------------------------------------------------------*/
#include "fmcPar.h"                                    /* DIM_CONF_FILE_NAME */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcGroffMacro.h"                             /* macro for man page */
#include "cmSrv.H"                                 /* SRV_NAME, RPC_TIME_OUT */
#include "CmAcl.IC"
#include "FmcUtils.IC"                               /* printServerVersion() */
/*****************************************************************************/
int deBug=0;
int errU=L_STD;
static char rcsid[]="$Id: cmUnset.C,v 1.7 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
int rpcTimeOut=RPC_TIME_OUT;
/* terminal settings */
static struct termios savedTermios;
static int ttysavefd=-1;
static enum{RESET,RAW,CBREAK}ttystate=RESET;
/* thread arbitration */
bool rpcDone;
pthread_mutex_t rpcMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t rpcCond=PTHREAD_COND_INITIALIZER;
/* ######################################################################### */
void usage(int mode);
void shortUsage(void);
void getServerVersion(int nodeHostNameC,char **nodeHostNameV);
int ttyRaw(int fd);                          /* put terminal into a raw mode */
int ttyReset(int fd);                             /* restore terminal's mode */
static void signalHandler(int signo);
/* ######################################################################### */
class ConfFileEraser:public DimRpcInfo
{
  /***************************************************************************/
  public:
  /***************************************************************************/
  void rpcInfoHandler()
  {
    char *rv=getString();
    /*.......................................................................*/
    printf("%s\n",rv);
    /*.......................................................................*/
    /* inform the main thread that we have done */
    rpcDone=true;
    pthread_cond_broadcast(&rpcCond);
    /*.......................................................................*/
  }
  /***************************************************************************/
  ConfFileEraser(char *name, char *noLink)
  :DimRpcInfo(name, rpcTimeOut, noLink)
  {
  };
  /***************************************************************************/
};
/* ######################################################################### */
int main(int argc,char **argv)
{
  /* counters */
  int i=0,j=0;
  /* pointers */
  char *p;
  /* getopt */
  int flag;
  /* dim DNS */
  char *dimDnsNode=NULL;
  /* service browsing */
  int nodeHostNameC=0;
  char **nodeHostNameV=NULL;
  string nodePttn;
  string successRawSvcPttn;
  string successSvcPttn;
  DimBrowser br;
  int type=0;
  char *format=NULL;
  string successSvc;
  size_t b,e;
  int totNodeFoundN=0;
  int pttnNodeFoundN=0;
  string nodeFound;
  string unsetRpc;
  char *noLink;
  /* command line switch */
  char *fileName=NULL;
  int doPrintServerVersion=0;
  int doForce=0;
  /* output buffer */
  char *oBuf;
  int oBufSz;
  char c;
  /* authentication */
  CmAcl acl;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  nodeHostNameC=0;
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt(argc,argv,"h::Vm:vN:p:t:f"))!=EOF)
  {
    switch(flag)
    {
      case 'm':
        nodeHostNameC++;
        nodeHostNameV=(char**)realloc(nodeHostNameV,nodeHostNameC*
                                      sizeof(char*));
        nodeHostNameV[nodeHostNameC-1]=optarg;
        break;
      case 'V':
        doPrintServerVersion=1;
        break;
      case 'v':
        deBug++;
        break;
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'p':
        fileName=optarg;
        break;
      case 'f':
        doForce=1;
        break;
      case 't':
        rpcTimeOut=(int)strtol(optarg,(char**)NULL,0);
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
  if(!nodeHostNameC)
  {
    nodeHostNameC=1;
    nodeHostNameV=(char**)realloc(nodeHostNameV,sizeof(char*));
    nodeHostNameV[nodeHostNameC-1]=(char*)"*";
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
    printf("[INFO] DIM_DNS_NODE: \"%s\" (from -N command-line option).\n",
           dimDnsNode);
  }
  else
  {
    dimDnsNode=getenv("DIM_DNS_NODE");
    if(dimDnsNode)
    {
      printf("[INFO] DIM_DNS_NODE: \"%s\" (from DIM_DNS_NODE environment "
             "variable).\n",dimDnsNode);
    }
    else
    {
      dimDnsNode=getDimDnsNode(dimConfFile,0,1);
      if(dimDnsNode)
      {
        printf("[INFO] DIM_DNS_NODE: \"%s\" (from \""DIM_CONF_FILE_NAME" "
               "file).\n",dimDnsNode);
      }
      else
      {
        fprintf(stderr,"[FATAL]: DIM Name Server (DIM_DNS_NODE) not defined!\n"
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
    fprintf(stderr,"[FATAL] setenv(): %s!\n",strerror(errno));
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  if(doPrintServerVersion)getServerVersion(nodeHostNameC,nodeHostNameV);
  /*-------------------------------------------------------------------------*/
  /* check command line */
  if(!fileName)
  {
    fprintf(stderr,"\nOption \"-p FILE_NAME\" must be specified in the"
            "cmd-line!\n");
    shortUsage();
  }
  /*-------------------------------------------------------------------------*/
  printf("[INFO] Node patterns:  ");
  for(i=0;i<nodeHostNameC;i++)                    /* loop over node patterns */
  {
    printf("\"%s\"",nodeHostNameV[i]);
    if(i<nodeHostNameC-1)printf(", ");
  }
  printf(".\n");
  /*-------------------------------------------------------------------------*/
  /* set the data to be returned if RPC service is not available */
  noLink=(char*)"[ERROR] RPC service not available!";
  /*-------------------------------------------------------------------------*/
  /* print the selected nodes and ask confirm */
  if(!doForce)
  {
    printf("Going to remove the file \"%s\" from the following nodes:\n",
           fileName);
    /*-----------------------------------------------------------------------*/
    /* print the selected nodes */
    totNodeFoundN=0;
    for(i=0,j=0;i<nodeHostNameC;i++)              /* loop over node patterns */
    {
      nodePttn=nodeHostNameV[i];
      nodePttn=FmcUtils::toUpper(nodePttn);
      successRawSvcPttn=string(SVC_HEAD)+"/*/"+SRV_NAME+"/success";
      successSvcPttn=string(SVC_HEAD)+"/"+nodePttn+"/"+SRV_NAME+"/success";
      if(deBug)printf("  Service pattern: \"%s\".\n",successSvcPttn.c_str());
      /*.....................................................................*/
      br.getServices(successRawSvcPttn.c_str());
      while((type=br.getNextService(p,format))!=0)        /* loop over nodes */
      {
        if(!fnmatch(successSvcPttn.c_str(),p,0))  /* if node matches pattern */
        {
          totNodeFoundN++;
          successSvc=p;
          /*.................................................................*/
          /* extract node hostname from "success" service name */
          e=successSvc.find_last_of('/');
          e=successSvc.find_last_of('/',e-1);
          b=successSvc.find_last_of('/',e-1)+1;
          nodeFound=successSvc.substr(b,e-b);
          nodeFound=FmcUtils::toLower(nodeFound);
          /*.................................................................*/
          printf(" %4d: %s\n",totNodeFoundN,nodeFound.c_str());
          /*.................................................................*/
        }                                         /* if node matches pattern */
      }                                                   /* loop over nodes */
    }                                             /* loop over node patterns */
    if(!totNodeFoundN)
    {
      printf("[FATAL] No node found matching node pattern(s):  ");
      for(i=0;i<nodeHostNameC;i++)                /* loop over node patterns */
      {
        printf("\"%s\"",nodeHostNameV[i]);
        if(i<nodeHostNameC-1)printf(", ");
      }
      printf("! Terminating...\n");
      return 1;
    }
    /*-----------------------------------------------------------------------*/
    /* get the confirm [yn] */
    signal(SIGINT,signalHandler);
    signal(SIGQUIT,signalHandler);
    signal(SIGTERM,signalHandler);
    for(;;)
    {
      printf("Sure you want to remove the file \"%s\" from the nodes listed "
             "above [yn]? ",fileName);
      fflush(stdout);
      ttyRaw(STDIN_FILENO);
      read(STDIN_FILENO,&c,1);
      ttyReset(STDIN_FILENO);
      c&=0xff;
      if(c=='y'||c=='n')break;
      printf("%c\n",c);
    }
    printf("%c\n",c);
    signal(SIGINT,SIG_DFL);
    signal(SIGQUIT,SIG_DFL);
    signal(SIGTERM,SIG_DFL);
    if(c=='n')
    {
      printf("[WARN] Remove operation cancelled on user request! "
             "Terminating...\n");
      return 1;
    }
    printf("[INFO] Remove operation confirmed by user.\n");
  }                                                          /* if(!doForce) */
  /*-------------------------------------------------------------------------*/
  /* set the output buffer for the RPC */
  string authString=acl.getAuthString();
  oBufSz=authString.size()+1+strlen(fileName)+1;
  oBuf=(char*)malloc(oBufSz);
  memset(oBuf,0,oBufSz);
  snprintf(oBuf,authString.size()+1+strlen(fileName)+1,"%s %s",
           authString.c_str(), fileName);
  /*-------------------------------------------------------------------------*/
  totNodeFoundN=0;
  for(i=0,j=0;i<nodeHostNameC;i++)                /* loop over node patterns */
  {
    printf("[INFO] Processing node pattern: \"%s\".\n",nodeHostNameV[i]);
    nodePttn=nodeHostNameV[i];
    nodePttn=FmcUtils::toUpper(nodePttn);
    successRawSvcPttn=string(SVC_HEAD)+"/*/"+SRV_NAME+"/success";
    successSvcPttn=string(SVC_HEAD)+"/"+nodePttn+"/"+SRV_NAME+"/success";
    if(deBug)printf("  Service pattern: \"%s\".\n",successSvcPttn.c_str());
    /*.......................................................................*/
    br.getServices(successRawSvcPttn.c_str());
    pttnNodeFoundN=0;
    while((type=br.getNextService(p,format))!=0)          /* loop over nodes */
    {
      pthread_mutex_lock(&rpcMutex);
      rpcDone=false;
      pthread_mutex_unlock(&rpcMutex);
      if(!fnmatch(successSvcPttn.c_str(),p,0))    /* if node matches pattern */
      {
        pttnNodeFoundN++;
        totNodeFoundN++;
        successSvc=p;
        /*...................................................................*/
        /* extract node hostname from "success" service name */
        e=successSvc.find_last_of('/');
        e=successSvc.find_last_of('/',e-1);
        b=successSvc.find_last_of('/',e-1)+1;
        nodeFound=successSvc.substr(b,e-b);
        nodeFound=FmcUtils::toLower(nodeFound);
        /*...................................................................*/
        /* invoke the "unset" RPC */
        unsetRpc=string(SVC_HEAD)+"/"+FmcUtils::toUpper(nodeFound)+"/"+
                 SRV_NAME+"/unset";
        if(deBug)printf("  %3d Contacting Configuration Manager RPC: \"%s\""
                        "...\n",j,unsetRpc.c_str());
        printf("[INFO] Processing node %d: \"%s\".\n",totNodeFoundN,
               nodeFound.c_str());
        ConfFileEraser confFileEraser((char*)unsetRpc.c_str(),noLink);
        confFileEraser.setData(oBuf,oBufSz);
        /*...................................................................*/
        /* wait until ConfFileReader::rpcInfoHandler() has done */
        pthread_mutex_lock(&rpcMutex);
        while(!rpcDone)pthread_cond_wait(&rpcCond,&rpcMutex);
        pthread_mutex_unlock(&rpcMutex);
        /*...................................................................*/
      }                                           /* if node matches pattern */
    }                                                     /* loop over nodes */
    if(!pttnNodeFoundN)
    {
      printf("[WARN] No node found for pattern \"%s\"!\n",nodeHostNameV[i]);
    }
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  if(!totNodeFoundN)
  {
    printf("[FATAL] No node found matching node pattern(s):  ");
    for(i=0;i<nodeHostNameC;i++)                  /* loop over node patterns */
    {
      printf("\"%s\"",nodeHostNameV[i]);
      if(i<nodeHostNameC-1)printf(", ");
    }
    printf("! Terminating...\n");
    return 1;
  }
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
int ttyRaw(int fd)                          /* put terminal into a raw mode */
{
  struct termios  buf;
  if(tcgetattr(fd,&savedTermios)<0)return -1;
  buf=savedTermios;                                         /* save settings */
  /* echo off, canonical mode off, extended input processing off */
  buf.c_lflag&=~(ECHO|ICANON|IEXTEN);
  /* no SIGINT on BREAK, CR-to-NL off, input parity check off */
  /* don't strip 8th bit on input, output flow control off */
  buf.c_iflag&=~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
  /* clear size bits, parity checking off */
  buf.c_cflag&=~(CSIZE|PARENB);
  /* set 8 bits/char */
  buf.c_cflag|=CS8;
  /* output processing off */
  buf.c_oflag&=~(OPOST);
  /* 1 byte at a time, no timer */
  buf.c_cc[VMIN]=1;
  buf.c_cc[VTIME]=0;
  if(tcsetattr(fd,TCSAFLUSH,&buf)<0)return -1;
  ttystate=RAW;
  ttysavefd=fd;
  return 0;
}
/*****************************************************************************/
int ttyReset(int fd)                              /* restore terminal's mode */
{
  if (ttystate!=CBREAK && ttystate!=RAW)return 0;
  if(tcsetattr(fd,TCSAFLUSH,&savedTermios)<0)return -1;
  ttystate=RESET;
  return 0;
}
/*****************************************************************************/
static void signalHandler(int signo)
{
  ttyReset(STDIN_FILENO);
  printf("\nSignal %d (%s) received. Operation cancelled. Exiting...\n",signo,
         strsignal(signo));
  _exit(0);
}
/*****************************************************************************/
void getServerVersion(int nodeHostNameC,char **nodeHostNameV)
{
  printf("HOSTNAME     FMC      SERVER\n");
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/process_controller/success */
  FmcUtils::printServerVersion(nodeHostNameC,nodeHostNameV,SVC_HEAD,SRV_NAME,
                               deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/process_controller/success */
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
"cmUnset [-v...] [-N DIM_DNS_NODE] [-m NODE_PATTERN...] [-f]\n"
"        [-t RPC_TIME_OUT] -p FILE_PATH\n"
"cmUnset -V [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"cmUnset -h\n"
"Try \"cmUnset -h\" for more information.\n";
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
"cmUnset.man\n"
"\n"
"..\n"
"%s"
".hw NODE_PATTERN \n"
".TH cmUnset 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis cmUnset\\ \\-\n"
"Remove a file from one or more farm nodes through the FMC Configuration "
"Manager\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis cmUnset\n"
".ShortOpt[] v...\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] f\n"
".ShortOpt[] t RPC_TIME_OUT\n"
"\\fB-p\\fP \\fIFILE_PATH\\fP\n"
".EndSynopsis\n"
".sp\n"
".Synopsis cmUnset\n"
".ShortOpt V\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN\n"
".EndSynopsis\n"
".sp\n"
".Synopsis cmUnset\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH INTRODUCTION\n"
".\n"
"The FMC \\fBConfiguration Manager\\fP is a tool to \\fBread\\fP, "
"\\fBwrite\\fP and \\fBremove configuration files\\fP (more generally, "
"ASCII files) to/from the farm nodes.\n"
".PP\n"
"The Process Controller Server, \\fBcmSrv\\fP(8), typically runs on each farm "
"node and allows to access the files on the node it is running on.\n"
".PP\n"
"The Process Controller Clients \\fBcmSet\\fP(1), \\fBcmUnset\\fP(1) and "
"\\fBcmGet\\fP(1) can contact one or more Process Controller Servers "
"running on remote nodes to write files, remove files and read files "
"respectively.\n"
".PP\n"
"File access can be \\fBrestricted\\fP in 2 different ways simultaneously: "
"(a) by means of a username/hostname access control and (b) by means of a "
"filename access control. See \\fBcmSrv\\fP(8) for more information.\n"
".PP\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"The command \\fBcmUnset\\fP(1) \\fBremoves\\fP the file at the absolute path "
"\\fIFILE_PATH\\fP from all the farm nodes whose hostname "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP.\n"
".PP\n"
".PP\n"
"Unless the option \\fB-f\\fP (force) is specified, before proceeding "
"removing the files, the list of the nodes matching the wildcard patterns "
"\\fINODE_PATTERN\\fP is shown and a \\fBconfirmation\\fP is asked.\n"
".PP\n"
"The DIM Name Server, looked up to seek for Configuration Managers Servers, "
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
"Operate only with the nodes whose hostname matches at least one of the "
"wildcard patterns \\fINODE_PATTERN\\fP (\\fBdefault\\fP: operate with all "
"the nodes).\n"
".\n"
".OptDef V \"\"\n"
"For each selected node print the FMC Configuration Manager Server version "
"and the FMC version, than terminate.\n"
".\n"
".OptDef p \"\" FILE_PATH (string, mandatory)\n"
"Remove the files at path \\fIFILE_PATH\\fP from the selected nodes.\n"
".\n"
".OptDef f \"\"\n"
"Do not ask for a confirmation before proceeding removing the files. "
"\\fBDefault\\fP before proceeding removing the files, the list of the nodes "
"matching the wildcard patterns \\fINODE_PATTERN\\fP is shown and a "
"confirmation is asked.\n"
".\n"
".OptDef t \"\" RPC_TIME_OUT (integer)\n"
"Set the timeout for the Remote Procedure Call to \\fIRPC_TIME_OUT\\fP "
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
"Remove the file \"/tmp/test.conf\" from all the farm nodes registered with "
"the current DIM name server:\n"
".PP\n"
".ShellCommand cmUnset -p /tmp/test.conf\n"
".PP\n"
".\n"
"Remove the file \"/tmp/test.conf\" only from the farm node \"farm0101\":\n"
".PP\n"
".ShellCommand cmUnset -m farm0101 -p /tmp/test.conf\n"
".PP\n"
".\n"
"Remove the file \"/tmp/test.conf\" from the farm nodes \"farm0101\" and "
"\"farm0102\":\n"
".PP\n"
".ShellCommand cmUnset -m farm0101 -m farm0102 -p /tmp/test.conf\n"
".PP\n"
".\n"
"Use wildcard patterns to select the nodes:\n"
".PP\n"
".ShellCommand cmUnset -m \\[dq]farm0[0-5]*\\[dq] -m \\[dq]farm07[135][0-5]"
"\\[dq] -p /tmp/test.conf\n"
".PP\n"
".\n"
"Do not ask for confirmation:\n"
".PP\n"
".ShellCommand cmUnset -f -m farm0101 -m farm0102 -p /tmp/test.conf\n"
".ShellCommand cmUnset -f -m \\[dq]farm0[0-5]*\\[dq] -m \\[dq]farm07[135][0-5]"
"\\[dq]\\: -p /tmp/test.conf\n"
".PP\n"
".\n"
"Check the version of the Configuration Manager Servers installed on the farm "
"nodes:\n"
".PP\n"
".ShellCommand cmUnset -V\n"
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
".BR \\%%cmSet (1),\n"
".BR \\%%cmGet (1),\n"
".BR \\%%cmSrv (8),\n"
".br\n"
".BR \\%%tmSrv (8),\n"
".BR \\%%ipmiSrv (8),\n"
".BR \\%%pcSrv (8).\n"
".br\n"
".BR \\%%glob (7).\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro,FMC_DATE,FMC_VERSION
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/* ######################################################################### */
