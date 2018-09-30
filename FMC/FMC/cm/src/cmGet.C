/* ######################################################################### */
/*
 * $Log: cmGet.C,v $
 * Revision 1.12  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.11  2008/10/16 13:49:50  galli
 * minor changes
 *
 * Revision 1.10  2008/10/15 06:42:38  galli
 * bug fixed
 *
 * Revision 1.8  2008/10/08 10:55:46  galli
 * groff manual
 *
 * Revision 1.6  2008/10/06 07:30:15  galli
 * Improved service browsing
 *
 * Revision 1.4  2008/10/03 14:38:58  galli
 * manage also protections, UID and GID
 *
 * Revision 1.2  2008/07/10 14:16:56  galli
 * usage() updated
 *
 * Revision 1.1  2008/07/10 13:57:48  galli
 * Initial revision
 */
/* ######################################################################### */
#include <string>
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>                                              /* PRIu64 */
#include <ctype.h>                                              /* toupper() */
#include <fnmatch.h>                                            /* fnmatch() */
#include <errno.h>
#include <pthread.h>
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
static char rcsid[]="$Id: cmGet.C,v 1.12 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
int printContent=1;
char *outDir=NULL;
int rpcTimeOut=RPC_TIME_OUT;
char sepLine[80];
/* thread arbitration */
bool rpcDone;
pthread_mutex_t rpcMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t rpcCond=PTHREAD_COND_INITIALIZER;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
void getServerVersion(int nodeHostNameC,char **nodeHostNameV);
/* ######################################################################### */
class ConfFileReader:public DimRpcInfo
{
  /***************************************************************************/
  string node;
  string fileName;
  /***************************************************************************/
  public:
  /***************************************************************************/
  void rpcInfoHandler()
  {
    /*.......................................................................*/
    char *out;
    struct tm fTimeL;
    char fTimeS[21];
    out=(char*)getData();
    uint64_t fTime=*((uint64_t*)out);
    uint64_t fSize=*((uint64_t*)(out+8));
    uint64_t fMode=*((uint64_t*)(out+16));
    uint64_t fUid=*((uint64_t*)(out+24));
    uint64_t fGid=*((uint64_t*)(out+32));
    char *outStr=out+5*sizeof(uint64_t);
    /*.......................................................................*/
    /* time conversion */
    time_t fTimeT=(time_t)fTime;
    localtime_r(&fTimeT,&fTimeL);
    strftime(fTimeS,21,"%Y %b %d %H:%M:%S",&fTimeL);
    /*.......................................................................*/
    /* printout */
    if(!strncmp(outStr,"[ERROR]",7))
    {
      printf("      %s\n",outStr);
    }
    else if(!strncmp(outStr,"[WARN]",6))
    {
      printf("      Date:        %s",fTimeS);
      printf("    Size: %"PRIu64" B\n",fSize);
      printf("      Permissions: %4.4"PRIo64,fMode&07777);
      printf("                    UID:  %6"PRIu64,fUid);
      printf("      GID:  %6"PRIu64"\n",fGid);
      printf("      %s\n",outStr);
    }
    else
    {
      printf("      Date:        %s",fTimeS);
      printf("    Size: %"PRIu64" B\n",fSize);
      printf("      Permissions: %4.4"PRIo64,fMode&07777);
      printf("                    UID:  %6"PRIu64,fUid);
      printf("      GID:  %6"PRIu64"\n",fGid);
      if(printContent && !outDir)
      {
        printf("      Content of file:\n");
        printf("%s\n",sepLine);
        printf("%s",outStr);
        printf("%s\n",sepLine);
      }
      else if(outDir)
      {
        string outFilePath;
        FILE *outFileFP;
        size_t i;
        if(fileName.at(0)=='/')fileName=fileName.substr(1);
        outFilePath=node+"."+fileName;
        for(i=0;i<outFilePath.size();i++)
        {
          if(outFilePath.at(i)=='/')outFilePath.at(i)='_';
        }
        if(outDir[strlen(outDir)-1]=='/')
        {
          outFilePath=string(outDir)+outFilePath;
        }
        else
        {
          outFilePath=string(outDir)+"/"+outFilePath;
        }
        outFileFP=fopen(outFilePath.c_str(),"w");
        if(!outFileFP)
        {
          fprintf(stderr,"[FATAL] Can't open file \"%s\" for writing. "
                  "fopen(3) error: %s!\n",outFilePath.c_str(),strerror(errno));
          exit(1);
        }
        fprintf(outFileFP,"%s",outStr);
        fclose(outFileFP);
        printf("[INFO] File: \"%s\" written.\n",outFilePath.c_str());
      }
    }
    /*.......................................................................*/
    /* inform the main thread that we have done */
    rpcDone=true;
    pthread_cond_broadcast(&rpcCond);
    /*.......................................................................*/
  }
  /***************************************************************************/
  ConfFileReader(char *name, string node, char *fileName, char *noLink,
                 int noLinkSz)
  :DimRpcInfo(name, rpcTimeOut, noLink, noLinkSz)
  {
    this->node=node;
    this->fileName=string(fileName);
  };
  /***************************************************************************/
};
/* ######################################################################### */
int main(int argc,char **argv)
{
  /* counters */
  int i=0,j=0;
  /* pointers */
  char *p=NULL;
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
  int nodeFoundN=0;
  string nodeFound;
  string getRpc;
  char *noLink;
  int noLinkSz;
  /* authentication */
  CmAcl acl;
  string rpcArgs;
  /* command line switch */
  char *fileName=NULL;
  int doPrintServerVersion=0;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* set separator line */
  memset(&sepLine,'-',79);
  sepLine[79]='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  nodeHostNameC=0;
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt(argc,argv,"h::Vm:vN:qp:t:o:"))!=EOF)
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
      case 'p':
        fileName=optarg;
        break;
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'o':
        outDir=optarg;
        break;
      case 'q':
        printContent=0;
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
  if(doPrintServerVersion)getServerVersion(nodeHostNameC,nodeHostNameV);
  /*-------------------------------------------------------------------------*/
  /* check command line */
  if(!fileName)
  {
    fprintf(stderr,"\nOption \"-p FILE_NAME\" must be specified in the "
            "cmd-line!\n");
    shortUsage();
  }
  /*-------------------------------------------------------------------------*/
  if(outDir)
  {
    struct stat outDirStat;
    if(access(outDir,W_OK)==-1)
    {
      fprintf(stderr,"[FATAL] Can't access directory \"%s\" for writing. "
              "access(2) error: %s!\n",outDir,strerror(errno));
      return 1;
    }
    if(lstat(outDir,&outDirStat)<0)
    {
      fprintf(stderr,"[FATAL] Can't get info about output directory \"%s\". "
              "lstat(2) error: %s!\n",outDir,strerror(errno));
      return 1;
    }
    if(!S_ISDIR(outDirStat.st_mode))
    {
      fprintf(stderr,"[FATAL] \"%s\" is not a directory!\n",outDir);
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  printf("Node patterns:  ");
  for(i=0;i<nodeHostNameC;i++)                    /* loop over node patterns */
  {
    printf("\"%s\"",nodeHostNameV[i]);
    if(i<nodeHostNameC-1)printf(", ");
  }
  printf(".\n");
  /*-------------------------------------------------------------------------*/
  /* set the data to be returned if RPC service is not available */
  const char *noLinkStr="[ERROR] RPC service not available!";
  noLinkSz=5*sizeof(uint64_t)+strlen(noLinkStr)+1;
  noLink=(char*)malloc(noLinkSz);
  memset(noLink,0,noLinkSz);
  strcpy(noLink+5*sizeof(uint64_t),noLinkStr);
  /*-------------------------------------------------------------------------*/
  for(i=0,j=0;i<nodeHostNameC;i++)                /* loop over node patterns */
  {
    printf("  Node pattern: \"%s\":\n",nodeHostNameV[i]);
    nodePttn=nodeHostNameV[i];
    nodePttn=FmcUtils::toUpper(nodePttn);
    successRawSvcPttn=string(SVC_HEAD)+"/*/"+SRV_NAME+"/success";
    successSvcPttn=string(SVC_HEAD)+"/"+nodePttn+"/"+SRV_NAME+"/success";
    if(deBug)printf("  Service pattern: \"%s\".\n",successSvcPttn.c_str());
    /*.......................................................................*/
    br.getServices(successRawSvcPttn.c_str());
    nodeFoundN=0;
    while((type=br.getNextService(p,format))!=0)          /* loop over nodes */
    {
      pthread_mutex_lock(&rpcMutex);
      rpcDone=false;
      pthread_mutex_unlock(&rpcMutex);
      if(!fnmatch(successSvcPttn.c_str(),p,0))    /* if node matches pattern */
      {
        nodeFoundN++;
        successSvc=p;
        /*...................................................................*/
        /* extract node hostname from "success" service name */
        e=successSvc.find_last_of('/');
        e=successSvc.find_last_of('/',e-1);
        b=successSvc.find_last_of('/',e-1)+1;
        nodeFound=successSvc.substr(b,e-b);
        nodeFound=FmcUtils::toLower(nodeFound);
        /*...................................................................*/
        /* invoke the "get" RPC */
        getRpc=string(SVC_HEAD)+"/"+FmcUtils::toUpper(nodeFound)+"/"+SRV_NAME+
               "/get";
        if(deBug)printf("  %3d Contacting Configuration Manager RPC: \"%s\""
                        "...\n",j,getRpc.c_str());
        printf("    Node: \"%s\":\n",nodeFound.c_str());
        rpcArgs=acl.getAuthString()+" "+fileName;
        ConfFileReader confFileReader((char*)getRpc.c_str(),nodeFound,fileName,
                                      noLink,noLinkSz);
        confFileReader.setData((void*)rpcArgs.c_str(),1+rpcArgs.size());
        /*...................................................................*/
        /* wait until ConfFileReader::rpcInfoHandler() has done */
        pthread_mutex_lock(&rpcMutex);
        while(!rpcDone)pthread_cond_wait(&rpcCond,&rpcMutex);
        pthread_mutex_unlock(&rpcMutex);
        /*...................................................................*/
        j++;
        /*...................................................................*/
      }                                           /* if node matches pattern */
    }                                                     /* loop over nodes */
    if(!nodeFoundN)
    {
      printf("    no node found for pattern: \"%s\"!\n",nodeHostNameV[i]);
    }
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  return 0;
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
"cmGet [-v...] [-N DIM_DNS_NODE] [-m NODE_PATTERN...] [-q]\n"
"      [-o OUT_DIR] [-t RPC_TIME_OUT] -p FILE_NAME\n"
"cmGet -V [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"cmGet -h\n"
"\n"
"Try \"cmGet -h\" for more information.\n";
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
"cmGet.man\n"
"\n"
"..\n"
"%s"
".hw NODE_PATTERN \n"
".TH cmGet 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis cmGet\\ \\-\n"
"Read a file from one or more farm nodes through the FMC Configuration "
"Manager\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis cmGet\n"
".ShortOpt[] v...\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] q\n"
".ShortOpt[] o OUT_DIR\n"
".ShortOpt[] t RPC_TIME_OUT\n"
"\\fB-p\\fP \\fIFILE_PATH\\fP\n"
".EndSynopsis\n"
".sp\n"
".Synopsis cmGet\n"
".ShortOpt V\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN\n"
".EndSynopsis\n"
".sp\n"
".Synopsis cmGet\n"
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
"The command \\fBcmGet\\fP(1) \\fBread\\fP the files at the absolute path "
"\\fIFILE_PATH\\fP from all the farm nodes whose hostname matches at least "
"one of the wildcard patterns \\fINODE_PATTERN\\fP and print their content "
"either to the standard output stream (terminal), if the option \\fB-o\\fP "
"\\fIOUT_DIR\\fP is omitted, or to a file in the directory \\fIOUT_DIR\\fP, "
"if the option \\fB-o\\fP \\fIOUT_DIR\\fP is specified.\n"
".PP\n"
"Together with the file content, the command \\fBcmGet\\fP(1) always print to "
"the standard output stream also: (a) the date and time of the last "
"modification of the file; (b) the size of the file (in Bytes); (c) the file "
"permission (in octal notation); (d) the UID (User Identifier of the file "
"owner); (e) the GID (Group Identifier of the file owner).\n"
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
"Read the files at path \\fIFILE_PATH\\fP from the selected nodes.\n"
".\n"
".OptDef o \"\" OUT_DIR (string)\n"
"Write the content of the remote files to local files in the directory "
"\\fIOUT_DIR\\fP. Local file are named \\fINODE.FILE_PATH\\fP where the slash "
"characters in \\fIFILE_PATH\\fP are substituted by underscore characters.\n"
".\n"
".OptDef q \"\"\n"
"Quiet mode. Do not print the file content. Print only the date and time of "
"the last modification of the file, the size of the file (in Bytes), the file "
"permission (in octal notation), the UID (User Identifier of the file owner), "
"and the GID (Group Identifier of the file owner) and possible error "
"messages.\n"
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
"Read the remote files \"/tmp/test.conf\" at all the farm nodes registered "
"with the current DIM name server and print their contents to the current "
"terminal (together with the date and time of the last modification, the "
"size, the permission (in octal notation) and the UID and the GID of the "
"owner):\n"
".PP\n"
".ShellCommand cmGet -p /tmp/test.conf\n"
".PP\n"
".\n"
"Read the remote files \"/tmp/test.conf\" at all the farm nodes registered "
"with the current DIM name server and print to the current terminal only the "
"date and time of the last modification, the size, the permission (in octal "
"notation) and the UID and the GID of the owner:\n"
".PP\n"
".ShellCommand cmGet -q -p /tmp/test.conf\n"
".PP\n"
".\n"
"Read the remote files \"/tmp/test.conf\" at all the farm nodes registered "
"with the current DIM name server and print their contents to the local files "
"\\fIhostname\\fP.tmp_test.conf in the current directory; the date and time "
"of the last modification, the size, the permission (in octal notation) and "
"the UID and the GID of the owner are still printed to the current terminal:\n"
".PP\n"
".ShellCommand cmGet -p /tmp/test.conf -o .\n"
".PP\n"
".\n"
"Read the remote file \"/tmp/test.conf\" only at the farm node farm0101 "
"and print its content to the current terminal (together with date, time, "
"size, etc.):\n"
".PP\n"
".ShellCommand cmGet -m farm0101 -p /tmp/test.conf\n"
".PP\n"
".\n"
"Use a wildcard pattern to select the nodes:\n"
".PP\n"
".ShellCommand cmGet -m \\[dq]farm0[0-5]*\\[dq] -m "
"\\[dq]farm07[135][0-5]\\[dq] -p /tmp/test.conf\n"
".ShellCommand cmGet -m \\[dq]farm07[135][0-5]\\[dq] -o . -p /tmp/test.conf\n"
".ShellCommand cmGet -m \\[dq]farm07[135][0-5]\\[dq] -o /tmp -p "
"/tmp/test.conf\n"
".PP\n"
".\n"
"Check the version of the Configuration Manager Servers installed on the farm "
"nodes:\n"
".PP\n"
".ShellCommand cmGet -V\n"
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
".BR \\%%cmUnset (1),\n"
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
