/*****************************************************************************/
/*
 * $Log: pcRm.C,v $
 * Revision 2.13  2012/12/13 16:36:39  galli
 * Minor changes in man page references
 *
 * Revision 2.12  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 2.9  2008/10/03 15:09:55  galli
 * DIM_CONF_FILE_NAME and DIM_DNS_SRV_LIST got from fmcPar.h
 *
 * Revision 2.6  2008/09/30 09:54:57  galli
 * groff manual
 *
 * Revision 2.2  2008/07/07 15:09:14  galli
 * service names starts with /FMC
 *
 * Revision 2.1  2007/12/14 12:24:53  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 2.0  2007/10/01 11:01:57  galli
 * compatible with pcSrv version 2
 * authentication
 * command-line switch --no-authentication for compatibility with
 * pcSrv version < 2
 *
 * Revision 1.6  2007/08/16 12:06:49  galli
 * usage() modified
 *
 * Revision 1.5  2006/09/06 08:21:06  galli
 * usage and messages improved
 *
 * Revision 1.4  2006/08/23 14:10:52  galli
 * usage() updated
 *
 * Revision 1.3  2006/08/08 14:07:47  galli
 * -C option commutes with option to pcSrv
 * -C option is repeatable
 *
 * Revision 1.2  2005/05/28 23:53:31  galli
 * working version
 *
 * Revision 1.1  2005/05/21 09:03:54  galli
 * Initial revision
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
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
int getLocalOpt(int *argcP,char ***argvP,const char* optS,int withArg,
                char **optArg);
char *getAuthString(void);
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: pcRm.C,v 2.13 2012/12/13 16:36:39 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i=0,j=0;
  /* pointers */
  char *p=NULL;
  char *rmCmd=NULL;
  char *rmCmdDup=NULL;
  char *s=NULL;
  /* service browsing */
  int ctrlPcHostNameC=0;
  char **ctrlPcHostNameV=NULL;
  char *srvPattern=NULL;
  char *cmdPattern=NULL;
  int cmdPatternLen=0;
  char *rawCmdPattern=NULL;
  int rawCmdPatternLen=0;
  DimBrowser br;
  char *format=NULL;
  int type=0;
  char cpcFound[128]="";
  int cpcFoundN=0;
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
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process local command-line options (-h, --help, -hh, -C, -v) */
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
  for(ctrlPcHostNameC=0;;)
  {
    found=getLocalOpt(&argc,&argv,"-C",1,&s);
    if(found==-1)shortUsage();
    else if(found==0)break;
    else
    {
      ctrlPcHostNameC++;
      ctrlPcHostNameV=(char**)realloc(ctrlPcHostNameV,ctrlPcHostNameC*
                                      sizeof(char*));
      ctrlPcHostNameV[ctrlPcHostNameC-1]=s;
    }
  }
  /*-------------------------------------------------------------------------*/
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
  /* prepare command line to be passed to pcSrv */
  argz_create(argv+1,&argz,&argz_len);
  argz_stringify(argz,argz_len,32);
  if(argz==NULL)
  {
    fprintf(stderr,"\nERROR: UTGID_PATTERN (non-option) argument required!\n");
    shortUsage();
  }
  /*-------------------------------------------------------------------------*/
  printf("Ctrl Pc patterns:  ");
  for(i=0;i<ctrlPcHostNameC;i++)                  /* loop over node patterns */
  {
    printf("\"%s\"",ctrlPcHostNameV[i]);
    if(i<ctrlPcHostNameC-1)printf(", ");
  }
  printf(".\n");
  /*-------------------------------------------------------------------------*/
  authString=getAuthString();
  if(!authenticated)cmd=argz;
  else
  {
    cmdLen=snprintf(NULL,0,"%s %s",authString,argz);
    cmd=(char*)malloc(1+cmdLen);
    snprintf(cmd,1+cmdLen,"%s %s",authString,argz);
  }
  /*-------------------------------------------------------------------------*/
  for(i=0,j=0;i<ctrlPcHostNameC;i++)           /* loop over ctrl PC patterns */
  {
    /*.......................................................................*/
    printf("  Ctrl PC pattern: \"%s\"\n",ctrlPcHostNameV[i]);
    srvPattern=(char*)realloc(srvPattern,1+strlen(ctrlPcHostNameV[i]));
    strcpy(srvPattern,ctrlPcHostNameV[i]);
    /* convert to upper case */
    for(p=srvPattern;*p;p++)*p=toupper(*p);
    cmdPatternLen=snprintf(NULL,0,"%s/%s/%s/rm",SVC_HEAD,srvPattern,SRV_NAME);
    cmdPattern=(char*)realloc(cmdPattern,1+cmdPatternLen);
    snprintf(cmdPattern,1+cmdPatternLen,"%s/%s/%s/rm",SVC_HEAD,srvPattern,
             SRV_NAME);
    if(deBug)printf("  Command pattern: \"%s\"\n",cmdPattern);
    /*.......................................................................*/
    rawCmdPatternLen=snprintf(NULL,0,"%s/*/%s/rm",SVC_HEAD,SRV_NAME);
    rawCmdPattern=(char*)realloc(rawCmdPattern,1+rawCmdPatternLen);
    snprintf(rawCmdPattern,1+rawCmdPatternLen,"%s/*/%s/rm",SVC_HEAD,SRV_NAME);
    /*.......................................................................*/
    br.getServices(rawCmdPattern);
    cpcFoundN=0;
    while((type=br.getNextService(rmCmd,format))!=0)   /* loop over ctrl PCs */
    {
      if(!fnmatch(cmdPattern,rmCmd,0))
      {
        cpcFoundN++;
        rmCmdDup=(char*)realloc(rmCmdDup,1+strlen(rmCmd));
        strcpy(rmCmdDup,rmCmd);
        p=strrchr(rmCmdDup,'/');
        *p='\0';
        p=strrchr(rmCmdDup,'/');
        *p='\0';
        p=strrchr(rmCmdDup,'/');
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
        printf("    found Ctrl PC: %3d: %s\n",j,cpcFound);
        if(deBug)printf("  %3d Contacting Process Controller Service: \"%s\""
                        "...\n",j,rmCmd);
        DimClient::sendCommand(rmCmd,cmd);
        j++;
      }
    }                                                  /* loop over ctrl PCs */
    if(!cpcFoundN)
    {
      printf("    no Ctrl PC found for pattern: \"%s\"\n",ctrlPcHostNameV[i]);
    }
    /*.......................................................................*/
  }                                            /* loop over ctrl PC patterns */
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
"pcRm [-v] [-N DIM_DNS_NODE] [-C CTRL_PC_PATTERN...]\n"
"     [-m NODE_PATTERN...]  [--no-authentication] [-s SIGNAL]\n"
"     [-d KILL_9_DELAY] UTGID_PATTERN\n"
"pcRm { -h | --help }\n"
"\n"
"Try \"pcRm -h\" for more information.\n";
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
"pcRm.man\n"
"\n"
"..\n"
"%s"
".hw no\\[hy]authentication NODE_PATTERN RT_PRIORITY\n"
".TH pcRm 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis pcRm\\ \\-\n"
"Remove one or more process(es) from the FMC Process Controller and stop them "
"on the farm nodes\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis pcRm\n"
".ShortOpt[] v\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] C CTRL_PC_PATTERN...\n"
".ShortOpt[] m NODE_PATTERN...\n"
".LongOpt[] no\\[hy]authentication\n"
".ShortOpt[] s SIGNAL\n"
".ShortOpt[] d KILL_9_DELAY\n"
"\\fIUTGID_PATTERN\\fP\n"
".EndSynopsis\n"
".sp\n"
".Synopsis pcRm\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
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
"At runtime, processes can be added to the list for one or more nodes by\n"
"means of the \\fBpcAdd\\fP(1) command and removed from the list for one or\n"
"more nodes by means of the \\fBpcRm\\fP(1) command. The commands\n"
"\\fBpcLs\\fP(1), \\fBpcLl\\fP(1) and \\fBpcLss\\fP(1) provide several\n"
"information about the controlled processes.\n"
".PP\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The command \\fBpcRm\\fP(1) remove the processes whose UTGID matches the "
"wildcard pattern \\fIUTGID_PATTERN\\fP from the Process Controller list for "
"the controlled nodes whose hostname matches at least one of the wildcard "
"patterns \\fINODE_PATTERN\\fP, controlled by the control PCs whose hostname "
"matches at least one of the wildcard patterns \\fICTRL_PC_PATTERN\\fP and "
"terminates these processes by sending them the signal \\fISIGNAL\\fP and "
"- if they block or ignore the signal - by sending them the \\fBKILL\\fP "
"signal after \\fIKILL_9_DELAY\\fP seconds.\n"
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
".OptDef h help\n"
"Get basic usage help from the command line.\n"
".\n"
".OptDef v \"\"\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef C \"\" CTRL_PC_PATTERN (string, repeatable)\n"
"Contact the Process Controller Server (\\fBpcSrv\\fP(8)) of the control\n"
"PCs whose hostname matches the wildcard pattern \\fICTRL_PC_PATTERN\\fP\n"
"(\\fBdefault\\fP: contact all the control PCs).\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"Remove the processes from the lists for the nodes (among those controlled by "
"the control PCs whose hostname matches the wildcard pattern "
"\\fICTRL_PC_PATTERN\\fP) whose hostname matches the wildcard pattern "
"\\fINODE_PATTERN\\fP (\\fBdefault\\fP: remove processes from the lists for "
"all the nodes controlled by the control PCs whose hostname matches the "
"wildcard pattern \\fICTRL_PC_PATTERN\\fP).\n"
".\n"
".OptDef \"\" no-authentication\n"
"Do not send authentication string. This option requires that the Process "
"Controller Servers \\fBpcSrv\\fP(8) and the Task Manager Servers "
"\\fBtmSrv\\fP(8) are also started with the \\fB--no-authentication\\fP "
"command-line switch.\n"
".\n"
".OptDef s \"\" SIGNAL (integer)\n"
"Send the signal \\fISIGNAL\\fP as the first signal to the process(es). "
"\\fBDefault\\fP: signal 15, \\fBSIGTERM\\fP.\n"
".\n"
".OptDef d \"\" KILL_9_DELAY (integer)\n"
"Wait \\fIKILL_9_DELAY\\fP seconds before sending the second signal "
"(which is always signal 9, \\fISIGKILL\\fP). \\fBDefault\\fP: 4 s.\n"
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
"Remove and stop the processes with UTGID \"counter_0\" from all the farm "
"nodes controlled by all the Control PCs running a Process Controller "
"servers registered with the current DIM DNS:\n"
".PP\n"
".ShellCommand pcRm counter_0\n"
".\n"
".PP\n"
"Use a wildcard pattern to select the UTGID:\n"
".PP\n"
".ShellCommand pcRm \\[dq]count*\\[dq]\n"
".ShellCommand pcRm count\\[rs]*\n"
".ShellCommand pcRm \\[dq]count*[2-5]\\[dq]\n"
".\n"
".PP\n"
"Remove and stop the processes with UTGID \"counter_0\" only from the farm "
"node \"farm0101\" controlled by the Control PC \"ctrl01\":\n"
".PP\n"
".ShellCommand pcRm -C ctrl01 -m farm0101 counter_0\n"
".\n"
".PP\n"
"Remove and stop the processes with UTGID \"counter_0\" only from the farm "
"node \"farm0101\" controlled by the Control PC \"ctrl01\" and the farm node "
"\"farm0201\" controlled by the Control PC \"ctrl01\":\n"
".PP\n"
".ShellCommand pcRm -C ctrl01 -C ctrl02 -m farm0101 -m farm0201 counter_0\n"
".\n"
".PP\n"
"Use wildcard patterns to select the UTGID, the NODE and the CTRL_PC:\n"
".PP\n"
".ShellCommand pcRm -C \\[dq]ctrl0?\\[dq] -C \\[dq]ctrl1?\\[dq] -m "
"\\[dq]farm01?\\[dq] -m \\[dq]farm02?\\[dq] \\[dq]counter?\\[dq]\n"
".ShellCommand pcRm -C ctrl0\\[rs]? -C ctrl1\\[rs]? -m farm01\\[rs]? -m "
"farm02\\[rs]? counter\\[rs]?\n"
".ShellCommand pcRm -C \\[dq]ctrl0*\\[dq] \\[dq]count*\\[dq]\n"
".ShellCommand pcRm -C ctrl0\\[rs]* count\\[rs]*\n"
".ShellCommand pcRm -C \\[dq]ctrl0[356]?\\[dq] \\[dq]count*[247]\\[dq]\n"
".ShellCommand pcRm -C \\[dq]ctrl0[3-7]?\\[dq] \\[dq]count*[2-5]\\[dq]\n"
".\n"
".PP\n"
"Send the signal 2 (\\fBSIGINT\\fP) as first signal, instead of default "
"signal 15 (\\fBSIGTERM\\fP):\n"
".PP\n"
".ShellCommand pcRm -s 2 counter_0\n"
".ShellCommand pcRm -s 2 \\[dq]count*[2-5]\\[dq]\n"
".ShellCommand pcRm -C ctrl01 -s 2 counter_0\n"
".ShellCommand pcRm -C \\[dq]ctrl0*\\[dq] -s 2 \\[dq]count*\\[dq]\n"
".\n"
".PP\n"
"Wait 8 seconds (instead of the default 4 second) before sending the second "
"signal (signal 9, \\fBSIGKILL\\fP), if the process has not terminated:\n"
".PP\n"
".ShellCommand pcRm -s 2 -d 8 counter_0\n"
".ShellCommand pcRm -s 2 -d 8 \\[dq]count*[2-5]\\[dq]\n"
".ShellCommand pcRm -C ctrl01 -s 2 -d 8 counter_0\n"
".ShellCommand pcRm -C \\[dq]ctrl0*\\[dq] -s 2 -d 8 \\[dq]count*\\[dq]\n"
".\n"
".PP\n"
"Remove and stop the processes with any UTGID from all the farm "
"nodes controlled by all the Control PCs running a Process Controller "
"servers registered with the current DIM DNS:\n"
".PP\n"
".ShellCommand pcRm \\[dq]*\\[dq]\n"
".ShellCommand pcRm \\[rs]*\n"
".ShellCommand pcRm -s 2 -d 3 \\[dq]*\\[dq]\n"
".ShellCommand pcRm -s 2 -d 3 \\[rs]*\n"
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
".BR \\%%pcLs (1),\n"
".BR \\%%pcLl (1),\n"
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
