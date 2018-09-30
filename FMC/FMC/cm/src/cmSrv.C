/* ######################################################################### */
/* cmSrv.C - Configuration Manager Server                                    */
/* ######################################################################### */
/*
 * $Log: cmSrv.C,v $
 * Revision 1.32  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.31  2009/10/03 15:20:43  galli
 * Signal block moved before the first DIM call to comply with DIMv19r4
 *
 * Revision 1.28  2008/10/09 09:34:29  galli
 * sig2msg() moved to fmcSigUtils.c
 *
 * Revision 1.24  2008/10/07 10:41:10  galli
 * groff manual
 *
 * Revision 1.20  2008/10/02 08:26:59  galli
 * several char* substituted with string
 *
 * Revision 1.19  2008/10/02 07:42:34  galli
 * class definitions moved to different files
 * use global CmAcl acl instead of int unSafeMode and char *allowedFiles[]
 *
 * Revision 1.17  2008/07/10 14:26:51  galli
 * improved error check in class GetFileRpc
 *
 * Revision 1.16  2008/07/09 14:57:37  galli
 * rewritten from scratch
 *
 * Revision 1.11  2007/10/24 09:07:36  galli
 * publishes service fmc_version
 *
 * Revision 1.9  2007/10/24 07:35:31  galli
 * usage() prints FMC version
 *
 * Revision 1.8  2007/08/14 13:29:32  galli
 * compatible with libFMCutils v 2
 * usage() changed
 *
 * Revision 1.7  2006/10/20 13:53:47  galli
 * compatible with libFMCutils v 1.0
 *
 * Revision 1.6  2006/02/09 12:26:21  galli
 * x86_64 compatibility problems fixed
 *
 * Revision 1.4  2006/01/04 13:23:04  galli
 * bug fixed
 *
 * Revision 1.3  2006/01/04 12:31:45  galli
 * added services ipmi/isSet and pc/isSet
 *
 * Revision 1.2  2006/01/04 09:00:04  galli
 * working version
 *
 * Revision 1.1  2006/01/04 08:45:21  galli
 * Initial revision
 */
/* ######################################################################### */
/* server, services, RPCs                                                    */
/*                                                                           */
/* SRV: /FMC/<HOSTNAME>/<SRV_NAME>                                           */
/*                                                                           */
/* SVC: /FMC/<HOSTNAME>/<SRV_NAME>/server_version                            */
/* SVC: /FMC/<HOSTNAME>/<SRV_NAME>/fmc_version                               */
/* SVC: /FMC/<HOSTNAME>/<SRV_NAME>/success                                   */
/*                                                                           */
/* RPC: /FMC/<HOSTNAME>/<SRV_NAME>/get                                       */
/* RPC: /FMC/<HOSTNAME>/<SRV_NAME>/set                                       */
/* RPC: /FMC/<HOSTNAME>/<SRV_NAME>/unset                                     */
/* ######################################################################### */
/* C headers */
#include <stdio.h>
#include <errno.h>
#include <string.h>                                           /* strerror(3) */
#include <getopt.h>                                         /* getopt_long() */
#include <sys/wait.h>         /* waitpid(), WIFEXITED(), WEXITSTATUS(), etc. */
/*---------------------------------------------------------------------------*/
/* DIM header */
#include <dis.hxx>
/*---------------------------------------------------------------------------*/
/* FMC headers */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcMacro.h"                                             /* eExit() */
#include "fmcSrvNameUtils.h"                               /* getSrvPrefix() */
#include "fmcMsgUtils.h"           /* mPrintf(), dfltLoggerOpen(), rPrintf() */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "fmcVersion.h"                                       /* FMC version */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcGroffMacro.h"                             /* macro for man page */
#include "fmcSigUtils.h"                                        /* sig2msg() */
/*---------------------------------------------------------------------------*/
/* hardcoded parameters */
#include "cmSrv.H"
/*---------------------------------------------------------------------------*/
/* cmSrv classes */
#include "CmSrvDimErrorHandler.IC"
#include "CmSrvGetFileRpc.IC"
#include "CmSrvSetFileRpc.IC"
#include "CmSrvUnsetFileRpc.IC"
#include "CmAcl.IC"
/* ######################################################################### */
/* functions prototype */
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
static void signalHandler(int signo);
/* ######################################################################### */
/* globals */
/*****************************************************************************/
/* configuration files */
string confFileName(CONF_FILE_NAME);                   /* configuration file */
string aclFileName(ACL_FILE_NAME);               /* Access Control List file */
/* safety */
CmAcl acl;
/* logging */
int deBug=0;                                                   /* debug mask */
int errU=L_DIM;                                                /* error unit */
/* version */
static char rcsid[]="$Id: cmSrv.C,v 1.32 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP=rcsid;
/* other */
int success=1;
/* ######################################################################### */
int main(int argc,char **argv)
{
  /* counters */
  int i;
  /* pointers */
  char *p;
  /* getopt_long */
  int flag=0;
  static struct option longOptions[]=
  {
    {"no-authentication",no_argument,NULL,0},
    {"logger",required_argument,NULL,'l'},
    {"dim_dns_node",required_argument,NULL,'N'},
    {"help",no_argument,NULL,'h'},
    {NULL,0,NULL,0}
  };
  /* dim stuff */
  string srvName(SRV_NAME);
  string srvPath;
  /* DIM DNS */
  const char *dimConfFile=DIM_CONF_FILE_NAME;
  char *dimDnsNode=NULL;
  /* signals */
  int signo;
  sigset_t signalMask;
  /* deBug */
  char *deBugS=NULL;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsidP,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* syslog version */
  mPrintf(L_SYS,INFO,__func__,0,"Starting FMC Configuration Manager Server... "
          "Using: \"%s\", \"FMC-%s\".",rcsidP,FMC_VERSION);
  /*-------------------------------------------------------------------------*/
  /* process command-line arguments */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt_long(argc,argv,"l:N:Uh::",longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 0: /* --no-authentication */
        acl.off();
        break;
      case 'l': /* -l, --logger */
        errU=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'N': /* -N, --dim_dns_node */
        dimDnsNode=optarg;
        break;
      case 'h': /* -h, -hh, --help */
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        mPrintf(L_STD|L_SYS,FATAL,__func__,0,"getopt(): invalid option "
                "\"%s\"!",argv[optind-1]);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* check non-option arguments */
  if(optind<argc)
  {
    i=optind;
    string msg="Invalid non-option command-line arguments: ";
    for(i=optind;i<argc;i++)
    {
      if(i!=optind)msg+=", ";
      msg+="\""+string(argv[i])+"\"";
    }
    msg+=".";
    mPrintf(L_STD|L_SYS,FATAL,__func__,0,"%s",msg.c_str());
    shortUsage();
  }
  if(argc-optind!=0)shortUsage();
  /*-------------------------------------------------------------------------*/
  /* check command line options */
  if(errU<0||errU>7)
  {
    mPrintf(L_STD|L_SYS,FATAL,__func__,0,"Logger type %d not allowed. Logger "
            "type must be in the range 0..7! Exiting...",errU);
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* open error logger */
  if(errU&L_DIM)
  {
    if(dfltLoggerOpen(10,0,ERROR,DEBUG,0)==-1)
    {
      mPrintf(L_STD|L_SYS,FATAL,__func__,0,"Can't initialize error logger! "
              "Exiting...");
      exit(1);
    }
  }
  mPrintf(errU,DEBUG,__func__,0,"Starting FMC Configuration Manager... Using: "
          "\"%s\", \"FMC-%s\".",rcsidP,FMC_VERSION);
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
      mPrintf(errU,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from DIM_DNS_NODE "
              "environment variable).",dimDnsNode);
    }
    else
    {
      dimDnsNode=getDimDnsNode(dimConfFile,0,1);
      if(dimDnsNode)
      {
        mPrintf(errU,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from "
                "\""DIM_CONF_FILE_NAME"\" file).",dimDnsNode);
      }
      else
      {
        mPrintf(errU,FATAL,__func__,0,"DIM Name Server (DIM_DNS_NODE) not "
                "defined!\nDIM Name Server can be defined (in decreasing "
                "order of priority):\n"
                "  1. Specifying the -N DIM_DNS_NODE command-line option;\n"
                "  2. Specifying the DIM_DNS_NODE environment variable;\n"
                "  3. Defining the DIM_DNS_NODE in the file "
                "\""DIM_CONF_FILE_NAME"\".");
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
  /* unsafe mode warning message */
  if(!acl.isAuthenticated())
  {
    mPrintf(errU,WARN,__func__,0,"The FMC Configuration Manager is running in "
            "unsafe mode! Through DIM is possible to delete or modify any "
            "file! To run cmSrv in safe mode, do not specify the command-line "
            "switch \"--no-authentication\"!");
  }
  /*-------------------------------------------------------------------------*/
  /* read the access control list */
  if(acl.isAuthenticated())
  {
    acl.readConf(confFileName);
    acl.readAcl(aclFileName);
    acl.printConf(INFO);
    acl.printAcl(INFO);
  }
  /*-------------------------------------------------------------------------*/
  /* define DIM server name */
  srvPath=string(getSrvPrefix())+"/"+srvName;
  /*-------------------------------------------------------------------------*/
  /* Block SIGINT, SIGTERM and SIGHUP, to be handled synchronously           */
  /* by sigtimedwait().                                                      */
  /* Signals must be blocked before the dim_init() call, which creates 2 new */
  /* threads, to keep the signals blocked in all the 3 threads.              */
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigaddset(&signalMask,SIGHUP);
  if(pthread_sigmask(SIG_BLOCK,&signalMask,NULL))eExit("pthread_sigmask()");
  /*-------------------------------------------------------------------------*/
  /* Start DIM. Here threads becomes 3 */
  dim_init();
  /*-------------------------------------------------------------------------*/
  /* define DIM service names */
  DimService successSvc((srvPath+"/success").c_str(),success);
  DimService fmcVersionSvc((srvPath+"/fmc_version").c_str(),
                           (char*)FMC_VERSION);
  DimService versionSvc((srvPath+"/server_version").c_str(),rcsidP);
  /* define DIM RPC names */
  CmSrvGetFileRpc getRpc(srvPath+"/get");
  CmSrvSetFileRpc setRpc(srvPath+"/set");
  CmSrvUnsetFileRpc unSetRpc(srvPath+"/unset");
  /*-------------------------------------------------------------------------*/
  /* define DIM server error handler */
  CmSrvDimErrorHandler errorHandler;     /* instantiate server error handler */
  /*-------------------------------------------------------------------------*/
  /* read environment variables */
  deBugS=getenv("deBug");
  if(deBugS)deBug=(int)strtol(deBugS,(char**)NULL,0);
  mPrintf(errU,DEBUG,__func__,0,"Debug mask = %#x.",deBug);
  /*-------------------------------------------------------------------------*/
  /* Start DIM server */
  DimServer::start(srvPath.c_str());
  /*-------------------------------------------------------------------------*/
  mPrintf(errU|L_SYS,INFO,__func__,0,"FMC Configuration Manager started. Using "
          "\"%s\", \"FMC-%s\". TGID=%d, srvPath=\"%s\"",rcsidP,FMC_VERSION,
          getpid(),srvPath.c_str());
  /*-------------------------------------------------------------------------*/
  /* main loop */
  for(;;)
  {
    signo=sigwaitinfo(&signalMask,NULL);
    if(signo==SIGHUP)
    {
       mPrintf(errU,WARN,__func__,0,"Signal %d (%s) caught. Continueing...",
               signo,sig2msg(signo));
    }
    else
    {
      signalHandler(signo);
    }
  }
  return 0;
}
/*****************************************************************************/
static void signalHandler(int signo)
{
  if(signo==SIGTERM || signo==SIGINT)
  {
    mPrintf(errU|L_SYS,WARN,__func__,0,"Signal %d (%s) caught. "
            "Exiting...",signo,sig2msg(signo));
    _exit(0);
  }
  else
  {
    mPrintf(errU|L_SYS,FATAL,__func__,0,"Signal %d (%s) caught. "
            "Exiting...",signo,sig2msg(signo));
    _exit(signo);
  }
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"cmSrv [-N | --dim_dns_node DIM_DNS_NODE] [-l | --logger LOGGER]\n"
"      [--no-authentication]\n"
"cmSrv { -h | --help }\n"
"Try \"cmSrv -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  mPrintf(L_DIM|L_STD|L_SYS,INFO,__func__,0,"\n%s",shortUsageStr);
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
"cmSrv.man\n"
"\n"
"..\n"
"%s"
".hw WILDCARD_PATTERN USER_NAME \n"
".TH cmSrv 8  %s \"FMC-%s\" \"FMC Servers\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis cmSrv\\ \\-\n"
"FMC Configuration Manager Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis cmSrv\n"
".DoubleOpt[] N dim\\[ru]dns\\[ru]node DIM_DNS_NODE\n"
".DoubleOpt[] l logger LOGGER\n"
".LongOpt[] no\\[hy]authentication\n"
".EndSynopsis\n"
".sp\n"
".Synopsis cmSrv\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".SH DESCRIPTION\n"
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
"filename access control. The \\fB--no-authentication\\fP command line option "
"bypass both the controls and should never be used.\n"
".PP\n"
".SS Username/hostname access control\n"
".\n"
"Only the couples \\fIuser@host\\fP which match at least one of the wildcard "
"patterns \\fIuser_pattern@host_pattern\\fP listed in the file "
"\\fI/etc/cmSrv.allow\\fP are allowed to issue \\fBcmSet\\fP(1), "
"\\fBcmUnset\\fP(1) and \\fBcmGet\\fP(1) commands to the Process Controller "
"Server. See FILES section below.\n"
".PP\n"
".SS Filename access control\n"
".\n"
"Only the file paths which match at least one of the wildcard patterns listed "
"in the file \\fI/etc/cmSrv.conf\\fP can be read, writed or removed by the "
"Process Controller Server. Read/write access (rw) or read-only access (ro) "
"can be specified for each entry. See FILES section below.\n"
".PP\n"
".SS Setting the DIM Name Server\n"
".\n"
"The DIM Name Server, looked up by \\fBcmSrv\\fP to register the provided\n"
"DIM services, can be chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\\fB"DIM_CONF_FILE_NAME"\\fP.\n"
".RE\n"
".\n"
".\n"
".SH OPTIONS\n"
".\n"
".OptDef h help\n"
"Get basic usage help from the command line then exit.\n"
".\n"
".OptDef N dim_dns_node DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef l logger LOGGER (integer)\n"
"Use the logger units defined in the \\fILOGGER\\fP mask to send diagnostic\n"
"and information messages. The \\fILOGGER\\fP mask can be the bitwise\n"
"\\fIOR\\fP of the following logger units:\n"
".RS\n"
".TP\n"
"0x1\nthe default FMC Message Logger (fifo: /tmp/logSrv.fifo);\n"
".TP\n"
"0x2\nthe standard error stream;\n"
".TP\n"
"0x4\nthe operating system syslog facility.\n"
".PP\n"
"\\fBDefault\\fP: \\fILOGGER\\fP = 0x1.\n"
".RE\n"
".OptDef \"\" no-authentication\n"
"Do not check neither the permission of the usernames and hostnames which "
"sent the command, nor the read/write permission of the files. Should never "
"be used, since this allows everybody to write and remove files. "
"\\fBDefault\\fP: do check both permissions.\n"
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
".TP\n"
".EnvVar deBug \\ (integer,\\ optional)\n"
"Debug mask. Can be set to 0x00...0x1f. The meaning of the mask bit are:\n"
".RS\n"
".TP\n"
"0x01:\nprint main() diagnostics;\n"
".TP\n"
"0x02:\nprint CmAcl diagnostics;\n"
".TP\n"
"0x04:\nprint CmSrvGetFileRpc diagnostics;\n"
".TP\n"
"0x08:\nprint CmSrvSetFileRpc diagnostics;\n"
".TP\n"
"0x10:\nprint CmSrvUnsetFileRpc diagnostics;\n"
".PP\n"
"\\fBDefault\\fP: \\fIdeBug\\fP = 0.\n"
".RE\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Start the FMC Configuration Manager with authentication, sending messages to "
"the default FMC Message Logger:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/cmSrv\n"
".PP\n"
"Start the FMC Configuration Manager with authentication, sending messages to "
"the default FMC Message Logger and to the system syslog facility:\n"
".PP\n"
".ShellCommand /opt/FMC/sbin/cmSrv -l 0x5\n"
".\n"
".\n"
".SH FILES\n"
".\n"
".I /etc/cmSrv.conf\n"
".br\n"
".I /etc/cmSrv.allow\n"
".\n"
".\n"
".SS Configuration File\n"
".I /etc/cmSrv.conf\n"
".PP\n"
".B Description:\n"
".br\n"
"The file \\fI/etc/cmSrv.conf\\fP must contain the list of the files that can "
"be created, read, modified and deleted by the FMC Configuration Manager "
"Server running on the current node. This file is ignored if the FMC "
"Configuration Manager Server is started with the \\fB--no-authentication\\fP "
"command line option. In this case all the files in the filesystem can be "
"created, modified and deleted by the FMC Configuration Manager Server "
"running on this node.\n"
".PP\n"
".B File format:\n"
".br\n"
"File is made of an arbitrary number of records. Records are separated by the "
"character '\\[rs]n' (new-line, line-feed, LF, ASCII 0x0a). Blank records as "
"well as comment records (i.e. records starting with the '#' character) are "
"skipped. If a record contains the '#' character, all charaters following "
"the '#' character (including the '#' itself) are ignored.\n"
".PP\n"
".B Record format:\n"
".br\n"
"(permission) file_name\n"
".PP\n"
".B Record fields description:\n"
".TP\n"
"permission:\nThe access permission, which can be \"ro\" (read only) or "
"\"rw\" (read and write).\n"
".TP\n"
"file_name:\nThe absolute path of the file, which must not contain blanks.\n"
".PP\n"
".B Record Examples:\n"
".br\n"
"(ro) /etc/pcSrv.allow\n"
".br\n"
"(rw) /etc/ipmiSrv.conf\n"
".br\n"
"(ro) *\n"
".\n"
".\n"
".SS Access Control File\n"
".I /etc/cmSrv.allow\n"
".PP\n"
".B Description:\n"
".br\n"
"The file \\fI/etc/cmSrv.allow\\fP must contain the list of the couples of\n"
"patterns \\fBuser_pattern@hostname_pattern\\fP allowed to issue commands\n"
"to the Configuration Manager Server running on the current node.\n"
".PP\n"
".B File format:\n"
".br\n"
"One couple \\fBuser_pattern@hostname_pattern\\fP per record. Records are "
"separated using '\\[rs]n' (new-line, line-feed, LF, ASCII 0x0a). Blank "
"records as well as comment records (i.e. records starting with the '#' "
"character) are skipped.\n"
".PP\n"
".B Record format:\n"
".br\n"
"\\fBuser_pattern@hostname_pattern\\fP.\n"
".br\n"
"user_pattern and host_pattern are separated by the '@' (at) character.\n"
".PP\n"
".B Fields description:\n"
".TP\n"
".B user_pattern\n"
"User name wildcard pattern (see glob(7)).\n"
".TP\n"
".B host_pattern\n"
"Host name wildcard pattern (see glob(7)).\n"
".\n"
".PP\n"
".B Record Examples:\n"
".br\n"
"online@farm0101.mydomain\n"
".br\n"
"*@farm0101.mydomain\n"
".br\n"
"online@*\n"
".br\n"
"*@*\n"
".br\n"
"online@farm01??.mydomain\n"
".br\n"
"online@farm01?[0-9].mydomain\n"
".br\n"
"online@farm01[0-9][0-9].mydomain\n"
".\n"
".\n"
".SH DIM INFORMATION\n"
".PP\n"
".SS DIM Server Name\n"
".\n"
".TP\n"
"%s/\\fIHOST_NAME\\fP/"SRV_NAME"\n"
"where \\fIHOST_NAME\\fP is the host name of the current PC, as returned by\n"
"the command \"hostname -s\", but written in upper case characters.\n"
".\n"
".\n"
".SS Published DIM Services\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/server_version (string)\n"
"Publishes a NULL-terminated string containing the RCS revision label of\n"
"the \\fBcmSrv.C\\fP source code of\n"
"the current \\fBcmSrv\\fP instance, as returned by the command\n"
"\"ident /opt/FMC/sbin/cmSrv\"; in the present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/fmc_version (string)\n"
"Publishes a NULL-terminated string containing the revision label of the FMC\n"
"package which includes the current \\fBcmSrv\\fP executable, in the\n"
"present case:\n"
".IP\n"
"\"%s\".\n"
".\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/success (int)\n"
"Always publishes the integer 1. This dummy services can be used to check\n"
"whether the current instance of \\fBcmSrv\\fP is up and running.\n"
".\n"
".\n"
".SS Published DIM Remote Procedure Calls:\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/set\n"
"Publishes a RPC to write a file to the local filesystem.\n"
".PP\n"
".RS\n"
"\\fBInput format\\fP: \\fI\"X:3;C\"\\fP.\n"
".ad l\n"
".br\n"
"(uint64_t)(mode_t) mode (Permissions, e.g. 0644 for "
"\\[hy]rw\\[hy]r\\[hy]\\[hy]r\\[hy]\\[hy])\n"
".br\n"
"(uint64_t)(uid_t)  UID (User Identifier of owner)\n"
".br\n"
"(uint64_t)(gid_t)  GID (Group Identifier of owner)\n"
".br\n"
"(char*) username+'\\[rs]f'+hostname+'\\ '+file_path+'\\[rs]0'+\\:"
"file_content+'\\[rs]0'\n"
".ad b\n"
".PP\n"
"\\fBOutput format\\fP: \\fI\"C\"\\fP.\n"
".br\n"
"(char*) info/error_message+'\\[rs]0'\n"
".RE\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/unset\n"
"Publishes a RPC to remove a file from the local filesystem.\n"
".PP\n"
".RS\n"
"\\fBInput format\\fP: \\fI\"C\"\\fP.\n"
".ad l\n"
".br\n"
"(char*) username+'\\[rs]f'+hostname+'\\ '+file_path+'\\[rs]0'\n"
".ad b\n"
".PP\n"
"\\fBOutput format\\fP: \\fI\"C\"\\fP.\n"
".br\n"
"(char*) info/error_message+'\\[rs]0'\n"
".RE\n"
".TP\n"
"%s/\\fIHOSTNAME\\fP/"SRV_NAME"/get\n"
"Publishes a RPC to read a file from the local filesystem.\n"
".PP\n"
".RS\n"
"\\fBInput format\\fP: \\fI\"C\"\\fP.\n"
".ad l\n"
".br\n"
"(char*) username+'\\[rs]f'+hostname+'\\ '+file_path+'\\[rs]0'\n"
".ad b\n"
".PP\n"
"\\fBOutput format\\fP: \\fI\"X:5;C\"\\fP.\n"
".ad l\n"
".br\n"
"(uint64_t)(time_t) mtime (time of last modification)\n"
".br\n"
"(uint64_t)(off_t)  size (total size, in Bytes)\n"
".br\n"
"(uint64_t)(mode_t) mode (Permissions, e.g. 0755 for "
"\\[hy]rwxr\\[hy]xr\\[hy]x)\n"
".br\n"
"(uint64_t)(uid_t)  UID (User Identifier of owner)\n"
".br\n"
"(uint64_t)(gid_t)  GID (Group Identifier of owner)\n"
".br\n"
"(char*) file_content+'\\[rs]0'\n"
".ad b\n"
".RE\n"
".\n"
".\n"
".SH AVAILABILITY\n"
".\n"
"Information on how to get the FMC (Farm Monitoring and Control System)\n"
"package and related information is available at the web sites:\n"
".PP\n"
".ad l\n"
".URL https://\\:lhcbweb.bo.infn.it/\\:twiki\\:/\\:bin/\\:view.cgi/\\:"
"LHCbBologna/\\:FmcLinux \"FMC Linux Development Homepage\".\n"
".PP\n"
".URL http://\\:itcobe\\:.\\:web\\:.\\:cern\\:.\\:ch\\:/\\:itcobe/\\:"
"Projects/\\:Framework/\\:"
"Download/\\:Components/\\:SystemOverview\\:/\\:fwFMC/\\:welcome.html "
"\"CERN ITCOBE: FW Farm Monitor and Control\".\n"
".PP\n"
".URL http://\\:lhcb-daq.web.cern.ch/\\:lhcb-daq/\\:online-rpm-repo\\:/\\:"
"index.html \"LHCb Online RPMs\".\n"
".ad b\n"
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
".BR \\%%cmGet (1).\n"
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
,SVC_HEAD,SVC_HEAD,rcsidP,SVC_HEAD,FMC_VERSION,SVC_HEAD
,SVC_HEAD,SVC_HEAD,SVC_HEAD
,rcsidP,FMC_VERSION
       );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/* ######################################################################### */
