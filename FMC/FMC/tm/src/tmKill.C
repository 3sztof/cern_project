/*****************************************************************************/
/*
 * $Log: tmKill.C,v $
 * Revision 2.14  2012/12/13 15:36:29  galli
 * minor changes
 *
 * Revision 2.13  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 2.12  2008/10/16 12:23:43  galli
 * minor changes
 *
 * Revision 2.6  2008/08/26 15:21:32  galli
 * groff manual
 *
 * Revision 2.5  2008/07/07 11:52:45  galli
 * service names starts with /FMC
 *
 * Revision 2.4  2007/12/14 10:15:51  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 2.2  2007/10/23 15:54:52  galli
 * usage() prints FMC version
 *
 * Revision 2.0  2007/09/29 22:23:55  galli
 * compatible with tmSrv 2.2
 * authentication
 * command-line switch --no-authentication for compatibility with
 * tmSrv version < 2.0
 *
 * Revision 1.10  2007/08/16 12:43:08  galli
 * usage() modified
 *
 * Revision 1.6  2006/09/07 14:45:58  galli
 * -m option is repeatable
 * usage and messages improved
 *
 * Revision 1.5  2005/08/26 08:26:53  galli
 * more restrictive DIM wildcard pattern to speed-up browsing
 *
 * Revision 1.3  2004/11/21 22:27:44  galli
 * option -m hostname_pattern substitutes option -s svc_pattern
 * avoid use of "--" (end of option-scanning)
 *
 * Revision 1.2  2004/10/15 00:17:31  galli
 * bug fixed
 *
 * Revision 1.1  2004/10/14 12:13:21  galli
 * Initial revision
 *
 */
/*****************************************************************************/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <argz.h>                         /* argz_create(), argz_stringify() */
#include <ctype.h>                                              /* toupper() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
#include <pwd.h>                                              /* getpwuid(3) */
#include <sys/wait.h>                                             /* wait(2) */
/*****************************************************************************/
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/*****************************************************************************/
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcRef.h"                                  /* FMC_URLS FMC_AUTHORS */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*****************************************************************************/
//#define BENCHMARK
#if BENCHMARK
#include <sys/timeb.h>
#endif
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
int getLocalOpt(int *argcP,char ***argvP,const char* optS,int withArg,
                char **optArg);
char *getAuthString(void);
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: tmKill.C,v 2.14 2012/12/13 15:36:29 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i=0,j=0;
  /* pointers */
  char *p=NULL;
  char *killCmd=NULL;
  char *killCmdDup=NULL;
  char *s=NULL;
  /* service browsing */
  int nodeHostNameC=0;
  char **nodeHostNameV=NULL;
  char *srvPattern=NULL;
  char *cmdPattern=NULL;
  int cmdPatternLen=0;
  char *rawCmdPattern=NULL;
  int rawCmdPatternLen=0;
  DimBrowser br;
  char *format=NULL;
  int type=0;
  int nodeFoundN=0;
  char nodeFound[128]="";
  char *dimDnsNode=NULL;
  /* command line switch */
  int found=0;
  /* command line args */
  char *argz=0;
  size_t argz_len=0;
  /* related to authentication */
  int authenticated=1;
  char *authString;
  char *cmd;
  int cmdLen;
  /*-------------------------------------------------------------------------*/
#if BENCHMARK
  /* for benchmark only */
  struct timeb now;
  double nowF=0.0;
  ftime(&now);
  nowF=(double)now.time+(double)now.millitm/1000.0;
  printf("StopTime=%f\n",nowF);
#endif
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process local command-line options (-h, --help, -hh, -m, -v) */
  if(argc==1)                                                /* no arguments */
  {
    fprintf(stderr,"\nERROR: UTGID_PATTERN (non-option) argument required!\n");
    shortUsage();
  }
  if(getLocalOpt(&argc,&argv,"-h",0,&s))usage(0);
  if(getLocalOpt(&argc,&argv,"--help",0,&s))usage(0);
  if(getLocalOpt(&argc,&argv,"-hh",0,&s))usage(1);
  if(getLocalOpt(&argc,&argv,"-v",0,&s))deBug++;
  if(getLocalOpt(&argc,&argv,"--no-authentication",0,&s))authenticated=0;
  found=getLocalOpt(&argc,&argv,"-N",1,&s);
  if(found==1)dimDnsNode=s;
  else if(found==-1)shortUsage();
  for(nodeHostNameC=0,found=0;;)
  {
    found=getLocalOpt(&argc,&argv,"-m",1,&s);
    if(found==-1)shortUsage();
    else if(found==0)break;
    else
    {
      nodeHostNameC++;
      nodeHostNameV=(char**)realloc(nodeHostNameV,nodeHostNameC*sizeof(char*));
      nodeHostNameV[nodeHostNameC-1]=s;
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
        printf("DIM_DNS_NODE: \"%s\" (from \"/etc/sysconfig/dim\" file).\n",
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
                "\"/etc/sysconfig/dim\".\n");
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
  /* prepare command line to be passed to tmSrv */
  argz_create(argv+1,&argz,&argz_len);
  argz_stringify(argz,argz_len,32);
  if(argz==NULL)
  {
    fprintf(stderr,"\nERROR: UTGID_PATTERN (non-option) argument required!\n");
    shortUsage();
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
  /* authentication */
  authString=getAuthString();
  if(!authenticated)cmd=argz;
  else
  {
    cmdLen=snprintf(NULL,0,"%s %s",authString,argz);
    cmd=(char*)malloc(1+cmdLen);
    snprintf(cmd,1+cmdLen,"%s %s",authString,argz);
  }
  /*-------------------------------------------------------------------------*/
  for(i=0,j=0;i<nodeHostNameC;i++)                /* loop over node patterns */
  {
    /*.......................................................................*/
    printf("  Node pattern: \"%s\"\n",nodeHostNameV[i]);
    srvPattern=(char*)realloc(srvPattern,1+strlen(nodeHostNameV[i]));
    strcpy(srvPattern,nodeHostNameV[i]);
    /* convert to upper case */
    for(p=srvPattern;*p;p++)*p=toupper(*p);
    cmdPatternLen=snprintf(NULL,0,"%s/%s/task_manager/kill",SVC_HEAD,
                           srvPattern);
    cmdPattern=(char*)realloc(cmdPattern,1+cmdPatternLen);
    snprintf(cmdPattern,1+cmdPatternLen,"%s/%s/task_manager/kill",SVC_HEAD,
             srvPattern);
    if(deBug)printf("  Command pattern: \"%s\"\n",cmdPattern);
    /*.......................................................................*/
    rawCmdPatternLen=snprintf(NULL,0,"%s/*/task_manager/kill",SVC_HEAD);
    rawCmdPattern=(char*)realloc(rawCmdPattern,1+rawCmdPatternLen);
    snprintf(rawCmdPattern,1+rawCmdPatternLen,"%s/*/task_manager/kill",
             SVC_HEAD);
    /*.......................................................................*/
    br.getServices(rawCmdPattern);
    nodeFoundN=0;
    while((type=br.getNextService(killCmd,format))!=0)    /* loop over nodes */
    {
      if(!fnmatch(cmdPattern,killCmd,0))
      {
        nodeFoundN++;
        killCmdDup=(char*)realloc(killCmdDup,1+strlen(killCmd));
        strcpy(killCmdDup,killCmd);
        p=strrchr(killCmdDup,'/');
        *p='\0';
        p=strrchr(killCmdDup,'/');
        *p='\0';
        p=strrchr(killCmdDup,'/');
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
        printf("    found node: %3d: %s\n",j,nodeFound);
        if(deBug)printf("  %3d Contacting Task Manager Service: \"%s\""
                        "...\n",j,killCmd);
        DimClient::sendCommand(killCmd,cmd);
        j++;
      }
    }                                                     /* loop over nodes */
    if(!nodeFoundN)
    {
      printf("    no node found for pattern: \"%s\"\n",nodeHostNameV[i]);
    }
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  return 0;
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
"       tmKill [-v] [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"              [--no-authentication] [-s SIGNAL] UTGID_PATTERN\n"
"       tmKill { -h | --help }\n"
"\n"
"Try \"tmKill -h\" for more information.\n";
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
"tmKill.man\n"
"\n"
"..\n"
"%s"
".hw no\\[hy]authentication\n"
".TH tmKill 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis tmKill\\ \\-\n"
"Send a signal to one or more running process(es) using the FMC Task Manager\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis tmKill\n"
".ShortOpt[] v\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".LongOpt[] no\\[hy]authentication\n"
".ShortOpt[] s SIGNAL\n"
"\\fIUTGID_PATTERN\\fP\n"
".EndSynopsis\n"
".sp\n"
".Synopsis tmKill\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The command \\fBtmKill\\fP sends the signal \\fISIGNAL\\fP\n"
"(\\fBdefault\\fP: signal 15, \\fBSIGTERM\\fP, if the \\fB-s\\fP flag\n"
"is omitted) to all the processes whose \\fBUTGID\\fP matches the wildcard\n"
"pattern \\fIUTGID_PATTERN\\fP at all the nodes whose \\fBhostname\\fP\n"
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP.\n"
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
".OptDef h help\n"
"Get basic usage help from the command line.\n"
".\n"
".OptDef v \"\"\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"Send the signal only to process(es) running on the nodes whose hostname\n"
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP\n"
"(\\fBdefault\\fP: send the signal to process(es) running at all the nodes).\n"
".\n"
".OptDef \"\" no-authentication\n"
"Do not send authentication string (compatible with \\fBtmSrv\\fP,\n"
"version\\~<\\~2.0). Works also with \\fBtmSrv\\fP version\\~\\[>=]\\~2.0 if\n"
"\\fBtmSrv\\fP is started with the \\fB--no-authentication\\fP command-line\n"
"switch.\n"
".\n"
".OptDef s \"\" SIGNAL (integer)\n"
"Send the signal \\fISIGNAL\\fP to the process(es) (\\fBdefault\\fP:\n"
"signal 15, \\fBSIGTERM\\fP).\n"
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
"Send the signal \\fBSIGTERM\\fP to the processes with \\fBUTGID\\fP\n"
"\"counter_0\", executing at all the nodes running the Task Manager Server\n"
"registered with the current DIM DNS:\n"
".\n"
".PP\n"
".ShellCommand tmKill counter_0\n"
".\n"
".PP\n"
"Send the signal \\fBSIGTERM\\fP to the processes whose \\fBUTGID\\fP starts\n"
"with \"count\", executing at all the nodes running the Task Manager Server\n"
"registered with the current DIM DNS:\n"
".\n"
".PP\n"
".ShellCommand tmKill \\[dq]count*\\[dq]\n"
".ShellCommand tmKill count\\[rs]*\n"
".\n"
".PP\n"
"Use another wildcard pattern to select the \\fBUTGID\\fP:"
".\n"
".PP\n"
".ShellCommand tmKill \\[dq]count*[2-5]\\[dq]\n"
".\n"
".PP\n"
"Send the signal \\fBSIGTERM\\fP only to the processes with \\fBUTGID\\fP\n"
"\"counter_0\", executing at nodes \"farm0101\" and \"farm0102\":\n"
".\n"
".PP\n"
".ShellCommand tmKill -m farm0101 -m farm0102 counter_0\n"
".\n"
".PP\n"
"Use wildcard patterns to select the \\fBUTGID\\fP and the nodes:\n"
".\n"
".PP\n"
".ShellCommand tmKill -m \\[dq]farm0*\\[dq] -m \\[dq]farm1*\\[dq] "
"\\[dq]count*\\[dq]\n"
".ShellCommand tmKill -m farm0\\[rs]* -m farm1\\[rs]* count\\[rs]*\n"
".ShellCommand tmKill -m \\[dq]farm0[3-7]01\\[dq] \\[dq]count*[2-5]\\[dq]\n"
".ShellCommand tmKill -m \\[dq]farm0[3-7]??\\[dq] \\[dq]count*[2-5]\\[dq]\n"
".ShellCommand tmKill -m \\[dq]farm010[1357]\\[dq] counter_\\[rs]*\n"
".\n"
".PP\n"
"Send the signal 2 (\\fBSIGINT\\fP) instead of default signal 15 "
"(\\fBSIGTERM\\fP):\n"
".\n"
".PP\n"
".ShellCommand tmKill -s 2 counter_0\n"
".ShellCommand tmKill -s 2 \\[dq]count*[2-5]\\[dq]\n"
".ShellCommand tmKill -m farm0103 -m farm0201 -s 2 counter_0\n"
".ShellCommand tmKill -m \\[dq]farm0*\\[dq] -s 2 \\[dq]count*\\[dq]\n"
".\n"
".PP\n"
"Send the signal to all the processes started by the Task Manager Servers\n"
"registered with the current DIM DNS:\n"
".\n"
".PP\n"
".ShellCommand tmKill \\[dq]*\\[dq]\n"
".ShellCommand tmKill \\[rs]*\n"
".ShellCommand tmKill -s 2 \\[dq]*\\[dq]\n"
".ShellCommand tmKill -s 2 \\[rs]*\n"
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
".BR \\%%tmSrv (8).\n"
".BR \\%%tmStart (1).\n"
".BR \\%%tmStop (1).\n"
".BR \\%%tmLs (1).\n"
".BR \\%%tmLl (1).\n"
".BR \\%%glob (7).\n"
".BR \\%%kill (2).\n"
".BR \\%%signal (7).\n"
".\n"
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
