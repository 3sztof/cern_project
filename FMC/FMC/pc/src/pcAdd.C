/*****************************************************************************/
/*
 * $Log: pcAdd.C,v $
 * Revision 2.16  2012/12/14 17:05:55  galli
 * Minor changes in man page references
 *
 * Revision 2.15  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 2.12  2008/10/03 15:14:45  galli
 * DIM_CONF_FILE_NAME got from fmcPar.h
 *
 * Revision 2.6  2008/09/02 14:36:23  galli
 * groff manual
 *
 * Revision 2.3  2008/07/07 13:02:21  galli
 * service names starts with /FMC
 *
 * Revision 2.1  2007/12/14 12:08:42  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 2.0  2007/10/01 11:01:19  galli
 * compatible with pcSrv version 2
 * authentication
 * command-line switch --no-authentication for compatibility with
 * pcSrv version < 2
 *
 * Revision 1.6  2006/08/08 12:26:04  galli
 * -C option is repeatable
 *
 * Revision 1.3  2006/08/07 12:10:13  galli
 * -C option commutes with option to pcSrv
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
static char rcsid[]="$Id: pcAdd.C,v 2.16 2012/12/14 17:05:55 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i=0,j=0;
  /* pointers */
  char *p=NULL;
  char *addCmd=NULL;
  char *addCmdDup=NULL;
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
  char *argz=NULL;
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
    fprintf(stderr,"\nERROR: PATH (non-option) argument required!\n");
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
  for(ctrlPcHostNameC=0,found=0;;)
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
    fprintf(stderr,"\nERROR: PATH (non-option) argument required!\n");
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
  for(i=0,j=0;i<ctrlPcHostNameC;i++)           /* loop over ctrl PC patterns */
  {
    /*.......................................................................*/
    printf("  Ctrl PC pattern: \"%s\"\n",ctrlPcHostNameV[i]);
    srvPattern=(char*)realloc(srvPattern,1+strlen(ctrlPcHostNameV[i]));
    strcpy(srvPattern,ctrlPcHostNameV[i]);
    /* convert to upper case */
    for(p=srvPattern;*p;p++)*p=toupper(*p);
    cmdPatternLen=snprintf(NULL,0,"%s/%s/%s/add",SVC_HEAD,srvPattern,SRV_NAME);
    cmdPattern=(char*)realloc(cmdPattern,1+cmdPatternLen);
    snprintf(cmdPattern,1+cmdPatternLen,"%s/%s/%s/add",SVC_HEAD,srvPattern,
             SRV_NAME);
    if(deBug)printf("  Command pattern: \"%s\"\n",cmdPattern);
    /*.......................................................................*/
    rawCmdPatternLen=snprintf(NULL,0,"%s/*/%s/add",SVC_HEAD,SRV_NAME);
    rawCmdPattern=(char*)realloc(rawCmdPattern,1+rawCmdPatternLen);
    snprintf(rawCmdPattern,1+rawCmdPatternLen,"%s/*/%s/add",SVC_HEAD,SRV_NAME);
    /*.......................................................................*/
    br.getServices(rawCmdPattern);
    cpcFoundN=0;
    while((type=br.getNextService(addCmd,format))!=0)  /* loop over ctrl PCs */
    {
      if(!fnmatch(cmdPattern,addCmd,0))
      {
        cpcFoundN++;
        addCmdDup=(char*)realloc(addCmdDup,1+strlen(addCmd));
        strcpy(addCmdDup,addCmd);
        p=strrchr(addCmdDup,'/');
        *p='\0';
        p=strrchr(addCmdDup,'/');
        *p='\0';
        p=strrchr(addCmdDup,'/');
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
                        "...\n",j,addCmd);
        DimClient::sendCommand(addCmd,cmd);
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
        fprintf(stderr,"\nERROR: option \"%s\" requires an argument!\n\n",optS);
        return -1;
      }
      else if((*argvP)[i+1][0]=='-')           /* following arg start with - */
      {
        fprintf(stderr,"\nERROR: option \"%s\" requires an argument!\n\n",optS);
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
"pcAdd [-v] [-N DIM_DNS_NODE] [-C CTRL_PC_PATTERN...]\n"
"      [-m NODE_PATTERN...]  [--no-authentication] [-c]\n"
"      [-D NAME=VALUE...]  [-S SCHEDULER] [-p NICE_LEVEL]\n"
"      [-r RT_PRIORITY] [-a CPU_NUM...]  [-d] [-n USER_NAME]\n"
"      [-g GROUP_NAME] [-w WD] [-o] [-e] [-O OUT_FIFO] [-E ERR_FIFO]\n"
"      [-A] [-M MAX_START_N] [-K CHECK_PERIOD] [-X DIS_PERIOD] -u UTGID\n"
"      PATH [ARG...]\n"
"pcAdd { -h | --help }\n"
"\n"
"Try \"pcAdd -h\" for more information.\n";
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
"pcAdd.man\n"
"\n"
"..\n"
"%s"
".hw no\\[hy]authentication NODE_PATTERN RT_PRIORITY\n"
".TH pcAdd 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis pcAdd\\ \\-\n"
"Add a new process to the FMC Process Controller and start it on the farm "
"nodes\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis pcAdd\n"
".ShortOpt[] v\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] C CTRL_PC_PATTERN...\n"
".ShortOpt[] m NODE_PATTERN...\n"
".LongOpt[] no\\[hy]authentication\n"
".ShortOpt[] c\n"
".ShortOpt[] D NAME=VALUE...\n"
".ShortOpt[] s SCHEDULER\n"
".ShortOpt[] p NICE_LEVEL\n"
".ShortOpt[] r RT_PRIORITY\n"
".ShortOpt[] a CPU_NUM...\n"
".ShortOpt[] d\n"
".ShortOpt[] n USER_NAME\n"
".ShortOpt[] g GROUP_NAME\n"
".ShortOpt[] w WD\n"
".ShortOpt[] o\n"
".ShortOpt[] e\n"
".ShortOpt[] O OUT_FIFO\n"
".ShortOpt[] E ERR_FIFO\n"
".ShortOpt[] A\n"
".ShortOpt[] M MAX_START_N\n"
".ShortOpt[] K CHECK_PERIOD\n"
".ShortOpt[] X DIS_PERIOD\n"
"\\fB-u\\fP \\fIUTGID\\fP\n"
"\\fIPATH\\fP [\\fIARG\\fP...]\n"
".EndSynopsis\n"
".sp\n"
".Synopsis pcAdd\n"
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
"The command \\fBpcAdd\\fP(1) \\fBadds\\fP a new process to the list of the\n"
"scheduled processes for the nodes whose hostname matches at least one of\n"
"the wildcard patterns \\fINODE_PATTERN\\fP, controlled by the control\n"
"PCs whose hostname matches at least one of the wildcard pattern\n"
"\\fICTRL_PC_PATTERN\\fP and \\fBstarts\\fP it, using the executable image\n"
"file located in \\fIPATH\\fP and\n"
"the arguments specified in \\fIARG\\fP. In the environment of the started\n"
"process the string variable \\fBUTGID\\fP (User assigned unique\n"
"Thread Group Identifier) is set to \\fIUTGID\\fP.\n"
".PP\n"
"By \\fBdefault\\fP, before starting the process, all file descriptors are\n"
"closed and standard file descriptors (STDIN_FILENO, STDOUT_FILENO and\n"
"STDERR_FILENO) are re-opened on /dev/null.\n"
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
"Add the process \\fIPATH\\fP to the list for the nodes (among those\n"
"controlled by the control PCs whose hostname matches the wildcard pattern\n"
"\\fICTRL_PC_PATTERN\\fP) whose hostname matches the wildcard pattern\n"
"\\fINODE_PATTERN\\fP (\\fBdefault\\fP: add process to the list for all the\n"
"nodes controlled by the control PCs whose hostname matches\n"
"the wildcard pattern \\fICTRL_PC_PATTERN\\fP).\n"
".\n"
".OptDef \"\" no-authentication\n"
"Do not send authentication string. This option requires that the Process "
"Controller Servers \\fBpcSrv\\fP(8) and the Task Manager Servers "
"\\fBtmSrv\\fP(8) are also started with the \\fB--no-authentication\\fP "
"command-line switch.\n"
".\n"
".OptDef c \"\"\n"
"Clear the \\fBenvironment\\fP inherited by the Task Manager.\n"
"\\fBDefault\\fP: inherit the Task Manager environment.\n"
".\n"
".OptDef D \"\" NAME=VALUE (string, repeatable)\n"
"Set the \\fBenvironment\\fP variable \\fINAME\\fP to the value\n"
"\\fIVALUE\\fP.\n"
".OptDef s \"\" SCHEDULER (integer in the range 0..2)\n"
"Set the process scheduler to \\fISCHEDULER\\fP, which can be: \n"
".RS\n"
".TP\n"
"\\fB0\\fP\\~(\\fBSCHED_OTHER\\fP)\n"
"The default Linux \\fBtime-sharing\\fP scheduler, with a\n"
"\\fBdynamic priority\\fP based on the \\fINICE_LEVEL\\fP.\n"
".TP\n"
"\\fB1\\fP\\~(\\fBSCHED_FIFO\\fP)\n"
"The \\fBstatic-priority\\fP Linux \\fBreal-time\\fP \\fBfifo\\fP scheduler,\n"
"without time slicing. A \\fBSCHED_FIFO\\fP process runs until either it is\n"
"blocked by an I/O request, it is preempted by a higher "
"priority process, or it calls sched_yield (2).\n"
".TP\n"
"\\fB2\\fP\\~(\\fBSCHED_RR\\fP)\n"
"The \\fBstatic-priority\\fP Linux \\fBreal-time Round-Robin\\fP scheduler.\n"
"It differs from \\fBSCHED_FIFO\\fP because each process is only allowed to\n"
"run for a maximum time quantum. If a \\fBSCHED_RR\\fP process has been\n"
"running for a time period equal to or longer than the time quantum, it\n"
"will be put at the end of the list for its priority.\n"
".PP\n"
"\\fBDefault\\fP: 0 (SCHED_OTHER).\n"
".RE\n"
".\n"
".OptDef p \"\" NICE_LEVEL (integer in the range -20..19)\n"
"If \\fISCHEDULER\\fP=\\fBSCHED_OTHER\\fP, set the \\fBnice level\\fP of the\n"
"process to \\fINICE_LEVEL\\fP. The \\fBnice level\\fP is used by the\n"
"\\fBSCHED_OTHER\\fP time-sharing Linux scheduler to compute the\n"
"\\fBdynamic priority\\fP. Allowed values for \\fINICE_LEVEL\\fP are in the\n"
"range -20..19 (-20 corresponds to the most favorable scheduling; 19\n"
"corresponds to the least favorable scheduling). The \\fBnice level\\fP\n"
"can be lowered by the Task Manager even for processes which run as a user\n"
"different from root.\n"
".\n"
".OptDef r \"\" RT_PRIORITY (integer in the range 0..99)\n"
"Set the \\fBstatic\\fP (real-time) \\fBpriority\\fP of the process to\n"
"\\fIRT_PRIORITY\\fP. Only value 0 (zero) for \\fIRT_PRIORITY\\fP is\n"
"allowed for scheduler \\fBSCHED_OTHER\\fP. For \\fBSCHED_FIFO\\fP and\n"
"\\fBSCHED_RR\\fP real-time schedulers, allowed values are in the range\n"
"1..99 (1 is the least favorable priority, 99 is the most favorable\n"
"priority).\n"
".\n"
".OptDef a \"\" CPU_NUM (integer, repeatable)\n"
"Add the CPU number \\fICPU_NUM\\fP to the \\fBprocess-to-CPU affinity\n"
"mask\\fP. More than one of these options can be present in the same command\n"
"to add more than one CPU to the affinity mask. Started process is allowed\n"
"to run only on the CPUs specified in the affinity mask. Omitting this\n"
"option, process is allowed to run on any CPU of the node. Allowed\n"
"\\fICPU_NUM\\fP depend on the PC architecture\n"
".\n"
".OptDef d \"\"\n"
"Start the process as \\fBdaemon\\fP: the process \\fBumask\\fP (user\n"
"file-creation mask) is changed into 0 (zero) and the program is run in a\n"
"new \\fBsession\\fP as \\fBprocess group leader\\fP.\n"
".\n"
".OptDef n \"\" USER_NAME (string)\n"
"Set the \\fBeffective UID\\fP (User IDentifier), the \\fBreal UID\\fP and\n"
"the \\fBsaved UID\\fP of the process to the \\fBUID\\fP corresponding to\n"
"the user \\fIUSER_NAME\\fP.\n"
".\n"
".OptDef g \"\" GROUP_NAME (string)\n"
"Set the \\fBeffective GID\\fP (Group IDentifier), the \\fBreal GID\\fP and\n"
"the \\fBsaved GID\\fP of the process to the \\fBGID\\fP corresponding to\n"
"the group \\fIGROUP_NAME\\fP.\n"
".\n"
".OptDef u \"\" UTGID (string, mandatory)\n"
"Set the process \\fButgid\\fP (User assigned unique Thread Group\n"
"Identifier) to \\fIUTGID\\fP.\n"
".\n"
".OptDef w \"\" WD (string)\n"
"Set the process \\fBworking directory\\fP to \\fIWD\\fP. File open by the\n"
"process without path specification are sought by the process in this\n"
"directory. Process \\fBrelative file path\\fP start from this directory.\n"
".\n"
".OptDef e \"\"\n"
"Redirect the process \\fBstderr\\fP (standard error stream) to the default\n"
"FMC Message Logger. Omitting \\fB-e\\fP or \\fB-E\\fP \\fIERR_FIFO\\fP\n"
"options, the standard error stream is thrown in /dev/null.\n"
".\n"
".OptDef o \"\"\n"
"Redirect the process \\fBstdout\\fP (standard output stream) to the default\n"
"FMC Message Logger. Omitting \\fB-o\\fP or \\fB-O\\fP \\fIOUT_FIFO\\fP\n"
"options, the standard output stream is thrown in /dev/null.\n"
".\n"
".OptDef E \"\" ERR_FIFO (string)\n"
"Redirect the process \\fBstderr\\fP to the FMC Message Logger which uses\n"
"the FIFO (named pipe) \\fIERR_FIFO\\fP. Omitting \\fB-e\\fP or \\fB-E\\fP\n"
"\\fIERR_FIFO\\fP options, the standard error stream is thrown in\n"
"/dev/null.\n"
".\n"
".OptDef O \"\" OUT_FIFO (string)\n"
"Redirect the process \\fBstdout\\fP to the FMC Message Logger which uses\n"
"the FIFO (named pipe) \\fIOUT_FIFO\\fP. Omitting \\fB-o\\fP or \\fB-O\\fP\n"
"\\fIOUT_FIFO\\fP options, the standard output stream is thrown in\n"
"/dev/null.\n"
".\n"
".OptDef A \"\"\n"
"In conjunction with \\fB-E\\fP \\fIERR_FIFO\\fP and\n"
"\\fB-O\\fP \\fIOUT_FIFO\\fP options, this option forces the \\fBno-drop\\fP\n"
"policy for the FIFO. \\fBDefault\\fP: the \\fBcongestion-proof\\fP policy\n"
"is used for the FIFO.\n"
".\n"
".OptDef M \"\" MAX_START_N (integer)\n"
".OptDef+ K \"\" CHECK_PERIOD (integer)\n"
".OptDef+ X \"\" DIS_PERIOD (integer)\n"
"Respawn control parameters. If a process is (re)started more than\n"
"\\fIMAX_START_N\\fP times in \\fICHECK_PERIOD\\fP seconds, then the\n"
"process respawn is disabled for \\fIDIS_PERIOD\\fP seconds. \\fBDefault\\fP\n"
"values are: \\fIMAX_START_N\\fP\\~=\\~10,\n"
"\\fICHECK_PERIOD\\fP\\~=\\~120\\~s and\n"
"\\fIDIS_PERIOD\\fP\\~=\\~300\\~s. If \\fIMAX_START_N\\fP\\~=\\~-1, then\n"
"the respawn control is excluded, i.e. process can be restarted\n"
"indefinitely. If \\fIDIS_PERIOD\\fP\\~=\\~-1, then the process restart\n"
"for the process, once disabled, is never re-enabled.\n"
".\n"
".\n"
".SS Parameter Expansion\n"
"If any of the strings \\fIUTGID\\fP, \\fIPATH\\fP, \\fIWD\\fP, "
"\\fIOUT_FIFO\\fP or \\fIERR_FIFO\\fP or any of the strings \\fIARG\\fP or "
"any of the strings \\fINAME=value\\fP includes the substring "
"\'${RUN_NODE}\', then the substring \'${RUN_NODE}\' is replaced by the "
"hostname of the node on which the process is started.\n"
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
"Start and keep up & running the process \"counter\" at all the farm nodes "
"controlled by all the Control PCs running a Process Controller servers "
"registered with the current DIM DNS:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -u counter_0 /opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C \\[dq]*\\[dq] -m \\[dq]*\\[dq] -u counter_0 "
"/opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C \\[rs]* -m \\[rs]* -u counter_0 "
"/opt/FMC/tests/counter\n"
".PP\n"
"Use the parameter expansion of the '${RUN_NODE}' substring:\n"
".PP\n"
".ShellCommand pcAdd -u cnt_'${RUN_NODE}' /opt/FMC/tests/cnt -a '${RUN_NODE}'\n"
".PP\n"
"This command will expand, on node farm0101, to:\n"
".IP\n"
"pcAdd -u cnt_farm0101 /opt/FMC/tests/cnt -a farm0101\n"
".PP\n"
"and, on node farm0102, to:\n"
".IP\n"
"pcAdd -u cnt_farm0102 /opt/FMC/tests/cnt -a farm0102\n"
".\n"
".PP\n"
"Start and keep up & running the process \"counter\" at all the farm nodes\n"
"controlled by the Ctrl-PC \"ctrl02\":\n"
".\n"
".PP\n"
".ShellCommand pcAdd -C ctrl02 -u counter_0 /opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C ctrl02 -m \\[dq]*\\[dq] -u counter_0 "
"/opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C ctrl02 -m \\[rs]* -u counter_0 "
"/opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Start and keep up & running the process \"counter\" only at the farm node\n"
"\"farm0205\" controlled by the Ctrl-PC \"ctrl02\":\n"
".\n"
".PP\n"
".ShellCommand pcAdd -C ctrl02 -m farm0205 -u counter_0 "
"/opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Use wildcard patterns to select Ctrl-PCs and farm nodes:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -C \\[dq]ctrl*\\[dq] -u counter_0 /opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C \\[dq]ctrl*\\[dq] -m \\[dq]farm01*\\[dq]\\: "
" -u counter_0 /opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C \\[dq]ctrl0*\\[dq] -C \\[dq]ctrl1*\\[dq]\\: "
"-m \\[dq]farm01*\\[dq]\\: -m \\[dq]farm12*\\[dq]\\: -u counter_0\\: "
"/opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C ctrl\\[rs]* -u counter_0 /opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C ctrl\\[rs]* -m farm01\\[rs]* -u counter_0\\: "
"/opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C \\[dq]ctrl0?\\[dq] -u counter_0\\: "
"/opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C ctrl0\\[rs]? -u counter_0 /opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C \\[dq]ctrl[3-7]0\\[dq] -u counter_0\\: "
"/opt/FMC/tests/counter\n"
".ShellCommand pcAdd -C \\[dq]ctrl[3-7]?\\[dq] -u counter_0\\: "
"/opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Redirect the stdout and the stderr of the started process \"counter\" to\n"
"the default FMC logger:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -u counter_0 -o -e /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Start the process as daemon and set the working directory:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -C ctrl03 -d -u counter_0 -w /opt/FMC/tests ./counter\n"
".\n"
".PP\n"
"Clear the environment and set the working directory:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -C ctrl03 -c -u myps -w /bin ps -e -f\n"
".\n"
".PP\n"
"Use the scheduler SCHED_FIFO with static priority set to 1:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -d -s 1 -r 1 -u counter_0 /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Use the scheduler SCHED_OTHER with nice level set to -10 and user set to\n"
"\"galli\":\n"
".\n"
".PP\n"
".ShellCommand pcAdd -d -p -10 -n galli -u counter_0 /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Clear the environment and set two new environment variables:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -c -d -u my_counter\\: "
"-D LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib\\: "
"-D DIM_DNS_NODE=ctrl00.daq.lhcb\\: /opt/FMC/tests/counter\n"
".\n"
".PP\n"
"Clear the environment, set two new environment variables, run as daemon,\n"
"with scheduler SCHED_FIFO and static priority set to 1,\n"
"set the user, the utgid and the working directory, redirect stdout\n"
"and stderr:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -m farm0101 -c -d\\: "
"-D LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib\\: "
"-D DIM_DNS_NODE=ctrl00.daq.lhcb\\: "
"-s 1\\: -r 1\\: -n galli\\: -u my_counter\\: -w /opt/FMC/tests\\: -e\\: "
"-o\\: ./counter 123\n"
".\n"
".PP\n"
"Set the CPU affinity of the process to CPU 0 and 2:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -m farm0141 -c -d\\: "
"-D LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib\\: "
"-D DIM_DNS_NODE=ctrl00.daq.lhcb\\: -s 1\\: -r 1\\: -a 0\\: -a 2\\: "
"-n galli\\: -u my_counter\\: -w /opt/FMC/tests\\: -e -o \\:./counter 123\n"
".\n"
".PP\n"
"Using a specific logger (non-default) for both stdout and \n"
"stderr:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -C crtl03 -m farm0301 -c -d\\: "
"-D LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib\\: "
"-D DIM_DNS_NODE=ctrl00.daq.lhcb\\: -s 1\\: -r 1\\: -n galli\\: "
"-u myCounter\\: -w /opt/FMC/tests\\: -E /tmp/counterErr.fifo\\: "
"-O /tmp/counterOut.fifo\\: -A ./counter 123\n"
".\n"
".PP\n"
"Modify the respawn control parameters:\n"
".\n"
".PP\n"
".ShellCommand pcAdd -C ctrl02\\: -m farm0141\\: -c\\: -d\\: "
"-D LD_LIBRARY_PATH=/opt/dim/linux:/opt/FMC/lib\\: "
"-D DIM_DNS_NODE=ctrl00.daq.lhcb\\: -s 1\\: -r 1\\: -a 0\\: -a 2\\: "
"-n galli\\: -u myCounter\\: -M 20\\: -K 180\\: -X 60\\: -w /opt/FMC/tests\\: "
"-e\\: -o\\: ./counter 123\n"
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
".BR \\%%pcRm (1),\n"
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
