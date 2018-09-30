/*****************************************************************************/
/*
 * $Log: psSetProperties.C,v $
 * Revision 1.6  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.5  2009/07/21 12:05:21  galli
 * improved man page
 *
 * Revision 1.1  2009/01/22 10:17:26  galli
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
/*---------------------------------------------------------------------------*/
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                               /* printServerVersion() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#include "psSrv.h"                  /* SRV_NAME, FIELD_N_ERR, MAX_HOSTNAME_L */
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
int getLocalOpt(int *argcP,char ***argvP,const char* optS,int withArg,
                char **optArg);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: psSetProperties.C,v 1.6 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i=0,j=0;
  /* pointers */
  char *p=NULL;
  char *setMonCmd=NULL;
  char *setMonCmdDup=NULL;
  char *s=NULL;
  /* service browsing */
  int hostPttnC=0;
  char **hostPttnV=NULL;
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
  int doPrintServerVersion=0;
  /* command line args */
  char *argz=0;
  size_t argz_len=0;
  char *cmd;
  char *action=NULL;
  //int cmdLen;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process local command-line options (-h, --help, -hh, -m, -v, -V) */
  if(getLocalOpt(&argc,&argv,"-V",0,&s))doPrintServerVersion=1;
  if(getLocalOpt(&argc,&argv,"--version",0,&s))doPrintServerVersion=1;
  if(argc==1 && !doPrintServerVersion)                       /* no arguments */
  {
    fprintf(stderr,"\nERROR: at least one action required {--start | "
            "--stop}!\n");
    shortUsage();
  }
  if(getLocalOpt(&argc,&argv,"-h",0,&s))usage(0);
  if(getLocalOpt(&argc,&argv,"--help",0,&s))usage(0);
  if(getLocalOpt(&argc,&argv,"-hh",0,&s))usage(1);
  if(getLocalOpt(&argc,&argv,"-v",0,&s))deBug++;
  found=getLocalOpt(&argc,&argv,"-N",1,&s);
  if(getLocalOpt(&argc,&argv,"--start",0,&s))action=(char*)"start";
  if(getLocalOpt(&argc,&argv,"--stop",0,&s))action=(char*)"stop";
  if(!action && !doPrintServerVersion)
  {
    fprintf(stderr,"\nERROR: at least one action required {--start | "
            "--stop}!\n");
    shortUsage();
  }
  if(found==1)dimDnsNode=s;
  else if(found==-1)shortUsage();
  for(hostPttnC=0,found=0;;)
  {
    found=getLocalOpt(&argc,&argv,"-m",1,&s);
    if(found==-1)shortUsage();
    else if(found==0)break;
    else
    {
      hostPttnC++;
      hostPttnV=(char**)realloc(hostPttnV,hostPttnC*sizeof(char*));
      hostPttnV[hostPttnC-1]=s;
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
  if(doPrintServerVersion) getServerVersion(hostPttnC,hostPttnV);
  /*-------------------------------------------------------------------------*/
  /* prepare command line to be passed to tmSrv */
  argz_create(argv+1,&argz,&argz_len);
  argz_stringify(argz,argz_len,32);
  if(argz==NULL)
  {
    fprintf(stderr,"\nERROR: at least one selection option among: "
            "[-t | --tid TID], [-u |--utgid UTGID], [-c | --cmd CMD], "
            "[-C | --cmdline CMDLINE], [--a1 ARG1], ..., [--a9 ARG9] "
            "required!\n");
    shortUsage();
  }
  /*-------------------------------------------------------------------------*/
  printf("Node patterns:  ");
  for(i=0;i<hostPttnC;i++)                        /* loop over node patterns */
  {
    printf("\"%s\"",hostPttnV[i]);
    if(i<hostPttnC-1)printf(", ");
  }
  printf(".\n");
  cmd=argz;
  /*-------------------------------------------------------------------------*/
  for(i=0,j=0;i<hostPttnC;i++)                    /* loop over node patterns */
  {
    /*.......................................................................*/
    printf("  Node pattern: \"%s\"\n",hostPttnV[i]);
    srvPattern=(char*)realloc(srvPattern,1+strlen(hostPttnV[i]));
    strcpy(srvPattern,hostPttnV[i]);
    /* convert to upper case */
    for(p=srvPattern;*p;p++)*p=toupper(*p);
    cmdPatternLen=snprintf(NULL,0,"%s/%s/%s/%sMonitoring",SVC_HEAD,
                           srvPattern,SRV_NAME,action);
    cmdPattern=(char*)realloc(cmdPattern,1+cmdPatternLen);
    snprintf(cmdPattern,1+cmdPatternLen,"%s/%s/%s/%sMonitoring",SVC_HEAD,
             srvPattern,SRV_NAME,action);
    if(deBug)printf("  Command pattern: \"%s\"\n",cmdPattern);
    /*.......................................................................*/
    rawCmdPatternLen=snprintf(NULL,0,"%s/*/%s/%sMonitoring",SVC_HEAD,SRV_NAME,
                              action);
    rawCmdPattern=(char*)realloc(rawCmdPattern,1+rawCmdPatternLen);
    snprintf(rawCmdPattern,1+rawCmdPatternLen,"%s/*/%s/%sMonitoring",
             SVC_HEAD,SRV_NAME,action);
    /*.......................................................................*/
    br.getServices(rawCmdPattern);
    nodeFoundN=0;
    while((type=br.getNextService(setMonCmd,format))!=0)  /* loop over nodes */
    {
      if(!fnmatch(cmdPattern,setMonCmd,0))
      {
        nodeFoundN++;
        setMonCmdDup=(char*)realloc(setMonCmdDup,1+strlen(setMonCmd));
        strcpy(setMonCmdDup,setMonCmd);
        p=strrchr(setMonCmdDup,'/');
        *p='\0';
        p=strrchr(setMonCmdDup,'/');
        *p='\0';
        p=strrchr(setMonCmdDup,'/');
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
        if(deBug)printf("  %3d Contacting Process Monitor Service: \"%s\""
                        "...\n",j,setMonCmd);
        DimClient::sendCommand(setMonCmd,cmd);
        j++;
      }
    }                                                     /* loop over nodes */
    if(!nodeFoundN)
    {
      printf("    no node found for pattern: \"%s\"\n",hostPttnV[i]);
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
void getServerVersion(int hostPttnC,char **hostPttnV)
{
  printf("HOSTNAME     FMC      SERVER\n");
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,SVC_HEAD,SRV_NAME,
                               deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"",SRV_NAME,
                               deBug,12,8);
  /* older service name rule */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"","procs",
                               deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"psSetProperties [-v] [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"                { --start | --stop } [-t | --tid TID]\n"
"                [-u | --utgid UTGID_PATTERN] [-c | --cmd CMD_PATTERN]\n"
"                [-C | --cmdline CMDLINE_PATTERN] [--a1 ARG1_PATTERN]\n"
"                [--a2 ARG2_PATTERN] [--a3 ARG3_PATTERN]\n"
"                [--a4 ARG4_PATTERN] [--a5 ARG5_PATTERN]\n"
"                [--a6 ARG6_PATTERN] [--a7 ARG7_PATTERN]\n"
"                [--a8 ARG8_PATTERN] [--a9 ARG9_PATTERN]\n"
"psSetProperties { -V | --version } [-v] [-N DIM_DNS_NODE]\n"
"                [-m NODE_PATTERN...]\n"
"psSetProperties { -h | --help }\n"
"\n"
"Try \"psSetProperties -h\" for more information.\n";
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
"psSetProperties.man\n"
"\n"
"..\n"
"%s"
".hw ARG1_PATTERN ARG2_PATTERN ARG3_PATTERN ARG4_PATTERN ARG5_PATTERN\n"
".hw ARG6_PATTERN ARG7_PATTERN ARG8_PATTERN ARG9_PATTERN\n"
".TH psSetProperties 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis psSetProperties\\ \\-\n"
"Set the filter options of the FMC Process Monitor Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis psSetProperties\n"
".ShortOpt[] v\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
"{\\ \\fB--start\\fP\\ |\\ \\fB--stop\\fP\\ }\n"
".DoubleOpt[] t tid TID\n"
".DoubleOpt[] u utgid UTGID_PATTERN\n"
".DoubleOpt[] c cmd CMD_PATTERN\n"
".DoubleOpt[] C cmdline CMDLINE_PATTERN\n"
".LongOpt[] a1 ARG1_PATTERN\n"
".LongOpt[] a2 ARG2_PATTERN\n"
".LongOpt[] a3 ARG3_PATTERN\n"
".LongOpt[] a4 ARG4_PATTERN\n"
".LongOpt[] a5 ARG5_PATTERN\n"
".LongOpt[] a6 ARG6_PATTERN\n"
".LongOpt[] a7 ARG7_PATTERN\n"
".LongOpt[] a8 ARG8_PATTERN\n"
".LongOpt[] a9 ARG9_PATTERN\n"
".EndSynopsis\n"
".sp\n"
".Synopsis psSetProperties\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".ShortOpt[] v\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis psSetProperties\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The command \\fBpsSetProperties\\fP sets the filter properties to the FMC "
"Process Monitor Server \\fBpsSrv\\fP(8), which are used to select the "
"processes to be monitored by means of a dedicated DIM service.\n"
".PP\n"
"\\fBpsSrv\\fP(8) keeps the filter properties in a \\fIfilter expression\\fP "
"which is published in the DIM service "
"\"/FMC/\\fIHOSTNAME\\fP/ps/monitored/properties\" and can be read by means "
"of \\fBdid\\fP(1).\n"
".PP\n"
"The option \\fB--start\\fP adds a new (\\fIOR\\fPed) \\fIterm\\fP to the "
"\\fIfilter expression\\fP, while the option \\fB--stop\\fP removes an "
"(\\fIOR\\fPed) \\fIterm\\fP from the \\fIfilter expression\\fP.\n"
".PP\n"
"Different filter options specified in the "
"same \\fBpsSetProperties --start\\fP command are put in logical \\fIAND\\fP "
"among them, while filter options specified in different \\fBpsSetProperties "
"--start\\fP commands are put in logical \\fIOR\\fP among them.\n"
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
"Set the filter properties only to the process(es) running on the nodes whose "
"hostname matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: set the filter properties to the process(es) running at "
"all the nodes).\n"
".\n"
".OptDef V version\n"
"For each selected node print the FMC Process Monitor Server version "
"and the FMC version, than terminate.\n"
".\n"
".OptDef \"\" start\n"
"Add a new \\fIterm\\fP to the \\fIfilter expression\\fP used to select the "
"processes to be monitored by means of a dedicated DIM SVC. Different options "
"specified within the same \\fBpsSetProperties --start\\fP command are put in "
"logical \\fIAND\\fP among them, while options specified in different "
"\\fBpsSetProperties --start\\fP commands are put in logical \\fIOR\\fP among "
"them. The current \\fIfilter expression\\fP can be viewed by means of the "
"\".../monitored/properties\" DIM SVC (e.g. by using \\fBdid\\fP(1)).\n"
".\n"
".OptDef \"\" stop\n"
"Remove an \\fIterm\\fP from the \\fIfilter expression\\fP used to select the "
"processes to be monitored by means of a dedicated DIM SVC. The command to "
"remove a certain filter \\fIterm\\fP must have \\fIexactly\\fP the same "
"options used before to add the same filter \\fIterm\\fP. E.g., if a filter "
"\\fIterm\\fP was added by means of the command:\n"
".ShellCommand psSetProperties --start -c *Srv -u *Srv_u\n"
"it can be removed by means of the command:\n"
".ShellCommand psSetProperties --stop -c *Srv -u *Srv_u\n"
"The current \\fIfilter expression\\fP can be viewed by means of the "
"\".../monitored/properties\" DIM SVC (e.g. by using \\fBdid\\fP(1)).\n"
".\n"
".OptDef t tid TID (unsigned integer)\n"
"Select/deselect the process which has \\fITID\\fP as Thread Identifier.\n"
".\n"
".OptDef u utgid UTGID_PATTERN (string)\n"
"Select/deselect the process(es) whose UTGID matches the wildcard pattern "
"\\fIUTGID_PATTERN\\fP.\n"
".\n"
".OptDef c cmd CMD_PATTERN (string)\n"
"Select/deselect the process(es) whose command name (basename, w/o path, of "
"the executable image file, without arguments, eventually truncated at 15 "
"characters) matches the wildcard pattern \\fICMD_PATTERN\\fP.\n"
".\n"
".OptDef C cmdline CMDLINE_PATTERN (string)\n"
"Select/deselect the process(es) whose command line (complete name of the "
"executable image file, including path but excluding arguments, up to "
"PATH_MAX = 4096 characters) matches the wildcard pattern "
"\\fICMDLINE_PATTERN\\fP.\n"
".\n"
".OptDef \"\" a\\fIn\\fP ARGn_PATTERN (string)\n"
"Select/deselect the process(es) whose \\fIn\\fP-th argument on the "
"command-line matches the wildcard pattern \\fIARGn_PATTERN\\fP (the digit "
"\\fIn\\fP ranges in 1..9).\n"
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
"Start monitoring the process whose command is \"gnome-terminal\"\n"
".PP\n"
".ShellCommand psSetProperties --start -c gnome-terminal\n"
".PP\n"
".\n"
"Stop monitoring the process whose command is \"gnome-terminal\"\n"
".PP\n"
".ShellCommand psSetProperties --stop -c gnome-terminal\n"
".PP\n"
".\n"
"Start monitoring the process whose UTGID is \"ipmiSrv_u\"\n"
".PP\n"
".ShellCommand psSetProperties --start -u ipmiSrv_u\n"
".PP\n"
".\n"
"Stop monitoring the process whose UTGID is \"ipmiSrv_u\"\n"
".PP\n"
".ShellCommand psSetProperties --stop -u ipmiSrv_u\n"
".PP\n"
".\n"
"Start monitoring the process whose UTGID matches the wildcard pattern "
"\"*Log\" and whose command matches the wildcard pattern \"*Srv\" on the "
"nodes whose hostname matches the wildcard pattern \"lhcb*\"\n"
".PP\n"
".ShellCommand psSetProperties --start -m lhcb\\[rs]* -u \\[rs]*Log -c "
"\\[rs]*Srv\n"
".PP\n"
".\n"
"Stop monitoring the process whose UTGID matches the wildcard pattern "
"\"*Log\" and whose command matches the wildcard pattern \"*Srv\" on the "
"nodes whose hostname matches the wildcard pattern \"lhcb*\"\n"
".PP\n"
".ShellCommand psSetProperties --stop -m lhcb\\[rs]* -u \\[rs]*Log -c "
"\\[rs]*Srv\n"
".PP\n"
".\n"
"Start monitoring the process whose PID is 137\n"
".PP\n"
".ShellCommand psSetProperties --start -t 137\n"
".PP\n"
".\n"
"Stop monitoring the process whose PID is 137\n"
".PP\n"
".ShellCommand psSetProperties --stop -t 137\n"
".PP\n"
".\n"
"Start monitoring the process whose command line is\n"
".br\n"
"\\[dq]syslogd -m 0 -r -a /var/empty/sshd/dev/log\\[dq]:\n"
".PP\n"
".ShellCommand psSetProperties --start \\[dq]--cmdline syslogd --a1 -m --a2 0 "
"--a3 -r\\: "
"--a4 -a --a5 /var/empty/sshd/dev/log\\[dq]\n"
".\n"
".PP\n"
".\n"
"Stop monitoring the process whose command line is\n"
".br\n"
"\\[dq]syslogd -m 0 -r -a /var/empty/sshd/dev/log\\[dq]:\n"
".PP\n"
".ShellCommand psSetProperties --stop \\[dq]--cmdline syslogd --a1 -m --a2 0 "
"--a3 -r\\: "
"--a4 -a --a5 /var/empty/sshd/dev/log\\[dq]\n"
".\n"
".\n"
".PP\n"
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
".SH SEE ALSO\n"
".ad l\n"
".BR \\%%psMonitor (1),\n"
".BR \\%%psSrv (8),\n"
".BR \\%%psViewer (1).\n"
".br\n"
".BR \\%%tmSrv (8),\n"
".BR \\%%tmStart (1).\n"
".br\n"
".BR \\%%top (1),\n"
".BR \\%%ps (1),\n"
".BR \\%%proc (5).\n"
".br\n"
".BR \\%%/usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt.\n"
".\n"
,groffMacro,FMC_DATE,FMC_VERSION
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/*****************************************************************************/
